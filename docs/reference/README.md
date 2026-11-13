# Referência C do CCS

Referência das funções, macros e tipos da biblioteca padrão C
(mais algumas extensões POSIX/Windows) **usados nos projetos do
CCS**. O objetivo é ter tudo num só lugar, em português, para
quem não consegue (ou não quer) ler a documentação em inglês.

## Como usar

- Cada arquivo cobre um **header** da libc (`<stdio.h>`,
  `<stdlib.h>`, etc.).
- Cada função tem uma **ficha** com: header, assinatura, o que
  faz, retorno, onde usamos, cuidados.
- As fichas são **curtas**: o objetivo é consulta rápida, não
  substituir um livro.

## Índice

### [`<stdio.h>`](stdio.md) — entrada e saída

Usado em: `001`, `002`, `003`, `004`, `005`.

- Tipos: [`FILE`](stdio.md#file)
- Streams: [`stdin`](stdio.md#stdin), [`stdout`](stdio.md#stdout), [`stderr`](stdio.md#stderr)
- Macros: [`EOF`](stdio.md#eof)
- Funções: [`fopen`](stdio.md#fopen), [`fclose`](stdio.md#fclose), [`fgetc`](stdio.md#fgetc), [`fputc`](stdio.md#fputc), [`fgets`](stdio.md#fgets), [`fread`](stdio.md#fread), [`fwrite`](stdio.md#fwrite), [`printf`](stdio.md#printf), [`fprintf`](stdio.md#fprintf), [`putchar`](stdio.md#putchar), [`fseek`](stdio.md#fseek), [`ftell`](stdio.md#ftell), [`ferror`](stdio.md#ferror), [`fflush`](stdio.md#fflush)

### [`<stddef.h>`](stddef.md) — definições padrão

Usado em: `001`, `002`, `003`, `004`, `005`.

- Tipos: [`size_t`](stddef.md#size_t)
- Macros: [`NULL`](stddef.md#null)

### [`<stdlib.h>`](stdlib.md) — utilidades gerais

Usado em: `001`, `002`, `003`, `004`, `005`.

- Alocação: [`malloc`](stdlib.md#malloc), [`calloc`](stdlib.md#calloc), [`realloc`](stdlib.md#realloc), [`free`](stdlib.md#free)
- Conversão: [`strtol`](stdlib.md#strtol), [`atoi`](stdlib.md#atoi), [`getenv`](stdlib.md#getenv)
- Processo: [`exit`](stdlib.md#exit), [`EXIT_SUCCESS`](stdlib.md#exit_success), [`EXIT_FAILURE`](stdlib.md#exit_failure)
- Outras: [`qsort`](stdlib.md#qsort), [`bsearch`](stdlib.md#bsearch), [`rand`](stdlib.md#rand), [`srand`](stdlib.md#srand), [`abs`](stdlib.md#abs)

### [`<string.h>`](string.md) — strings e memória

Usado em: `002`, `003`, `004`, `005`.

- Strings: [`strlen`](string.md#strlen), [`strcmp`](string.md#strcmp), [`strncmp`](string.md#strncmp), [`strcpy`](string.md#strcpy), [`strncpy`](string.md#strncpy), [`strcat`](string.md#strcat), [`strchr`](string.md#strchr), [`strrchr`](string.md#strrchr), [`strstr`](string.md#strstr)
- Memória: [`memcpy`](string.md#memcpy), [`memmove`](string.md#memmove), [`memset`](string.md#memset), [`memcmp`](string.md#memcmp)
- Outras: [`strdup`](string.md#strdup), [`strtok`](string.md#strtok)

### [`<ctype.h>`](ctype.md) — classificação de caracteres

Usado em: `003`, `005`.

- Classificação: [`isdigit`](ctype.md#isdigit), [`isspace`](ctype.md#isspace), [`isalpha`](ctype.md#isalpha), [`isprint`](ctype.md#isprint)
- Conversão: [`tolower`](ctype.md#tolower), [`toupper`](ctype.md#toupper)

### POSIX e Windows

- [`<unistd.h>`](unistd.md) — POSIX. Usado em: `004`.
  - [`isatty`](unistd.md#isatty), [`fileno`](unistd.md#fileno)
- [`<io.h>`](io.md) — Windows. Usado em: `001`, `002`, `003`, `004`.
  - [`_isatty`](io.md#_isatty), [`_fileno`](io.md#_fileno), [`_setmode`](io.md#_setmode)
- [`<fcntl.h>`](fcntl.md) — Windows. Usado em: `001`, `002`, `003`.
  - [`_O_BINARY`](fcntl.md#_o_binary), [`_O_TEXT`](fcntl.md#_o_text)
- [`<errno.h>`](errno.md) — mencionado, não usado ainda.
  - [`errno`](errno.md#errno), [`ERANGE`](errno.md#erange)
