# openGLMap
A rudimentary UAV Mapping Plotter using openGL.

# Dependencies
##  Automated Install
A few scripts have been created to automate the installation process. In the event these fail, you may have to compile the dependencies manually.
### Linux (Ubuntu)
On Ubuntu these dependencies can be installed by running
```
sudo chmod +x installDependicies.sh
./installDependiciesUbuntu.txt
```
### Windows (Windows 10)
Windows has a clunkier installation procedure.
Independices are installed in two steps.
* Run installDependiciesWindows.bat
* Make sure that you follow the instructions to add the correct paths to your PATH variable.
* In a new command prompt (one that will grab the updated PATH), run installDependiciesWindowspart2.bat

## Manual Install
### Linux (Ubuntu)
* Building dependicies
	* cmake
	```
	sudo apt-get install cmake
	```

* assimp (The Open Asset Import Library Repository).
	* Clone into any directory using
	```
	git clone https://github.com/assimp/assimp.git
	```
	* Build from source (requires boost).
	```
	cd assimp
	cmake CMakeLists.txt -G 'Unix Makefiles'
	make -j4
	sudo make install
	```

* GLFW (multi-platform library for OpenGL)
	* Download from http://www.glfw.org/download.html and extract.
	* Install dependencies
	```
	sudo apt-get install xorg-dev libglu1-mesa-dev
	```
	* Navigate to the glfw root directory and use cmake
	```
	cmake -G "Unix Makefiles"
	make -j4
	sudo make install
	```

* GLEW (The OpenGL Extension Wrangler Library)
	* Install the development library
	```
	sudo apt-get install libglew-dev
	```

* GLM (The OpenGL Mathematics Library)
	* Download a release from Github and extract, https://github.com/g-truc/glm/tags
	* Copy the glm folder inside the glm release into the include directory
	```
	cd <glm_root_dir>
	sudo cp -r glm /usr/local/include/
	```

* SOIL (Simple OpenGL Image Library)
	* Download SOIL from http://www.lonesock.net/soil.html
	* Navigate to the root directory and copy files
	```
	cd <soil_root>
	sudo cp lib/libSOIL.a /usr/local/include/
	sudo cp src/SOIL.h /usr/local/include/
	```
	* Install the development library
	```
	sudo apt-get install libsoil-dev
	```

* FreeType - Font Rendering Library
	* Download and extract from https://www.freetype.org/index.html
	* Naigate to the root directory and run
	* Copy include files to /usr/local/include
	```
	sudo cp -r include/* /usr/local/include 
	```
	Please DO NOT run ./configure on a system running the Unity window manager, as this will most likely break Unity.

### Windows (Windows 10)
Firstly, ensure that cywin is not in your path.
* MinGW-w64
	* Go to https://mingw-w64.org/doku.php
	* Select Mingw-builds and start the installation.
	* Select x86_64 for the Architecture.
	* Leave Threads as posix.
	* Once the installation has finished, add the path to the system environment variables.
		* Search PATH - find "Edit the system environment variables"
		* Select Environment Variables...
		* Select PATH and click Edit...
		* Enter the path, in this case C:\Program Files\mingw-w64\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\
		* Move it to the top
		* Click OK twice.
* CMake
	* Download the MSI Installer of CMake from https://cmake.org/download/ 
	* Run the installer and follow the prompts, selecting "Add cmake to the system path"
* Git
	* Download git from https://git-scm.com/downloads
	* Install git.
	* Add the bin folder to the system PATH, C:\Program Files\Git\cmd\
* GnuMake for Windows
	* Download from http://gnuwin32.sourceforge.net/packages/make.htm and select the setup package.
	* Install.
* GLFW
	* Open a new command prompt for the remaining steps.
	* Clone the repository
	```
	git clone https://github.com/glfw/glfw.git
	```
	* Make the libraries
	```
	cd glfw
	mkdir build
	cmake -DBUILD_SHARED_LIBS=OFF -G "MinGW Makefiles" ..
	make -j4
	cd ..
	```
	* Copy the required includes
	```
	mkdir <openGLRace root>\Includes
	mkdir <openGLRace root>\Lib
	xcopy include\GLFW\* <openGLRace root>\Includes\GLFW\ /s/h/e/k/f/c/y
	xcopy build\src\libglfw3.a <openGLRace root>\Lib\ /s/h/e/k/f/c/y
	```
* GLEW
	* Clone the repository
	```
	git clone https://github.com/nigels-com/glew.git
	```
	* Open a git-bash shell and enter the following (it will only compile in a bash environment).
	```
	cd auto
	make
	cd ../build/cmake
	cmake -G "MinGW Makefiles"
	make
	```
	* Back in the command prompt, in the glew directory, enter the following
	```
	xcopy include\GL* <openGLRace root>\Includes\ /s/i
	xcopy build\cmake\lib\* <openGLRace root>\Lib\ /s/i
	xcopy build\cmake\bin\glew32.dll <openGLRace root>\Lib
	```

