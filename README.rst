RMANBackupsReport
=================

This is a test application to use the OCILIB Oracle database access tools from `<https://github.com/vrogier/ocilib>`_ on Windows and Linux. It seems to work.

The application itself requires that OCILIB's lib directory and the ORACLE_HOME/lib directory have been installed on PATH and/or LD_LIBRARY_PATH.

The application logs in to a database (or some databases) and reports back on the list of RMAN backups that have taken place since ``some days ago`` - which it does as the DBA_USER account, you can edit this in ``rmanBackups.c`` if you need to change it.

Compiling
---------

*   Make sure that OCILIB is installed.
*   Edit ``build.cmd`` (Windows) or ``build.sh`` (Linux) and change the ``includes`` and ``libLocation`` settings to suit where you have installed OCILIB. On Linux, make sure ``LIBPATH`` is correct for your ``$ORACLE_HOME`` - we need that in ``LD_LIBRARY_PATH`` at compile time.
*   Save the file(s) and execute them as per your system.

Execution
---------

Once you have a compiled version of the software, probably called ``rmanBackupsReport`` or ``rmanBackupsReport.exe``, you can run it with the folloiwng command line parameters:

*   a list of space separated database alias names as per your client's ``tnsnames.ora`` file. Double quotes can be used around any of the alias names if there are *interesting* characters in the name, etc.

*   The password for a used, defaulting to ``dba_user`` (yes, I *know*, I should have passed it on the command line, but that's not how *my* system is set up, and I'm lazy, ok!) - but you can change the default account in the file ``rmanBackups.c`` as mentioned above.
    
    It is assumed, because my systems does it, that all the databases in the list, will use the same account and password. You may need to run the report separately if this is diofferent at your installation. It's only a test application after all!

*   The number of days ago you wish to look back at the RMAN backups for. 1, for example, will give you everything from yesterday plus anything from today, up until now. (In other words, it's not just the backups take on those numbers of days, but always includes anything up to now.)

For example:

..  code-block:: bash

    ./rmanBackupsReport db_one db_two db_three secreat_password_string 3

Output Report Files
-------------------

The application produces n HTML report on stdout, so you should redirect that to a file if you want to capture the report data. For example:

..  code-block:: bash

    ./rmanBackupsReport db_one db_two db_three secreat_password_string 3 >rmanReport.html

You also can, if you wsih, redirect stderr to a lgfile so that any errors are logged and not just displayed on screen.

..  code-block:: bash

    ./rmanBackupsReport db_one db_two db_three secreat_password_string 3 >rmanReport.html 2>rmanReportErrors.log

Easy or what?

Norman Dunbar
May 2018.






