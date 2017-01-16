:: Only run after installDependiciesWindows.bat has completed successfully
:: Must run in a new command prompt, to get the updated PATHS
echo off
CALL:echored "IMPORTANT: REMOVE CYGWIN FROM PATH BEFORE INSTALLING."
:: Change to download directory
cd ..
SET ORIGINAL=%CD%
cd %UserProfile%\Downloads\


:: Download GLFW
call:echoblue " =================================== GLFW ====================================="
if exist "%UserProfile%\Downloads\glfw\" (
	echo GLFW already cloned.
) else (
	echo Downloading GLFW.
	git clone https://github.com/glfw/glfw.git
)
:: Copy Required Includes and Libraries
cd glfw
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=OFF -G "MinGW Makefiles" ..
make -j4
cd ..
if not exist %ORIGINAL%\Includes\ mkdir %ORIGINAL%\Includes\
xcopy include\GLFW\* %ORIGINAL%\Includes\GLFW\ /s/h/e/k/f/c/y
if not exist %ORIGINAL%\Lib\ mkdir %ORIGINAL%\Lib\
xcopy build\src\libglfw3.a %ORIGINAL%\Lib\ /s/h/e/k/f/c/y
cd ..

:: Download GLFW
call:echoblue " =================================== GLEW ====================================="
if exist "%UserProfile%\Downloads\glew\" (
	echo GLEW already cloned.
) else (
	echo Downloading GLEW.
	git clone https://github.com/nigels-com/glew.git
)
:: Run with the git-bash terminal
cd glew
start /w "" "%ProgramFiles%\Git\git-bash.exe" -i %ORIGINAL%\glewWindowsInstall.sh
xcopy include\GL* %ORIGINAL%\Includes\ /s/i/y
xcopy build\cmake\lib\* %ORIGINAL%\Lib\ /s/i/y
xcopy build\cmake\bin\glew32.dll %ORIGINAL%\Lib /y
cd ..

:: Download assimp
call:echoblue " ================================== Assimp ===================================="
if exist "%UserProfile%\Downloads\assimp\" (
	echo assimp already cloned.
) else (
	echo Downloading assimp.
	git clone https://github.com/assimp/assimp.git
)
cd assimp
cmake -G "MinGW Makefiles" -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_ZLIB=ON
make -j4
make install
cd bin
xcopy libassimp.a %ORIGINAL%\Lib\ /s/h/e/k/f/c/y
cd ..
xcopy include\assimp\* %ORIGINAL%\Includes\assimp /s/i/y
xcopy lib\libzlibstatic.a %ORIGINAL%\Lib\ /s/i/y
cd ..

:: Download GLM
call:echoblue " =================================== GLM ======================================"
if exist "%UserProfile%\Downloads\GLM\" (
	echo GLM already cloned.
) else (
	echo Downloading GLM.
	git clone https://github.com/g-truc/glm.git
)
xcopy /E/I/y glm\glm %ORIGINAL%\includes\glm

:: Download SOIL
call:echoblue " ================================= SOIL =================================="
if exist "%UserProfile%\Downloads\SOIL\" (
	echo SOIL already cloned.
) else (
	echo Downloading SOIL.
	"%ProgramFiles(x86)%\GnuWin32\bin\wget.exe" --no-check-certificate http://www.lonesock.net/files/soil.zip
	start /w "" "%ProgramFiles%\Git\git-bash.exe" -i %ORIGINAL%\Installers\unzipSOIL.sh
)
cd SOIL\Simple OpenGL Image Library\projects\makefile
mkdir obj
make
cd ..\..
xcopy lib\libSOIL.a %ORIGINAL%\Lib /E/I/y
xcopy src\SOIL.h %ORIGINAL%\Includes /E/I/y
cd ..\..

:: Download FreeType
call:echoblue " =============================== FreeType ================================"
if exist "%UserProfile%\Downloads\ft271.zip" (
	echo ft271.zip already downloaded.
) else (
	echo Downloading FreeType.
	"%ProgramFiles(x86)%\GnuWin32\bin\wget.exe" --no-check-certificate http://internode.dl.sourceforge.net/project/freetype/freetype2/2.7.1/ft271.zip
	start /w "" "%ProgramFiles%\Git\git-bash.exe" -i %ORIGINAL%\Installers\unzipFreetype.sh
)
cd freetype\freetype-2.7.1
make
xcopy /E/I/y objs\freetype.a %ORIGINAL%\Lib
xcopy /E/I/y include\* %ORIGINAL%\Includes\
cd ..\..



:: Return to original directory
cd %ORIGINAL%

:: Functions
:ECHORED
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -backgroundcolor Red %1
goto:eof

:ECHOBLUE
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -backgroundcolor blue %1
goto:eof

:ECHOGREEN
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -backgroundcolor green -foreground black %1
goto:eof
