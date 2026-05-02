CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iengine $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_ttf
BUILD_DIR = build

SRC = main.cpp \
      engine/map.cpp \
      engine/tile.cpp \
      engine/image-menu.cpp \
      toolbar/toolbar.cpp

OBJ = $(SRC:%.cpp=$(BUILD_DIR)/%.o)
TARGET = map-editor

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild