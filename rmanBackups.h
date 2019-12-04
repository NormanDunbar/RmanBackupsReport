#ifndef __RMANBACKUPS_H__
#define __RMANBACKUPS_H__

#include <time.h>
#include <strings.h>
#include "css.h"
#include "sql.h"
#include "version.h"
#include "html.h"

/*
 * Flag that indicates if database errors occurred.
 */
boolean dbErrors = FALSE;


/*
 * Error handler for database operations.
 */
void err_handler(OCI_Error *err);

/*
 * Print out the HTML file headings, plus the actual content main heading.
 */
void HTMLHeader(const char *displayTime);


/*
 * Print out the HTML file trailers to close down the file.
 */
void HTMLFooter(const char *displayTime, const char *programName);

/*
 * Print out the details of a single database backup list.
 */
void HTMLDatabaseHeading(const char *database, const char *days);

/*
 * Print out the details of a single database backup list.
 */
void HTMLDatabase(OCI_Resultset* rs);

/*
 * Save the details of a single database backup.
 */
void HTMLSummary(OCI_Resultset* rs);

/*
 * Function to split something like 'username/password@alias' into
 * username, password and alias.
 */
boolean splitConnectionString(const char*connectionString, char **username, char **password, char **alias, char **daysAgo, boolean *sysDBA);

/*
 * Function to split something like 'username/password@alias' into
 * username, password and alias.
 */
boolean checkDatabase(const char *userName, const char *passWord, const char *databaseName, const char *daysAgo, boolean sysDBA);

/*
 * A function to safely append two strings together.
 * In the event that the destination is not big enough, it will be truncated.
 * This is used for the summaryBuffer whihc is limited to SUMMARY_BUFFER_SIZE characters.
 */
boolean appendSummary(const char *source);


#endif
