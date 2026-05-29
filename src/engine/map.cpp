#include <SDL2/SDL.h>
#include <iostream>
#include "map.hpp"

extern SDL_Renderer* rend;

#include <fstream>
#include <limits>

#include "../globalStateHandler.hpp"
#include "menu.hpp"

#pragma pack(push, 1)

struct FileTile {
    int8_t spawn_pool_id;
    uint16_t texture_id;
};

struct MapHeader {
    uint32_t magic_number;
    uint32_t map_width_chunks;
    uint32_t map_height_chunks;
    int32_t origin_x;
    int32_t origin_y;
};

#pragma pack(pop)

#define CHUNK_SIZE 16

extern globalStateHandler* stateHandler;

namespace Map {

    MapRenderer::MapRenderer(int16_t render_padding){
        this->render_padding = render_padding;
    }

    void MapRenderer::RenderVisible(MapViewport* m){
        int32_t w, h;
        SDL_GetRendererOutputSize(rend, &w, &h);
        int32_t tile_num_x = w / m->tile_size;
        int32_t tile_num_y = h / m->tile_size;

        int32_t start_x = m->offsetX - this->render_padding;
        int32_t start_y = m->offsetY - this->render_padding;

        int32_t end_x = m->offsetX + tile_num_x + this->render_padding;
        int32_t end_y = m->offsetY + tile_num_y + this->render_padding;

        for(auto y = start_y; y < end_y; y++){
            for(auto x = start_x; x < end_x; x++){
                TileCoord coord(x,y);
                
                SDL_Rect dest = {
                    (x - m->offsetX) * m->tile_size,
                    (y - m->offsetY) * m->tile_size,
                    m->tile_size,
                    m->tile_size
                };

                auto tile = this->grid.find(TileCoord(x, y));
                if(tile != this->grid.end()) {
                    tile->second.TileDraw(dest);
                }
                else{
                    Tile new_tile(coord, m->tile_size);
                    auto inserted = this->grid.insert({coord, new_tile});
                    inserted.first->second.TileDraw(dest);
                }
            }
        }

    }

    MapViewport::MapViewport(){
        this->offsetX = 0;
        this->offsetY = 0;

        this->zoom = 0.0f;
        this->tile_size = BASE_TILE_SIZE;

        this->is_mouse_down = false;
        this->ctrl_down = false;
    }

    void MapViewport::ZoomIn() {
        
    };

    void MapViewport::ZoomOut() {
        
    }

    void MapViewport::Drag(int32_t new_mouse_x, int32_t new_mouse_y) {
        this->offsetX -= new_mouse_x;
        this->offsetY -= new_mouse_y;
    }

    int MapRenderer::SelectTile(int32_t mouse_x, int32_t mouse_y, MapViewport* m) {
        TileCoord coord((mouse_x / m->tile_size) + m->offsetX, (mouse_y / m->tile_size) + m->offsetY);

        bool are_tiles_selected = !m->selected_tiles.empty();
        if(are_tiles_selected && !m->ctrl_down){

            std::map<TileCoord, Tile*>::iterator it;
            for(it = m->selected_tiles.begin(); it != m->selected_tiles.end(); it++){
                it->second->border_color = BASE_BORDER_COLOR;
            }

            m->selected_tiles.clear();
        }

        auto tile = this->grid.find(coord);
        if(tile != this->grid.end()){
            m->selected_tiles.insert({coord, &tile->second});
            tile->second.Select();
            return 1;
        }

        return 0;
    }

    void MapRenderer::SelectTileRectangle(MapViewport* m) {

        if(m->selected_tiles.size() != 2) {
            std::cerr << "Only two tiles can be selected for RECTANGLE SELECT." << std::endl;
            return;
        }

        int32_t first_x = m->selected_tiles.begin()->first.x;
        int32_t last_x  = m->selected_tiles.rbegin()->first.x;

        int32_t first_y = m->selected_tiles.begin()->first.y;
        int32_t last_y  = m->selected_tiles.rbegin()->first.y;

        int32_t min_x = (first_x < last_x) ? first_x : last_x;
        int32_t max_x = (first_x > last_x) ? first_x : last_x;
        int32_t min_y = (first_y < last_y) ? first_y : last_y;
        int32_t max_y = (first_y > last_y) ? first_y : last_y;

        for(int32_t x = min_x; x <= max_x; x++) {

            auto it = this->grid.lower_bound(Map::TileCoord(x, min_y));
            while (it != this->grid.end() && it->first.x == x && it->first.y <= max_y) {
                
                m->selected_tiles.insert({it->first, &it->second});
                it->second.Select();
                it++;
            }
        }

    }

