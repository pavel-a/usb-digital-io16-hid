:@echo off
: Make release, Windows binaries
: Run this after binaries are built

: Release tree:
:   bin-Win32/
:   bin-Win64/
:   doc/
:   Test/
:   usb_digital_io16_hid_devel/
:     Win32, Win64 ...
:   bin-Linux-x86/
:   bin-Linux-x64/
:   bin-OSX/

set REL=_rel
if not exist _rel mkdir _rel
if not exist %REL%\bin-Win32 mkdir %REL%\bin-Win32
if not exist %REL%\bin-Win64 mkdir %REL%\bin-Win64

@echo Copying x86 binaries...
copy Windows-VS\Release\usb_io_interface.dll %REL%\bin-Win32
if errorlevel 1 goto :ERR
copy Windows-VS\Release\test_gui_app.exe %REL%\bin-Win32
if errorlevel 1 goto :ERR

if not exist Windows-VS\x64\Release\* goto L64_1
@echo Copying x64 binaries...
copy Windows-VS\x64\Release\usb_io_interface.dll %REL%\bin-Win64
:- if errorlevel 1 goto :ERR
copy Windows-VS\x64\Release\test_gui_app.exe %REL%\bin-Win64
:- if errorlevel 1 goto :ERR
:L64_1

@echo Copying docum ...
if not exist %REL%\doc mkdir %REL%\doc
copy doc\usb_hid_digital_io_16.md  %REL%\doc
if errorlevel 1 goto :ERR
copy doc\Readme_usb_io_interface_lib.md %REL%\doc
if errorlevel 1 goto :ERR

@echo Copying developer stuff ...
if not exist %REL%\usb_digital_io16_hid_devel mkdir %REL%\usb_digital_io16_hid_devel
mkdir %REL%\usb_digital_io16_hid_devel\Win32
mkdir %REL%\usb_digital_io16_hid_devel\Win64
copy lib\usb_io_device.h  %REL%\usb_digital_io16_hid_devel
if errorlevel 1 goto :ERR
copy Windows-VS\Release\usb_io_interface.lib  %REL%\usb_digital_io16_hid_devel\Win32
copy Windows-VS\x64\Release\usb_io_interface.lib  %REL%\usb_digital_io16_hid_devel\Win64

: ?? Copy also PDBs, debug binaries?

: Tests
if not exist %REL%\test mkdir %REL%\test
copy python\test_dio1.py  %REL%\test
if errorlevel 1 goto :ERR
copy Windows-VS\Powershell\test_usb_hid_io16_led_blink.ps1 %REL%\test

@echo Done.
