#include "termbox.h"

#include <vector>
#include <algorithm>
#include <functional>

class GameOfLife
{
  std::vector< bool > cells_, cells_buffer_;
  std::size_t width_, height_;

public:
  GameOfLife(std::size_t width, std::size_t height);

  // bool* operator[](std::size_t y);
  bool at(std::size_t x, std::size_t y);
  bool at_checked(std::size_t x, std::size_t y, bool oob_value) const;
  void set(std::size_t x, std::size_t y, bool value);

  std::size_t width() const;
  std::size_t height() const;

  void reset(std::function< bool(int x, int y) > callback);

  void update();
  void tb_render(int offset_x, int offset_y, int width, int height);
};
