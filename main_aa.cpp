#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <fstream>
#include <aalib.h>
#include "board.h"

void render_board(aa_context* ctx, Board const &board, int color)
{
  // render board
  for(int x= 0; x< board.width; ++ x)
    for(int y= 0; y< board.height; ++ y)
      aa_putpixel(ctx, x, y, board.board[x][y]* color);
  aa_fastrender(ctx, 0, 0, board.width, board.height);
  aa_puts(ctx, 0, 0, AA_SPECIAL, "Press any key to stop");
  aa_flush(ctx);
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
  int dilution= 5; // 1-100
  if(reader.parse(strm_config.str(), root))
  {
    delay= root["delay"].asInt();
    color= root["color"].asInt();
    dilution= root["dilution"].asInt();
  }

  // initialize graphics
  auto ctx= gc(aa_autoinit(&aa_defparams), aa_close);
  // initialize keyboard handling
  aa_autoinitkbd(ctx.get(), 0);
  
  // initialize board
  Board board(aa_scrwidth(ctx.get()), aa_scrheight(ctx.get()));
  board.do_seed(dilution); // decoupled from initialization to allow unit testing
  
  while(aa_getevent(ctx.get(), 0)== AA_NONE)
  {
    render_board(ctx.get(), board, color);
    board.do_next();
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
  }
}

