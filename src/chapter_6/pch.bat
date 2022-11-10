@echo off
@SET PCH_INPUT_SOURCE_NAME_DEBUG=pch_d.cpp
@SET PCH_INPUT_SOURCE_NAME_RELEASE=pch.cpp
@SET PCH_INPUT_HEADER_NAME=pch.h
@SET PCH_OUTPUT_NAME_DEBUG=pch_d.pch
@SET PCH_OUTPUT_NAME_RELEASE=pch.pch
@SET APP_ARCH=x64
@SET SCRIPT_DIR=%cd%

::------------------------------
::
:: Compilation
:: Requires Visual Studio 2022
::
::------------------------------
where cl >nul 2>nul
IF %ERRORLEVEL% NEQ 0 call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" %APP_ARCH%
IF %ERRORLEVEL% NEQ 0 GOTO :VS_NOT_FOUND

::
:: Store msvc clutter elsewhere
::-----------------------------
mkdir msvc_landfill >nul 2>nul
pushd msvc_landfill >nul

::
:: Compile Options
::------------------------------
:: /TC      Compile as C code.
:: /TP      Compile as C++ code.
:: /Oi      Enable intrinsic functions.
:: /Od 	    Disables optimization.
:: /Qpar    Enable parallel code generation.
:: /Ot      Favor fast code (over small code).
:: /Ob2     Enable full inline expansion. Note: Debugging impact.
:: /Z7	    Full symbolic debug info. No pdb. (See /Zi, /Zl).
:: /GS	    Detect buffer overruns.
:: /GA      Optimizes for Windows applications.
:: /MD*     Multi-thread specific, DLL-specific runtime lib. (See /MDd, /MT, /MTd, /LD, /LDd).
:: /GL	    Whole program optimization.
:: /EHsc    No exception handling (Unwind semantics requrie vstudio env). (See /W1).
:: /I<arg>  Specify include directory.

::
:: Source Files
::-------------
@SET SourceFilesDebug=%SCRIPT_DIR%\..\common\%PCH_INPUT_SOURCE_NAME_DEBUG%
@SET SourceFilesRelease=%SCRIPT_DIR%\..\common\%PCH_INPUT_SOURCE_NAME_RELEASE%

:: Include Parameters
@SET IncludeParameters=/I%cd%\.. ^
/I%SCRIPT_DIR%\.. ^
/I%SCRIPT_DIR%\..\.. ^
/I%SCRIPT_DIR%\..\..\include ^
/I%SCRIPT_DIR%\..\..\include\GL ^
/I%SCRIPT_DIR%\..\..\include\GLFW ^
/I%SCRIPT_DIR%\..\..\include\glm ^
/I%SCRIPT_DIR%\..\..\include\stb ^
/I%SCRIPT_DIR%\..\..\src\common ^
/I%SCRIPT_DIR%\..\..\src\common\platform

:: General Parameters
@SET GeneralParameters=/W4 /WX /GA /Oi /Qpar /EHsc /GL /nologo /Ot /TP /std:c++latest

:: Debug Paramters
@SET DebugParameters=/Od /Z7 /MTd /c /Yc%PCH_INPUT_HEADER_NAME% /Fp%PCH_OUTPUT_NAME_DEBUG% %SourceFilesDebug%

:: Release Parameters
@SET ReleaseParameters=/O2 /Ob2 /MT /c /Yc%PCH_INPUT_HEADER_NAME% /Fp%PCH_OUTPUT_NAME_RELEASE% %SourceFilesRelease%

::
:: Compiler Invocation
::--------------------
@SET "INVOKE_RELEASE=cl %GeneralParameters% %ReleaseParameters% %IncludeParameters%"
@SET "INVOKE_DEBUG=cl %GeneralParameters% %DebugParameters% %IncludeParameters%"

echo Building Precompiled Headers [ debug ]...
(%INVOKE_DEBUG%)
echo.
echo Building Precompiled Headers [ release ]...
(%INVOKE_RELEASE%)

xcopy /y %PCH_OUTPUT_NAME_DEBUG% ..\..\..\src\common >nul
xcopy /y %PCH_OUTPUT_NAME_RELEASE% ..\..\..\src\common >nul
popd >nul
echo Done.
echo.
GOTO :exit

:exit
