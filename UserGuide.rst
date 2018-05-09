===================
RMAN Backup Reports
===================

Introduction
============

See the file ``SourceCode.docx`` if you want to make changes. This file explains how to use the ``rmanBackups`` utility.

``RMANBackups.exe`` is a utility that will login to a list of databases as the ``DBA_USER`` account, and create a report detailing all the RMAN jobs that were executed in the past 'n' days, where 'n' is a supplied parameter on the command line. Normally, you would be interested in backups from yesterday, or 1 day ago.

The information retrieved form the database(s) is obtained from the views ``V$RMAN_BACKUP_JOB_DETAILS`` and ``V$RMAN_STATUS`` which are, effectively, interrogating the data from the control file for the database.

Any database backup job that did not finish with a status of 'COMPLETED' will be flagged in bold, red text - to attract your attention.

The OEM Repository database does not able store any backup data other than for the most recent one that was run - so a backup consisting of the database, archived logs and the control file auto-backup would only show details of the control file auto-backup and not the other two. (From view MGMT$HA_BACKUP.)

Copying
=======

You may copy the utility from the DBA software Repository (*P:\Parcelnet System Development\dba\software_repository\RMANBackupReports\Latest*)to a safe place on your own PC, if necessary.

Only the ``rmanBackups.exe`` file is required.


Execution
=========

Setting Your Path
-----------------

Before executing the utility, you need to have set your path to make sure that the Oracle client's ``bin`` folder is listed. Within this folder, there will be a file named ``oci.dll`` and this is required for the operation of the utility. Different versions of Oracle client have different versions of this dll, but the utility should/will work with anything from 11g onwards. (I am reliably informed!)

If you do not have the oci.dll in your path somewhere, you will see the following errors at runtime:

..  code-block:: none

    Cannot load OCI shared library (oci.dll)Error ORA-00000 detected
    Cannot initialise the Oracle Environment.

Parameters
----------

The utility takes a variable number of parameters:

*   A list of database alias names, which must be specified in your tnsnames.ora, separated by spaces.
*   The password for the DBA_USER account. This may be wrapped in double quotes if it uses any "weird" characters.
*   The number of days you are interested in.

For example, to examine the backups for MISA01P1 and PNET01P1, over the last 3 days, the following would be used:

..  code-block:: none

    rmanBackups misa01p1 pnet01p1 ToPs3cr3T 3 >rmanBackups.html

That's not the real password for DBA_USER by the way! 

If you don't have an alias in your ``tnsnames.ora`` for a particular database, then the normal tns timeout period will eventually pass, and the utility will continue to the next database in the list. 

Output File
-----------

It is advised to redirect the output to an html file, as shown in the example above, as the program produces HTML output only which is not easy to read on the screen. By redirecting to a file, you can then open the file in a browser (or you could use Internet Explorer!) for easy viewing. 

The HTML text for the report is written to 'standard output' while informational and error messages are written to 'standard error'. Both can be redirected as follows:

..  code-block:: none

    rmanBackups misa01p1 pnet01p1 T0PS3cr3t 3 >rmanBackups.html 2>rmanBackups.error.log

In this case, nothing will be written to the screen, HTML output will be written to ``rmanBackups.html`` and messages and errors will go to ``rmanBackups.error.log``.

Report Format
-------------

The report includes the following details:

*   The job id - created by Oracle for the main session that initiated the RMAN command(s).
*   The start time - the time that the first RMAN command began.
*   The end time - the time that the last RMAN command finished.
*   The run time of the full job, in hh:mm:ss format.
*   The input size - the full size of the input files for the entire job.
*   The written size - the size written by RMAN to the output file(s). Normally smaller than the input size due to compression etc.
*   The job's final status - 'COMPLETED' is good. Other values may not be so good and should be brought to the attention of the German DBAs who are responsible for the backup jobs.
*   The tasks executed by the job. This is a list of the various steps that RMAN execute within the job. 

An example report can be found in the software repository, it is named ``rmanBackups.html``.

..  image:: Example.png
    :scale: 60 %
    :alt: Example RMAN Backup Report.
