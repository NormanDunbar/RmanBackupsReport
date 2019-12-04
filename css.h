#ifndef __CSS_H__
#define __CSS_H__

/*
 * This file defines the CSS styles that will be used for the HTML output.
 * Be careful about how you change it!
 */
char *css = "<style>\n\n"
            "body {\n"
                "background: ivory;\n"
                "color: maroon;\n"
                "font-size: smaller;\n"
            "}\n\n"
            "table, th, td {\n"
                "border: 1px solid bisque;\n"
                "vertical-align: top;\n"
                "width:850px;\n"
            "}\n\n"
            "table {\n"
                "border-collapse: collapse;\n"
                "background: beige;\n"
                "/*font-family: \"courier new\",\"lucida console\",mono;*/\n"
                "font-size: smaller;\n"
                "table-layout: fixed;\n"
                "margin-left: 2.5%;\n"
            "}\n\n"
            "h2 {\n"
                "text-transform: uppercase;\n"
            "}\n\n"
            "th {\n"
                "background: burlywood;\n"
                "color: maroon;\n"
            "}\n\n"
            ".number {\n"
                "text-align: right;\n"
                "padding-right: 3px;\n"
            "}\n\n"
            ".text {\n"
                "text-align: left;\n"
                "padding-left: 3px;\n"
            "}\n\n"
            ".red {\n"
                "font-weight: bold;\n"
                "color: red;\n"
            "}\n\n"
            ".amber {\n"
                "font-weight: bold;\n"
                "color: orange;\n"
            "}\n\n"
            ".green {\n"
                "font-weight: bold;\n"
                "color: green;\n"
            "}\n\n"
            ".narrow {\n"
                "width:7%;\n"
            "}\n\n"
            ".wide {\n"
                "width:40%;\n"
            "}\n\n"
            ".date {\n"
                "width:8%;\n"
            "}\n\n"
            ".status {\n"
                "width:12%;\n"
            "}\n\n"
            ".database {\n"
                "width:20%;\n"
            "}\n\n"
            ".bold {\n"
                "font-weight: bold;\n"
            "}\n\n"
            ".footer {\n"
                "font-size: x-small;\n"
                "text-align: center;\n"
            "}\n\n"
            "</style>\n\n";
#endif
