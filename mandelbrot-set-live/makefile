SRC_FILES = \
$(wildcard ./*.cpp)

LINKS =                               \
-L ../vendor/bin/glfw/ -lglfw3 \
-ldl                                  \
-lGL                                  \
-lpthread                             \
-L ../vendor/bin/glew/ -lGLEW

all:
	g++ -Wshadow -Wall -std=c++17 $(SRC_FILES) -o driver -O2 $(LINKS) -I ../vendor/bin/
