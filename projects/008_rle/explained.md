# Explicação detalhada do rle

Explicação **linha por linha** do `rle.c` e do `main.c`, para
quem está começando em C e quer entender **o que cada linha faz**
e **por quê**. Não substitui o `notes.md` (que é sobre decisões);
aqui o foco é **didático**.

## O que é RLE

**RLE** (Run-Length Encoding) é o algoritmo de compressão mais
simples que existe. Ele substitui **sequências repetidas** de um
byte por um par `(byte, contagem)`.

**Exemplo**:

```
Original:    A A A A A B B B C C
Comprimido:  (A,5) (B,3) (C,2)
```

- `AAAAA` (5 bytes) vira `(A,5)` (2 bytes).
- `BBB` (3 bytes) vira `(B,3)` (2 bytes).
- `CC` (2 bytes) vira `(C,2)` (2 bytes).

**Total**: 10 bytes → 6 bytes (40% de redução).

**Quando funciona bem**: dados com **muitas repetições** (imagens
com fundo uniforme, fax, ícones).

**Quando funciona mal**: dados **aleatórios** (sem repetição). Se
não há repetição, cada byte vira um par de 2 bytes, e o arquivo
**aumenta** (100% de aumento).

## O formato do arquivo comprimido

O `rle_compress` gera uma sequência de **pares de bytes**:

```
[byte][contagem][byte][contagem]...
```

- **`byte`**: o valor repetido (1 byte, 0–255).
- **`contagem`**: quantas vezes repetir (1 byte, 1–255).

**Exemplo**: `AAAAABBBCC` comprime para:

```
[A][5][B][3][C][2]
```

**Não há cabeçalho** (número mágico, versão, tamanho original). O
arquivo é só os pares. O `notes.md` registra que um cabeçalho
seria mais robusto, mas fica para depois.

**Limitação**: a contagem é 1 byte (máximo 255). Se uma sequência
tem mais de 255 repetições, o `rle_compress` **divide** em vários
pares:

```
500 repetições de A  →  (A,255) (A,255)
```

**Por que 1 byte**: simplicidade. A contagem em 2 bytes (até
65535) seria mais eficiente, mas mais complexa. O `notes.md`
registra.

## `rle.c`

### Include

```c
#include "rle.h"

#include <stdio.h>
```

**`"rle.h"`**: o header do módulo, com as assinaturas das
funções.

**`<stdio.h>`**: para `fgetc`, `fputc`, `ferror`, `fprintf`,
`EOF`.

### `#define MAX_COUNT 255`

```c
#define MAX_COUNT 255   /* a contagem é 1 byte (0-255) */
```

**`MAX_COUNT`**: o valor máximo que a contagem pode ter (255, o
máximo de um byte).

**Por que `#define` e não `const int`**: `#define` é uma **macro
de pré-processador** (substituição textual). Para constantes
usadas em comparações (`if (count == MAX_COUNT)`), `#define` é o
padrão. `const int` funcionaria, mas `#define` deixa claro que é
uma **constante de compilação**.

**Por que 255 e não 256**: um byte tem 8 bits, e o valor máximo é
`2^8 - 1 = 255`. A contagem vai de 1 a 255 (0 não é usado).

### `rle_compress`: a assinatura

```c
int rle_compress(FILE *out, FILE *in)
{
```

**`int`**: retorna 0 (sucesso) ou -1 (erro).

**`FILE *out, FILE *in`**: o **destino** antes da **origem**,
seguindo a convenção da biblioteca C (`strcpy(dest, src)`,
`memcpy(dest, src, n)`).

### `rle_compress`: as variáveis

```c
    int current;
    int previous;
```

**`current`**: o byte atual (lido agora).

**`previous`**: o byte anterior (o que estamos contando).

