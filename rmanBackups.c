/*
 *=============================================================================
 * This code connects to the database(s) supplied in the parameter file, as the
 * using the supplied credentials and retrieves details of the RMAN backups
 * taken since the start of 'n' days ago.
 *
 *  rmanBackups parameter_file_name [ >Report_file_name.html [ 2>Log_name.log]]
 *
 * The result will be some HTML which is written to stdout, so if you need to
 * view it in a browser, best you redirect to a file. Errors etc are written to
 * the logfile on stderr, which may also be redirected. The default is to the
 * display.
 *
 * The report can be viewed in your browser.
 *=============================================================================
 */

/*
 *-----------------------------------------------------------------------------
 * AMENDMENT HISTORY
 *-----------------------------------------------------------------------------
 * 01/05/2018   Norman Dunbar   Version 1.00.
 *
 * Created new utility due to a lack of OEM access to the repository where such
 * information may be available. OEM reports were no good either. :-(
 *-----------------------------------------------------------------------------
 * 03/05/2018   Norman Dunbar   Version 1.01.
 *
 * Added current date and time to the output.
 * Added footer text.
 * Fixed bug where HTMLFooter() was called at the end of every database, not
 * just once at the end of ALL databases. Sigh.
 *-----------------------------------------------------------------------------
 * 15/02/2019   Norman Dunbar   Version 1.02.
 *
 * Attempting to discover why SQL*Plus, Toad and SQLDeveloper can run the
 * query, but OCILIB gets "ORA-01489: result of string concatenation is
 * too long" errors. Weird. (Not weird - VARCHAR2 limits.)
 *-----------------------------------------------------------------------------
 * ??/??/2019   Norman Dunbar   Versions 1.03.
 *
 * Made the report more readable and easier to see where the database backup
 * was in amongst all the archive log backups etc.
 *-----------------------------------------------------------------------------
 * 06/06/2019   Norman Dunbar   Version 1.04.
 *
 * Report ordered descending by start_time now - easier to find the latest
 * backup details as they are at the top.
 *
 * Allow "C##" in front of the database name to force a connection to the root
 * database for a container/pluggable setup. (Requires a 'common' C##username.)
 *-----------------------------------------------------------------------------
 * 26/06/2019   Norman Dunbar   Version 1.05.
 *
 * The stuff in 1.04 about "C##" in the database name has been removed and the
 * database details are supplied in a text file now. The format is:
 *
 *      "username/password@alias:SYSDBA?daysAgo"
 *
 * one entry per line. Blank lines and lines with '#' in column 1 are
 * ignored.
 *
 * The SQL query had to be changed to allow it to run in a standby database
 * which is not open. The 'with' clause was a problem, strange, as it can
 * easily be joined as an inline view. Hmm.
 *-----------------------------------------------------------------------------
 * 26/11/2019   Norman Dunbar   Version 1.06
 *
 * Amended to produce a summary of the entire list of database backups. This is
 * only the database backups, not the archived logs, no deletions etc.
 *-----------------------------------------------------------------------------
 * 04/12/2019   Norman Dunbar   Version 1.07
 *
 * Summary listed multiple entries for the same database if there were multiple
 * backups of said database. Only the last is required. Added link from summary
 * of databases to full list of all backups and a count of database backups to
 * the summary table.
 *-----------------------------------------------------------------------------
 */


#include "ocilib.h"
#include "rmanBackups.h"

/* Global database name - for error reporting. */
char *gDatabaseName;

/* Global connection string limits. */
#define MAX_CONNECTION_STRING 255

/* Summary buffer size */
#define SUMMARY_BUFFER_SIZE 16384
char *summaryBuffer;


/*
 * Global error handler for the OCILIB. Called automagically
 * whenever an error occurs.
 */
void err_handler(OCI_Error *err)
{
    int errorCode = OCI_ErrorGetOCICode(err);

    fprintf
    (
        stderr,
        "\n--------------------------------------------------------------------------------\n"
        "The following database warning/error has occurred:\n"
        "%s"
        "--------------------------------------------------------------------------------\n\n",
        OCI_ErrorGetString(err)
    );

    /* Set my flag to show errors occurred */
    /* But not for warnings ORA-24347 */
    if (errorCode != 24347) {
        dbErrors = TRUE;
    }

    // This is not ideal. If we hit an ORA-01489 error, it means that there
    // were too many sub-tasks or a single job and it has blown out the size
    // of a varchar. Tell the user.
    if (errorCode == 1489) {
        fprintf(stdout, dbHeading, gDatabaseName);
        fprintf(stdout, "<strong>Too many jobs yesterday, exceeded Oracle's VARCHAR2 limit of 4,000 characters.</strong>");
    }
}


