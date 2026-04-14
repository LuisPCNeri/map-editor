CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iengine $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)

SRC = main.cpp \
      engine/map.cpp \
      engine/tile.cpp

OBJ = $(SRC:.cpp=.o)
TARGET = map-editor

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean