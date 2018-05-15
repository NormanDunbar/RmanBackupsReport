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
export TZ="Europe/London"
export LANG=en_GB.utf8

savedLibraryPath=${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${ORACLE_HOME}/lib:/home/oracle/norman/ocilib/lib
#--------------------------------------------------------------------
exeBase=/home/oracle/norman/rmanBackups
reportBase=/var/www/html/reports/oracle/backups
#--------------------------------------------------------------------
daysAgo=1
pw=*******
#--------------------------------------------------------------------

for db in misa01p pnet01p ukmhprddb modd01p
do
   reportFile=${reportBase}/${db}/index.html
   echo ${exeBase}/rmanBackupsReport ${db} ${pw} ${daysAgo} 1>${reportFile}
   ${exeBase}/rmanBackupsReport ${db} ${pw} ${daysAgo} 1>${reportFile}
done

export LD_LIBRARY_PATH=${savedLibraryPath}
