# Notas de implementação — 005_getopt

Registro do raciocínio por trás do código. Não é tutorial: é o
"porquê" das decisões, incluindo os tropeços.

## O que é `getopt` (e por que implementar)

`getopt` é uma função POSIX que faz **parsing de flags de linha
de comando**. Ela processa `argv` e retorna, a cada chamada, o
próximo caractere de flag. O chamador usa um `switch` para tratar
cada flag.

**Por que implementar**:

1. **Valor didático**: `getopt` é um dos algoritmos mais usados em
   C, e quase ninguém sabe como funciona por dentro. Implementar
   ensina parsing de flags, estado persistente, e variáveis
   globais.
2. **Reutilização**: os projetos seguintes do CCS podem usar o
   `getopt` do CCS em vez de fazer parsing manual. Isso reduz
   duplicação e demonstra a filosofia "ferramenta que constrói
   ferramentas".
3. **Portabilidade**: `getopt` não é C padrão. No Linux/macOS,
   está em `<unistd.h>`. No Windows/MSVC, **não existe**. Ter a
   nossa implementação garante portabilidade.

## A interface POSIX

O `getopt` real tem esta interface:

```c
int getopt(int argc, char *const argv[], const char *optstring);

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;
```

**Variáveis globais** (POSIX):

| Variável | O que é |
|---|---|
| `optarg` | O argumento da flag (se ela espera um) |
| `optind` | Índice do próximo argumento não-flag |
| `optopt` | A flag inválida (quando `getopt` retorna `'?'`) |
| `opterr` | Se 0, `getopt` não imprime erros (padrão: 1) |

**Retorno**:

- Caractere da flag (`'v'`, `'o'`) — flag válida.
- `-1` — fim das flags.
- `'?'` — flag inválida, ou argumento faltando.

## Por que variáveis globais (e não struct de estado)

O `getopt` real usa **variáveis globais** (`optarg`, `optind`,
`optopt`, `opterr`). Isso é uma escolha histórica (POSIX
especifica assim), e **não é thread-safe**.

**Duas opções**:

**(a)** Seguir o POSIX: variáveis globais. Compatível com o
`getopt` real, mas não thread-safe.

**(b)** Struct de estado: `struct GetoptState { char *optarg; int
optind; ... }`, e funções que recebem o estado. Thread-safe, mas
**não** compatível com o `getopt` real.

**Escolhemos (a)**, porque:

- É o que o POSIX especifica (compatibilidade).
- É o que o aluno vai encontrar em código real.
- O CCS não é multi-thread (por enquanto).
- O `notes.md` registra a limitação (não thread-safe) e a
  alternativa (struct de estado) como melhoria futura.

**A limitação**: duas threads chamando `getopt` ao mesmo tempo
corrompem o estado (as globais são compartilhadas). Para
thread-safety, seria preciso uma struct de estado passada por
parâmetro (como o `getopt_r` do BSD, ou o `getopt_long` com
estado).

## O estado interno: `static char *nextchar`

O `getopt` é chamado **repetidamente** (em um `while`). Entre
chamadas, ele precisa lembrar **onde parou dentro de uma flag
agrupada**.

**Exemplo**: `argv[optind]` é `"-vo"`. O `getopt` processa o `v`
na primeira chamada, e o `o` na segunda. Entre as chamadas, ele
precisa lembrar que ainda falta o `o`.

**A solução**: `static char *nextchar`.

```c
static char *nextchar = NULL;
```

**`nextchar`** aponta para o **próximo caractere** dentro da flag
atual:

- `nextchar == NULL`: não estamos no meio de uma flag agrupada.
  O `getopt` precisa avançar para o próximo `argv`.
- `nextchar != NULL` e `*nextchar != '\0'`: estamos no meio de
  uma flag agrupada. O `getopt` processa o próximo caractere.
- `nextchar != NULL` e `*nextchar == '\0'`: a flag agrupada
  terminou. O `getopt` avança para o próximo `argv`.

**Por que `static`**: o estado precisa **persistir** entre
chamadas. `nextchar` é interno ao `getopt.c` (não é visível fora),
mas mantém o valor entre chamadas. **Isso é o uso 3 de `static`**
(variável local que persiste), que o
[docs/static.md](../../docs/static.md) explica.

**Atenção**: `static` local em `getopt` é **global disfarçado**.
Se duas threads chamam `getopt` ao mesmo tempo, `nextchar` é
compartilhado, e o parsing quebra. É a mesma limitação do
`optind` global.

## O algoritmo, passo a passo

### 1. Resetar `optarg` e `optopt`

```c
optarg = NULL;
optopt = 0;
```

A cada chamada, `optarg` e `optopt` são resetados. O chamador não
deve confiar no valor anterior.

