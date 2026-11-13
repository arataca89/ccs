# `<stddef.h>` — definições padrão

Tipos e macros usados em **toda** a biblioteca C. Pequeno, mas
fundamental.

## Tipos

### `size_t`

**Header**: `<stddef.h>` (e em outras, como `<stdlib.h>`,
`<string.h>`, `<stdio.h>`).

**O que é**: um tipo inteiro **sem sinal** (`unsigned`), usado
para representar **tamanhos** e **índices** de objetos em memória.
É o tipo que `sizeof` retorna, e o que `malloc`, `memcpy`,
`strlen`, `fread`, `fwrite` esperam.

**Por que existe**: o tamanho máximo de um objeto depende da
plataforma. Em 32-bit, `size_t` tem **32 bits** e, por ser
**unsigned**, vai de 0 a **4 GB**. Em 64-bit, tem **64 bits** e
vai de 0 a **16 EB** (exabytes).

**Por que não usar `int`**: `int` é **signed** (com sinal), então
em 32 bits vai de **-2 GB a +2 GB** (o bit mais significativo é
usado para o sinal). Se o objeto tem **3 GB**, o `int` **não
representa** o tamanho (estoura, e o valor fica negativo ou
errado). O `size_t`, por ser unsigned, representa até **4 GB** em
32 bits.

**Resumindo**: `size_t` (unsigned, 4 GB em 32-bit) ≠ `int`
(signed, 2 GB em 32-bit). O `size_t` usa **todos** os 32 bits
para o valor; o `int` usa 31 bits para o valor e 1 bit para o
sinal.

**Onde usamos**: `003` (`read_line`, `grow_buffer`), `004`
(`list_size`).

**Cuidados**: `size_t` é **unsigned**. Misturar com `int` gera
warnings (`-Wsign-compare`) e, às vezes, bugs (a comparação
`-1 < strlen(s)` é **falsa** em C, porque `-1` é convertido para
`SIZE_MAX`). Ver
[notes.md do 003_grep](../../projects/003_grep/notes.md#size_t-o-tipo-que-a-maioria-dos-livros-esquece).

**Ver também**: [`NULL`](#null), [`malloc`](stdlib.md#malloc),
[`strlen`](string.md#strlen).

## Macros

### `NULL`

**Header**: `<stddef.h>` (e em outras).

**O que é**: uma macro que expande para um **ponteiro nulo**
(tipicamente `((void *)0)` ou `0`). Usada para indicar "nenhum
ponteiro válido".

**Onde usamos**: todos os projetos.

**Cuidados**: `NULL` é um **ponteiro**, não um inteiro. `int x =
NULL;` é **erro conceitual** (embora o compilador possa aceitar
com warning). Use `0` para inteiros, `NULL` para ponteiros.

**Ver também**: [`size_t`](#size_t).
