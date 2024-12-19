#include "gol.h"
#include "cli.h"

#include <chrono>
#include <random>
#include <iostream>

void tb_write(int x, int y, const char *str, uint16_t fg, uint16_t bg)
{
  for(int i = 0; str[i]; ++i)
  {
    tb_change_cell(x + i, y, str[i], fg, bg);
  }
}

int main(int argc, const char** argv)
{
  int width = -1, height = -1;
  int update_time = 250, reset_time = -1;
  CLI{}
    .on("--width", [&](std::string_view arg) {
      width = std::stoi(std::string(arg));
    }).on("--height", [&](std::string_view arg) {
      height = std::stoi(std::string(arg));
    }).on("--update-time", [&](std::string_view arg) {
      update_time = std::stoi(std::string(arg));
    }).on("--reset-time", [&](std::string_view arg) {
      reset_time = std::stoi(std::string(arg));
    }).on("--help", []() {
      std::cout << "Usage: gol [options]\n"
        "Options:\n"
        "  --width <integer>         Set the width of the field\n"
        "  --height <integer>        Set the height of the field\n"
        "  --update-time <integer>   Set the update time in milliseconds\n"
        "  --reset-time <integer>    Set the reset time in milliseconds\n"
        "  --help                    Print this message\n\n"
        "Controls:\n"
        "  l: toggle legend\n"
        "  a: toggle auto-update\n"
        "  r: randomize the game field\n"
        "  space: step the simulation\n";
      exit(0);
    }).parse(argc, argv);

  if(tb_init())
  {
    std::cerr << "Failed to initialize termbox" << std::endl;
    return 2;
  }

  if(width == -1)
    width = tb_width();
  if(height == -1)
    height = tb_height();

  tb_clear();
  tb_write(0, 0, "Loading", TB_WHITE, TB_BLACK);
  tb_present();
  GameOfLife field(width, height);

  // randomize field
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution dist(0, 1);
  bool randomize_ready = true;

  tb_event event;
  // int offset_y = 0;
  bool running = true;
  bool auto_update = true,
    show_legend = false;
  bool render_ready = true,
    update_ready = false;
  std::chrono::time_point<std::chrono::system_clock> last_reset;

  while(running)
  {
    switch(tb_peek_event(&event, update_time))
    {
    case 0:
      // peek timeout
      if(auto_update)
      {
        update_ready = true;
      }
      break;
    case -1:
      // error
      running = false;
      break;
    case TB_EVENT_KEY:
      switch(event.key)
      {
      case TB_KEY_ESC:
        running = false;
        break;
      case TB_KEY_SPACE:
        update_ready = true;
        break;
      case 0:
        // no special key, normal input
        switch(event.ch)
        {
        case 'l':
          show_legend = !show_legend;
          render_ready = true;
          break;
        case 'r':
          randomize_ready = true;
          break;
        case 'a':
          auto_update = !auto_update;
          break;
        default:
          break;
        }
        break;
      }
      break;
    }

    if(reset_time > -1 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_reset).count() > (long long)reset_time)
    {
      randomize_ready = true;
      last_reset = std::chrono::system_clock::now();
    }

    if(randomize_ready)
    {
      randomize_ready = false;
      field.reset([&](int, int) { return dist(gen); });
      update_ready = false;
      render_ready = true;
    }

    if(update_ready)
    {
      update_ready = false;
      field.update();
      render_ready = true;
    }

    if(render_ready)
    {
      render_ready = false;
      tb_clear();
      int offset_y = 0;
      if(show_legend)
      {
        offset_y = 1;
        tb_write(0, 0, "l: toggle legend, a: toggle auto-update, r: randomize, space: update step, esc: quit", TB_WHITE, TB_BLACK);
      }
      field.tb_render(0, offset_y, tb_width(), tb_height());
      tb_present();
    }
  }

  tb_shutdown();
  return 0;
}