### 2. Avançar para o próximo argumento (se necessário)

```c
if (nextchar == NULL || *nextchar == '\0') {
    if (optind >= argc)
        return -1;

    if (argv[optind][0] != '-' || argv[optind][1] == '\0')
        return -1;

    if (strcmp(argv[optind], "--") == 0) {
        optind++;
        return -1;
    }

    nextchar = argv[optind] + 1;
    optind++;
}
```

**Quando**: se `nextchar` é `NULL` (não estamos no meio de uma
flag) ou `*nextchar == '\0'` (a flag agrupada terminou).

**O que faz**:

- **`optind >= argc`**: não há mais argumentos. Retorna `-1`
  (fim).
- **`argv[optind][0] != '-'`**: o próximo argumento não começa
  com `-`. Não é flag. Retorna `-1` (fim do parsing; o resto são
  argumentos).
- **`argv[optind][1] == '\0'`**: é `"-"` sozinho. Não é flag (é
  stdin, por convenção). Retorna `-1`.
- **`strcmp(argv[optind], "--") == 0`**: é `"--"`. Para o
  parsing. `optind++` (consome o `--`) e retorna `-1`.
- **Senão**: é uma flag. `nextchar = argv[optind] + 1` (aponta
  para o caractere depois do `-`). `optind++` (o próximo
  argumento será processado na próxima chamada).

**Por que `optind++` antes de processar a flag**: o `optind`
aponta para o **próximo** argumento a processar. Se a flag
consome o argumento inteiro (`-v`), o próximo argumento é o
seguinte. Se a flag consome só parte (`-vo`), o `nextchar` aponta
para o resto, e o `optind` já aponta para o próximo argumento.

### 3. Pegar o próximo caractere da flag

```c
int c = *nextchar++;
optopt = c;
```

**`c = *nextchar++`**: pega o caractere atual, e avança o
`nextchar`. **`optopt = c`**: guarda a flag (para o caso de ser
inválida).

### 4. Verificar se a flag existe e se espera argumento

```c
int expects = flag_expects_arg(optstring, c);
```

**`flag_expects_arg`**:

```c
static int flag_expects_arg(const char *optstring, int c)
{
    const char *p = strchr(optstring, c);
    if (!p)
        return -1;
    if (p[1] == ':')
        return 1;
    return 0;
}
```

- **`strchr(optstring, c)`**: procura o caractere na
  `optstring`. Se não achar, `p == NULL` → flag não existe →
  `-1`.
- **`p[1] == ':'`**: se o próximo caractere é `:`, a flag espera
  argumento → `1`.
- **Senão** → `0` (flag sem argumento).

### 5. Flag inválida

```c
if (expects < 0) {
    if (opterr)
        fprintf(stderr, "%s: opcao invalida -- '%c'\n",
                argv[0], c);
    return '?';
}
```

**`argv[0]`**: o nome do programa. A mensagem fica `"./prog:
opcao invalida -- 'x'"`. **`opterr`**: controla se imprime.
**`optopt`**: já foi setado (`optopt = c` antes).

**Por que `stderr` e não `stdout`**: mensagens de erro vão para
`stderr` (convenção Unix). Scripts podem redirecionar `stdout`
sem perder os erros.

### 6. Flag sem argumento

```c
if (expects == 0)
    return c;
```

Simples: retorna o caractere. `nextchar` já avançou (pode
apontar para a próxima flag agrupada, ou para `'\0'`).

### 7. Flag com argumento colado

```c
if (*nextchar != '\0') {
    optarg = nextchar;
    nextchar = NULL;
    return c;
}
```

**`*nextchar != '\0'`**: ainda há caracteres na flag atual. O
argumento está **colado** (`-oarquivo`). `optarg = nextchar`
(`"arquivo"`). `nextchar = NULL` (a flag terminou). Retorna `c`.

### 8. Flag com argumento separado

```c
if (optind >= argc) {
    if (opterr)
        fprintf(stderr, "%s: opcao requer argumento -- '%c'\n",
                argv[0], c);
    return '?';
}

optarg = argv[optind];
optind++;
nextchar = NULL;
return c;
```

**`optind >= argc`**: não há próximo argumento para ser o
`optarg`. Erro: "opção requer argumento". **Senão**: `optarg =
argv[optind]` (o próximo argumento é o `optarg`). `optind++` (o
próximo a processar). `nextchar = NULL` (a flag terminou).

## O que **não** foi implementado (e por quê)

### 1. `getopt_long` (flags longas)

Flags longas (`--verbose`, `--output=arquivo`) exigem outra
função (`getopt_long`), com uma struct de opções. É **muito**
mais complexo, e fica para um projeto futuro (sem número
definido).

