#include <stdio.h>

void printSpecialChar (const char* const str)
{
  printf("%c%s", 27, str);
  fflush(stdout);
}

void clearScreen ()
{
  printf("\033[H\033[J");
}
void gotoxy (const int x, const int y)
{
  printf("\033[%d;%dH", (y), (x));
}

void clearLine ()
{
  printSpecialChar("[2K\r");
  fflush(stdout);
}

void saveCursorPos ()
{
  printSpecialChar("7");
}

void restoreCursorPos ()
{
  printSpecialChar("8");
}