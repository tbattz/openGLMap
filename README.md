# openGLRace
A rudimentary vehicle simulation using openGL. 

# Dependencies
On Ubuntu these dependencies can be installed by running
```
sudo chmod +x installDependicies.sh
./installDependiciesUbuntu.txt
```
If this is not successful, then you will need to do it manually, as below.

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
	```
	./configure
	make -j4
	sudo make install
	```
	* Copy include files to /usr/local/include
	```
	sudo cp -r include/* /usr/local/include 
	```

# Compiling
Currently you will need to generate a Makefile with Eclipse. The makefile should allow the program to be compiled and linked, given that the dependencies are met. Navigate to the Debug or Release directory and enter the following commands.
```
make clean && make
```
The program can then be run with
```
./openGLRace
```

# Making Changes with Eclipse
While the makefile provided can be used to compile the program using the command line, it will not adjust when mew files are added, unless the makefile is regenerated with Eclipse. This requires a little bit of setup, using the standard CDF management build system.

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