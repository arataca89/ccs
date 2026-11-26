#ifndef TERMINAL_H
#define TERMINAL_H

/* Teclas especiais (valores fora da faixa ASCII, para não
 * confundir com letras). */
#define KEY_UP     1000
#define KEY_DOWN   1001
#define KEY_LEFT   1002
#define KEY_RIGHT  1003

/* Retorna o tamanho do terminal (em colunas e linhas).
 *
 * `cols` e `rows` são parâmetros de saída: a função escreve neles
 * o número de colunas e linhas do terminal.
 *
 * Retorna 0 em sucesso, -1 se não for possível obter o tamanho
 * (ex.: `stdout` redirecionado para um arquivo).
 *
 * Windows: usa `GetConsoleScreenBufferInfo`.
 * Linux/macOS: usa `ioctl` com `TIOCGWINSZ`.
 */
int get_terminal_size(int *cols, int *rows);

/* Habilita o modo ANSI (Virtual Terminal) no Windows.
 *
 * No Windows 10+, o console não interpreta códigos ANSI (como
 * `\033[92m`) por padrão. Esta função habilita o modo VT com
 * `SetConsoleMode`.
 *
 * No Linux/macOS, é uma função vazia (no-op), porque o terminal
 * já interpreta ANSI.
 */
void enable_ansi(void);

/* Lê uma tecla sem bloquear (não-bloqueante).
 *
 * Retorna o código da tecla (0-255), ou -1 se nenhuma tecla
 * estiver disponível.
 *
 * Windows: usa `_kbhit` e `_getch` (de `<conio.h>`).
 * Linux/macOS: configura o terminal em modo não-canônico
 * (desliga `ICANON` e `ECHO`) e usa `read` com `VMIN = 0`.
 */
int read_key(void);

/* Dorme por `ms` milissegundos.
 *
 * Windows: usa `Sleep` (de `<windows.h>`).
 * Linux/macOS: usa `usleep` (de `<unistd.h>`).
 */
void sleep_ms(int ms);

/* Restaura o terminal ao estado original.
 *
 * No Linux/macOS, restaura o `termios` salvo por `read_key`
 * (que desligou `ICANON` e `ECHO`). Sem isso, o terminal fica
 * em modo não-canônico após o programa terminar.
 *
 * No Windows, é uma função vazia (no-op), porque `_getch` não
 * altera o terminal permanentemente.
 *
 * Chamar no final do programa (e em handlers de `SIGINT`, se
 * houver).
 */
void restore_terminal(void);

#endif /* TERMINAL_H */