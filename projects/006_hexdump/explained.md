# Explicação detalhada do hexdump

Explicação **linha por linha** do `hexdump.c` e do `main.c`, para
quem está começando em C e quer entender **o que cada linha faz**
e **por quê**. Não substitui o `notes.md` (que é sobre decisões);
aqui o foco é **didático**.

## 1. `hexdump.c`

### `#include "hexdump.h"`

```c
#include "hexdump.h"
```

**O que faz**: inclui o header do próprio módulo. O `hexdump.c`
precisa do `HexdumpOptions` e do protótipo de `hexdump_stream`.

**Por que o header vem primeiro**: convenção do CCS (e do
`docs/style-guide.md`): o `.c` inclui seu próprio `.h` **antes**
dos headers do sistema. Isso pega erros de dependência escondida
(se o `.h` não for autossuficiente, o compilador reclama aqui).

### `#define BLOCK 16`

```c
#define BLOCK 16
```

**O que faz**: define uma **macro** que expande para `16`. É o
tamanho do bloco lido por vez.

**Por que 16**: é o padrão do `hexdump -C` (8 bytes hex + espaço
+ 8 bytes hex = 16 bytes por linha). Poderia ser 8, 32, 64 — cada
um tem trade-offs. 16 é o "sweet spot" (legível e alinhado com
bytes).

**Por que `#define` e não `const int`**: `#define` é uma **macro
de pré-processador** (substituição textual). `const int` seria uma
variável (com tipo). Para constantes de tamanho de array
(`unsigned char buf[BLOCK]`), `#define` é o que o C padrão
garante (C99 tem VLA, mas `#define` é mais portável).

### `is_printable`

```c
static int is_printable(unsigned char c)
{
    /* 32 = espaco (0x20), 126 = ~ (0x7E) */
    return c >= 32 && c <= 126;
}
```

**`static`**: a função só é visível **dentro do `hexdump.c`**. É
um helper interno (não está no `.h`). Ver
[`docs/static.md`](../../docs/static.md).

