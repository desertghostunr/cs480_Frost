# PA1: Spinning Cube

# Dependencies, Building, and Running

## Dependency Instructions
To run this project installation of these three programs are required [GLEW](http://glew.sourceforge.net/), [GLM](http://glm.g-truc.net/0.9.7/index.html), and [SDL2](https://wiki.libsdl.org/Tutorials).

This project uses OpenGL 3.3.

### Ubuntu/Linux
```bash
sudo apt-get install libglew-dev libglm-dev libsdl2-dev
```

## Building and Running
Please build this project using CMake. It is recommended to create build directory and to build the project there.

Running the make in a separate directory will allow easy cleanup of the build data, and an easy way to prevent unnecessary data to be added to the git repository.

### CMake Instructions
The building of the project is done using CMake, installation with apt-get may be necessary.

Example build and running:

```bash
mkdir build
cd build
cmake ..
make
./Tutorial -v shaders/cubeVertexShader.glsl -f shaders/cubeFragmentShader.glsl
```

### Specific Instructions on Running
Running the program requires specifying a vertex shader and a fragment shader. These shaders should have been moved into a directory under build named shaders.

The program has three options to accomplish this: -v, -f, --h.

-v specifies the file path of the vertex shader.

-f specifies the file path of the fragment shader.

--h brings up a series of instructions for help using the program.

The file path for the specified shader must come directly after the option identifier. If one of the option commands is found where a file path is expected the program will terminate.

Running this program is done in the following format:

```bash
./Tutorial -[SHADER_OPTION] [SHADER_FILE_PATH] -[SHADER_OPTION] [SHADER_FILE_PATH]  
```
An example run command would look like:
```bash
./Tutorial -v shaders/cubeVertexShader.glsl -f shaders/cubeFragmentShader.glsl
```

## Ubuntu.cse.unr.edu
OpenGL 3.3 will run on the [ubuntu.cse.unr.edu](https://ubuntu.cse.unr.edu/) website. To do so follow the build instructions, but when running the Tutorial executable use this line to execute.
```bash
/usr/NX/scripts/vgl/vglrun ./Tutorial
```
