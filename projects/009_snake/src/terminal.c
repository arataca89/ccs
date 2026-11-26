#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

int get_terminal_size(int *cols, int *rows)
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        return -1;

    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return 0;
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
        return -1;

    *cols = w.ws_col;
    *rows = w.ws_row;
    return 0;
#endif
}

void enable_ansi(void)
{
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode)) {
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(h, mode);
    }
#endif
}

#ifdef _WIN32

int read_key(void)
{
    if (!_kbhit())
        return -1;

    int c = _getch();

    /* Teclas especiais (setas, F1-F12) enviam dois bytes:
     * o primeiro é 0 ou 0xE0, o segundo é o código da tecla. */
    if (c == 0 || c == 0xE0) {
        int code = _getch();
        /* Retorna um valor único (fora da faixa ASCII) para
         * distinguir das letras normais. */
        switch (code) {
        case 72: return KEY_UP;      /* seta para cima */
        case 80: return KEY_DOWN;    /* seta para baixo */
        case 75: return KEY_LEFT;    /* seta para esquerda */
        case 77: return KEY_RIGHT;   /* seta para direita */
        default: return -1;          /* outra tecla especial */
        }
    }

    return c;   /* tecla normal (letra, número, etc.) */
}

void sleep_ms(int ms)
{
    Sleep((DWORD)ms);
}

#else

/* Estado do terminal (para restaurar no final). */
static struct termios old_termios;
static int termios_saved = 0;

/* Configura o terminal para modo não-canônico (sem Enter, sem eco). */
static void setup_terminal(void)
{
    if (termios_saved)
        return;

    tcgetattr(STDIN_FILENO, &old_termios);
    struct termios new_termios = old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);   /* desliga modo canônico e eco */
    new_termios.c_cc[VMIN] = 0;                /* leitura não-bloqueante */
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    termios_saved = 1;
}

int read_key(void)
{
    setup_terminal();

    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) != 1)
        return -1;

    /* Teclas especiais (setas) enviam 3 bytes: ESC [ letra. */
    if (c == 27) {   /* ESC */
        unsigned char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return -1;
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return -1;
        if (seq[0] == '[') {
            switch (seq[1]) {
            case 'A': return KEY_UP;
            case 'B': return KEY_DOWN;
            case 'C': return KEY_RIGHT;
            case 'D': return KEY_LEFT;
            }
        }
        return -1;   /* outra tecla especial */
    }

    return c;   /* tecla normal (WASD, Q, etc.) */
}

void sleep_ms(int ms)
{
    usleep((useconds_t)ms * 1000);
}

#endif

/* Restaura o terminal (chamado no final do programa). */
void restore_terminal(void)
{
#ifndef _WIN32
    if (termios_saved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
        termios_saved = 0;
    }
#endif
}