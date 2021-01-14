#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "./constants.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *image = NULL;
SDL_Texture *texture = NULL;
SDL_Rect r;

bool quit = false;
int last_frame_time = 0;

struct speed
{
  int x;
  int y;
} speed;

void setup();
void update();
void render();
void cleanup();
void process_input();
bool initialize_window();

int main(int argc, char *argv[])
{
  quit = initialize_window();

  setup();

  // Main loop init
  while (!quit)
  {
    process_input();
    update();
    render();
  }

  // clean up resources before exiting
  cleanup();
  return 0;
}

void destroy_window()
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

bool initialize_window()
{
  // attempt to initialize graphics and timer system
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
  {
    printf("error initializing SDL: %s\n", SDL_GetError());
    return true;
  }

  window = SDL_CreateWindow("DVD",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            WINDOW_WIDTH,
                            WINDOW_HEIGHT,
                            0);
  if (!window)
  {
    printf("error creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return true;
  }

  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  // create a renderer, which sets up the graphics hardware
  renderer = SDL_CreateRenderer(window, -1, render_flags);
  if (!renderer)
  {
    printf("error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return true;
  }

  // load the image into memory using SDL_image library function
  image = IMG_Load("resources/dvd_logo.png");
  if (!image)
  {
    printf("error creating image\n");
    destroy_window();
    return true;
  }

  // load the image data into the graphics hardware's memory
  texture = SDL_CreateTextureFromSurface(renderer, image);
  SDL_FreeSurface(image);

  // connects texture with r
  SDL_QueryTexture(texture, NULL, NULL, &r.w, &r.h);

  if (!texture)
  {
    printf("error creating texture: %s\n", SDL_GetError());
    destroy_window();
    return true;
  }

  return false;
}

void set_texture_color()
{
  int r = rand() % 255;
  int g = rand() % 255;
  int b = rand() % 255;
  // Modulate texture
  SDL_SetTextureColorMod(texture, r, g, b);
}

void setup()
{
  // set initial speed of object
  speed.x = 2;
  speed.y = 2;

  // sets initial x-position of object
  r.x = (WINDOW_WIDTH - r.w) / 2;
  // sets initial y-position of object
  r.y = (WINDOW_HEIGHT - r.h) / 2;
}

void process_input()
{
  SDL_Event event;
  SDL_PollEvent(&event);

  // Events mangement
  switch (event.type)
  {

  case SDL_QUIT:
    quit = true;
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE)
    {
      quit = true;
    }
    break;
  }
}

void update()
{
  // Sleep the execution until we reach the target frame time in milliseconds
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

  // Only call delay if we are too fast too process this frame
  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
  {
    SDL_Delay(time_to_wait);
  }

  last_frame_time = SDL_GetTicks();

  r.x += speed.x;
  r.y += speed.y;

  if ((r.x + r.w) >= WINDOW_WIDTH)
  {
    speed.x = -speed.x;
    r.x = WINDOW_WIDTH - r.w;
    set_texture_color();
  }
  else if (r.x <= 0)
  {
    speed.x = -speed.x;
    r.x = 0;
    set_texture_color();
  }

  // bottom/upper boundary
  if ((r.y + r.h) >= WINDOW_HEIGHT)
  {
    speed.y = -speed.y;
    r.y = WINDOW_HEIGHT - r.h;
    set_texture_color();
  }
  else if (r.y <= 0)
  {
    speed.y = -speed.y;
    r.y = 0;
    set_texture_color();
  }
}

void render()
{
  // Set background to black color
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

  // Clears the window
  SDL_RenderClear(renderer);

  // Renders texture in the window
  SDL_RenderCopy(renderer, texture, NULL, &r);
  SDL_RenderPresent(renderer);
}

void cleanup()
{
  // Free loaded image
  SDL_DestroyTexture(texture);
  texture = NULL;

  // Destroy window
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  renderer = NULL;
  window = NULL;

  // Quit SDL subsystems
  SDL_Quit();
}
