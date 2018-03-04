#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <ctype.h>
#include "ascii_symbols.h"
#include "term_commands.h"
#include "tv100_commands.h"
#include <stdbool.h>

struct winsize w;

int stop = 0;


void printString (const char* const str)
{
  printf("%s", str);
  fflush(stdout);
}


void redrawPrevLines (const char matrix[1000][1000], const int currentLine)
{
  const int width = w.ws_col;
  saveCursorPos();
  gotoxy(0, 0);
  clearLine();
  for (int i = 1; i <= currentLine; ++i) {
    gotoxy(0, i);
    clearLine();
    for (int j = 1; j <= width; ++j) {
      gotoxy(j, i);
      printf("%c", matrix[i][j]);
    }
  }
  restoreCursorPos();
}

void *drawProgress (void *null)
{
  const int width = w.ws_col;
  int currentRow = 0;
  while (stop == false) {
    usleep(20000);
    saveCursorPos();
    // Moves carret one line up and to start of line
    printSpecialChar("[1A\r");
    currentRow++;
    if (currentRow > width - 1) {
      currentRow = 0;
      clearLine();
    }
    for (int i = 0; i < currentRow; ++i) {
      printString("#");
    }
    restoreCursorPos();
  }
  return NULL;
}

void *proceedInput (void *null)
{
  int c;
  // Matrix shpuld be limited to width and height
  // But unfortunantely for now it will be 1000 * 1000
  char charMatrix[1000][1000];
  int currentLine = 1;
  int currentRow = 1;

  int quitSeqIter = 0;
  while((c = getchar()) != EOF) {
    printf("%c", c);
    int lastIndxInQuitSeq = sizeof(QUIT) - 1;
    if (tolower(c) == QUIT[quitSeqIter] || c == ESC || c == CANCEL) {
      if (quitSeqIter == lastIndxInQuitSeq || c == ESC || c == CANCEL) {
        stop = true;
        printf("\n\r");
        return NULL;
      }
      quitSeqIter++;
    } else {
      quitSeqIter = 0;
    }
    if (c == NEW_LINE) {
      currentLine++;
      redrawPrevLines(charMatrix, currentLine);
      currentRow = 1;
    } else if (c == BACKSPACE) {
      printSpecialChar("[2D");
      printf(" ");
      printSpecialChar("[1D");
      charMatrix[currentLine][currentRow - 1] = '\0';
      currentRow--;
    } else {
      charMatrix[currentLine][currentRow] = c;
      currentRow++;
    }
}
  return NULL;
}

int main () {
  pthread_t drawProgress_thread;
  pthread_t proceedInput_thread;
  disableTerminalEcho();
  clearScreen();
  // Moves carret one line down
  printSpecialChar("[1B");
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  if(pthread_create(&drawProgress_thread, NULL, drawProgress, NULL)) {
    fprintf(stderr, "Error creating thread drawProgress\n");
    enableTerminalEcho();
    return 1;
  }
  if(pthread_create(&proceedInput_thread, NULL, proceedInput, NULL)) {
    fprintf(stderr, "Error creating thread proceedInput_thread\n");
    enableTerminalEcho();
    return 1;
  }
  if(pthread_join(proceedInput_thread, NULL)) {
    fprintf(stderr, "Error joining thread proceedInput_thread\n");
    enableTerminalEcho();
    return 2;
  }
  enableTerminalEcho();
  return 0;
}