* Assimp
	* Clone assimp into your Downloads directory
	```
	git clone https://github.com/assimp/assimp.git
	```
	* Make Assimp
	```
	cd assimp
	cmake -G "MinGW Makefiles" -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_ZLIB=ON
	make -j4
	make install
	```
	* Copy requied files
	```
	cd bin
	xcopy libassimp.a <openGLRace root>/Lib/ /s/h/e/k/f/c
	cd ..
	xcopy include\assimp\* <openGLRace root>\Includes\assimp /s/i
	```
	* Copy zlib library
	```
	xcopy lib\libzlibstatic.a <openGLRace root>\Lib\ /s/i/y
	```
* GLM
	* Clone GLM
	```
	git clone https://github.com/g-truc/glm.git
	```
	* As GLM is a header only library, just copy everything to the include folder.
	```
	xcopy /E/I/y glm\glm <openGLRace root>\includes\glm
	```
* SOIL
	* Download SOIL from http://www.lonesock.net/soil.html
	* Extract to /SOIL
	* Change to the makefile directory and make
	```
	cd SOIL\Simple OpenGL Image Library\projects\makefile
	mkdir obj
	make
	```
	* Copy the required includes
	```
	cd ..\..
	xcopy lib\libSOIL.a %ORIGINAL%\Lib /E/I
	xcopy src\SOIL.h %ORIGINAL%\Includes /E/I
	cd ..\..
	```
* FreeType
	* Download freetype using the sourceforge link from https://www.freetype.org/download.html
	* Extract the zip
	* Change to the root directory
	* Make
	```
	make
	```
	* Copy the required includes
	```
	xcopy /E/I/y objs\freetype.a <openGLRace root>\Lib
	xcopy /E/I/y include\* <openGLRace root>\Includes\
	```

# Compiling
A CMakeLists.txt file is included for compiling with Cmake. This should work for both Debian and Windows based systems. Navigate to the build directory and remove any old CMakeFiles
```
cd build
rm -r *
```
Run cmake to generate a makefile.
```
cmake ../src/						# Linux
cmake -G "MinGW Makefiles" ../src/	# Windows
```
Compile the program.
```
make
```
This may take longer on a Windows based system.

# Compiling an Eclipse Project
To generate an Eclipse project, from the root directory,
```
cd build
rm -r CMakeFiles/
cmake -G "Eclipse CDT4 - Unix Makefiles" ../src
```
Then import the project into Eclipse using File >> Import >> General >> Existing Projects into Workspace. Click next and set the root directory to <workspace>/openGLRace/build. Click Finish. The project can now be built with Eclipse using the 'all' Build Target. The source files will be inside the "[Source Directory]" and are linked to their actual counterpats.

# Models
Rudimentary models are stored [here](https://drive.google.com/drive/folders/0B8WtiKHIU0VNV3l5aDEzdHhkV3c?usp=sharing). They should be placed in a Models folder at the top directory, parallel to src and Debug.

# Run Options
* The -w argument draws using wireframe mode
* The -f argument displays the current fps

To run, navigate to the Debug directory and enter
```
./openGLRace
```
Move around in the world using the wasd keys.

# Making Changes with Eclipse
These instructions are retained to give a manual method for building the project with Eclipse, in the case where cmake fails due to configuration issues.

If you just want to compile the source code without making changes, then running make from the command line, while in the Debug directory will give you your desired result.

* Change the source directory. Project >> Properties >> C/C++ General >> Paths and Symbols >> Source Location. Delete the /openGLRace folder and add /openGLRace/src. This should be done for both the Debug and Release configurations by changing the selected configuration in this tab.

* Add libraries. Project >> Properties >> C/C++ Build >> Settings >> Tool Settings >> GCC C++ Linker >> Libraries. Under Libraries (-l), add each of the following as separate entries. Again remember to do this for both Debug and Release configurations. Note that they must be added in this order or you will get errors.
	* glfw3
	* Xinerama
	* Xcursor
	* rt
	* m
	* assimp
	* dl
	* Xxf86vm
	* GL
	* GLU
	* X11
	* pthread
	* Xrandr
	* GLEW
	* Xi
	* SOIL
	* freetype

* Add the path to the header only GLM library to the GCC C++ Compiler Includes. Project >> Properties >> C/C++ Build >> Settings >> Tool Settings >> GCC C++ Compiler >> Includes. Under include paths, add /usr/local/include

* This project needs to be compiled with c++11. Project >> Properties >> C/C++ Build >> Settings >> Tool Settings >> GCC C++ Compiler >> Miscellaneous. Under the Other flags box, add -std=c++11. Eclipse will build the project but may still show errors. To fix this, go to Project >> Properties >> C/C++ General >> Preprocessor Include Paths, Macros etc. >> Providers. Locate CDT GCC Built-in Compiler Settings. Uncheck the "Use global provider shader between project" box, and append -std=c++0x to the "Command to get compiler specs:" entry box. Move this entry to the top of the list using Move Up. Apply and click ok. Then re-resolve the includes using Project >> C/C++ Index >> Re-resolve Unresolved Includes.