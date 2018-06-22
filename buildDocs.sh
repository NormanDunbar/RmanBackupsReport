# ---------------------------------------------------------- 
# Build the docs from asciidoc to PDF. You may desire to use
# a different theme. Please do so.
# ---------------------------------------------------------- 
# Assumes that you have asciidoctor and asciidoctor-pdf
# installed. If not, just read the *.adoc files.
# ---------------------------------------------------------- 
asciidoctor-pdf  -a pdf-style=default README.adoc
asciidoctor-pdf  -a pdf-style=default UserGuide.adoc
asciidoctor-pdf  -a pdf-style=default SourceCode.adoc
ls *.pdf
