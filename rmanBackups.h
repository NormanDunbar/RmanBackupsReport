#ifndef __RMANBACKUPS_H__
#define __RMANBACKUPS_H__

#include <time.h>
#include "css.h"
#include "sql.h"
#include "version.h"
#include "html.h"

/*
 * An easy way to remember the columns in the query!
 */

 /*
#define COL_JOB_ID          1
#define COL_START_TIME      2
#define COL_END_TIME        3
#define COL_RUN_TIME        4
#define COL_INPUT_SIZE      5
#define COL_WRITTEN_SIZE    6
#define COL_STATUS          7
#define COL_TASKS           8
*/

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
 * Function to split something like 'username/password@alias' into
 * username, password and alias.
 */
boolean splitConnectionString(const char*connectionString, char **username, char **password, char **alias, char **daysAgo, boolean *sysDBA);

/*
 * Function to split something like 'username/password@alias' into
 * username, password and alias.
 */
boolean checkDatabase(const char *userName, const char *passWord, const char *databaseName, const char *daysAgo, boolean sysDBA);



#endif
