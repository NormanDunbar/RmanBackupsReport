@echo off
rem ===========================================================================
rem This file will compile a source file named 'rmanBackups.c', in the current
rem folder (ie, where this file lives too) and create an executable named
rem 'rmanBackups.exe'.
rem
rem Norman Dunbar
rem 01/05/2018.
rem ===========================================================================
rem WARNING:
rem While there is the option to build this application as 32 and 64 bits, you
rem cannot build it in a particular bit size if your current Oracle Client
rem is not the same bit size. So, if yo utry, you will get lots of errors
rem telling you about 'undefined reference to OCI_whatever'. 
rem You have been warned. ;o)
rem ===========================================================================


rem ===========================================================================
rem SETINGS
rem ===========================================================================
rem mingw64 = location of the 64 bit mingw compiler's bin folder.
rem ---------------------------------------------------------------------------
rem mingw32 = location of the 32 bit mingw compiler's bin folder. Which may be 
rem           the same. If it is, change ccopts to include -m32 as well.
rem ---------------------------------------------------------------------------
rem gcc = the compiler executable filename - within mingw32/mingw64.
rem ---------------------------------------------------------------------------
rem ccopts = compiler options. -m32, for example, if compiling 32 bit code with
rem          the same compiler executable as the 64 bit compiler.
rem ---------------------------------------------------------------------------
rem base = the ocilib installation directory.
rem ---------------------------------------------------------------------------
rem libs = where the 64 or 32 bit library file 'libociliba.a' can be found.
rem ---------------------------------------------------------------------------
rem includes = where the ocilib header files (ocilib.h etc) are located.
rem ---------------------------------------------------------------------------
rem compileOptions = DO NOT CHANGE UNLESS YOU KNOW EXACTLY WHAT YOU ARE DOING!
rem                  This option tells ocilib how to link to Oracle oci.dll - at 
rem                  runtime by default. It also specifies that we are using 
rem                  ANSI strings, not UNICODE, which doesn't work on Windows.
rem ===========================================================================


rem ---------------------------------------------------------------------------
rem We assume Assume 64bit builds and change to 32 bit if the developer adds
rem '32' to the end of the command line when calling  this code to do 
rem compilations - eg 'build 32' instead of 'build'.
rem ---------------------------------------------------------------------------

rem ---------------------------------------------------------------------------
rem Compiler's bin folder location.
rem ---------------------------------------------------------------------------
set compiler_bin_folder=e:\TDM-GCC-64\bin


rem ---------------------------------------------------------------------------
rem Compiler name, within its 'bin' folder, and options.
rem ---------------------------------------------------------------------------
set gcc=%compiler_bin_folder%\gcc
set ccopts=-m64
set bits=64

rem ---------------------------------------------------------------------------
rem Where the OCILIB system lives.
rem ---------------------------------------------------------------------------
set base=E:\SourceCode\ocilib\ocilib-4.5.1-windows

rem ---------------------------------------------------------------------------
rem Assume 64bit libraries are required.
rem ---------------------------------------------------------------------------
set libs=%base%\lib64

rem ---------------------------------------------------------------------------
rem Where the OCILIB Include files are found.
rem ---------------------------------------------------------------------------
set includes=%base%\include

rem ---------------------------------------------------------------------------
rem Compiler options for OCILIB Applications. Don't change these please!
rem ---------------------------------------------------------------------------
set compileOptions="-DOCI_IMPORT_LINKAGE -DOCI_CHARSET_ANSI"

rem ---------------------------------------------------------------------------
rem Change everything if "32" supplied on command line. Which
rem unfortunately will not work if we don't have a 32bit
rem Oracle Client installed.
rem
rem The 64bit compiler can produce 32 bit applications with -m32 specified.
rem ---------------------------------------------------------------------------
if "%1" equ "32" (
    set mingw32=%compiler_bin_folder%
    set ccopts=-m32
    set libs=%base%\lib32
    set bits=32
)

rem ---------------------------------------------------------------------------
rem Do a compile. We need the libraries after the filenames or we get linker 
rem errors.
rem ---------------------------------------------------------------------------
@echo on
%gcc% %ccopts% -o rmanBackups%bits%.exe rmanBackups.c -L %libs% -I %includes%  -l ociliba %compileOptions% 

