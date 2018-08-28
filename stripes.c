#include "graphics.h"

int main(int argc, char **argv)
{
	init_graphics();

	void *buf = new_offscreen_buffer();
  int centerX = 319, centerY = 239, length = 20, x, y;
  int modx = 0, mody = 0, temp, work = 1;
  color_t color = RGB( 31, 31, 31);
	char key;
  do{
      key = getkey();
      if(key == 'q')
      {
        break;
      }
      if(key == '+' && length < 220) // Increase Length
      {
        length += 20;
        work = 1;
        clear_screen(buf);
      }
      else if(key == '-' && length > 20) // Decrease Length
      {
        length -= 20;
        work = 1;
        clear_screen(buf);
      }
      else if(key == 'r' && color != RGB(31,0,0)) // Set Color to Red
      {
        color = RGB(31,0,0);
        work = 1;
      }
      else if(key == 'g' && color != RGB(0,31,0)) // Set Color to Green
      {
        color = RGB(0,31,0);
        work = 1;
      }
      else if(key == 'b' && color != RGB(0,0,31)) // Set Color to Blue
      {
        color = RGB(0,0,31);
        work = 1;
      }
      else if (key == 'x') // Draw Horizontal Gridlines
      {
        modx = !modx;
        work = 1;
        clear_screen(buf);
      }
      else if (key == 'y') // Draw Vertical Gridlines
      {
        mody = !mody;
        work = 1;
        clear_screen(buf);
      }

      // Only run if something changed
      if(work)
      {
        // Actually Draw The Gridlines
        for(temp = 0;modx&&temp<480;temp+=20)
          draw_line(buf,0,temp,639,temp,color);
        for(temp = 0;mody&&temp<640;temp+=20)
          draw_line(buf,temp,0,temp,479,color);

        // Begin Drawing Cool Diamond
        for(y = centerY, x = centerX + length;;)
        {
            draw_line(buf, centerX, centerY, x, y, color);
            if(x == centerX + length - 1 && y == centerY + 1)
              break;
            else if(x > centerX && y <= centerY)
            {
              x--;
              y--;
            }
            else if(x < centerX && y >= centerY)
            {
              x++;
              y++;
            }
            else if(x <= centerX && y < centerY)
            {
                x--;
                y++;
            }
            else if( x >= centerX && y > centerY)
            {
                x++;
                y--;
            }
        }
      blit(buf);
      sleep_ms(500);
      }
      work = 0;
    }
  while(1);
  exit_graphics();
  return 0;
}
