# openGLMap
A rudimentary UAV Mapping Plotter using openGL.

[![openGLMap Demonstration](https://raw.github.com/tbattz/openGLMap/master/Other/youtubeLink.png)](https://www.youtube.com/watch?v=QKWYyx10PYw "openGLMap Demonstration")

# Dependencies
##  Automated Install
A few scripts have been created to automate the installation process. In the event these fail, you may have to compile the dependencies manually. Support for Windows systems have been removed as of 1/2/2017. If may still be possible to complie on Windows, check the Windows-old-store branch for a bit of help for installation of some of the required packages.

### Linux (Ubuntu)
On Ubuntu these dependencies can be installed by running
```
sudo chmod +x installDependicies.sh
./installDependiciesUbuntu.sh
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
* Initialise Submodules
	```
	git submodule init
	git submodule update
	```

# Compiling
A CMakeLists.txt file is included for compiling with Cmake. This should work for systems running Ubuntu. Navigate to the build directory and remove any old CMakeFiles
```
cd build
rm -r *
```
Run cmake to generate a makefile.
```
cmake ..						# Linux
```
To debug.
```
cmake .. -DCMAKE_BUILD_TYPE=Debug
```
Compile the program.
```
make all -j4
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

# Configuration
A configuration file must be generated before the program will run. A python tool has been developed to generate this configuration file, which is a submodule of the current repository. To run the tool, from the root directory of openGLMap,
```
cd configureOpenGLMap/src
python main.py
```
See the configureOpenGLMap README.md for detailed instructions: https://github.com/tbattz/configureOpenGLMap/blob/master/README.md

After generation, check that a configuration file exists in Configs/currentConfig.txt.

# Run Options
* The -w argument draws using wireframe mode
* The -f argument displays the current fps


If taking an input mavlink feed from ardupilot/SITL, sim_vehicle.py must be run with -C --streamrate 5 and with --out=192.168.1.1:14550.
```
sim_vehicle.sh --out=192.168.1.1:14550 -C --streamrate 5
```
For a quadplane,
```
cd ArduPlane
sim_vehicle.py --map -f quadplane --console -L PolPad --out=192.168.1.1:14550 -C --streamrate 5
```

To run, navigate to the build directory and enter
```
./openGLMap
```
# Controls
| Control					| Input		|
| --------------------------------------------- |:-------------:|
| Move Camera Position (free views only)	| WASD Keys	|
| Move Camera Direction				| Move Mouse	|
| Toggle Lock Camera Direction			| P Key		| 
| Change Field of View (Zoom)			| Mouse Scroll	|
| Change View (Forward/Back)			| V/B Keys	|
| Track Another Aircraft (Onboard Free View)	| N Key		|
| Change Aircraft (Forward/Backward)		| Z/X Keys	|
| Toggle Help Information			| H Key		|


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
	* Xi
	* SOIL
	* freetype

* Add the path to the header only GLM library to the GCC C++ Compiler Includes. Project >> Properties >> C/C++ Build >> Settings >> Tool Settings >> GCC C++ Compiler >> Includes. Under include paths, add /usr/local/include

* This project needs to be compiled with c++11. Project >> Properties >> C/C++ Build >> Settings >> Tool Settings >> GCC C++ Compiler >> Miscellaneous. Under the Other flags box, add -std=c++11. Eclipse will build the project but may still show errors. To fix this, go to Project >> Properties >> C/C++ General >> Preprocessor Include Paths, Macros etc. >> Providers. Locate CDT GCC Built-in Compiler Settings. Uncheck the "Use global provider shader between project" box, and append -std=c++0x to the "Command to get compiler specs:" entry box. Move this entry to the top of the list using Move Up. Apply and click ok. Then re-resolve the includes using Project >> C/C++ Index >> Re-resolve Unresolved Includes.

# License
OpenGLMap is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

OpenGLMap is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Cmavnode. If not, see http://www.gnu.org/licenses/.

OpenGLMap should not be used to replace ground station software, but rather as a tool to aid in the visualisation of the current state of an aircraft. The author(s) of OpenGLMap will not be held accountable for damage to any aircraft or property caused while using OpenGLMap. Use at your own risk.
