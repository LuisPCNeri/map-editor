#include "utils.hpp"

namespace Utils {
    std::string getAssetPath(const std::string &relativePath){
        char* basePath = SDL_GetBasePath();
        std::string fullPath = "";

        if (basePath) {
            fullPath = std::string(basePath) + relativePath;
            SDL_free(basePath);
        } else {
            fullPath = relativePath;
        }

        return fullPath;
    }
}
