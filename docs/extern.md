# `extern`: declarar sem definir

> `extern` diz ao compilador: "essa variável (ou função) existe,
> mas não está aqui. Confie em mim, o linker vai encontrar".
> É a diferença entre **declarar** e **definir**.

## O problema: usar algo que está em outro arquivo

Em C, um programa pode ter **vários** arquivos `.c`. Cada um é
compilado **separadamente**, gerando um `.o` (objeto). O
**linker** junta os `.o` em um executável.

**Problema**: se o `main.c` usa uma variável ou função que está no
`getopt.c`, o compilador do `main.c` **não sabe** que ela existe.
Ele precisa de uma **declaração** (um "aviso" de que ela existe em
outro lugar).

**`extern`** é esse aviso.

## Declaração vs. definição: o coração do assunto

**Declaração**: diz ao compilador que algo **existe**, mas **não
aloca memória**. Pode aparecer **muitas vezes** (em vários
arquivos).

**Definição**: diz ao compilador que algo **existe** e **aloca
memória** (ou dá o corpo da função). Só pode aparecer **uma vez**
(regra do "one definition rule").

**Exemplo com variável**:

```c
/* getopt.c */
int optind = 1;   /* DEFINIÇÃO: aloca memória, inicializa com 1 */
```

```c
/* getopt.h */
extern int optind;   /* DECLARAÇÃO: diz que existe, não aloca */
```

```c
/* main.c */
#include "getopt.h"
/* ... usa optind ... */
```

**O que acontece**:

1. O `getopt.c` é compilado. O compilador vê `int optind = 1;` e
   **aloca** 4 bytes (ou 8, dependendo da plataforma) no segmento
   de dados. Gera um símbolo **global** `optind` no `.o`.
2. O `main.c` é compilado. O compilador vê `extern int optind;` e
   **não aloca** nada. Só anota: "quando o linker rodar, procure
   `optind` em outro lugar".
3. O linker junta os `.o`. Encontra `optind` no `getopt.o`, e
   resolve a referência do `main.o`. **Funciona**.

**Se você esquecer o `extern`** no header:

```c
/* getopt.h */
int optind;   /* ⚠️ DEFINIÇÃO, não declaração! */
```

Aí **cada** arquivo que inclui o header **define** `optind`
(aloca memória). O linker vê **múltiplas definições** de `optind`,
e reclama:

```
multiple definition of `optind'
```

**Esse é o erro mais comum** com `extern`: esquecer o `extern` no
header, e ter múltiplas definições.

## `extern` em variáveis globais

O uso mais comum. No `getopt.h`:

```c
extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;
```

No `getopt.c`:

```c
char *optarg = NULL;
int optind = 1;
int opterr = 1;
int optopt = 0;
```

**O header tem `extern`** (declaração). **O `.c` tem a definição**
(sem `extern`, com inicialização).

**Por que o header tem `extern`**: para que **todos** os arquivos
que incluem o header saibam que essas variáveis existem, **sem**
defini-las. A definição fica em **um** `.c` (o `getopt.c`).

**Inicialização**: a definição **pode** ter inicialização
(`int optind = 1;`). A declaração **não pode** (o `extern int
optind;` não tem `= 1`). Se o header tivesse `extern int optind =
1;`, o compilador reclamaria (ou o linker, dependendo do caso).

## `extern` em funções

Funções são **extern por padrão**. Ou seja:

```c
/* getopt.h */
int getopt(int argc, char *const argv[], const char *optstring);
```

Isso **já é** uma declaração `extern` (implícita). Adicionar
`extern` explicitamente é **redundante**:

```c
extern int getopt(...);   /* funciona, mas é redundante */
```

**Por que funções são extern por padrão**: o C padrão diz que
funções têm **ligação externa** (external linkage) a menos que
sejam `static`. Então, `extern` em funções é **opcional** e
**raro**.

**Quando `extern` em funções é usado**: em headers que são
compartilhados entre C e C++ (com `extern "C"`), ou por
**explicitação** (alguns programadores gostam de deixar claro).

**No CCS**: não usamos `extern` em funções (é redundante). Só em
variáveis globais (onde é **obrigatório**).

## `extern "C"`: para C++ (menção rápida)

Em C++, funções têm **name mangling** (o compilador muda o nome
da função para incluir os tipos dos parâmetros). Isso permite
**overload** (várias funções com o mesmo nome, tipos diferentes).

**Problema**: se um programa C++ usa uma função C, o nome
"mangled" não bate com o nome C. O linker não encontra.

**Solução**: `extern "C"`:

```cpp
extern "C" {
    #include "getopt.h"
}
```

Isso diz ao compilador C++: "trate essas funções como C (sem
mangling)". É **essencial** para usar bibliotecas C em C++.

**No CCS**: não usamos `extern "C"` (o CCS é só C). Mas é bom
saber que existe, porque **todo** header de biblioteca C tem:

```c
#ifdef __cplusplus
extern "C" {
#endif

/* ... declarações ... */

#ifdef __cplusplus
}
#endif
```

**Isso permite** que o header seja incluído em C **e** em C++. O
`005_getopt` **não** tem isso (porque é só C), mas se um dia
quisermos usar o `getopt` do CCS em C++, precisaríamos adicionar.

## O que acontece se você esquecer `extern`

### Caso 1: esquecer no header (variável)

```c
/* getopt.h */
int optind;   /* ⚠️ DEFINIÇÃO, não declaração */
```

**Resultado**: múltiplas definições. O linker reclama:

```
multiple definition of `optind'
```