/*
 * Split a connection string into the three separate parts required.
 */
boolean splitConnectionString(const char *connectionString, char **username, char **password, char **alias, char **daysAgo, boolean *sysDBA) {
    // Example:
    //
    // 012345678901234567890123456789
    // ----/----@-----:------?-------
    // user/pass@alias:SYSDBA?days
    //
    // Length = 27, indexes = 0 - 26.
    //
    // User = 0 -> '/' - 1.
    // User = 0 -> 3 = 4 characters.
    //
    // Pass = '/' + 1 -> '@' - 1.
    // Pass = 5 -> 8 = 4 characters.
    //
    // SYSDBA (optional) = ':' + 1 -> '?' - 1.
    // Should be 6 characters, always.
    //
    // Alias = '@' + 1 - len() - 1.
    // Alias = 10 -> 14 = 5 characters.


    // Find slash and 'at' characters.
    char *slash = strchr(connectionString, '/');
    char *at = strchr(connectionString, '@');
    char *qmark = strchr(connectionString, '?');
    char *colon = strchr(connectionString, ':');

    // Convert to strings. Username will be the whole string
    // if no separators found.
    // Casting to lose the const'ness warnings.
    *username = (char *)connectionString;

    // Do we have a password?
    if (slash) {
        *slash = '\0';
        *password = slash + 1;
    } else {
        fprintf(stderr, "'%s': No slash (/) found.\n", connectionString);
        return FALSE;
    }

    // Do we have an alias?
    if (at) {
        *at = '\0';
        *alias = at + 1;
    } else {
        fprintf(stderr, "'%s': No 'at' (@) found.\n", connectionString);
        return FALSE;
    }

    // Do we have a days ago number?
    if (qmark) {
        *qmark = '\0';
        *daysAgo = qmark + 1;
    } else {
        fprintf(stderr, "'%s': No 'question mark' (?) found.\n", connectionString);
        return FALSE;
    }

    // If we get here, we have a usename, password, alias and days ago values.
    // Do we have (optional) SYSDBA? We assume not, unless we find ':SYSDBA'
    // whereupon we assume true
    *sysDBA = FALSE;
    if (colon) {
        if (!strncasecmp(":SYSDBA", colon, 7)) {
            *sysDBA = TRUE;
        }

        // Terminate the alias name.
        *colon = '\0';
    }

    return TRUE;
}
/*
 * Start here...
 */
