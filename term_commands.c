#include <termios.h>
#include <stdio.h>

struct termios term; 
 
void disableTerminalEcho ()
{
  tcgetattr(fileno(stdin), &term);
  term.c_lflag &= ~ICANON;
  term.c_lflag &= ~ECHO;
  tcsetattr(fileno(stdin), TCSANOW, &term); 
} 
 
void enableTerminalEcho ()
{
  term.c_lflag |= ICANON; 
  term.c_lflag |= ECHO; 
  tcsetattr(fileno(stdin), TCSANOW, &term); 
}