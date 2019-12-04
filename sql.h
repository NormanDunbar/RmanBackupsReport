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
    "select  jobs.session_key as job_id,\n"
    "        to_char(jobs.start_time, 'dd/mm/yyyy') || '<br>' || to_char(jobs.start_time, 'hh24:mi') as start_time,\n"
    "        to_char(jobs.end_time, 'dd/mm/yyyy') || '<br>' || to_char(jobs.end_time, 'hh24:mi') as end_time,\n"
    "        jobs.time_taken_display as run_time,\n"
    "        trim(jobs.input_bytes_display) as input_size,\n"
    "        trim(jobs.output_bytes_display) written_size,\n"
    "        jobs.status,\n"
    "        taskList.sub_jobs as tasks_executed_in_job\n"
    "from    V$RMAN_BACKUP_JOB_DETAILS jobs\n"
    "join    (\n"
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
    ") taskList\n"
    "    on  (jobs.session_key = taskList.session_recid)\n"
    "where   jobs.start_time >= trunc(sysdate) - %s\n"
    "order   by jobs.start_time desc";

char *SQLSummary =
    "select  upper('%s') as database,\n"
    "        jobs.session_key as job_id,\n"
    "        stats.operation || ' ' || stats.object_type as rman_command,\n"
    "        jobs.status,\n"
    "        to_char(jobs.start_time, 'dd/mm/yyyy') || '<br>' || to_char(jobs.start_time, 'hh24:mi') as start_time,\n"
    "        to_char(jobs.end_time, 'dd/mm/yyyy') || '<br>' || to_char(jobs.end_time, 'hh24:mi') as end_time,\n"
    "        jobs.time_taken_display as run_time,\n"
    "        trim(jobs.input_bytes_display) as input_size,\n"
    "        trim(jobs.output_bytes_display) written_size\n"
    "from    V$RMAN_BACKUP_JOB_DETAILS jobs\n"
    "join    V$RMAN_STATUS stats\n"
    "        on (jobs.session_key = stats.session_recid)\n"
    "where   jobs.start_time >= trunc(sysdate) - %s\n"
    "and     stats.operation = 'BACKUP'\n"
    "and     stats.object_type like 'DB%%'   -- This has to be two percent signs in C++ \n"
    "order   by jobs.start_time";



#endif