int main(int argc, char *argv[])
{
    /*
     * Time related stuff for the report.
     */
    time_t currentTime = time(NULL);
    char *displayTime;

    if (currentTime != (time_t)-1) {
        displayTime = ctime(&currentTime);
        displayTime[strlen(displayTime) - 1] = '\0';    /* Lose linefeed. */
    } else {
        displayTime = "Date & time unknown!";
    }

    /*
     * Database related stuff.
     */
    char *databaseName;
    char *userName;
    char *passWord;
    char *daysAgo;
    boolean sysDBA;

    /*
     * We need 2 parameters. (Program name and parameter file name.)
     */
    if (argc != 2) {
        fprintf(stderr, "\nUSAGE: %s <credentails file>\n\n", argv[0]);
        fprintf(stderr, "EXAMPLE: %s logins.txt\n", argv[0]);
        fprintf(stderr, "\n\nWhere the format of each line in the file is:\n");
        fprintf(stderr, "\nusername/password@tns_alias:SYSDBA?Days\n\n");
        fprintf(stderr, "Blank lines and those beginning with # are ignored.\n");
        fprintf(stderr, "\n The colon (:) and SYSDBA can be omitted if required.\n");
        exit(1);
    }

    /*
     * Parameter file stuff.
     */
    FILE *pFile;
    char current[MAX_CONNECTION_STRING + 1];

    pFile = fopen(argv[1], "r");
    if (pFile == NULL || ferror(pFile)) {
        fprintf(stderr, "\nCannot open file '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    /*
     * Display report details in the log file on stderr.
     */
    fprintf(stderr, "RmanBackups - version %.2f\n", VERSION);
    fprintf(stderr, "Backup Report executed on %s.\n\n", displayTime);

    /*
     * Start the HTML file.
     */
    HTMLHeader(displayTime);

    /*
     * Initialise the Oracle OCI environment.
     */
    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT)) {
        fprintf(stderr, "Cannot initialise the Oracle Environment.\n");
        return EXIT_FAILURE;
    }

    /* Allocate a huge buffer for the summary table. */
    summaryBuffer = (char *)malloc(SUMMARY_BUFFER_SIZE);
    if (summaryBuffer == NULL) {
        fprintf(stderr, "Cannot allocate Summary Buffer.");
        return EXIT_FAILURE;
    }

    /* Copy the summary headers over */
    *summaryBuffer = '\0';
    if (!appendSummary(summaryHeadings)) {
        /* stderr already updated. */
        return EXIT_FAILURE;
    }



    /*
     * The main loop starts here ....
     */
    while (1) {
        // Fetch a connection string.
        fgets(current, MAX_CONNECTION_STRING, pFile);

        // Errors reading?
        if (ferror(pFile)) {
            fprintf(stderr, "\nOops! Read error on '%s'\n", argv[1]);
            break;
        }

        // End of file?
        if (feof(pFile)) {
                break;
        }

        // fgets() doesn't delete the '\n' if one is found.
        current[strlen(current) -1] = '\0';

        // Comment lines have '#' at the start.
        if (*current == '#') {
            continue;
        }

        // Blank lines are also ignored.
        if (*current == '\0') {
            continue;
        }


        // All good - split.
        if (splitConnectionString(current, &userName, &passWord, &databaseName, &daysAgo, &sysDBA)) {

            // Update global database name for errors.
            gDatabaseName = databaseName;

            // Add database headings to the report.
            HTMLDatabaseHeading(databaseName, daysAgo);

            // Check this database
            if (!checkDatabase(userName, passWord, databaseName, daysAgo, sysDBA)) {
                fprintf(stderr, "Checking FAILED.\n");
                fprintf(stderr, "================================================================================\n\n");
            }

        } else {
            // Log errors encountered for this connection string.
            fprintf(stderr, "Failed to split connection string '%s', one or more parts missing.\n", current);
        }
    }       // End While(1)

    /*
     * Clean up database stuff.
     */
    OCI_Cleanup();

    /*
     * Close the parameter file now, no longer needed.
     */
    fclose(pFile);

    /*
     * Print the report summary.
     */
    fprintf(stdout, summaryTitle);
    fprintf(stdout, summaryPara);

    appendSummary("</table>\n");

    fprintf(stdout, "\n%s\n", summaryBuffer);
    free(summaryBuffer);

    /*
     * Print the report footers.
     */
    HTMLFooter(displayTime, argv[0]);

    if (dbErrors) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}   // End of main().



/*
 * Login and check a database. This assumes that the OCI Environment has been initialised.
 */
