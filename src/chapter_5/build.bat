@echo off

::------------------------------
::
:: Relase / Debug
::
::------------------------------
@SET /A RELEASE_BUILD=0


::------------------------------
::
:: Environment Settings
:: Vulkan SDK Version, app name, architecture, shader directories
::
::------------------------------
@SET SCRIPT_DIR=%cd%
@SET APP_NAME=chapter_5
@SET APP_ARCH=x64
:: @SET SHADER_DIR=%SCRIPT_DIR%\shaders
:: @SET SHADER_SRC_DIR=%SHADER_DIR%\source
:: @SET SHADER_BIN_DIR=%SHADER_DIR%\bin
:: @SET "VULKAN_SDK_PATH=C:\VulkanSDK\1.1.126.0\"
:: @SET VULKAN_SDK_VERSION=1.2.141.2
:: @SET "VULKAN_SDK_PATH=C:\VulkanSDK\%VULKAN_SDK_VERSION%\"


::------------------------------
::
:: Clang Format
::
::------------------------------
echo Formatting files...
clang-format.exe -i .\src\*.c         >nul 2>nul
clang-format.exe -i .\src\*.cpp       >nul 2>nul
clang-format.exe -i .\src\*.h         >nul 2>nul
clang-format.exe -i .\src\*.hpp       >nul 2>nul
clang-format.exe -i .\..\common\*.h   >nul 2>nul
clang-format.exe -i .\..\common\*.hpp >nul 2>nul
clang-format.exe -i .\..\common\*.cpp >nul 2>nul
echo.


::------------------------------
::
:: Compile Shaders
::
::------------------------------
:: mkdir %SHADER_BIN_DIR% 2>nul
:: echo Compiling shaders...
:: %VULKAN_SDK_PATH%\Bin\glslc.exe %SHADER_SRC_DIR%\vkTriangle.vert -o %SHADER_BIN_DIR%\vkTriangle_vert.spv -Werror
:: %VULKAN_SDK_PATH%\Bin\glslc.exe %SHADER_SRC_DIR%\vkTriangle.frag -o %SHADER_BIN_DIR%\vkTriangle_frag.spv -Werror
:: IF %ERRORLEVEL% NEQ 0 GOTO :SHADER_COMP_ERR
:: echo Done.
:: echo.


::------------------------------
::
:: Compilation
:: Requires Visual Studio 2022
::
::------------------------------
where cl >nul 2>nul
IF %ERRORLEVEL% NEQ 0 call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" %APP_ARCH% >nul 2>nul
IF %ERRORLEVEL% NEQ 0 GOTO :VS_NOT_FOUND

::
:: Store msvc clutter elsewhere
::-----------------------------
mkdir msvc_landfill >nul 2>nul
pushd msvc_landfill >nul

::
:: Compile & Link Options
::------------------------------
:: /TC                  Compile as C code.
:: /TP                  Compile as C++ code.
:: /Oi                  Enable intrinsic functions.
:: /Od 	                Disables optimization.
:: /Qpar                Enable parallel code generation.
:: /Ot                  Favor fast code (over small code).
:: /Ob2                 Enable full inline expansion. Note: Debugging impact.
:: /Z7	                Full symbolic debug info. No pdb. (See /Zi, /Zl).
:: /GS	                Detect buffer overruns.
:: /MD*                 Multi-thread specific, DLL-specific runtime lib. (See /MDd, /MT, /MTd, /LD, /LDd).
:: /GL	                Whole program optimization.
:: /EHsc                No exception handling (Unwind semantics requrie vstudio env). (See /W1).
:: /I<arg>              Specify include directory.
:: /link                Invoke microsoft linker options.
:: /NXCOMPAT            Comply with Windows Data Execution Prevention.
:: /MACHINE:<arg>       Declare machine arch (should match vcvarsall env setting).
:: /NODEFAULTLIB:<arg>  Ignore a library.
:: /LIBPATH:<arg>       Specify library directory/directories.

