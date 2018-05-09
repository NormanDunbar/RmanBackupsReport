/*
 *=============================================================================
 * This code connected to the database(s) supplied on the command line, as the
 * DBA_USER, using the password supplied and retrieves details of the RMAN
 * backups taken since the start of 'yesterday'.
 *
 * rmanBackups database [database ...] password how_many_days
 *
 * The result will be some HTML which is written to stdout, so if you need to
 * view it in a browser, best you redirect to a file, as follows:
 *
 * rmanBackups database [database ...] password  how_many_days >backups.html
 *
 * you can now view backups.html in a browser.
 *
 *=============================================================================
 */

/*
 *-----------------------------------------------------------------------------
 * AMENDMENT HISTORY
 *-----------------------------------------------------------------------------
 * 01/05/2018   Norman Dunbar   Version 1.00.
 *                              Created new utility due to a lack of OEM access
 *                              to the repository where such information may be
 *                              available. OEM reports were no good either. :-(
 *-----------------------------------------------------------------------------
 * 03/05/2018   Norman Dunbar   Version 1.01.
 *                              Added current date and time to the output.
 *                              Added footer text.
 *                              Fixed bug where HTMLFooter() was called at the
 *                              end of every database, not once at the end of
 *                              ALL databases. Sigh.
 *-----------------------------------------------------------------------------
 */


#include "ocilib.h"
#include "rmanBackups.h"

/*
 * Start here...
 */
int main(int argc, char *argv[])
{
    /*
     * We need at least 4 parameters. (Program name, database, password, days.)
     */
    if (argc < 4) {
        fprintf(stderr, "\nUSAGE: %s database [ database ...] Password how_many_days\n\n", argv[0]);
        fprintf(stderr, "EXAMPLE: %s pnet01p1 misa01p01 ukmhprddb TopSecret 5\n", argv[0]);
        exit(1);
    }

    char *passWord = argv[argc - 2];
    char *daysAgo = argv[argc - 1];

    /*
     * Time related stuff.
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
     * Display details of command line.
     */
    fprintf(stderr, "\nRmanBackups - version %.2f\n\n", VERSION);
    fprintf(stderr, "Checking databases: ");
    for (int x = 1; x < argc - 2; x++) {
        fprintf(stderr, "%s ", argv[x]);
    }
    fprintf(stderr, "\nUsing dba_user password '%s'\n", passWord);
    fprintf(stderr, "Checking backups in the last %s day(s).\n", daysAgo);
    fprintf(stderr, "Report executed on %s.\n", displayTime);

    /*
     * Add the days into the SQLTemplate.
     * BEWARE: This must be bigger than the code in SQLTemplate.
     * ALSO: The number of days appears TWICE!
     */
    char SQL[2048];
    sprintf(SQL, SQLTemplate, daysAgo, daysAgo);


    /*
     * Start the HTML file.
     */
    HTMLHeader(displayTime);


    /*
     * Database stuff.
     */
    OCI_Connection* cn;
    OCI_Statement* st;
    OCI_Resultset* rs;


    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT)) {
        fprintf(stderr, "Cannot initialise the Oracle Environment.\n");
        return EXIT_FAILURE;
    }

    /*
     *======================================================================
     * Any time we bale out, we must clean up. BEWARE!
     * This means we have to exit via cleanUp below and end the OCI session.
     *======================================================================
     */

    /*
     * Loop around all the supplied databases.
     */
    for (int x = 1; x < argc - 2; x++) {

        /*
         * Where are we?
         */
        fprintf(stderr, "\nChecking database %s\n", argv[x]);

        /*
         * Connect here ...
         */
        cn = OCI_ConnectionCreate(argv[x], "dba_user", passWord, OCI_SESSION_DEFAULT);
        if (dbErrors) {
            fprintf(stderr, "Unable to connect to database (%s).\n", argv[x]);
            dbErrors = FALSE;

            /* Try next database. */
            continue;
        }

        /*
         *======================================================================
         * Any time we bale out now, we must log off the database. BEWARE!
         * This means we have to exit via logOff below and end the DB session.
         *======================================================================
         */

        /*
         * Statement creation.
         */
        st = OCI_StatementCreate(cn);
        if (dbErrors) {
            fprintf(stderr, "%s: Unable to create statement.\n", argv[x]);
            goto logOff;
        }

        /* BEWARE: This isn't working here! It gives a warning
         * ORA-24347: Warning of a NULL column in an aggregate function
         * on the first row of the resultset and no output for that row.
         */
        //OCI_SetFetchMode(st, OCI_SFM_SCROLLABLE);
        //if (dbErrors) {
        //    fprintf(stderr, "%s: Unable to scroll statement.\n", argv[x]);
        //    goto logOff;
        //}

        /*
         * Execution.
         */
        OCI_ExecuteStmt(st, SQL);
        if (dbErrors) {
            fprintf(stderr, "%s: Unable to execute statement.\n", argv[x]);
            goto logOff;
        }

        /*
         * Grab results.
         */
        rs = OCI_GetResultset(st);
        if (dbErrors) {
            fprintf(stderr, "%s: Unable to fetch results.\n", argv[x]);
            goto logOff;
        }


        /*
         * Print database details.
         */
        HTMLDatabase(argv[x], rs, daysAgo);

    logOff:
        /*
         * Logoff database.
         */
        OCI_ConnectionFree(cn);
        fprintf(stderr, "Checking complete.\n");
        dbErrors = FALSE;
    }

    /*
     * Finish HTML file.
     */
    HTMLFooter(displayTime, argv[0]);

cleanUp:
    /*
     * Clean up database stuff.
     */
    OCI_Cleanup();

    if (dbErrors) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/*
 * Print out the HTML file headings, plus the actual content main heading.
 */
void HTMLHeader(char *displayTime) {
    fprintf(stdout, htmlHeader, css, displayTime);
}


/*
 * Print out the HTML file trailers to close down the file.
 */
void HTMLFooter(char *displayTime, char *programName) {
    fprintf(stdout, htmlFooter, displayTime, programName);
}


/*
 * Print out the details of a single database backup list.
 */
void HTMLDatabase(char *database, OCI_Resultset* rs, char *days) {
    /*
     * Print the header and introduction.
     */
    fprintf(stdout, dbHeading, database);
    fprintf(stdout, dbParagraph, days, (strcmp(days, "1") ? "s" : ""));

    /*
     * Now the HTML table and headings.
     */
    fprintf(stdout, tableHeadings);

    /*
     * Scan the results and print the details.
     */
    while (OCI_FetchNext(rs))
    {
        const char *status = OCI_GetString(rs, COL_STATUS);
        char class[10] = {"normal"};
        if (strcmp("COMPLETED", status)) {
            strcpy(class , "error");
        }

        fprintf(stdout,
                tableRow,
                OCI_GetInt(rs, COL_JOB_ID),
                OCI_GetString(rs, COL_START_TIME),
                OCI_GetString(rs, COL_END_TIME),
                OCI_GetString(rs, COL_RUN_TIME),
                OCI_GetString(rs, COL_INPUT_SIZE),
                OCI_GetString(rs, COL_WRITTEN_SIZE),
                class, status, //OCI_GetString(rs, COL_STATUS),
                OCI_GetString(rs, COL_TASKS)
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
 * Error handler for database operations.
 */
void err_handler(OCI_Error *err)
{
    fprintf(stderr, "%s", OCI_ErrorGetString(err));
    fprintf(stderr, "Error ORA-%05i detected\n", OCI_ErrorGetOCICode(err));
    dbErrors = TRUE;
}