boolean checkDatabase(const char *userName, const char *passWord, const char *databaseName, const char *daysAgo, boolean sysDBA) {

    OCI_Connection* cn;
    OCI_Statement* st;
    OCI_Resultset* rs;

    /*
     * Where are we?
     */
    fprintf(stderr, "\n================================================================================\n");
    fprintf(stderr, "*** %s ***\n", databaseName);
    fprintf(stderr, "================================================================================\n");
    if (!sysDBA) {
        fprintf(stderr, "Checking '%s@%s' for backups in the last %s day(s).\n", userName, databaseName, daysAgo);
    } else {
        fprintf(stderr, "Checking '%s@%s as SYSDBA' for backups in the last %s day(s).\n", userName, databaseName, daysAgo);
    }


    /*
     * Connect here ...
     */
    if (sysDBA) {
        /* Connect as SYSDBA */
        cn = OCI_ConnectionCreate(databaseName, userName, passWord, OCI_SESSION_SYSDBA);
    } else {
        /* Connect as NORMAL */
        cn = OCI_ConnectionCreate(databaseName, userName, passWord, OCI_SESSION_DEFAULT);
    }
    if (dbErrors) {
        fprintf(stderr, "Unable to connect to '%s@%s'.\n", userName, databaseName);
        dbErrors = FALSE;

        /* Try next database. */
        return FALSE;
    }

    /*
     *======================================================================
     * Any time we bale out now, we must log off the database. BEWARE!
     * This means we have to exit via logOff below and end the DB session.
     *======================================================================
     */

    /*
     * Add the days into the SQLTemplate.
     * BEWARE: The buffer must be bigger than the code size in SQLTemplate.
     * ALSO: The number of days appears TWICE!
     */
    char SQL[4096];
    sprintf(SQL, SQLTemplate, daysAgo, daysAgo);
    //sprintf(SQL, SQLTemplate);


    /*
     * Statement creation.
     */
    st = OCI_StatementCreate(cn);
    if (dbErrors) {
        fprintf(stderr, "Unable to create SQL query.\n\n");
        goto logOff;
    }

    /* BEWARE: This isn't working here! It gives an error:
     * "ORA-01220: file based sort illegal before database is open"
     * and no output if the database is a standby WITHOUT Active Data Guard.
     */
    //OCI_SetFetchMode(st, OCI_SFM_SCROLLABLE);
    // ...

    /*
     * Execution.
     */
    //OCI_ExecuteStmt(st, "alter session set tracefile_identifier=BACKUP_CHECKER");
    //OCI_ExecuteStmt(st, "alter session set events '10046 trace name context forever, level 12'");
    OCI_ExecuteStmt(st, SQL);
    if (dbErrors) {
        fprintf(stderr, "Unable to execute SQL query.\n\n");
        goto logOff;
    }

    /*
     * Grab results.
     */
    rs = OCI_GetResultset(st);
    if (dbErrors) {
        fprintf(stderr, "Unable to fetch query results.\n\n");
        goto logOff;
    }


    /*
     * Print database details.
     */
    HTMLDatabase(rs);


    // Try to get a summary of this database's backups of the database only.
    // This will be - hopefully - printed at the top of the report.
    sprintf(SQL, SQLSummary, databaseName, daysAgo);

    OCI_Statement* st2;
    OCI_Resultset* rs2;


    // Execute statement for summary.
    st2 = OCI_StatementCreate(cn);
    if (dbErrors) {
        fprintf(stderr, "Unable to create Summary SQL query.\n\n");
        goto logOff;
    }

    OCI_ExecuteStmt(st2, SQL);
    if (dbErrors) {
        fprintf(stderr, "Unable to execute Summary SQL query.\n\n");
        goto logOff;
    }

    // Get results.
    rs2 = OCI_GetResultset(st2);
    if (dbErrors) {
        fprintf(stderr, "Unable to fetch Summary query results.\n\n");
        goto logOff;
    }

    // Save results.
    HTMLSummary(rs2);

logOff:
    /*
     * Logoff database.
     */
    OCI_ConnectionFree(cn);
    fprintf(stderr, "Backup checking complete.\n");
    fprintf(stderr, "================================================================================\n\n");
    dbErrors = FALSE;

    return TRUE;
}


/*
 * Print out the HTML file headings, plus the actual content main heading.
 */
void HTMLHeader(const char *displayTime) {
    fprintf(stdout, htmlHeader, css, displayTime);
}


/*
 * Print out the HTML file trailers to close down the file.
 */
void HTMLFooter(const char *displayTime, const char *programName) {
    fprintf(stdout, htmlFooter, displayTime, programName);
}


/*
 * Print out the details of a single database backup list.
 */
void HTMLDatabaseHeading(const char *database, const char *days) {
    /*
     * Print the Database header and introduction.
     */
    fprintf(stdout, dbHeading, database, database);
    fprintf(stdout, dbParagraph, days, (strcmp(days, "1") ? "s" : ""));
}


/*
 * Print out the details of a single database backup list.
 */