:: General Parameters
@SET GeneralParameters=/Oi /Qpar /EHsc /GL /nologo /Ot /std:c++latest /DGLEW_STATIC

:: Debug Paramters
@SET DebugParameters=/Od /W4 /WX /Z7 /MTd

:: Release Parameters
@SET ReleaseParameters=/O2 /W4 /WX /Ob2 /MT

:: Include Parameters
@SET IncludeParameters=/I%cd%\.. ^
/I%SCRIPT_DIR%\.. ^
/I%SCRIPT_DIR%\..\.. ^
/I%SCRIPT_DIR%\..\..\include ^
/I%SCRIPT_DIR%\..\..\include\GL ^
/I%SCRIPT_DIR%\..\..\include\GLFW ^
/I%SCRIPT_DIR%\..\..\include\glm ^
/I%SCRIPT_DIR%\..\..\include\stb ^
/I%SCRIPT_DIR%\..\..\src\common

::
:: General Link Parameters
::----------------
@SET GeneralLinkParameters=/SUBSYSTEM:CONSOLE ^
/LIBPATH:%SCRIPT_DIR%\..\..\lib ^
/NXCOMPAT ^
/MACHINE:x64 ^
OpenGL32.lib ^
user32.lib ^
gdi32.lib ^
shell32.lib ^
odbccp32.lib

::
:: Debug Link Parameters
::----------------------
@SET DebugLinkParameters=glfw3dll.lib

::
:: Release Link Parameters
::------------------------
@SET ReleaseLinkParameters=glfw3_mt.lib

::
:: Source Files
::-------------
@SET SourceFiles=%SCRIPT_DIR%\src\%APP_NAME%.cpp ^
%SCRIPT_DIR%\..\..\include\GL\glew.c ^
%SCRIPT_DIR%\..\..\src\common\logging.cpp ^
%SCRIPT_DIR%\..\..\src\common\gl_tools.cpp ^
%SCRIPT_DIR%\..\..\src\common\app_window.cpp ^
%SCRIPT_DIR%\..\..\src\common\camera.cpp ^
%SCRIPT_DIR%\..\..\src\common\model.cpp ^
%SCRIPT_DIR%\..\..\src\common\object3.cpp ^
%SCRIPT_DIR%\..\..\src\common\orientation.cpp ^
%SCRIPT_DIR%\..\..\src\common\position.cpp ^
%SCRIPT_DIR%\..\..\src\common\image_tools.cpp ^
%SCRIPT_DIR%\..\..\src\common\fileio.cpp

:: STB Image
@SET SourceFiles=%SourceFiles% ^
%SCRIPT_DIR%\..\..\include\stb\stb_image.cpp

::
:: Compiler Invocation
::--------------------
@SET "INVOKE_RELEASE=cl %GeneralParameters% %ReleaseParameters% %SourceFiles% %IncludeParameters% /link %GeneralLinkParameters% %ReleaseLinkParameters%"
@SET "INVOKE_DEBUG=cl %GeneralParameters% %DebugParameters% %SourceFiles% %IncludeParameters% /link %GeneralLinkParameters% %DebugLinkParameters%"

IF /I "%RELEASE_BUILD%" EQU "1" ( echo Building [ release ]... ) else ( echo Building [ debug ]... )
IF /I "%RELEASE_BUILD%" EQU "1" (%INVOKE_RELEASE%) else (%INVOKE_DEBUG%)
IF %ERRORLEVEL% NEQ 0 GOTO :exit

xcopy /y %APP_NAME%.exe ..\..\..\bin >nul
popd >nul
echo Done.
echo.
GOTO :exit

:VS_NOT_FOUND
echo.
echo Unable to find vcvarsall.bat. Did you install Visual Studio to the default location?
echo This build script requries Visual Studio 2022; with the standard C/C++ toolset.
echo.
GOTO :exit


:: :SHADER_COMP_ERR
:: echo.
:: echo Unable to compile shaders. Did you install the Vulkan SDK to the default location?
:: echo This build script requires SDK version:  %VULKAN_SDK_VERSION%
:: echo.
:: GOTO :exit

:exit
