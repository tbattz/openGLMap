# Installs GLEW on Windows using the Git-bash terminal
# In a bash shell
cd auto
make
cd ../build/cmake
cmake -G "MinGW Makefiles"
make

sleep 1s