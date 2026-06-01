# map-editor
 
A tile-based map editor built in C++ for a game currently in development. Designed to load image tilesets, assign textures to map tiles, and save/load map projects.
 
---
 
## Features
 
- **Flexible tile selection** — click individual tiles, select multiples, draw rectangles, or paint-select by dragging
- **Texture assignment** — apply any texture from the image menu to one or more selected tiles at once
- **Image import** — currently supports `.bmp` files; support for PNG, JPG, and other formats is planned
- **Texture deletion** — remove textures from individual tiles or wipe a texture entirely from the map
- **Viewport navigation** — drag the screen, zoom in/out, and snap back to default zoom with a keystroke
- **Save & Load** — persist map projects to disk and reopen them

---

## Quick Start (Pre-compiled Binary)

If you just want to run the map editor without compiling the code, you can download the standalone executable from the **Releases** tab.

Because the release binary is statically linked, **you do not need to install any system dependencies.** Simply download the binary, grant it execute permissions, and run it:

```bash
chmod +x map-editor
./map-editor
```
 
---
 
## Building from Source (For Developers)

If you wish to modify the source code or compile the editor yourself, you will need to install the required development libraries.

### Dependencies
 
- [SDL2](https://github.com/libsdl-org/SDL)
- [SDL_image](https://github.com/libsdl-org/SDL_image)
- [SDL_ttf](https://github.com/libsdl-org/SDL_ttf)

**Additional Build Tools:**
Compiling the project requires `pkg-config` (to resolve system library paths) and `xxd` (to bake font assets directly into the executable). Generating a static Linux build also requires Wayland window decorations and text shaping development libraries.
 
Install them on Ubuntu/Debian:

```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev pkg-config xxd libdecor-0-dev libharfbuzz-dev
```
 
On Arch Linux:

```bash
sudo pacman -S sdl2 sdl2_image sdl2_ttf pkgconf xxd libdecor harfbuzz
```
 
On Fedora / Red Hat:

```bash
sudo dnf install SDL2-devel SDL2_image-devel SDL2_ttf-devel pkgconf vim-common libdecor-devel harfbuzz-devel
```
 
On macOS with Homebrew:

```bash
brew install sdl2 sdl2_image sdl2_ttf pkg-config
```
 
### Compilation
 
Before compiling for the first time, you must bake the default font into a C++ header file. This allows the application to read the font directly from memory without relying on local filesystem paths:

```bash
cd src
xxd -i fonts/arial.ttf > font_data.h
cd ..
```

The project uses a `Makefile` with two separate build pipelines. From the repository root:

**Standard Build (Dynamic)** For rapid compilation and debugging during day-to-day development:

```bash
make
```
 
**Standalone Build (Static)** To compile a self-contained executable that statically links the SDL2 libraries (used for the official releases):

```bash
make static
```
 
---
 
## Controls
 
### Selection
 
| Action | Input |
|---|---|
| Select tile | `LMB` |
| Select multiple tiles | `LCtrl` + `LMB` |
| Rectangle select | `LCtrl` + `LShift` + `LMB` on two tiles — selects the rectangle they define |
| Paintbrush select | `LCtrl` + `LShift` + hold `LMB` + move mouse — selects every tile the cursor passes over |
| Deselect all | `Escape` |
 
### Editing
 
| Action | Input |
|---|---|
| Assign texture to selected tiles | Click a texture in the **Image Menu** while tiles are selected |
| Delete texture from selected tiles | `Delete` |
| Delete a texture from the entire map | Deselect all → select the target texture in the **Image Menu** → `Delete` |
 
### Viewport
 
| Action | Input |
|---|---|
| Drag screen | Hold `LMB` + move mouse |
| Zoom in | `LCtrl` + scroll wheel up |
| Zoom out | `LCtrl` + scroll wheel down |
| Reset zoom | `LCtrl` + `Q` |
| Move to center of map *(not yet implemented)* | `LCtrl` + `0` |
 
---
 
## Usage
 
1. Open or create a map project.
2. Load a tileset image via the **Image Menu**.
3. Select one or more tiles on the map canvas.
4. Click a texture in the **Image Menu** to paint it onto the selected tiles.
5. Save the project when done.

---
 
## Project Status
 
This editor is under active development alongside the game it was built for. Expect rough edges, missing features, and breaking changes.