**Por que `int` e não `char`**: `fgetc` retorna `int` (não
`char`), porque precisa representar 256 valores de byte **mais**
o `EOF` (negativo). Ver
[`notes.md` do `001_wc`](../../projects/001_wc/notes.md#por-que-int-c-e-não-char-c).

### `rle_compress`: o primeiro byte

```c
    previous = fgetc(in);
    if (previous == EOF) {
        if (ferror(in))
            return -1;
        return 0;
    }
```

**`previous = fgetc(in)`**: lê o **primeiro** byte do arquivo.
Ele é o "previous" (o que vamos contar).

**`if (previous == EOF)`**: se o arquivo está **vazio** (ou houve
erro).

**`if (ferror(in)) return -1`**: se houve **erro de leitura**,
retorna -1. O `ferror` distingue EOF (arquivo vazio) de erro.
Ver [`docs/ferror.md`](../../docs/ferror.md).

**`return 0`**: se o arquivo está vazio (sem erro), retorna 0
(sucesso, nada a comprimir).

**Por que ler o primeiro byte antes do loop**: o loop precisa de
um `previous` para comparar com o `current`. Sem o primeiro byte,
o `previous` seria indefinido.

### `rle_compress`: o contador

```c
    unsigned count = 1;
```

**`count`**: quantas vezes o `previous` apareceu até agora.
Começa em **1** (o `previous` já foi lido uma vez).

**`unsigned`**: a contagem é não-negativa (1–255). `unsigned`
deixa claro.

### `rle_compress`: o loop principal

```c
    while ((current = fgetc(in)) != EOF) {
```

**`while ((current = fgetc(in)) != EOF)`**: lê o próximo byte, e
continua enquanto não for EOF.

**A atribuição dentro do `while`**: `current = fgetc(in)` é
avaliada **antes** da comparação. Se `fgetc` retorna `EOF`, o
loop para. Isso é idiomático em C (e evita ler o `fgetc` duas
vezes).

```c
        if (current == previous) {
            count++;
```

**Se `current == previous`**: a sequência continua. Incrementa
`count`.

```c
            if (count == MAX_COUNT) {
                if (fputc(previous, out) == EOF) return -1;
                if (fputc((int)count, out) == EOF) return -1;
                count = 0;
            }
```

**Se `count` chegou a 255**: a sequência atingiu o máximo. Emite
o par `(previous, count)`, e **zera** `count`.

**`fputc(previous, out)`**: escreve o byte.

**`fputc((int)count, out)`**: escreve a contagem. O `(int)` evita
warning (o `count` é `unsigned`).

**`count = 0`**: por quê? Porque o `current` que disparou o
`count++` **já foi lido**, mas **ainda não foi emitido**. Se
zerarmos `count`, o próximo `current` (se for igual a
`previous`) fará `count++` para 1, e a sequência recomeça.

**Exemplo**: 260 repetições de `A`.

- `count` chega a 255 → emite `(A, 255)`, `count = 0`.
- Próximo `current` (ainda `A`): `current == previous` → `count++`
  → 1.
- Mais 4 repetições: `count` chega a 5.
- Fim do arquivo: emite `(A, 5)`.

**Total**: `(A, 255)(A, 5)` = 260 repetições. ✅

```c
        } else {
            if (fputc(previous, out) == EOF) return -1;
            if (fputc((int)count, out) == EOF) return -1;
            previous = current;
            count = 1;
        }
```

**Se `current != previous`**: a sequência terminou. Emite o par
`(previous, count)`, e **recomeça** com `current` e `count = 1`.

**`previous = current`**: o novo `previous` é o `current`.

**`count = 1`**: o `current` já foi lido uma vez.

### `rle_compress`: o `ferror` e o último par

```c
    if (ferror(in))
        return -1;
```

**`if (ferror(in)) return -1`**: se houve **erro de leitura**
(não EOF), retorna -1. O loop terminou, mas pode ter sido por
erro.

```c
    if (count > 0) {
        if (fputc(previous, out) == EOF) return -1;
        if (fputc((int)count, out) == EOF) return -1;
    }

    return 0;
}
```

**`if (count > 0)`**: emite o **último par** (a sequência que
estava em andamento quando o loop terminou).

**Por que `count > 0`**: se o arquivo terminou **exatamente**
quando `count` atingiu 255 (e foi emitido), `count` é 0, e não há
nada a emitir. O `if` evita emitir um par com contagem 0.

**`return 0`**: sucesso.

### `rle_decompress`: a assinatura

```c
int rle_decompress(FILE *out, FILE *in)
{
```

**`int`**: retorna 0 (sucesso) ou -1 (erro).

**`FILE *out, FILE *in`**: destino antes da origem.

### `rle_decompress`: as variáveis

```c
    int byte;
    int count;
```

**`byte`**: o byte a repetir.

**`count`**: quantas vezes repetir (1–255).

**Por que `int`**: `fgetc` retorna `int` (para o `EOF`).

### `rle_decompress`: o loop principal

```c
    while ((byte = fgetc(in)) != EOF) {
        count = fgetc(in);
```

**Lê o byte** (`fgetc`). Se `EOF`, o arquivo terminou.

**Lê a contagem** (`fgetc`). Se `EOF`, o arquivo está
**corrompido** (número ímpar de bytes: falta a contagem do último
par).

**Aqui está a sutileza**: o loop **sempre tenta ler 2 bytes por
iteração** (um para `byte`, outro para `count`). Se o arquivo tem
um número **par** de bytes (como todo arquivo gerado pelo
`rle_compress`), o loop lê pares até o `EOF`. Se tem um número
**ímpar**, a última iteração lê o `byte`, mas o `count` é `EOF`
(o arquivo acabou). **Não é que o loop leia 1 byte e pare**: ele
**sempre** tenta ler 2, e se o segundo não existe, detecta a
corrupção.

**Visualmente**:

- **Arquivo válido** (6 bytes): `[A][5][B][3][C][2]`
  - Iteração 1: lê `A`, lê `5`.
  - Iteração 2: lê `B`, lê `3`.
  - Iteração 3: lê `C`, lê `2`.
  - Iteração 4: `fgetc` retorna `EOF`, o loop para.

- **Arquivo corrompido** (5 bytes): `[A][5][B][3][C]`
  - Iteração 1: lê `A`, lê `5`.
  - Iteração 2: lê `B`, lê `3`.
  - Iteração 3: lê `C`, tenta ler o `count`, mas `fgetc` retorna
    `EOF`. **Corrompido**.

```c
        if (count == EOF) {
            fprintf(stderr, "rle: arquivo corrompido (falta a contagem)\n");
            return -1;
        }
```

**Se `count == EOF`**: o arquivo tem um número **ímpar** de bytes
(um byte sem a contagem correspondente). É corrupção.

**`fprintf(stderr, ...)`**: imprime a mensagem em `stderr` (não
`stdout`). Ver
[`docs/reference/stdio.md`](../../docs/reference/stdio.md#stderr).

**`return -1`**: erro.

```c
        if (count == 0) {
            fprintf(stderr, "rle: arquivo corrompido (contagem zero)\n");
            return -1;
        }
```

**Se `count == 0`**: o `rle_compress` **nunca** gera contagem 0.
Se o arquivo tem `count = 0`, é corrupção (ou um arquivo gerado
por outro programa).

```c
        for (int i = 0; i < count; i++) {
            if (fputc(byte, out) == EOF)
                return -1;
        }
    }
```

**Escreve o byte `count` vezes** (`for` com `fputc`).

**`if (fputc(byte, out) == EOF) return -1`**: se `fputc` falhar
(disco cheio, etc.), retorna -1.

### `rle_decompress`: o `ferror`

```c
    if (ferror(in))
        return -1;

    return 0;
}
```

**`if (ferror(in)) return -1`**: se houve **erro de leitura**
(não EOF), retorna -1.

**`return 0`**: sucesso.

## Resumo do `rle.c`

- **`rle_compress`**: lê o primeiro byte, e itera. Se o byte atual
  é igual ao anterior, incrementa `count`. Se a sequência atinge
  255, emite o par e zera `count`. Se o byte muda, emite o par e
  recomeça. No final, emite o último par.
- **`rle_decompress`**: lê pares `(byte, count)`. Se o arquivo tem
  número ímpar de bytes, ou `count == 0`, é corrompido. Senão,
  escreve o byte `count` vezes.
- **`ferror`** em ambas: distingue EOF de erro.
- **Convenção**: `out` antes de `in` (destino antes da origem).

---

arataca89@gmail.com

Aulas de programação