void HTMLDatabase(OCI_Resultset* rs) {
    /*
     * Print the HTML table and headings.
     */
    fprintf(stdout, tableHeadings);

    /*
     * Scan the results and print the details.
     */
    while (OCI_FetchNext(rs))
    {
        const char *status = OCI_GetString2(rs, "STATUS");
        char class[10] = {"normal"};

        /*
         * Issue #3, RAG status required.
         */
        if (!strcmp("COMPLETED", status)) {
            /* GREEN = All OK. No further action. */
            strcpy(class, "green");
        } else {
            if (strstr(status, "WARNINGS") || !strcmp("RUNNING", status)) {
                /* AMBER = OK, so far, or WARNINGs reported. Check again later. */
                strcpy(class, "amber");
            }  else {
                if (strstr(status, "ERRORS") || !strcmp("FAILED", status)) {
                    /* RED = Failed or ERRORs reported. Raise incident. */
                    strcpy(class, "red");
                }
            }
        }

        fprintf(stdout,
                tableRow,
                OCI_GetInt2(rs, "JOB_ID"),
                OCI_GetString2(rs, "START_TIME"),
                OCI_GetString2(rs, "END_TIME"),
                OCI_GetString2(rs, "RUN_TIME"),
                OCI_GetString2(rs, "INPUT_SIZE"),
                OCI_GetString2(rs, "WRITTEN_SIZE"),
                class, status,
                OCI_GetString2(rs, "TASKS_EXECUTED_IN_JOB")
               );
}

    /*
     * Close down the table for this database.
     */
    fprintf(stdout,
            "</table>\n"
           );
}


/*
 * Save the details of database backup summary.
 */
void HTMLSummary(OCI_Resultset* rs) {

    char tempBuffer[2048] = "\0";
    unsigned char backupCount = 0;

    /*
     * Scan the results and print the details. If a database has more than
     * one backup in the past 24 hours, we can only put an anchor on the
     * start of the table, so no need to have multiple entries in the summary
     * table - which links back to the full table of all backups for the database.
     * So, we only pull the last row for each database.
     */
    while (OCI_FetchNext(rs))
    {
        const char *status = OCI_GetString2(rs, "STATUS");
        char class[10] = {"normal"};

        /* Wipe the buffer each time around. */
        tempBuffer[0] = '\0';
        backupCount++;

        /*
         * RAG status.
         */
        if (!strcmp("COMPLETED", status)) {
            /* GREEN = All OK. No further action. */
            strcpy(class, "green");
        } else {
            if (strstr(status, "WARNINGS") || !strcmp("RUNNING", status)) {
                /* AMBER = OK, so far, or WARNINGs reported. Check again later. */
                strcpy(class, "amber");
            }  else {
                if (strstr(status, "ERRORS") || !strcmp("FAILED", status)) {
                    /* RED = Failed or ERRORs reported. Raise incident. */
                    strcpy(class, "red");
                }
            }
        }

        sprintf(tempBuffer,
                summaryRow,
                gDatabaseName,
                OCI_GetString2(rs, "DATABASE"),
                backupCount,
                OCI_GetInt2(rs, "JOB_ID"),
                OCI_GetString2(rs, "RMAN_COMMAND"),
                class, status,
                OCI_GetString2(rs, "START_TIME"),
                OCI_GetString2(rs, "END_TIME"),
                OCI_GetString2(rs, "RUN_TIME"),
                OCI_GetString2(rs, "INPUT_SIZE"),
                OCI_GetString2(rs, "WRITTEN_SIZE")
               );
    }

    /* The last row is still in the buffer. */
    if (!tempBuffer[0] == '\0') {
        appendSummary(tempBuffer);
    }

}


/*
 * A function to safely append two strings together.
 * In the event that the destination is not big enough, it will be truncated.
 * This is used for the summaryBuffer which is limited to SUMMARY_BUFFER_SIZE characters.
 */
boolean appendSummary(const char *source)
{
    size_t currentSpace = strlen(summaryBuffer);
    if (currentSpace >= SUMMARY_BUFFER_SIZE) {
        fprintf(stderr, "Summary Buffer full up. Only %d characters permitted\n", SUMMARY_BUFFER_SIZE);
        return FALSE;
    }

    /* This will truncate as required. */
    strncat(summaryBuffer, source, SUMMARY_BUFFER_SIZE - currentSpace);
    summaryBuffer[strlen(summaryBuffer)] = '\0';
    return TRUE;
}

