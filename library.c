#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

int fileDescriptor, total_size;
color_t *buffer;
struct fb_var_screeninfo varScreenInfo;
struct fb_fix_screeninfo fixScreenInfo;
struct termios settings;

void init_graphics()
{
    fileDescriptor = open("/dev/fb0", O_RDWR);

    ioctl(fileDescriptor, FBIOGET_VSCREENINFO,&varScreenInfo);
    ioctl(fileDescriptor, FBIOGET_FSCREENINFO,&fixScreenInfo);
    total_size = varScreenInfo.yres_virtual * fixScreenInfo.line_length;
    // Map Our Buffer
    buffer = (color_t*)mmap(NULL, total_size, PROT_READ|PROT_WRITE, MAP_SHARED, fileDescriptor, 0);
    // Clear Screen
    write(STDIN_FILENO, "\033[2J", 8);

    // Disable Keypress Echo
    ioctl(STDIN_FILENO,TCGETS,&settings);
    settings.c_lflag &= ~ICANON;
    settings.c_lflag &= ~ECHO;
    ioctl(STDIN_FILENO,TCSETS,&settings);
}

void exit_graphics()
{
  // Clear Screen
  write(STDIN_FILENO, "\033[2J", 8);
  // Enable Keypress Echo
  ioctl(STDIN_FILENO,TCGETS,&settings);
  settings.c_lflag |= ECHO;
  settings.c_lflag |= ICANON;
  ioctl(STDIN_FILENO, TCSETS, &settings);
  // Unmap Our Buffer
  munmap(buffer,total_size);
  close(fileDescriptor);
}

// Linux Select Man Page(2)
char getkey()
{
  fd_set rfds;
  struct timeval tv;
  int retval;
  FD_ZERO(&rfds);
  FD_SET(0,&rfds);

  // Waiting only 1 second instead of 5
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  retval = select(1, &rfds, NULL, NULL, &tv);

  // Default value of key
  char key = '\0';

  if(retval!=-1 && retval)
    read(STDIN_FILENO, &key, sizeof(key));
  return key;
}
void sleep_ms(long ms)
{
  // Only sleep for positive values
  if(ms > 0)
  {
    struct timespec tv;
    // tv_nsec has a max value, so split into tv_nsec and tv_sec
    tv.tv_sec = ms / 1000;
    tv.tv_nsec = (ms%1000) * 1000000;
    nanosleep(&tv, NULL);
  }
}

void clear_screen(void *img)
{
  color_t *off = (color_t *)img;
  int x;
  // Loop through every index of off, setting each to 0
  for(x = 0;x<total_size/2;x++)
  {
    buffer[x] = 0;
    off[x] = 0;
  }
}

void draw_pixel(void *img, int x, int y, color_t color)
{
  // Don't draw if either the x coordinate or the y coordinate is illegal
  if(x<0||y<0||x>=varScreenInfo.xres_virtual||y>=varScreenInfo.yres_virtual)
    return;
  color_t *off = (color_t*) img;
  // Set pixel at (x,y) to be color, index is x + y * 640
  off[x+(y * varScreenInfo.xres_virtual)] = color;
}

// Rosetta Code
//https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
void draw_line(void *img, int x1, int y1, int x2, int y2, color_t c)
{
  int dx, dy, sx, sy, err, e2;
  if(x1 < x2)
  {
    dx = x2 - x1;
    sx = 1;
  }
  else
  {
    dx = x1 - x2;
    sx = -1;
  }
  if(y1 < y2)
  {
    dy = y2 - y1;
    sy = 1;
  }
  else
  {
    dy = y1 - y2;
    sy = -1;
  }
  if(dx > dy)
  {
    err = dx/2;
  }
  else err = -dy/2;
  for(;;)
  {
    draw_pixel(img, x1, y1, c);
    if (x1==x2 && y1==y2)
      break;
    e2 = err;
    if (e2 >-dx)
    {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dy)
    {
      err += dx;
      y1 += sy;
    }
  }
}

void *new_offscreen_buffer()
{
  return mmap(NULL, total_size, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void blit(void *src)
{
  color_t *off = (color_t *)src;
  int x;
  for(x = 0; x < total_size/2; x++)
  {
    buffer[x] = off[x];
  }
}
