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
void HTMLHeader(char *displayTime);


/*
 * Print out the HTML file trailers to close down the file.
 */
void HTMLFooter(char *displayTime, char *programName);

/*
 * Print out the details of a single database backup list.
 */
void HTMLDatabase(char *database, OCI_Resultset* rs, char *days);


#endif
