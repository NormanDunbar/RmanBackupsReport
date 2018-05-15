@echo off

rem ---------------------------------------------------------------------------
rem This script is executed at 08:10 daily by the Windows Task Scheduler.
rem It produces a report on the RMAN backups for production databases 
rem since 'yesterday'. PRIMARY servers only.
rem ---------------------------------------------------------------------------
rem Norman Dunbar
rem 03/05/2018.
rem ---------------------------------------------------------------------------

echo Running ....
set base=C:\Users\hisg494\utilities
set logfile=%base%\rmanBackupsReport.log
set reportfile=%base%\rmanBackupsReport.html
set pw=*****


%base%\rmanBackups.exe misa01p1 pnet01p1 myhermes modd01p %pw% 1 >%reportfile% 2>%logfile%
exit %ERRORLEVEL%