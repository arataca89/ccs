# `<stdio.h>` — entrada e saída

Funções e tipos para entrada/saída (I/O) em C. A maioria dos
projetos do CCS usa `<stdio.h>`.

## Tipos

### `FILE`

**Header**: `<stdio.h>`

**O que é**: uma struct opaca que representa um **stream** de
arquivo. O conteúdo é **implementation-defined** (não importa o
que tem dentro; você usa as funções, não os campos).

**Onde usamos**: todos os projetos.

## Streams padrão

### `stdin`

**Header**: `<stdio.h>`

**O que é**: o **stream de entrada padrão**. Por padrão, é o
teclado; pode ser redirecionado de um arquivo (`programa <
arquivo`) ou de um pipe (`echo "x" | programa`).

**Onde usamos**: `001`, `002`, `003`, `004`, `005`.

**Cuidados**: **não** feche `stdin` com `fclose`. O sistema fecha
no fim do programa.

### `stdout`

**Header**: `<stdio.h>`

**O que é**: o **stream de saída padrão**. Por padrão, é o
terminal; pode ser redirecionado para um arquivo (`programa >
arquivo`) ou para um pipe (`programa | outro`).

**Onde usamos**: todos os projetos.

**Cuidados**: **não** feche `stdout` com `fclose`. E, após um
prompt sem `\n`, use `fflush(stdout)` (ver [`fflush`](#fflush)).

### `stderr`

**Header**: `<stdio.h>`

**O que é**: o **stream de saída de erro**. Por padrão, é o
terminal; **não** é redirecionado junto com `stdout` (a menos que
você use `2>` ou `2>&1`).

**Onde usamos**: `002`, `003`, `004`, `005`.

**Cuidados**: use `stderr` para **mensagens de erro** (convenção
Unix). Scripts podem redirecionar `stdout` sem perder os erros.
**Não** feche `stderr` com `fclose`.

## Macros

### `EOF`

**Header**: `<stdio.h>`

**O que é**: uma **macro** que expande para uma constante
negativa (tipicamente `-1`), usada para indicar **fim de
arquivo** ou **erro de leitura**.

**Por que é `int`**: `fgetc` retorna `int` (não `char`), porque
precisa representar 256 valores de byte **mais** o `EOF`. Ver
[notes.md do 001_wc](../../projects/001_wc/notes.md#por-que-int-c-e-não-char-c).

**Onde usamos**: `001`, `002`, `003`, `004`.

## Funções

### `fopen`

**Header**: `<stdio.h>`

**Assinatura**: `FILE *fopen(const char *path, const char *mode);`

**O que faz**: abre um arquivo e retorna um `FILE *` (stream).

**Modos**: `"r"` (leitura), `"w"` (escrita, trunca), `"a"`
(append), `"r+"` (leitura/escrita), `"rb"`, `"wb"`, `"ab"`
(binário). O `b` é **ignorado** no Linux/macOS, mas **essencial**
no Windows (desliga a tradução `\r\n`).

**Retorno**: `FILE *` em sucesso, `NULL` em erro.

**Onde usamos**: `001`, `002`, `003`, `004`.

**Cuidados**: **sempre** verifique `NULL`. E **sempre** abra em
modo binário (`"rb"`, `"wb"`) se for copiar bytes fielmente.

### `fclose`

**Header**: `<stdio.h>`

**Assinatura**: `int fclose(FILE *stream);`

**O que faz**: fecha um stream, descarrega o buffer, e libera os
recursos.

**Retorno**: `0` em sucesso, `EOF` em erro.

**Onde usamos**: `001`, `002`, `003`, `004`.

**Cuidados**: **não** feche `stdin`/`stdout`/`stderr`. E **não**
use o `FILE *` depois de `fclose` (use-after-close).

### `fgetc`

**Header**: `<stdio.h>`

**Assinatura**: `int fgetc(FILE *stream);`

**O que faz**: lê **um caractere** do stream.

**Retorno**: o caractere (0–255), ou `EOF` em fim de arquivo/erro.

**Por que `int`**: precisa representar 256 valores + `EOF`.

**Onde usamos**: `001`, `002`, `003`.

**Cuidados**: **sempre** declare `int c`, não `char c`. Ver
[notes.md do 001_wc](../../projects/001_wc/notes.md#por-que-int-c-e-não-char-c).

### `fputc`

**Header**: `<stdio.h>`

**Assinatura**: `int fputc(int c, FILE *stream);`

**O que faz**: escreve **um caractere** no stream.

**Retorno**: o caractere escrito, ou `EOF` em erro.

**Onde usamos**: `002`, `006`.

### `fgets`

**Header**: `<stdio.h>`

**Assinatura**: `char *fgets(char *s, int size, FILE *stream);`

**O que faz**: lê **até `size - 1` caracteres** ou até `\n`
(inclusive), e armazena em `s`, terminando com `'\0'`.

**Retorno**: `s` em sucesso, `NULL` em EOF/erro.

**Onde usamos**: `004`.

**Cuidados**: **trunca** linhas maiores que `size - 1`. O `\n`
fica na string (se couber). Ver
[notes.md do 004](../../projects/004_linked_list/notes.md#fgets-com-buffer-fixo-vs-read_line-do-grep).

### `fread`

**Header**: `<stdio.h>`

**Assinatura**: `size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);`

**O que faz**: lê **`nmemb` elementos** de `size` bytes cada, e
armazena em `ptr`. Retorna o número de **elementos** lidos (não
bytes).

**Retorno**: o número de elementos lidos. Se `< nmemb`, houve
EOF ou erro.

**Onde usamos**: `006_hexdump`.

**Cuidados**: o retorno é em **elementos**, não bytes. Para ler
bytes, use `size = 1` e `nmemb = N` (retorna o número de bytes).

### `fwrite`

**Header**: `<stdio.h>`

**Assinatura**: `size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);`

**O que faz**: escreve **`nmemb` elementos** de `size` bytes cada.

**Retorno**: o número de elementos escritos.

**Onde usamos**: (será usado em projetos futuros).

### `printf`

**Header**: `<stdio.h>`

**Assinatura**: `int printf(const char *format, ...);`

**O que faz**: escreve em `stdout`, formatando com `format`.

**Retorno**: o número de caracteres escritos, ou negativo em erro.

**Onde usamos**: todos os projetos.

**Cuidados**: `%d` (int), `%u` (unsigned), `%ld` (long), `%lu`
(unsigned long), `%zu` (size_t), `%s` (string), `%c` (char),
`%x` (hex), `%02x` (hex com 2 dígitos, zero-padded), `%%`
(literal `%`).

### `fprintf`

**Header**: `<stdio.h>`

**Assinatura**: `int fprintf(FILE *stream, const char *format, ...);`

**O que faz**: como `printf`, mas escreve em `stream`.

**Onde usamos**: `002`, `003`, `004`, `005`.

**Cuidados**: use `fprintf(stderr, ...)` para mensagens de erro.

### `putchar`

**Header**: `<stdio.h>`

**Assinatura**: `int putchar(int c);`

**O que faz**: escreve **um caractere** em `stdout`.

**Retorno**: o caractere escrito, ou `EOF` em erro.

**Onde usamos**: `002`.

**Equivalente**: `fputc(c, stdout)`.

### `fseek`

**Header**: `<stdio.h>`

**Assinatura**: `int fseek(FILE *stream, long offset, int whence);`

**O que faz**: move o cursor do stream para `offset`, a partir de
`whence` (`SEEK_SET`, `SEEK_CUR`, `SEEK_END`).

**Retorno**: `0` em sucesso, não-zero em erro.

**Onde usamos**: `006_hexdump`.

**Cuidados**: só funciona em arquivos **buscáveis** (não em
pipes/stdin).

### `ftell`

**Header**: `<stdio.h>`

**Assinatura**: `long ftell(FILE *stream);`

**O que faz**: retorna a posição atual do cursor.

**Retorno**: a posição, ou `-1L` em erro.

**Onde usamos**: (será usado no `006_hexdump`).

**Cuidados**: como `fseek`, só funciona em arquivos buscáveis.

### `ferror`

**Header**: `<stdio.h>`

**Assinatura**: `int ferror(FILE *stream);`

**O que faz**: verifica se houve **erro de I/O** no stream.

**Retorno**: não-zero se houve erro, `0` caso contrário.

**Onde usamos**: `001`, `002`, `003`, `006`.

**Cuidados**: `ferror` **não** indica EOF; indica erro. Para
distinguir, use `feof` (não usamos ainda).

### `fflush`

**Header**: `<stdio.h>`

**Assinatura**: `int fflush(FILE *stream);`

**O que faz**: descarrega o buffer do stream para o dispositivo.

**Retorno**: `0` em sucesso, `EOF` em erro.

**Onde usamos**: `004` (após o prompt `> `, para ele aparecer
antes do `fgets`).

**Cuidados**: `fflush(stdout)` é **obrigatório** após um prompt
sem `\n`. Ver
[notes.md do 004](../../projects/004_linked_list/notes.md#fflushstdout-após-o-prompt).

---

arataca89@gmail.com

Aulas de programação