**`unsigned char c`**: o parâmetro é `unsigned char`, não
`char`. Por quê? `char` pode ser **signed** (dependendo da
plataforma), e valores > 127 viram **negativos**. Comparar
`c >= 32` com `char` signed daria resultados **errados** para
bytes > 127. `unsigned char` garante **0 a 255**. Ver
[`notes.md`](notes.md#por-que-unsigned-char-no-buffer-e-no-is_printable)
para a explicação completa.

**`32` e `126`**: a faixa **imprimível** em ASCII.

- `32` = espaço (o primeiro imprimível).
- `126` = `~` (o último imprimível).
- Abaixo de `32`: caracteres de **controle** (NULL, tab, newline,
  etc.), não imprimíveis.
- Acima de `126`: `127` (DEL) e `128`–`255` (estendidos, acentos
  em UTF-8, etc.), não imprimíveis em ASCII puro.

**`return c >= 32 && c <= 126`**: retorna 1 (verdadeiro) se `c`
está na faixa, 0 (falso) caso contrário.

**Por que não `isprint()`**: `isprint` depende de **locale** (em
alguns locales, acentos são imprimíveis). Para **portabilidade**,
a verificação manual é mais previsível.

### `print_line`

```c
static void print_line(unsigned long offset,
                       const unsigned char *buf, size_t n,
                       FILE *out)
{
    fprintf(out, "%08lx  ", offset);
```

**`unsigned long offset`**: o offset pode ser grande (arquivos de
GB). `unsigned long` é o tipo que `fseek`/`ftell` usam.

**`const unsigned char *buf`**: o buffer de bytes. `const` porque
`print_line` **não modifica** o buffer (só lê).

**`size_t n`**: o número de bytes **válidos** no buffer. Na
última linha, `n` pode ser < 16 (se o arquivo não for múltiplo de
16).

**`FILE *out`**: o stream de saída (geralmente `stdout`, mas pode
ser um arquivo).

**`fprintf(out, "%08lx  ", offset)`**: imprime o offset em
**hex**, 8 dígitos, zero-padded, seguido de **dois espaços**.

- `%08lx` = `l` (long), `x` (hex), `08` (8 dígitos, zero-padded).
- Ex.: offset `0` → `00000000`; offset `16` → `00000010`.
- Os **dois espaços** separam o offset dos bytes.

```c
    for (size_t i = 0; i < BLOCK; i++) {
        if (i == 8)
            fputc(' ', out);   /* espaço extra no meio */
```

**O loop** percorre os 16 bytes (mesmo que `n < 16`).

**`if (i == 8) fputc(' ', out)`**: um **espaço extra** no meio
(depois do 8º byte). É o que o `hexdump -C` faz: dois grupos de 8
bytes, separados por espaço. Visualmente, ajuda a contar os
bytes.

```c
        if (i < n)
            fprintf(out, "%02x ", buf[i]);
        else
            fprintf(out, "   ");   /* padding para bytes faltantes */
    }
```

**`if (i < n) fprintf(out, "%02x ", buf[i])`**: se o byte é
**válido** (`i < n`), imprime em hex, 2 dígitos, zero-padded,
seguido de espaço.

- `%02x` = `x` (hex), `02` (2 dígitos, zero-padded).
- Ex.: `0x0A` → `0a`; `0xFF` → `ff`; `0x00` → `00`.

**`else fprintf(out, "   ")`**: se o byte **não é válido**
(`i >= n`, na última linha), imprime **3 espaços** (padding).
Isso **alinha** a coluna ASCII, mesmo que a última linha tenha
menos de 16 bytes.

**Exemplo**: arquivo com 5 bytes (`"Hello"`):

```
00000000  48 65 6c 6c 6f                              |Hello|
```

Os 11 bytes faltantes viram 3 espaços cada (33 espaços),
alinhando o `|Hello|`.

```c
    fputc('|', out);
    for (size_t i = 0; i < n; i++) {
        unsigned char c = buf[i];
        fputc(is_printable(c) ? c : '.', out);
    }
    fputc('|', out);
    fputc('\n', out);
}
```

**`fputc('|', out)`**: abre a coluna ASCII com `|`.

**O loop** percorre os `n` bytes **válidos** (não os 16).

**`unsigned char c = buf[i]`**: o byte atual, como `unsigned
char` (para `is_printable` funcionar corretamente).

**`fputc(is_printable(c) ? c : '.', out)`**: se o byte é
imprimível, imprime o **caractere**; senão, imprime `.`. É o que
o `hexdump -C` faz. Usa o **operador ternário** (ver
[`docs/conditional-operator.md`](../../docs/conditional-operator.md)).

**`fputc('|', out)`**: fecha a coluna ASCII.

**`fputc('\n', out)`**: nova linha.

### `hexdump_stream`

```c
int hexdump_stream(FILE *in, FILE *out, const HexdumpOptions *opts)
{
    if (opts->skip > 0) {
        if (fseek(in, (long)opts->skip, SEEK_SET) != 0) {
            fprintf(stderr, "hexdump: nao foi possivel pular %lu bytes\n",
                    opts->skip);
            return -1;
        }
    }
```

**`if (opts->skip > 0)`**: se o usuário passou `-s N` (pular N
bytes).

**`fseek(in, (long)opts->skip, SEEK_SET)`**: move o cursor para
`skip` bytes a partir do **início** (`SEEK_SET`).

- **`(long)opts->skip`**: `fseek` espera `long`, não `unsigned
  long`. O cast evita warning (embora possa truncar em `long` de
  32 bits, mas offsets > 2 GB são raros).

**`if (... != 0)`**: `fseek` retorna `0` em sucesso, não-zero em
erro. Se falhar (ex.: `stdin` não é buscável), imprime erro e
retorna `-1`. Esse é o padrão
[`if (chamada() != 0)`](../../docs/error-check-pattern.md).

**Por que `fseek` pode falhar**: em **pipes** e **stdin**, o
stream não é **buscável** (não dá para "voltar" ou "pular"). O
`fseek` retorna erro. O `notes.md` registra a limitação.

```c
    unsigned char buf[BLOCK];
    unsigned long offset = opts->skip;
    unsigned long total = 0;
    size_t n;
```

**`unsigned char buf[BLOCK]`**: o buffer de 16 bytes.
**`unsigned char`** (não `char`), porque bytes são 0–255, e
`char` signed pode dar problema.

**`offset = opts->skip`**: o offset **inicial** é o `skip`. Se o
usuário pulou 100 bytes, o primeiro offset mostrado é `100` (em
hex: `00000064`).

**`total = 0`**: o total de bytes **lidos** (para o `-n`).
Diferente de `offset` (que é a posição no arquivo).

**`n`**: o número de bytes lidos no `fread` atual.

```c
    while ((n = fread(buf, 1, BLOCK, in)) > 0) {
        if (opts->limit > 0 && total + n > opts->limit)
            n = (size_t)(opts->limit - total);
```

**`fread(buf, 1, BLOCK, in)`**: lê até **16 elementos** de **1
byte** cada. Retorna o número de **elementos** lidos (= número
de bytes).

**`while (... > 0)`**: continua enquanto houver bytes. Quando
`fread` retorna 0 (EOF), o loop para.

**`if (opts->limit > 0 && total + n > opts->limit)`**: se há
limite (`-n N`) e o `total + n` ultrapassa o limite, **ajusta
`n`** para ler só o necessário.

**Exemplo**: `-n 10`, arquivo com 16 bytes. Primeira iteração:
`fread` lê 16, mas `total + n = 0 + 16 = 16 > 10`. Ajusta
`n = 10 - 0 = 10`. Lê 10 bytes. Segunda iteração: `total = 10`,
`fread` lê 6, mas `total + n = 16 > 10`. Ajusta `n = 10 - 10 =
0`. O loop para.

```c
        print_line(offset, buf, n, out);

        offset += n;
        total += n;

        if (opts->limit > 0 && total >= opts->limit)
            break;
    }
```

**`print_line(...)`**: imprime a linha.

**`offset += n`**: avança o offset pelo número de bytes lidos.

**`total += n`**: avança o total.

**`if (opts->limit > 0 && total >= opts->limit) break`**: se o
limite foi atingido, para.

```c
    if (ferror(in)) {
        fprintf(stderr, "hexdump: erro de leitura\n");
        return -1;
    }

    return 0;
}
```

**`ferror(in)`**: verifica se houve **erro de I/O** durante o
`fread` (não EOF, mas erro de leitura). Se sim, imprime erro e
retorna `-1`. Ver
[`docs/ferror.md`](../../docs/ferror.md).

**`return 0`**: sucesso.

## Resumo do `hexdump.c`

- **`#define BLOCK 16`**: tamanho do bloco.
- **`is_printable`**: verificação manual (decimal) da faixa
  imprimível ASCII.
- **`print_line`**: formata uma linha (offset, hex, ASCII, com
  padding).
- **`hexdump_stream`**: lê blocos, aplica `-s`/`-n`, e chama
  `print_line`.


## 2. `main.c`

### Includes

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hexdump.h"
```

**`<stdio.h>`**: para `printf`, `fprintf`, `fopen`, `fclose`,
`fread`, `stdin`, `stdout`, `stderr`.

**`<stdlib.h>`**: para `strtoul`, `exit` (não usamos `exit`, mas
`strtoul` precisa).

**`<string.h>`**: para `strcmp` (não usamos `strcmp` no `main`
do `hexdump`, mas o parsing manual poderia precisar; no nosso
caso, não usamos, então **pode remover** o `#include
<string.h>`). **Nota**: o `main.c` do `hexdump` **não usa**
`strcmp` (o parsing é por `switch (arg[1])`). O `#include
<string.h>` é **redundante**. Mas o `-Wall -Wextra` não reclama
de include não usado (só de variável/função não usada). **Pode
remover** para limpar.

**`"hexdump.h"`**: o header do módulo, com `HexdumpOptions` e
`hexdump_stream`.

### `print_help`

```c
static void print_help(const char *prog)
{
    printf(
        "Uso: %s [OPCOES] [ARQUIVO]\n"
        ...
        prog, prog, prog, prog, prog
    );
}
```

**`static`**: a função só é visível no `main.c`.

**`const char *prog`**: o nome do programa (`argv[0]`), para as
mensagens de ajuda. `const` porque não modificamos.

**`printf(...)`**: imprime o help. O `%s` é substituído por
`prog` (5 vezes, uma para cada linha que usa o nome do programa).

**Por que `prog` e não `argv[0]` direto**: a função é
**reutilizável** (poderia ser chamada de outro lugar, com outro
nome). Passar `prog` como parâmetro é mais limpo que acessar
`argv[0]` dentro da função (que exigiria passar `argv` inteiro).

### `parse_ulong`

```c
static int parse_ulong(const char *s, unsigned long *out)
{
    char *end;
    unsigned long v = strtoul(s, &end, 10);

    while (*end == ' ' || *end == '\t')
        end++;

    if (*end != '\0' || end == s)
        return -1;

    *out = v;
    return 0;
}
```

**`static`**: helper interno do `main.c`.

**`const char *s`**: a string a converter.

**`unsigned long *out`**: parâmetro de **saída** (o valor
convertido). O `*out` é modificado, e o chamador vê o resultado.
É o padrão "retorno por ponteiro" (o `return` é para o status,
não para o valor).

**`strtoul(s, &end, 10)`**: converte `s` para `unsigned long`, na
base 10. O `end` recebe um ponteiro para o primeiro caractere
**não consumido**.

**`while (*end == ' ' || *end == '\t') end++`**: pula espaços e
tabs no final. Isso permite `-s 16 ` (com espaço) sem erro.

**`if (*end != '\0' || end == s) return -1`**: duas verificações:

1. **`*end != '\0'`**: sobrou lixo depois do número (`"16abc"`).
2. **`end == s`**: nada foi convertido (`"abc"`, `""`).

Se qualquer uma for verdadeira, retorna `-1` (erro).

**`*out = v`**: atribui o valor convertido ao parâmetro de saída.

**`return 0`**: sucesso.

### `main`

```c
int main(int argc, char **argv)
{
    HexdumpOptions opts = {0};
    const char *path = NULL;
```

**`argc`/`argv`**: os argumentos da linha de comando (ver
[`docs/argc-argv.md`](../../docs/argc-argv.md)).

**`HexdumpOptions opts = {0}`**: inicializa a struct com **todos
os campos zero**. `{0}` é o "initializer" do C: o primeiro campo
recebe 0, e os demais também (porque o C preenche o resto com
zero). Isso é **essencial**: sem `{0}`, os campos teriam **lixo**
da pilha.

**`const char *path = NULL`**: o caminho do arquivo. `NULL`
significa "sem arquivo" (lê de stdin).

```c
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
```

**O loop** percorre `argv` (começando em 1, porque `argv[0]` é o
nome do programa).

**`const char *arg = argv[i]`**: o argumento atual.

```c
        if (arg[0] != '-' || arg[1] == '\0') {
            path = arg;
            continue;
        }
```

**Se não é flag**: `arg[0] != '-'` (não começa com `-`) **ou**
`arg[1] == '\0'` (é `-` sozinho, que é stdin). Nesse caso, é o
arquivo (`path = arg`), e `continue` pula para o próximo.

**Por que `-` sozinho não é flag**: convenção Unix (`-` = stdin).

```c
        switch (arg[1]) {
        case 's':
            ...
        case 'n':
            ...
        case 'h':
            ...
        default:
            ...
        }
```

**`arg[1]`**: o **segundo** caractere (o primeiro é `-`). Para
`-s`, `arg[1]` é `'s'`. Para `-n`, `'n'`. Para `-h`, `'h'`.

**`switch`**: despacha para o caso correspondente. Se não for
nenhum dos conhecidos, cai no `default` (erro).

### Caso `-s`

```c
        case 's':
            if (i + 1 >= argc) {
                fprintf(stderr, "%s: -s requer um numero\n", argv[0]);
                return 1;
            }
            if (parse_ulong(argv[++i], &opts.skip) != 0) {
                fprintf(stderr, "%s: -s: numero invalido\n", argv[0]);
                return 1;
            }
            break;
```

**`if (i + 1 >= argc)`**: não há próximo argumento. O `-s`
**requer** um número. Se não há, erro.

**`parse_ulong(argv[++i], &opts.skip)`**: converte o **próximo**
argumento (`argv[i+1]`) para `unsigned long`, e armazena em
`opts.skip`. O `++i` **avança** o índice (o próximo argumento é
o número, não uma flag).

**`if (... != 0)`**: se `parse_ulong` falhar (número inválido),
erro.

**`break`**: sai do `switch`, e o `for` continua.

### Caso `-n`

Igual ao `-s`, mas armazena em `opts.limit`.

### Caso `-h`

```c
        case 'h':
            print_help(argv[0]);
            return 0;
```

**`print_help(argv[0])`**: mostra o help.

**`return 0`**: sai com **sucesso** (o usuário pediu ajuda, não é
erro).

### Caso `default` (flag inválida)

```c
        default:
            fprintf(stderr, "%s: opcao invalida -- '%c'\n",
                    argv[0], arg[1]);
            fprintf(stderr, "Tente '%s -h' para ajuda.\n", argv[0]);
            return 1;
```

**`fprintf(stderr, ...)`**: imprime a mensagem de erro em
`stderr` (não `stdout`).

**`return 1`**: sai com **erro** (código 1).

### Abertura do arquivo

```c
    FILE *in;
    int is_stdin = (path == NULL);

    if (is_stdin) {
        in = stdin;
    } else {
        in = fopen(path, "rb");
        if (!in) {
            fprintf(stderr, "%s: %s: nao foi possivel abrir\n",
                    argv[0], path);
            return 1;
        }
    }
```

**`is_stdin`**: verdadeiro se o usuário **não** passou arquivo.

**`in = stdin`**: se não há arquivo, lê de stdin.

**`in = fopen(path, "rb")`**: abre o arquivo em modo
**binário** (`"rb"`). O `b` é essencial no Windows (sem ele, o
`\r\n` é traduzido, e o hexdump mostra bytes diferentes do
arquivo real).

**`if (!in)`**: se `fopen` falhar (retorna `NULL`), erro.

### Execução e fechamento

```c
    int rc = hexdump_stream(in, stdout, &opts);

    if (!is_stdin)
        fclose(in);

    return (rc == 0) ? 0 : 1;
}
```

**`hexdump_stream(in, stdout, &opts)`**: chama a função principal
do módulo. O `stdout` é o stream de saída.

**`if (!is_stdin) fclose(in)`**: fecha o arquivo **só se não for
stdin**. Fechar `stdin` é erro (o sistema fecha no fim).

**`return (rc == 0) ? 0 : 1`**: usa o **operador ternário**. Se
`hexdump_stream` retornou 0 (sucesso), retorna 0; senão, 1. Ver
[`docs/conditional-operator.md`](../../docs/conditional-operator.md).

## Resumo do `main.c`

- **Parsing manual de flags**: `-s`, `-n` (com argumento), `-h`,
  e flag inválida.
- **`parse_ulong`**: converte string para `unsigned long`, com
  validação.
- **Abertura do arquivo**: `fopen(path, "rb")` (binário), ou
  `stdin` se não houver arquivo.
- **Chamada do módulo**: `hexdump_stream(in, stdout, &opts)`.
- **Fechamento**: `fclose` só se não for stdin.
- **Código de retorno**: 0 (sucesso) ou 1 (erro), via operador
  ternário.
