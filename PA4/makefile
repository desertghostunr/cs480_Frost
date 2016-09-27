# Check for OS Type
UNAME_S := $(shell uname -s)

# Linux
ifeq ($(UNAME_S), Linux)
	CC=g++
	LIBS=-lSDL2 -lGLEW -lGL
# Mac
else
	CC=clang++
	LIBS=-lSDL2 -framework OpenGL -framework Cocoa -lGLEW -stdlib=libc++
endif

# Paths, use this to set Include directories, or library directories; uncomment in all: if used
#PATHI=-I/usr/local/include/ 
#PATHL=-L/usr/local/lib/

#Compiler Flags
CXXFLAGS=-g -Wall -std=c++0x

# .o Compilation
O_FILES=main.o camera.o engine.o graphics.o object.o shader.o window.o

# Point to includes of local directories
INCLUDES=-I../include


all: $(O_FILES)
	$(CC) $(CXXFLAGS) -o Tutorial $(O_FILES) $(LIBS) #$(PATHI) $(PATHL)
	cp -r ../shaders .
	cp -r ../models .

main.o: ../src/main.cpp
	$(CC) $(CXXFLAGS) -c ../src/main.cpp -o main.o $(INCLUDES)

camera.o: ../src/camera.cpp
	$(CC) $(CXXFLAGS) -c ../src/camera.cpp -o camera.o $(INCLUDES)

engine.o: ../src/engine.cpp
	$(CC) $(CXXFLAGS) -c ../src/engine.cpp -o engine.o $(INCLUDES)

graphics.o: ../src/graphics.cpp
	$(CC) $(CXXFLAGS) -c ../src/graphics.cpp -o graphics.o $(INCLUDES)

object.o: ../src/object.cpp
	$(CC) $(CXXFLAGS) -c ../src/object.cpp -o object.o $(INCLUDES)

shader.o: ../src/shader.cpp
	$(CC) $(CXXFLAGS) -c ../src/shader.cpp -o shader.o $(INCLUDES)

window.o: ../src/window.cpp
	$(CC) $(CXXFLAGS) -c ../src/window.cpp -o window.o $(INCLUDES)

ObjectTable.o: ../src/ObjectTable.cpp
	$(CC) $(CXXFLAGS) -c ../src/ObjectTable.cpp -o ObjectTable.o $(INCLUDES)

clean:
	-@if rm *.o Tutorial 2>/dev/null || true; then echo "Main Removed"; else echo "No Main"; fi
	rm shaders -rf
