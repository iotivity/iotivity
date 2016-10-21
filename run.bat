@echo off
REM Helper script to build and run IoTivity on Windows
SETLOCAL ENABLEDELAYEDEXPANSION

if [%1]==[] goto USAGE

set IOTIVITY_DIR=%~dp0
set HOME=%USERPROFILE%

IF "%1" == "msys" (
  set BUILD_MSYS="YES"
  SHIFT
) ELSE (
  set BUILD_MSYS=
)

REM *** Default BUILD OPTIONS ***
set TARGET_OS=windows

if "%TARGET_ARCH%" == "" (
  set TARGET_ARCH=amd64
) 

if "%TEST%" == "" (
  set TEST=1
)

if "%LOGGING%" == "" (
  set LOGGING=0
)

if "%RELEASE%" == "" (
  set RELEASE=0
)

set SECURED=1
set WITH_RD=1
set ROUTING=EP
set WITH_UPSTREAM_LIBCOAP=1

set RUN_ARG=%1
SHIFT

set DEBUG=

:processArgs
IF NOT "%1"=="" (
  IF "%1"=="-arch" (
    SET TARGET_ARCH=%2
    SHIFT
  )
  IF "%1"=="-noTest" (
    SET TEST=0
  )
  IF "%1"=="-logging" (
    SET LOGGING=1
  )    
  IF "%1"=="-debugger" (
    set DEBUG="%ProgramFiles(x86)%\Windows Kits\10\Debuggers\x64\cdb.exe" -2 -c "g" 
  )
  IF "%1"=="-release" (
    SET RELEASE=1
  )

  SHIFT
  GOTO :processArgs
)

REM We need to append the "PATH" so the octbstack.dll can be found by executables
IF "%BUILD_MSYS%" == "" (
  set BUILD_DIR=out\windows\%TARGET_ARCH%\debug
  set PATH=!PATH!;!IOTIVITY_DIR!!BUILD_DIR!;
) ELSE (
  set BUILD_DIR=out\msys_nt\x86_64\debug
  set PATH=!PATH!;!BUILD_DIR!;C:\msys64\mingw64\bin
)

set BUILD_OPTIONS= TARGET_OS=%TARGET_OS% TARGET_ARCH=%TARGET_ARCH% RELEASE=%RELEASE% WITH_RA=0 TARGET_TRANSPORT=IP SECURED=%SECURED% WITH_TCP=0 BUILD_SAMPLE=ON LOGGING=%LOGGING% TEST=%TEST% WITH_RD=%WITH_RD% ROUTING=%ROUTING% WITH_UPSTREAM_LIBCOAP=%WITH_UPSTREAM_LIBCOAP%

REM Use MSVC_VERSION=12.0 for VS2013, or MSVC_VERSION=14.0 for VS2015.
REM If MSVC_VERSION has not been defined here, SCons chooses automatically a VS version.
IF NOT "%MSVC_VERSION%" == "" (
  set BUILD_OPTIONS=%BUILD_OPTIONS% MSVC_VERSION=%MSVC_VERSION%
)

REM *** BUILD OPTIONS ***

