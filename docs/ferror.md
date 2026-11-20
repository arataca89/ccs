# `ferror`: detectando erros de I/O

> `ferror` responde a uma pergunta simples: **houve erro de
> leitura/escrita neste stream?** É diferente de `feof` (fim de
> arquivo), e a distinção é importante: um `fread` que retorna 0
> pode ser **fim normal** (EOF) ou **erro** (disco cheio, conexão
> caiu). O `ferror` diz qual.

## O que é

`ferror` verifica se o **indicador de erro** do stream está
setado. O indicador é setado por funções de I/O (`fread`,
`fwrite`, `fgetc`, `fputc`, `fprintf`, etc.) quando ocorre um
**erro de I/O** (não um fim de arquivo).

```c
#include <stdio.h>

int ferror(FILE *stream);
```

**Retorno**: não-zero se houve erro, 0 caso contrário.

## `ferror` vs. `feof`

| Função | O que verifica | Quando é verdadeira |
|---|---|---|
| `feof` | Fim de arquivo | O `fread`/`fgetc` tentou ler **além** do fim |
| `ferror` | Erro de I/O | O `fread`/`fgetc` encontrou um **erro** (disco, rede, etc.) |

**As duas podem ser verdadeiras ao mesmo tempo** (se o erro
ocorreu no fim do arquivo). O `fread` retorna 0 em **ambos** os
casos; o `feof` e o `ferror` distinguem.

**Importante**: `feof` **não** é "o arquivo acabou". É "o
`fread`/`fgetc` tentou ler **depois** do fim". Se você lê um
arquivo até o fim com `fread`, o `feof` só fica verdadeiro
**depois** da última leitura (que retorna 0).

## O padrão de uso

O `ferror` é verificado **depois** do loop de leitura, para
distinguir "fim normal" de "erro":

```c
size_t n;
while ((n = fread(buf, 1, BLOCK, in)) > 0) {
    /* ... processa buf[0..n-1] ... */
}

if (ferror(in)) {
    fprintf(stderr, "erro de leitura\n");
    return -1;
}
return 0;
```

**Por que depois do loop**: o `fread` retorna 0 tanto em EOF
quanto em erro. O loop termina em ambos. O `ferror` (ou `feof`)
diz **qual** dos dois foi.

**Por que não dentro do loop**: o `ferror` só faz sentido no
**final** (o indicador é cumulativo). Verificar dentro do loop
seria redundante (o `fread` já retorna 0 em erro, e o loop
para).

## Onde o `ferror` é usado no CCS

- **`001_wc`**: `wc_count` retorna `-1` se `ferror(stream)`. O
  `main` converte em erro.
- **`002_cat`**: `cat_copy` retorna `-1` se `ferror(input)`.
- **`003_grep`**: `read_line` retorna `-2` se `ferror(f)`;
  `grep_stream` converte em `-1`.
- **`006_hexdump`**: `hexdump_stream` retorna `-1` se
  `ferror(in)`.

**Em todos**, o padrão é o mesmo: verificar **depois** do loop, e
retornar `-1` (ou `-2`, no `grep`) se houve erro.

## Quando o `ferror` fica verdadeiro

O `ferror` fica verdadeiro em **erros de I/O**, como:

- **Disco cheio** ao escrever (`fwrite`, `fprintf`).
- **Disco com defeito** ao ler (`fread`, `fgetc`).
- **Conexão caiu** (se o stream for um socket, pipe, etc.).
- **Permissão negada** (raro, porque `fopen` já teria falhado).
- **Erro de encoding** (raro, em streams de texto com locale).

**Não** fica verdadeiro em:

- **Fim de arquivo** (EOF): use `feof`.
- **Erro de argumento** (ex.: `fopen` com caminho inválido
  retorna `NULL`, não seta `ferror`).
- **Erro de formato** (ex.: `printf` com `%d` e um `char *`):
  comportamento indefinido, não seta `ferror`.

## Como limpar o indicador de erro

O indicador de erro **persiste** até ser limpo. Para limpá-lo:

```c
clearerr(in);
```

**Quando usar**: se você quer **continuar** lendo/escrevendo
depois de um erro (ex.: tentar de novo). Na prática, a maioria
dos programas **para** no primeiro erro (retorna `-1`), então
`clearerr` é raro.

**No CCS**: não usamos `clearerr` (paramos no primeiro erro).

## O que acontece se você **não** verificar o `ferror`

O programa **não detecta** o erro, e:

- **Na leitura**: pode processar dados **incompletos** ou
  **corrompidos** (o `fread` leu menos do que devia, mas o
  programa não sabe).
- **Na escrita**: pode **perder** dados (o `fwrite` não escreveu
  tudo, mas o programa não sabe).
- **Exit code**: o programa retorna 0 (sucesso), mesmo tendo
  falhado.

**Isso é um bug silencioso**: o programa "funciona" na maioria
dos casos, mas falha em casos de erro (disco cheio, rede caiu).
**Sempre** verifique o `ferror`.

## `ferror` no `hexdump.c`

```c
if (ferror(in)) {
    fprintf(stderr, "hexdump: erro de leitura\n");
    return -1;
}
```

**O que faz**: se houve erro de leitura, imprime mensagem em
`stderr`, e retorna `-1`.

**Por que `-1` e não `-2`**: o `hexdump` só tem um tipo de erro
(leitura). O `grep` usa `-2` porque distingue "erro de
alocação" (`-2`) de "erro de leitura" (também `-2`, na
verdade). No `hexdump`, `-1` é suficiente.

**Por que `stderr` e não `stdout`**: mensagens de erro vão para
`stderr` (convenção Unix). Scripts podem redirecionar `stdout`
sem perder os erros.

## Resumo

| Pergunta | Resposta |
|---|---|
| O que `ferror` verifica? | Se houve **erro de I/O** no stream |
| Quando usar? | **Depois** do loop de leitura/escrita |
| Onde colocar? | Após o `while`, antes do `return` |
| O que retorna? | Não-zero (erro) ou 0 (sem erro) |
| Diferença de `feof`? | `feof` = fim de arquivo; `ferror` = erro |
| O que fazer se verdadeiro? | Imprimir erro, retornar `-1` (ou código de erro) |
| Como limpar? | `clearerr(stream)` (raro) |

**Regra prática**: **sempre** verifique `ferror` depois de um
loop de leitura/escrita. Sem isso, o programa pode "funcionar"
mas perder dados silenciosamente.

## Leitura recomendada

- `man 3 ferror` (Linux/macOS) — a referência completa.
- *The C Programming Language*, Kernighan & Ritchie — seção 7.6
  (error handling).
- *Modern C*, Jens Gustedt — capítulo sobre I/O.

---

arataca89@gmail.com

Aulas de programação
