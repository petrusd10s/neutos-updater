/* I wrote this header to be as portable as possible for other homebrew switch projects */
/* The only thing that needs to be changed will be the name / number of textures you want to load */
/* If you decide to use this header and add functions, please consider opening a pr for said functions */
/* I would greatly appreaciate it :) */


#include <SDL2/SDL_image.h>
#include <switch.h>

#include "sdl.h"

static SDL_Window *main_window;
static SDL_Renderer *main_renderer;

SDL_Colour colours[] = {
    { 255, 255, 255 },          //white
    { 140, 140, 140, 255 },     //grey
    { 0, 0, 0, 255 },                //black
    { 255, 192, 203 },          //pink
    { 255, 165, 0 },            //orange
    { 255, 255, 0 },            //yellow
    { 139, 69, 19 },            //brown
    { 255, 0, 0 },              //red
    { 0, 128, 0 },              //green
    { 0, 191, 255 },            //light blue
    { 0, 0, 255 },              //blue
    { 28, 33, 73 },             //dark blue
    { 160, 32, 240 },           //purple
    { 81, 81, 81 },             //dark grey
    { 131, 177, 218 },          //jordy blue
    { 97, 115, 255 }            //faint blue
    };

SDL_Colour SDL_GetColour(int colour_option)
{
    return colours[colour_option];
}

SDL_Window *SDL_GetWindow(void)
{
    return main_window;
}

void clearRenderer()
{
    SDL_RenderClear(SDL_GetRenderer(SDL_GetWindow()));
    SDL_RenderCopy(main_renderer, background, NULL, NULL);
}

void updateRenderer()
{
    SDL_RenderPresent(main_renderer);
}

void imageLoad(SDL_Texture **texture, char *path)
{
	SDL_Surface *Surface = IMG_Load(path);
    SDL_ConvertSurfaceFormat(Surface, SDL_PIXELFORMAT_RGBA8888, 0);
    *texture = SDL_CreateTextureFromSurface(main_renderer, Surface);
	SDL_FreeSurface(Surface);
}

void imageLoadMem(SDL_Texture **texture, void *data, int size)
{
	SDL_Surface *surface = IMG_Load_RW(SDL_RWFromMem(data, size), 1);
	if (surface) *texture = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
}

void drawText(TTF_Font *font, int x, int y, SDL_Color colour, const char *text)
{
    SDL_Surface *Surface = TTF_RenderText_Blended_Wrapped(font, text, colour, 1920);
    SDL_Texture *Tex = SDL_CreateTextureFromSurface(main_renderer, Surface);
    SDL_Rect pos = { pos.x = x, pos.y = y, pos.w = Surface ->w, pos.h = Surface->h };

    SDL_RenderCopy(main_renderer, Tex, NULL, &pos);
    SDL_DestroyTexture(Tex);
    SDL_FreeSurface(Surface);
}

void drawImage(SDL_Texture *texture, int x, int y)
{
    SDL_Rect pos = { pos.x = x, pos.y = y };
	SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
	SDL_RenderCopy(main_renderer, texture, NULL, &pos);
}

void drawImageScale(SDL_Texture *texture, int x, int y, int w, int h)
{
    SDL_Rect pos = { pos.x = x, pos.y = y, pos.w = w, pos.h = h };
	SDL_RenderCopy(main_renderer, texture, NULL, &pos);
}

void drawShape(SDL_Colour colour, int x, int y, int w, int h)
{
    SDL_Rect pos = { pos.x = x, pos.y = y, pos.w = w, pos.h = h };
    SDL_SetRenderDrawColor(main_renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderFillRect(main_renderer, &pos);
}

void loadFonts()
{
    // Get the font from system
    PlFontData font;
    plGetSharedFontByType(&font, PlSharedFontType_Standard);

    // Load that font fro memory
    SDL_RWops *RWops = SDL_RWFromMem(font.address, font.size);

    fntSmall    = TTF_OpenFontRW(RWops, 1, 28);
    fntMedium   = TTF_OpenFontRW(RWops, 1, 36);
    fntLarge    = TTF_OpenFontRW(RWops, 1, 48);
}

void loadTextures()
{
    imageLoad(&background,      "romfs:/ams_background.png");
    imageLoad(&app_icon,        "romfs:/app_icon.png");
    imageLoad(&ams_icon,        "romfs:/ams_icon.png");
    imageLoad(&ams_plus_icon,   "romfs:/ams_plus_icon.png");
    imageLoad(&reboot_icon,     "romfs:/reboot_icon.png");
    imageLoad(&hekate_icon,     "romfs:/hekate_icon.png");
}

void destroyTextures()
{
    // only need to close one as all point to the same file...
    TTF_CloseFont(fntSmall);

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(app_icon);
    SDL_DestroyTexture(ams_icon);
    SDL_DestroyTexture(ams_plus_icon);
    SDL_DestroyTexture(reboot_icon);
    SDL_DestroyTexture(hekate_icon);
}

void sdlInit()
{
    TTF_Init();
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    main_window = SDL_CreateWindow("totaljustice", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);
    main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    //SDL_SetRenderDrawBlendMode(main_renderer, SDL_BLENDMODE_MOD);
    
    // highest quality
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    loadFonts();
    loadTextures();
}

void sdlExit()
{
    destroyTextures();
    SDL_DestroyRenderer(main_renderer);
    SDL_DestroyWindow(main_window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}