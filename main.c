#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))


struct winsize w;

int stop = 0;

void clearLine () {
    printf("%c[2K", 27);
    printf("\r");
    fflush(stdout);
}

void printString (char* str) {
    printf("%s", str);
    fflush(stdout);
}

void saveCursorPos () {
    printf("%c7", 27);
    fflush(stdout);
}

void restoreCursorPos () {
    printf("%c8", 27);
    fflush(stdout);
}

void *drawProgress(void *null)
{
    int width = w.ws_col;
    int height = w.ws_row;
    int n = 0;
    while (stop == 0) {
        usleep(20000);
        saveCursorPos();
        gotoxy(height, n);
        n++;
        if (n > width - 1) {
            n = 0;
            clearLine();
        }
        printString("#");
        restoreCursorPos();
    }
    return NULL;

}

int main () {
    pthread_t drawProgress_thread;
    int c;

    clear();

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if(pthread_create(&drawProgress_thread, NULL, drawProgress, NULL)) {

        fprintf(stderr, "Error creating thread\n");
        return 1;

    }
    if(pthread_join(drawProgress_thread, NULL)) {

        fprintf(stderr, "Error joining thread\n");
        return 2;

    }
    while((c = getchar()) != EOF) {
        putchar(c);
    }
    return 0;
}