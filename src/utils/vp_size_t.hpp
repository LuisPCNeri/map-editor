#ifndef __VP_SIZE_T__
#define __VP_SIZE_T__

#include <SDL2/SDL.h>

extern SDL_Renderer* rend;

typedef struct vh_size_t {
    float percent;
    vh_size_t(float p) : percent(p) {};

    operator int() const {
        int w,h;
        SDL_GetRendererOutputSize(rend, &w, &h);
        return static_cast<int>((percent / 100.0f) * h);
    }
} vh_size_t;

typedef struct vw_size_t {
    float percent;
    vw_size_t(float p) : percent(p) {};

    operator int() const {
        int w,h;
        SDL_GetRendererOutputSize(rend, &w, &h);
        return static_cast<int>((percent / 100.0f) * w);
    }
} vw_size_t;


#endif