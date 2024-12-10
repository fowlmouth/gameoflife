#include "gol.h"
#include "cli.h"

#include <random>

void tb_write(int x, int y, const char *str, uint16_t fg, uint16_t bg)
{
  for (int i = 0; str[i] != '\0'; ++i)
  {
    tb_change_cell(x + i, y, str[i], fg, bg);
  }
}

int main(int argc, const char** argv)
{
  int width = 1000, height = 1000;
  CLI{}
    .on("--width", [&](std::string_view arg) {
      width = std::stoi(std::string(arg));
    }).on("--height", [&](std::string_view arg) {
      height = std::stoi(std::string(arg));
    }).parse(argc, argv);

  tb_init();
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
  bool auto_update = true;
  bool render_ready = true,
    update_ready = false;
  std::string top_message;

  while(running)
  {
    switch(tb_peek_event(&event, 50))
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
        case 'r':
          randomize_ready = true;
          break;
        default:
          break;
        }
        break;
      }
      break;
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
      tb_write(0, 0, top_message.c_str(), TB_WHITE, TB_BLACK);
      field.tb_render(0, 0, tb_width(), tb_height());
      tb_present();
    }
  }
  tb_shutdown();
  return 0;
}
