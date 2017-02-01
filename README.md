# openGLMap
A rudimentary UAV Mapping Plotter using openGL.

# Dependencies
##  Automated Install
A few scripts have been created to automate the installation process. In the event these fail, you may have to compile the dependencies manually. Support for Windows systems have been removed as of 1/2/2017. If may still be possible to complie on Windows, check the Windows-old-store branch for a bit of help for installation of some of the required packages.

### Linux (Ubuntu)
On Ubuntu these dependencies can be installed by running
```
sudo chmod +x installDependicies.sh
./installDependiciesUbuntu.txt
```
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

* Boost Libraries
	* Install boost
	```
	sudo apt-get install libboost-all-dev
	```

* cLibrary - Version 2
	* Clone the repo and copy to your include folder
	```
	git clone https://github.com/mavlink/c_library_v2.git
	sudo cp -r c_library_v2 /usr/local/include

	```

* gnuplot
	* Install gnuplot
	```
	sudo apt-get install gnuplot5-x11
	```

* gnuplot-iostream
	* Clone the repository
	```
	git clone https://github.com/dstahlke/gnuplot-iostream.git
	```
	* Copy the required include header
	```
	sudo cp gnuplot-iostream/gnuplot-iostream.h /usr/local/includ
	```
	
# Compiling
A CMakeLists.txt file is included for compiling with Cmake. This should work for systems running Ubuntu. Navigate to the build directory and remove any old CMakeFiles
```
cd build
rm -r *
```
Run cmake to generate a makefile.
```
cmake ../src/						# Linux
```
Compile the program.
```
make -j4
```

## Debugging
To enable debugging, when running cmake, enable the debugging flag.
```
cmake -DCMAKE_BUILD_TYPE=Debug
```

# Compiling an Eclipse Project
To generate an Eclipse project, from the root directory,
```
cd build
rm -r CMakeFiles/
cmake -G "Eclipse CDT4 - Unix Makefiles" ../src
```
Then import the project into Eclipse using File >> Import >> General >> Existing Projects into Workspace. Click next and set the root directory to <workspace>/openGLMap/build. Click Finish. The project can now be built with Eclipse using the 'all' Build Target. The source files will be inside the "[Source Directory]" and are linked to their actual counterpats.

# Models
Rudimentary models are stored [here](https://drive.google.com/drive/folders/0B8WtiKHIU0VNV3l5aDEzdHhkV3c?usp=sharing). They should be placed in a Models folder at the top directory, parallel to src and Debug.

# Run Options
* The -w argument draws using wireframe mode
* The -f argument displays the current fps

To run, navigate to the Debug directory and enter
```
./openGLMap
```
Move around in the world using the wasd keys.

If taking an input mavlink feed from ardupilot/SITL, sim_vehicle.py must be run with -C --streamrate 5 and with --out=192.168.1.1:14550.
```
sim_vehicle.sh --out=192.168.1.1:14550 -C --streamrate 5
```

# Making Changes with Eclipse
These instructions are retained to give a manual method for building the project with Eclipse, in the case where cmake fails due to configuration issues.

If you just want to compile the source code without making changes, then running make from the command line, while in the Debug directory will give you your desired result.

* Change the source directory. Project >> Properties >> C/C++ General >> Paths and Symbols >> Source Location. Delete the /openGLMap folder and add /openGLMap/src. This should be done for both the Debug and Release configurations by changing the selected configuration in this tab.

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