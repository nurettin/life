#ifndef BOARD_H
#define BOARD_H

#include <memory>
#include <vector>
#include <random>

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
#endif

