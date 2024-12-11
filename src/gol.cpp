#include "gol.h"

GameOfLife::GameOfLife(std::size_t width, std::size_t height)
: cells_(width * height), cells_buffer_(width * height),
  width_(width), height_(height)
{
}

bool GameOfLife::at(std::size_t x, std::size_t y)
{
  return cells_[y * width_ + x];
}

bool GameOfLife::at_checked(std::size_t x, std::size_t y, bool oob_value) const
{
  if(x < width_ && y < height_)
  {
    return cells_[y * width_ + x];
  }
  return oob_value;
}

void GameOfLife::set(std::size_t x, std::size_t y, bool value)
{
  cells_.at(y * width_ + x) = value;
}

std::size_t GameOfLife::width() const
{
  return width_;
}

std::size_t GameOfLife::height() const
{
  return height_;
}

void GameOfLife::reset(std::function< bool(int x, int y) > callback)
{
  for(std::size_t y = 0; y < height_; ++y)
  {
    for(std::size_t x = 0; x < width_; ++x)
    {
      cells_[y * width_ + x] = callback(x, y);
    }
  }
}

void GameOfLife::update()
{
  // update game of life cells
  for(std::size_t y = 0; y < height_; ++y)
  {
    for(std::size_t x = 0; x < width_; ++x)
    {
      int neighbors = 0;
      for(int dy = -1; dy <= 1; ++dy)
      {
        for(int dx = -1; dx <= 1; ++dx)
        {
          if(dx == 0 && dy == 0)
          {
            continue;
          }
          neighbors += at_checked(x + dx, y + dy, false) ? 1 : 0;
        }
      }
      cells_buffer_[y * width_ + x] = (neighbors == 3 || (neighbors == 2 && at(x, y)));
    }
  }

  cells_.swap(cells_buffer_);
}

void GameOfLife::tb_render(int offset_x, int offset_y, int width, int height)
{
  int max_y = std::min((std::size_t)height, height_);
  int max_x = std::min((std::size_t)width, width_);
  for(int y = 0; y < max_y; ++y)
  {
    for(int x = 0; x < max_x; ++x)
    {
      tb_change_cell(offset_x + x, offset_y + y,
        at(x, y) ? '#' : ' ', TB_WHITE, TB_BLACK);
    }
  }
}
