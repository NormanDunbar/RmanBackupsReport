#ifndef __HTML_H__
#define __HTML_H__

char *htmlHeader =
    "<html>\n"
    "<head>\n"
    "<title>RMAN Backup Report</title>\n"
    "<meta name=\"description\" content=\"RMAN Backups Report\">\n"
    "<meta name=\"author\" content=\"Norman Dunbar: norman.dunbar@hermes-europe.co.uk\">\n"
    "<meta name=\"author\" content=\"Norman Dunbar: norman@dunbar-it.co.uk\">\n"
    "%s"
    "</head>\n"
    "<body>\n"
    "<h1>RMAN Backup Report</h1>\n"
    "<p class=\"bold\">Report executed on: %s.</p>\n";

char *htmlFooter =
    "<hr>\n"
    "<p class=\"footer\">Report created on %s</p>"
    "<p class=\"footer\">by</p>"
    "<p class=\"footer\">%s</p>"
    "</body>\n"
    "</html>\n";

char *dbHeading =
    "\n<h2>%s</h2>\n";

char *dbParagraph =
    "<p>The following RMAN backup jobs have been executed in the last %s day%s:</p>\n";

char *tableHeadings =
    "<table>\n"
    "<tr>"
    "<th class=\"narrow\">JOB<br>ID</th>"
    "<th class=\"date\">START<br>TIME</th>"
    "<th class=\"date\">END<br>TIME</th>"
    "<th class=\"narrow\">RUN<br>TIME</th>"
    "<th class=\"narrow\">READ<br>SIZE</th>"
    "<th class=\"narrow\">WRITE<br>SIZE</th>"
    "<th class=\"status\">STATUS</th>"
    "<th class=\"wide\">TASKS EXECUTED IN JOB</th>"
    "</tr>\n";

char *tableRow =
    "<tr>"
    "<td class=\"number\">%i</td>"
    "<td>%s</td>"
    "<td>%s</td>"
    "<td>%s</td>"
    "<td class=\"number\">%s</td>"
    "<td class=\"number\">%s</td>"
    "<td class=\"%s\">%s</td>"
    "<td>%s</td>"
    "</tr>\n";

#endif
