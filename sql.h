#ifndef __SQL_H__
#define __SQL_H__

/*
 * This is the main SQL query that we are executing to retrieve
 * details of the list of backups that have been submitted since
 * yesterday.
 *
 * PS. The format is messy in a C program! :-(
 * PPS. There is some HTML code embedded below, a couple of '<br>' are to be found.
 * PPPS. There are two string format specifiers in there too. They look like '%s'.
 */

char *SQLTemplate =
    "------------------------------------------------------------------------------------------\n"
    "-- Get a list of sub-tasks, carried out by a session, within RMAN, in the last 24 hours.\n"
    "-- Fixes issue #3 - DB backups require highlighting.\n"
    "------------------------------------------------------------------------------------------\n"
    "with taskList as (\n"
    "    SELECT  tasks.session_recid, \n"
    "    LISTAGG(\n"
    "            case when tasks.object_type like 'DB%%' and tasks.operation = 'BACKUP' then \n"
    "                '<span class=\"green\"><strong>' || tasks.operation || ' ' || tasks.object_type || '</strong></span>'\n"
    "            else\n"
    "                tasks.operation || ' ' || tasks.object_type\n"
    "            end, '<br>')\n"
    "                WITHIN GROUP (ORDER BY tasks.start_time) AS sub_jobs\n"
    "    FROM    V$RMAN_STATUS tasks\n"
    "    where   parent_recid is not null\n"
    "    and     start_time >= trunc(sysdate) - %s\n"
    "    GROUP   BY session_recid\n"
    ")\n"
    "------------------------------------------------------------------------------------------\n"
    "-- Get a list of RMAN jobs that started in the last 24 hours.\n"
    "------------------------------------------------------------------------------------------\n"
    "select  jobs.session_key as job_id,\n"
    "        to_char(jobs.start_time, 'dd/mm/yyyy') || '<br>' || to_char(jobs.start_time, 'hh24:mi') as start_time,\n"
    "        to_char(jobs.end_time, 'dd/mm/yyyy') || '<br>' || to_char(jobs.end_time, 'hh24:mi') as end_time,\n"
    "        jobs.time_taken_display as run_time,\n"
    "        trim(jobs.input_bytes_display) as input_size,\n"
    "        trim(jobs.output_bytes_display) written_size,\n"
    "        jobs.status,\n"
    "        taskList.sub_jobs as tasks_executed_in_job\n"
    "from    V$RMAN_BACKUP_JOB_DETAILS jobs\n"
    "join    taskList\n"
    "    on  (jobs.session_key = taskList.session_recid)\n"
    "where   jobs.start_time >= trunc(sysdate) - %s \n"
    "order   by jobs.start_time desc";

/*
   It would be nice if the following worked. It gets the data from the database
   ok, but when rendering the HTML report, it converts the data to NULL in the
   final sub-tasks column which messes up the report. It could be because the
   data are a CLOB and GetString() doesn't cope very well (unlikely) or it's a
   bug in the code here somewhere.

   I'LL HAVE ANOTHER LOOK AT SOME POINT IN THE FUTURE.

char *SQLTemplate =
    "--=========================================================================\n"
    "-- A script to extract the hierarchical details of an RMAN top-level backup\n"
    "-- job, and, all the sub-tasks within it.\n"
    "--\n"
    "-- Norman Dunbar\n"
    "--=========================================================================\n"
    "---------------------------------------------------------------------------\n"
    "-- Get a list of sub-tasks, carried out by a session, within RMAN, in the\n"
    "-- last 'n' days. Because there's a limit of 4,000 characters in LISTAGG \n"
    "-- we have to use XMLAGG here which is limited to about 178TB!\n"
    "---------------------------------------------------------------------------\n"
    "with taskList as (\n"
    "    SELECT tasks.session_recid,\n"
    "        XMLELEMENT(\"tasks\",\n"
    "            XMLAGG(\n"
    "                XMLELEMENT(\n"
    "                    \"task\", trim(\n"
    "                                   case tasks.operation\n"
    "                                       when 'BACKUP' then '<strong>BACKUP</strong>'\n"
    "                                       else tasks.operation\n"
    "                                   end || ' ' ||\n"
    "                        case tasks.object_type\n"
    "                            when 'DB FULL' then '<strong>DB FULL</strong>'\n"
    "                            when 'DB INCR' then '<strong>DB INCR</strong>'\n"
    "                        else tasks.object_type\n"
    "                        end)\n"
    "                )\n"
    "            )\n"
    "        )\n"
    "    AS sub_jobs\n"
    "    FROM    V$RMAN_STATUS tasks\n"
    "    where   tasks.parent_recid is not null\n"
    "    and     tasks.start_time >= trunc(sysdate) - %s \n"
    "    GROUP   BY tasks.session_recid)\n"
    "---------------------------------------------------------------------------\n"
    "-- Get a list of RMAN jobs that started in the last 'n' days\n"
    "---------------------------------------------------------------------------\n"
    "select  jobs.session_key as job_id, \n"
    "        --jobs.input_type, \n"
    "        to_char(jobs.start_time, 'dd/mm/yyyy hh24:mi') as start_time, \n"
    "        to_char(jobs.end_time, 'dd/mm/yyyy hh24:mi') as end_time,\n"
    "        jobs.time_taken_display as run_time, \n"
    "        trim(jobs.input_bytes_display) as input_size, \n"
    "        trim(jobs.output_bytes_display) written_size,\n"
    "        jobs.status, \n"
    "        -- taskList.sub_jobs as tasks_executed_in_job\n"
    "        XMLSerialize(\n"
    "            DOCUMENT\n"
    "                taskList.sub_jobs.transform(\n"
    "'<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<xsl:stylesheet version=\"1.0\"\n"
    "xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\n"
    "\n"
    "<!-- Match the document itself -->\n"
    "<xsl:template match=\"/\">\n"
    "   <xsl:apply-templates/>\n"
    "</xsl:template>\n"
    "\n"
    "<!-- Match the root node, <tasks> -->\n"
    "<!-- This has to be wrapped in <td> tags or we\n"
    "     get a VARCHAR2 result, not a CLOB. -->\n"
    "<xsl:template match=\"tasks\">\n"
    "  <td>\n"
    "    <xsl:apply-templates/>\n"
    "  </td>\n"
    "</xsl:template>\n"
    "\n"
    "\n"
    "<!-- Match a single <task> node -->\n"
    "<xsl:template match=\"task\">\n"
    "  <xsl:apply-templates select=\"text()\"/>\n"
    "  <br />\n"
    "</xsl:template>\n"
    "\n"
    "</xsl:stylesheet>'\n"
    "                )\n"
    "        )\n"
    "from    V$RMAN_BACKUP_JOB_DETAILS jobs\n"
    "join    taskList\n"
    "    on  (jobs.session_key = taskList.session_recid)\n"
    "where   jobs.start_time >= trunc(sysdate) - %s \n"
    "order   by jobs.start_time\n";
*/

#endif
