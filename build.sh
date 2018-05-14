#!/bin/bash
#--------------------------------------------------------------------
# A script to build (or rebuild) the rmanBackups application
# which will connect to a list of databases (who have an alias
# in tnsnames.ora) and retrieve the status of the RMAN backups
# for the last 'n' days.
#
# Norman Dunbar
# 14/05/2018
#--------------------------------------------------------------------
gcc=gcc
srcFile=rmanBackups.c
exeFile=rmanBackupsReport
includes=/home/oracle/norman/ocilib/include
libLocation=/home/oracle/norman/ocilib/lib
libName=ocilib
options="-DOCI_IMPORT_LINKAGE -DOCI_CHARSET_ANSI"
#--------------------------------------------------------------------
# gcc -o rmanBackups rmanBackups.c -I ~/norman/ocilib/include -std=c99 -l ocilib -L ~/norman/ocilib/lib
$gcc -o $exeFile $srcFile -I $includes -L $libLocation -std=c99 -l $libName $options

[oracle@ip-10-128-3-242 SourceCode]$ ls
build.sh  html.h         rmanBackups.h      rmanBackupsReport.sh  version.h
css.h     rmanBackups.c  rmanBackupsReport  sql.h
[oracle@ip-10-128-3-242 SourceCode]$ cat build.sh
#!/bin/bash
#--------------------------------------------------------------------
# A script to build (or rebuild) the rmanBackups application
# which will connect to a list of databases (who have an alias
# in tnsnames.ora) and retrieve the status of the RMAN backups
# for the last 'n' days.
#
# Norman Dunbar
# 14/05/2018
#--------------------------------------------------------------------
gcc=gcc
srcFile=rmanBackups.c
exeFile=rmanBackupsReport
includes=/home/oracle/norman/ocilib/include
libLocation=/home/oracle/norman/ocilib/lib
libName=ocilib
options="-DOCI_IMPORT_LINKAGE -DOCI_CHARSET_ANSI"
#--------------------------------------------------------------------
# gcc -o rmanBackups rmanBackups.c -I ~/norman/ocilib/include -std=c99 -l ocilib -L ~/norman/ocilib/lib
$gcc -o $exeFile $srcFile -I $includes -L $libLocation -std=c99 -l $libName $options


