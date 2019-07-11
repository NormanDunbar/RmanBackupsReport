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
# 22/06/2018 Needs $ORACLE_HOME/lib on LD_LIBRARY_PATH to compile.
#--------------------------------------------------------------------
gcc=gcc
srcFile=rmanBackups.c
exeFile=rmanBackupsReport
includes=/home/oracle/norman/ocilib/include
libLocation=/home/oracle/norman/ocilib/lib
libName=ocilib
options="-DOCI_IMPORT_LINKAGE -DOCI_CHARSET_ANSI"
LIBPATH=$ORACLE_HOME/lib:$libLocation
#--------------------------------------------------------------------

LD_LIBRARY_PATH=$LIBPATH
$gcc -o $exeFile $srcFile -I $includes -L $libLocation -std=c99 -l $libName $options
