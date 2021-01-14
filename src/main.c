#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "./constants.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *surface = NULL;
SDL_Texture *texture = NULL;
SDL_Rect r;

int quit = 0;

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
int initialize_window();

int main(void)
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

int initialize_window()
{
  // attempt to initialize graphics and timer system
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
  {
    printf("error initializing SDL: %s\n", SDL_GetError());
    return 1;
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
    return 1;
  }

  // create a renderer, which sets up the graphics hardware
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer)
  {
    printf("error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // load the image into memory using SDL_image library function
  surface = IMG_Load("resources/logo.png");
  if (!surface)
  {
    printf("error creating surface\n");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // load the image data into the graphics hardware's memory
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  // connects texture with r
  SDL_QueryTexture(texture, NULL, NULL, &r.w, &r.h);

  if (!texture)
  {
    printf("error creating texture: %s\n", SDL_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  return 0;
}

void setup()
{
  // set initial speed of object
  speed.x = 10;
  speed.y = 10;

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
    quit = 1;
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE)
    {
      quit = 1;
    }
    break;
  }
}

void update()
{
  r.x += speed.x;
  r.y += speed.y;

  if ((r.x + r.w) >= WINDOW_WIDTH)
  {
    speed.x = -speed.x;
    r.x = WINDOW_WIDTH - r.w;
  }
  else if (r.x <= 0)
  {
    speed.x = -speed.x;
    r.x = 0;
  }

  // bottom/upper boundary
  if ((r.y + r.h) >= WINDOW_HEIGHT)
  {
    speed.y = -speed.y;
    r.y = WINDOW_HEIGHT - r.h;
  }
  else if (r.y <= 0)
  {
    speed.y = -speed.y;
    r.y = 0;
  }

  // Make delay for 60 FPS
  SDL_Delay(1000 / 60);
}

void render()
{
  // Set background to white
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

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