**No CCS**: os projetos usam flags curtas. Se um dia precisarem
de longas, o `getopt_long` pode ser um projeto futuro.

### 2. `optstring` com `:` no início

Se a `optstring` começa com `:`, o `getopt` muda o comportamento
de erro: retorna `:` em vez de `?` para argumento faltando. É uma
variação rara, e **não implementamos**.

**Exemplo**: `":vo:"` — argumento faltando retorna `:`, não `?`.

### 3. Reordenação de `argv` (GNU `getopt`)

O `getopt` do GNU reordena os argumentos para que as flags venham
**antes** dos não-flags. Exemplo: `prog arquivo -v` é tratado
como `prog -v arquivo`. O POSIX **não** exige isso, e **não
implementamos** (o `optind` final aponta para o primeiro
argumento não-flag, sem reordenar).

**Implicação**: no nosso `getopt`, `prog arquivo -v` **para** o
parsing no `arquivo` (não é flag), e o `-v` fica como argumento.
Isso é o comportamento POSIX (não-GNU), e é **mais previsível**.

### 4. Thread-safety

As globais (`optarg`, `optind`, `optopt`, `opterr`) e o
`nextchar` (`static`) **não são thread-safe**. Duas threads
chamando `getopt` ao mesmo tempo corrompem o estado.

**Alternativa** (para um projeto futuro): struct de estado
(`struct GetoptState`), passada por parâmetro. É o que o `getopt_r`
do BSD faz.

## O que o `demo` faz

O `main.c` é uma **demonstração** que usa o `getopt` para
parsing de flags:

- `-v`: modo verboso (pode ser repetida: `-vv`).
- `-o ARQUIVO`: arquivo de saída (aceita `-o arquivo` ou
  `-oarquivo`).
- `-h`: ajuda.

**Por que `-v` pode ser repetida**: cada `v` incrementa
`verbose`. Isso é comum em CLIs (`-v`, `-vv`, `-vvv` = níveis de
verbosidade).

**Por que `-o` espera argumento**: a `optstring` é `"vo:h"`, e o
`:` depois do `o` indica que ele espera argumento.

**Por que `-h` sai com 0**: o help é um caso **terminal** (o
usuário pediu ajuda, o programa mostra e sai). O `exit(0)` (ou
`return 0`) é apropriado.

**Por que `?` sai com 1**: flag inválida é um **erro** (o
usuário errou). O `return 1` sinaliza falha.

## `opterr`: controlar mensagens de erro

O `getopt` imprime mensagens de erro por padrão (`opterr = 1`).
Se o chamador quiser silenciar (e imprimir a sua própria
mensagem), seta `opterr = 0` **antes** do loop:

```c
opterr = 0;
while ((opt = getopt(...)) != -1) { ... }
```

**No `demo`**, não setamos `opterr = 0`, então o `getopt`
imprime a mensagem de erro. O `main` adiciona a sugestão de `-h`.

**Quando silenciar**: se o programa quer uma mensagem de erro
**customizada** (ex.: traduzida, ou com mais contexto). O `getopt`
imprime "opcao invalida -- 'x'", e o `main` pode querer algo
diferente. `opterr = 0` desliga a mensagem do `getopt`, e o
`main` imprime a sua.

## Lições gerais deste projeto

1. **Variáveis globais** (`optarg`, `optind`, `optopt`, `opterr`)
   são o estado do `getopt`. **Não são thread-safe**, e o
   `notes.md` registra a limitação.
2. **Variáveis `static` locais** (`nextchar`) persistem entre
   chamadas. **Uso 3 de `static`** (ver
   [docs/static.md](../../docs/static.md)).
3. **`strchr`** para procurar a flag na `optstring`.
4. **Parsing de flags**: agrupadas (`-vo`), coladas (`-osaida`),
   separadas (`-o saida`), `--`, `-` sozinho.
5. **`optind`** é o índice do próximo argumento não-flag. O
   chamador usa `argv[optind..argc-1]` para os arquivos.
6. **`opterr`** controla mensagens de erro. O padrão é 1.
7. **O que não foi implementado**: `getopt_long`, `optstring`
   com `:`, reordenação de `argv`, thread-safety. Cada um fica
   para um projeto futuro (ou upgrade).
8. **Portabilidade**: `getopt` não é C padrão (no Windows/MSVC,
   não existe). Ter a nossa implementação garante portabilidade.
9. **A interface POSIX é global**: o chamador não passa estado,
   usa as globais. Isso é simples, mas limitado.
10. **O `demo` é uma demonstração**, não uma ferramenta. O
    objetivo é mostrar o `getopt` funcionando, não fazer algo
    útil.