if "!RUN_ARG!"=="server" (
  cd %BUILD_DIR%\resource\examples
  %DEBUG% simpleserver.exe
) else if "!RUN_ARG!"=="client" (
  cd %BUILD_DIR%\resource\examples
  %DEBUG% simpleclient.exe
) else if "!RUN_ARG!"=="serverhq" (
  cd %BUILD_DIR%\resource\examples
  %DEBUG% simpleserverHQ.exe
) else if "!RUN_ARG!"=="clienthq" (
  cd %BUILD_DIR%\resource\examples
  %DEBUG% simpleclientHQ.exe
)else if "!RUN_ARG!"=="mediaclient" (
  cd %BUILD_DIR%\resource\examples
  %DEBUG% mediaclient.exe
) else if "!RUN_ARG!"=="mediaserver" (
  cd %BUILD_DIR%\resource\examples
  %DEBUG% mediaserver.exe
) else if "!RUN_ARG!"=="winuiclient" (
  cd %BUILD_DIR%\resource\examples
  %DEBUG% winuiclient.exe
) else if "!RUN_ARG!"=="occlient" (
  cd %BUILD_DIR%\resource\csdk\stack\samples\linux\SimpleClientServer
  %DEBUG% occlientbasicops.exe -u 0 -t 3 -c 1
) else if "!RUN_ARG!"=="ocserver" (
  cd %BUILD_DIR%\resource\csdk\stack\samples\linux\SimpleClientServer
  %DEBUG% ocserverbasicops.exe
) else if "!RUN_ARG!"=="provclient" (
  cd %BUILD_DIR%\resource\csdk\security\provisioning\sample
  %DEBUG% provisioningclient.exe
) else if "!RUN_ARG!"=="justworks" (
  cd %BUILD_DIR%\resource\csdk\security\provisioning\sample
  %DEBUG% sampleserver_justworks.exe
) else if "!RUN_ARG!"=="test" (
  %DEBUG% %BUILD_DIR%\resource\csdk\connectivity\test\catests.exe
  %DEBUG% %BUILD_DIR%\resource\csdk\stack\test\stacktests.exe
  %DEBUG% %BUILD_DIR%\resource\csdk\stack\test\cbortests.exe
  %DEBUG% %BUILD_DIR%\resource\csdk\security\unittest\unittest.exe
  %DEBUG% %BUILD_DIR%\resource\csdk\security\provisioning\unittest\unittest.exe
  %DEBUG% %BUILD_DIR%\resource\c_common\ocrandom\test\randomtests.exe
  %DEBUG% %BUILD_DIR%\resource\c_common\oic_malloc\test\malloctests.exe
  %DEBUG% %BUILD_DIR%\resource\c_common\oic_string\test\stringtests.exe
  %DEBUG% %BUILD_DIR%\resource\c_common\oic_time\test\timetests.exe
  %DEBUG% %BUILD_DIR%\resource\unittests\unittests.exe
) else if "!RUN_ARG!"=="build" (
  echo Starting IoTivity build with these options:
  echo   TARGET_OS=%TARGET_OS%
  echo   TARGET_ARCH=%TARGET_ARCH%
  echo   SECURED=%SECURED%
  echo   RELEASE=%RELEASE%
  echo   TEST=%TEST%
  echo   LOGGING=%LOGGING%
  echo   WITH_RD=%WITH_RD%
  echo   ROUTING=%ROUTING%
  echo   WITH_UPSTREAM_LIBCOAP=%WITH_UPSTREAM_LIBCOAP%
  echo.scons VERBOSE=1 %BUILD_OPTIONS%
  scons VERBOSE=1 %BUILD_OPTIONS%
) else if "!RUN_ARG!"=="clean" (
  scons VERBOSE=1 %BUILD_OPTIONS% -c clean
  rd /s /q out
  del .sconsign.dblite
  del extlibs\gtest\gtest*.lib
  del extlibs\gtest\gtest-1.7.0\src\gtest*.obj
  erase /s *.obj
  erase resource\c_common\iotivity_config.h
  erase extlibs\libcoap\coap.lib
  erase extlibs\libcoap\libcoap\include\coap\coap_config.h
) else if "!RUN_ARG!"=="cleangtest" (
  rd /s /q extlibs\gtest\gtest-1.7.0
  del extlibs\gtest\gtest-1.7.0.zip
) else (
    echo %0 - Script requires a valid argument!
    goto :EOF
)

cd %IOTIVITY_DIR%

echo Done!

goto EOF

:USAGE
echo %0 - Helper to build/test iotivity.  Requires an argument.
echo Installation: Drop this into your iotivity root directory to use it.
echo.
echo. Default buidl settings are: debug binaries run unittests and no logging
echo.
echo. Default build parameters can be overridden using the following arguments
echo. 
echo   -arch [x86 | amd64]    - Build either amd64 or x86 architecture binaries
echo.
echo   -noTest                - Don't run the unittests after building the binaries
echo.
echo   -logging               - Enable logging while building the binaries
echo.
echo   -debugger              - Debug the requested application
echo.
echo   -release               - Build release binaries
echo.
echo. Usage examples:
echo   Launch SimpleClient with debugger:
echo      %0 client -debugger
echo.
echo   Launch SimpleServer:
echo      %0 server
echo.
echo   Launch WinUIClient built in msys:
echo      %0 msys winuiclient
echo.
echo   Build amd64 debug binaries and run unit tests:
echo      %0 build
echo.
echo   Build amd64 release binaries w/o running tests:
echo      %0 build -arch amd64 -noTest -release
echo.
echo   Build x86 debug binaries and run unit tests:
echo      %0 build -arch x86
echo.
echo   Build amd64 release binaries with logging enabled:
echo      %0 build -arch amd64 -release -logging
echo.
echo   Run all tests:
echo      %0 test
echo.
echo   Clean:
echo      %0 clean

:EOF