**Solução**: adicionar `extern` no header.

### Caso 2: esquecer no header (função)

```c
/* getopt.h */
int getopt(...);   /* OK: funções são extern por padrão */
```

**Resultado**: **funciona**. Funções são extern por padrão, então
não precisa de `extern`.

### Caso 3: esquecer a definição

```c
/* getopt.h */
extern int optind;   /* declaração */

/* getopt.c */
/* ... esqueceu de definir optind ... */
```

**Resultado**: undefined reference. O linker reclama:

```
undefined reference to `optind'
```

**Solução**: definir `optind` em **algum** `.c`.

### Caso 4: definir no header (com `extern`)

```c
/* getopt.h */
extern int optind = 1;   /* ⚠️ declaração COM inicialização */
```

**Resultado**: o compilador pode aceitar (dependendo do caso), mas
é **erro conceitual**. A declaração não deve ter inicialização. O
linker pode reclamar, ou pode funcionar por acidente. **Não
faça**.

## O `extern` no `005_getopt`

O `getopt.h` tem:

```c
extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;
```

E o `getopt.c` tem:

```c
char *optarg = NULL;
int optind = 1;
int opterr = 1;
int optopt = 0;
```

**Isso é o padrão**: declaração no header (com `extern`),
definição no `.c` (sem `extern`, com inicialização).

**Por que o `optind` começa em 1**: o `argv[0]` é o nome do
programa, e o parsing começa em `argv[1]`. Ver
[argc-argv.md](argc-argv.md).

## Resumo

| Onde | O que é | Exemplo |
|---|---|---|
| **Header** (`.h`) | Declaração com `extern` | `extern int optind;` |
| **Fonte** (`.c`) | Definição sem `extern` | `int optind = 1;` |
| **Funções** | Extern por padrão (sem `extern`) | `int getopt(...);` |

**Regras práticas**:

1. **Variáveis globais**: `extern` no header, definição no `.c`.
2. **Funções**: sem `extern` (é o padrão).
3. **Nunca** inicialize no header com `extern`.
4. **Nunca** defina no header sem `extern`.
5. **Se o linker reclamar** de "multiple definition", procure um
   header com definição (sem `extern`).
6. **Se o linker reclamar** de "undefined reference", procure uma
   declaração sem definição.

## Onde `extern` aparece no CCS

- **`005_getopt`**: `extern char *optarg;`, `extern int optind;`,
  `extern int opterr;`, `extern int optopt;` no `getopt.h`.
- **Outros projetos**: nenhum usa `extern` (as variáveis globais
  são `static` ou não existem). O `001_wc`, `002_cat`, `003_grep`,
  `004_linked_list` não têm variáveis globais compartilhadas entre
  arquivos.

**Por que o `getopt` usa**: o `getopt` é uma **interface** (o
chamador precisa de `optarg`, `optind`, etc.). As variáveis são
**globais por design** (POSIX). O `extern` permite que o header
declare, e o `.c` defina.

## Leitura recomendada

- *The C Programming Language*, Kernighan & Ritchie — seção 4.4
  ("Scope Rules") e 4.5 ("Header Files").
- *Modern C*, Jens Gustedt — capítulo sobre ligação (linkage).
- *Expert C Programming: Deep C Secrets*, Peter van der Linden —
  capítulo 4 ("Linkers and Loaders") explica o que o linker faz.