    void MapRenderer::ExportMapToBin(const std::string& fpath) {
        if(this->grid.empty()) return;

        int32_t min_x = std::numeric_limits<int32_t>::max();
        int32_t max_x = std::numeric_limits<int32_t>::min();
        int32_t min_y = std::numeric_limits<int32_t>::max();
        int32_t max_y = std::numeric_limits<int32_t>::min();

        for (const auto& pair : this->grid) {
            if (pair.first.x < min_x) min_x = pair.first.x;
            if (pair.first.x > max_x) max_x = pair.first.x;
            if (pair.first.y < min_y) min_y = pair.first.y;
            if (pair.first.y > max_y) max_y = pair.first.y;
        }

        int32_t total_width = max_x - min_x + 1;
        int32_t total_height = max_y - min_y + 1;

        uint32_t width_chunks = (total_width + CHUNK_SIZE - 1) / CHUNK_SIZE;
        uint32_t height_chunks = (total_height + CHUNK_SIZE - 1) / CHUNK_SIZE;

        std::ofstream outfile(fpath, std::ios::binary);
        if (!outfile.is_open()) {
            std::cerr << "Failed to export map binary!" << std::endl;
            return;
        }

        MapHeader header;
        header.magic_number = 0x4D415031; // Hex for "MAP1"
        header.map_width_chunks = width_chunks;
        header.map_height_chunks = height_chunks;
        header.origin_x = min_x;
        header.origin_y = min_y;
        outfile.write(reinterpret_cast<const char*>(&header), sizeof(MapHeader));

        for (uint32_t cy = 0; cy < height_chunks; cy++) {
            for (uint32_t cx = 0; cx < width_chunks; cx++) {
                
                FileTile chunk_data[CHUNK_SIZE][CHUNK_SIZE];

                for (int x = 0; x < CHUNK_SIZE; x++) {
                    for (int y = 0; y < CHUNK_SIZE; y++) {
                        int32_t world_x = min_x + (cx * CHUNK_SIZE) + x;
                        int32_t world_y = min_y + (cy * CHUNK_SIZE) + y;

                        auto it = this->grid.find(Map::TileCoord(world_x, world_y));
                        
                        if (it != this->grid.end()) {
                            chunk_data[x][y].spawn_pool_id = it->second.spawnable_ids.empty() ? -1 : it->second.spawnable_ids[0];
                            chunk_data[x][y].texture_id = it->second.textureId;
                        } else {
                            // Blank padding tile at map border
                            chunk_data[x][y].spawn_pool_id = -1;
                            chunk_data[x][y].texture_id = 0; 
                        }
                    }
                }
                outfile.write(reinterpret_cast<const char*>(chunk_data), sizeof(chunk_data));
            }
        }
        
        outfile.close();
        std::cout << "Successfully exported binary map to: " << fpath << std::endl;
    }

    void MapRenderer::ImportMapFromBinary(const std::string& filepath) {

        std::ifstream infile(filepath, std::ios::binary);
        if (!infile.is_open()) {
            std::cerr << "Failed to open map binary for reading: " << filepath << std::endl;
            return;
        }

        MapHeader header;
        infile.read(reinterpret_cast<char*>(&header), sizeof(MapHeader));

        if (header.magic_number != 0x4D415031) { // "MAP1"
            std::cerr << "CRITICAL ERROR: File is not a valid map file or is corrupted!" << std::endl;
            infile.close();
            return;
        }

        this->grid.clear();
        if(stateHandler && stateHandler->mapViewport){
            stateHandler->mapViewport->selected_tiles.clear();
        }

        Menu::ImageMenu* imgMenu = nullptr;
        if (stateHandler && stateHandler->imageMenu) {
            imgMenu = stateHandler->imageMenu;
        }

        for (uint32_t cy = 0; cy < header.map_height_chunks; cy++) {
            for (uint32_t cx = 0; cx < header.map_width_chunks; cx++) {
                
                // Read an entire 16x16 chunk of bytes in one go
                FileTile chunk_data[CHUNK_SIZE][CHUNK_SIZE];
                infile.read(reinterpret_cast<char*>(chunk_data), sizeof(chunk_data));

                for (int x = 0; x < CHUNK_SIZE; x++) {
                    for (int y = 0; y < CHUNK_SIZE; y++) {
                        
                        FileTile& fTile = chunk_data[x][y];
                        
                        // If it's an empty padding tile, skip it
                        if (fTile.texture_id == 0 && fTile.spawn_pool_id == -1) {
                            continue;
                        }

                        // Calculate the true world coordinates
                        int32_t world_x = header.origin_x + (cx * CHUNK_SIZE) + x;
                        int32_t world_y = header.origin_y + (cy * CHUNK_SIZE) + y;
                        
                        TileCoord coord(world_x, world_y);
                        
                        // Create the tile (using BASE_TILE_SIZE defined in map.hpp)
                        Tile new_tile(coord, BASE_TILE_SIZE);
                        
                        new_tile.textureId = fTile.texture_id;
                        if (fTile.spawn_pool_id != -1) {
                            new_tile.spawnable_ids.push_back(fTile.spawn_pool_id);
                        }

                        // RESTORE TEXTURE POINTER:
                        // Find the texture in the ImageMenu using the ID
                        if (imgMenu && fTile.texture_id > 0) {
                            auto it = imgMenu->images.find({fTile.texture_id, 0});
                            if (it != imgMenu->images.end()) {
                                new_tile.texture = it->second.texture;
                            }
                        }

                        this->grid[coord] = new_tile;
                    }
                }
            }
        }
        
        infile.close();
        std::cout << "Successfully loaded map from: " << filepath << std::endl;
    }
}