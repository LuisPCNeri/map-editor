CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I. -Iengine -I/usr/include/SDL2 -D_REENTRANT

LDFLAGS = -lSDL2 -lSDL2_ttf

SDL_STATIC_DEPS = $(shell pkg-config --libs --static sdl2 SDL2_ttf | sed 's/-lSDL2_ttf//g' | sed 's/-lSDL2//g')
STATIC_LDFLAGS = -Wl,-Bstatic -lSDL2 -lSDL2_ttf -Wl,-Bdynamic $(SDL_STATIC_DEPS) -lharfbuzz

SRC = main.cpp \
      engine/map.cpp \
      engine/tile.cpp \
      engine/image-menu.cpp \
      engine/menu.cpp \
      toolbar/toolbar.cpp \
      toolbar/buttons/toobarlBtn.cpp \
      toolbarMenus/createProjMenu.cpp \
      toolbarMenus/openProjMenu.cpp \
      engine/assetManager/assetManager.cpp \
      engine/SpawnIdMenu.cpp

OBJ = $(SRC:%.cpp=build/%.o)
TARGET = map-editor

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS)

static: $(OBJ)
	$(CXX) -o $(TARGET) $(OBJ) $(STATIC_LDFLAGS)

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build $(TARGET)

rebuild: clean all