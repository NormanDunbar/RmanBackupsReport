rem ---------------------------------------------------------- 
rem Build the docs from asciidoc to PDF. You may desire to use
rem a different theme. Please do so.
rem ---------------------------------------------------------- 
rem Assumes that you have asciidoctor and asciidoctor-pdf
rem installed. If not, just read the *.adoc files.
rem ---------------------------------------------------------- 
asciidoctor-pdf  -a pdf-style=default README.adoc
asciidoctor-pdf  -a pdf-style=default UserGuide.adoc
asciidoctor-pdf  -a pdf-style=default SourceCode.adoc
dir *.pdf
