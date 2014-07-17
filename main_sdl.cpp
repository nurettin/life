#include <iostream>
#include <sstream>
#include <fstream>
#include <SDL2/SDL.h>
#include "board.h"

void render_board(SDL_Renderer* ctx, Board const &board, int color)
{
  // render board
  for(int x= 0; x< board.width; ++ x)
    for(int y= 0; y< board.height; ++ y)
    {
      if(board.board[x][y]== 1) 
        SDL_SetRenderDrawColor(ctx, 255, 255, 255, 0);
      else
        SDL_SetRenderDrawColor(ctx, 0, 0, 0, 0);
      SDL_RenderDrawPoint(ctx, x, y);
    }
  SDL_RenderPresent(ctx);
}

#include <json/json.h>

int main()
{
  Json::Value root;
  Json::Reader reader;
  std::ifstream config("config.json");
  std::ostringstream strm_config;
  strm_config<< config.rdbuf();
  
  // configure
  int delay= 100; // 100ms
  int color= 100; // 20-255
  int dilution= 50; // 1-100
  if(reader.parse(strm_config.str(), root))
  {
    delay= root["delay"].asInt();
    color= root["color"].asInt();
    dilution= root["dilution"].asInt();
  }

  // initialize graphics
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window* win= SDL_CreateWindow("Game of Life", 0, 0, 800, 600, SDL_WINDOW_FULLSCREEN_DESKTOP);
  SDL_Renderer* rend= SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC);

  // initialize board
  int width= 0, height= 0;
  SDL_GetWindowSize(win, &width, &height);
  Board board(width, height);
  board.do_seed(dilution); // decoupled from initialization to allow unit testing
 
  SDL_Event e;
  for(bool run= true; run; SDL_Delay(delay))
  {
    while(SDL_PollEvent(&e))
      if(e.type== SDL_KEYDOWN|| e.type== SDL_QUIT)
      {
        run= false;
        break;
      }
    render_board(rend, board, color);
    board.do_next();
  }

  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();
}

