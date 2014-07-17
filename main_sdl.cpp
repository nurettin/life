#include <memory>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <sstream>
#include <fstream>
#include <SDL2/SDL.h>

template <typename T, typename F>
std::unique_ptr<T, F> gc(T* t, F f)
{
  return std::unique_ptr<T, F>(t, f);
}

struct Board
{
  int width;
  int height;
  std::vector<std::vector<int>> board, next_board;

  Board(int width, int height)
  : width(width)
  , height(height)
  , board(width, std::vector<int>(height))
  , next_board(board)
  {}

  void do_seed(int dilution)
  {
    std::random_device randomizer;
    std::mt19937 flip(randomizer());
    std::uniform_int_distribution<> coin(0, dilution);
    for(int x= 0; x< width; ++ x)
      for(int y= 0; y< height; ++ y)
        board[x][y]= coin(flip)== 0;
    next_board= board;   
  }

  int count_neighbors(int x, int y) const
  {
    // within surrounding rectangle
    int sx= x- 1; if(sx< 0) sx= 0; 
    int ex= x+ 2; if(ex> width) ex= width;
    int sy= y- 1; if(sy< 0) sy= 0; 
    int ey= y+ 2; if(ey> height) ey= height;
    // count neighbors
    int count= 0;
    for(int ix= sx; ix< ex; ++ ix)
      for(int iy= sy; iy< ey; ++ iy)
        count+= board[ix][iy];
    // but don't count self
    count-= board[x][y];

    return count;
  }

  void do_next()
  {
    // compute next board
    for(int x= 0; x< width; ++ x)
      for(int y= 0; y< height; ++ y)
      {
        int count= count_neighbors(x, y);   
        // mark death or birth
        if(count< 2|| count> 3)
          next_board[x][y]= 0;
        else if(count== 3) 
          next_board[x][y]= 1;
      }
    board= next_board;
  }
};

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
