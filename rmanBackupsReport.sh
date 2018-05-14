#!/bin/bash
#--------------------------------------------------------------------
# A script to be called to generate an HTML report consisting of all
# the backups over the last "n" days, for each database listed. The
# database names must be as per tnsnames.ora and this may not be the
# same as the actual database name.
#
# The commandline call has the following parameters.
#
# 1. database list. As many as you like, separated by spaces. Double
#    quotes are permitted around "interestingly" named aliases.
#
# 2. dba_user password. Once only. All databases are, so far, found
#    to be using the same password.
#
# 3. number of days - how many days ago do you want to see the RMAN
#    backup reports for? 1 means everything from the start of
#    "yesterday" and includes anything taken "up to now".
#
# The report is in HTML format, and will be written to the screen, so
# you will need to redirect the standard output to a file, as follows:
#
# ./rmanBackupsReport .... 1>rmanBackupsReport.html
#--------------------------------------------------------------------
savedLibraryPath=${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/home/oracle/norman/ocilib/lib
#--------------------------------------------------------------------
base=/home/oracle/norman/rmanBackups
logFile=${base}/rmanBackupsReports.log
reportFile=${base}/rmanBackupsReports.html
#--------------------------------------------------------------------
${base}/rmanBackupsReport misa01p pnet01p ukmhprddb modd01p bilbo4327 1 1>${reportFile} 2>${logFile}

#
# Mail the report to the huk dba team.
#
# WARNING: Not Working. Yet.
echo "Please find attached the RMAN Backups Report." | mail -s "RMAN Backups Report" -a ${reportFile} huk.dba@hermes-europe.co.uk

export LD_LIBRARY_PATH=${savedLibraryPath}

