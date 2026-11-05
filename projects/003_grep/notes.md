# Notas de implementação — 003_grep

Registro do raciocínio por trás do código. Não é tutorial: é o
"porquê" das decisões, incluindo os tropeços.

## Por que `malloc`/`realloc` e não `fgets`?

`fgets` com buffer fixo trunca linhas longas. Para um `grep` que
promete buscar em qualquer linha, isso é inaceitável: uma linha
de 10 KB seria lida em pedaços, e a busca falharia.

A solução é ler **caractere por caractere**, crescendo o buffer
com `realloc` quando necessário. Isso é o que `getline` (POSIX)
faz. Como `getline` não é C padrão, implementamos a nossa.

**Vantagem didática**: o aluno aprende `malloc`, `realloc`,
`free`, e o padrão "cresce conforme precisa".

## O padrão doubling

Quando o buffer enche, em vez de crescer 1 byte (O(n²)), crescemos
**× 2**:

```c
size_t new_cap = *cap ? *cap : 64;
while (new_cap < min_cap)
    new_cap *= 2;
```

Isso dá complexidade **amortizada O(1)** por caractere: a maioria
das inserções é O(1), e as raras que crescem custam O(n), mas
distribuídas dão O(1) amortizado. É o mesmo padrão de
`std::vector` (C++), `ArrayList` (Java), `list` (Python).

## O erro clássico de `realloc`

```c
/* ERRADO: se realloc falhar, buf original é perdido (memory leak) */
buf = realloc(buf, new_cap);

/* CERTO: usar ponteiro temporário */
char *tmp = realloc(buf, new_cap);
if (!tmp)
    return -1;   /* buf original ainda é válido */
buf = tmp;
```

O `grow_buffer` usa o ponteiro temporário. **Esse é o erro mais
comum** com `realloc`, e vale registrar.

## `size_t`: o tipo que a maioria dos livros esquece

### O que é

`size_t` é um tipo inteiro **sem sinal** (`unsigned`), definido em
`<stddef.h>` (e em outras headers como `<stdlib.h>`, `<string.h>`,
`<stdio.h>`). É o tipo que `sizeof` retorna, e o tipo que
`malloc`, `memcpy`, `strlen`, `fread`, `fwrite` esperam.

### Por que existe

Porque o tamanho máximo de um objeto em memória **depende da
plataforma**:

- Em sistemas **32-bit**, `size_t` é tipicamente `unsigned int`
  (32 bits, até 4 GB).
- Em sistemas **64-bit**, é `unsigned long` (64 bits).

Usar `int` para tamanhos **quebra** em plataformas onde `int` é
32 bits mas o objeto pode ter mais de 2 GB. `size_t` é o tipo
"certo" para tamanho, em qualquer plataforma.

### Por que a maioria dos livros não explica

Porque `size_t` é "chato": é um `typedef` para um tipo unsigned
que varia por plataforma. Muitos livros didáticos usam `int` para
tudo (inclusive tamanhos), e o aluno aprende errado. Quando vai
programar de verdade, descobre que:

- `strlen` retorna `size_t`, não `int`.
- `malloc` recebe `size_t`, não `int`.
- Misturar `int` com `size_t` gera **warnings**
  (`-Wsign-compare`).
- E, às vezes, **bugs**: `int` é signed, `size_t` é unsigned. A
  comparação `-1 < strlen(s)` é **falsa** em C (porque `-1` é
  convertido para `SIZE_MAX`), o que confunde.

### Quando usar

Sempre que o valor representa:

- Um **tamanho** (em bytes ou em elementos).
- Um **índice** de array.
- Um **contador** que pode chegar a valores grandes.

**Nunca** use `int` para isso.

### Onde aparece no `003_grep`

- `read_line`: `size_t *cap` (capacidade do buffer), `size_t len`
  (tamanho da linha).
- `grow_buffer`: `size_t min_cap`, `size_t new_cap`.
- `grep_stream`: `size_t cap` (capacidade inicial do buffer).

Se usássemos `int`:

- O buffer não poderia passar de 2 GB (em 32-bit) — mas o `grep`
  real lida com linhas enormes.
- `-Wsign-compare` reclamaria ao comparar `int` com o `size_t` de
  `realloc`/`fread`.

### Leitura recomendada

- *Modern C*, Jens Gustedt — capítulo sobre tipos inteiros.
- `man size_t` (Linux/macOS) — a definição formal.

## O bug do `-l`: `filename` vs `show_filename`

### O sintoma

`grep -l "compilacao" tests/compilacao.txt` imprimia
`(standard input)` em vez do nome do arquivo.

### O diagnóstico

O `main` passava `NULL` como `filename` para o `grep_stream`
quando havia **um** arquivo (para não prefixar as linhas). Mas o
`-l` precisa do `filename` **sempre**, independente de quantos
arquivos há.

**A causa**: `show_filename` (prefixar linhas?) e `-l` (mostrar
nome do arquivo?) são coisas **diferentes**:

- **`show_filename`**: prefixa **cada linha** com o nome, quando
  há **múltiplos** arquivos. Com **um** arquivo, o `grep` real
  **não** prefixa.
- **`-l`**: mostra o **nome do arquivo** em vez das linhas,
  **independente** de quantos arquivos há.

### A solução

Separar os dois conceitos:

- `show_filename` vira campo de `GrepOptions`, setado pelo `main`
  como `(n_files > 1)`.
- `filename` é passado ao `grep_stream` **sempre** (o nome real,
  ou `NULL` para stdin).
- O `grep_stream` usa `opts->show_filename` para o prefixo de
  linha, e `filename` para o `-l`.

### Lição

> Dois conceitos parecidos (`show_filename` e `-l`) podem parecer
> um só, mas têm semânticas diferentes. Misturar os dois gera bug.
> **Uma variável, um conceito** (de novo — o mesmo problema do
> `cat -s`).

## EOF vs linha vazia: o bug do loop infinito

### O sintoma

O `grep` travava (loop infinito) ao processar alguns arquivos.

### O diagnóstico

O `read_line` retorna `0` para **linha vazia** (válido) e `-1`
para **EOF**. Mas, em alguns casos, o `read_line` retornava `0`
no EOF também (quando a última linha era vazia, ou o arquivo
terminava com `\n\n`). O `grep_stream` usa `>= 0` como condição
de continuação, então `0` fazia o loop continuar para sempre.

### A solução

Distinguir "saiu por EOF" de "saiu por `\n`" com uma variável
`saw_eof`:

```c
int saw_eof = 0;
while ((c = fgetc(f)) != EOF) { ... }
if (c == EOF)
    saw_eof = 1;

if (saw_eof && len == 0)
    return -1;   /* EOF de verdade */

/* senão, é linha (mesmo que len == 0, é linha vazia válida) */
```

### Lição

> `0` é um valor **válido** de retorno (linha vazia). Se você usa
> `>= 0` como condição de loop, `0` continua o loop. Precisa de
> um valor **negativo** para EOF (`-1`), e o `read_line` precisa
> **distinguir** os dois casos.

## O `-U` no grep: converter na entrada, não na saída

No `cat`, `-U` converte `\r\n → \n` **na saída**. No `grep`, a
situação é diferente: o `grep` não emite a linha inteira (emite
a linha que casou, ou conta, ou mostra o nome do arquivo).
"Converter na saída" não faz sentido.

A solução é **converter na leitura**: ao ler a linha, remover o
`\r` do final (se for `\r\n`). Isso tem dois efeitos:

1. A busca funciona como se o arquivo fosse LF (embora, na
   prática, `strstr` já ache a substring antes do `\r` na maioria
   dos casos).
2. A saída não tem `\r`, então o terminal mostra as linhas limpas.

**Cuidado**: o `\r` só é removido se for o **último** caractere
antes do `\n`/EOF. Um `\r` no meio da linha (raro, mas possível
em arquivos antigos de Mac) é **conteúdo**, não EOL, e deve ficar.

### Lição

> O mesmo nome de flag (`-U`) pode ter implementações diferentes
> em programas diferentes, porque o **ponto de conversão** muda
> (saída no `cat`, entrada no `grep`). Documentar a diferença é
> parte do design.

## `tolower` com `unsigned char`

```c
tolower((unsigned char)*h)
```

O `tolower` espera `unsigned char` (ou `EOF`). Passar `char`
diretamente pode dar **comportamento indefinido** para caracteres
> 127 (ex.: acentos em UTF-8). O cast é **obrigatório** para
portabilidade.

**Isso é uma pegadinha clássica** de C, e vale registrar.

## Por que `strcasestr` própria?

`strcasestr` não é C padrão. É extensão GNU/BSD. No Linux/macOS,
está em `<string.h>` (com `_GNU_SOURCE`). No Windows/MSVC, **não
existe**. Implementar a nossa garante portabilidade.

A implementação é ingênua (O(n*m)), mas suficiente para o escopo.
Um projeto futuro  pode otimizar

## Código de retorno POSIX

O `grep` retorna:

- `0`: pelo menos uma linha casou.
- `1`: nenhuma linha casou.
- `2`: erro (arquivo não encontrado, flag inválida).

Isso é **diferente** do `wc` e do `cat` (que retornam 0/1). O
código de retorno é parte do **contrato** do `grep`, e scripts
dependem dele (`if grep -q padrao arquivo; then ...`).

### Lição

> O código de retorno é parte da interface do programa. `grep`
> usa 0/1/2; `wc` e `cat` usam 0/1. Não é "detalhe": é contrato.

## Por que os fixtures do `grep` são CRLF (e não LF)

Os fixtures `compilacao.txt` e `historia.txt` estão em **CRLF**
(Windows), não em LF. Isso foi **decisão consciente**, não
descuido.

### Por que CRLF funciona no `grep`

A busca literal (`strstr`) acha a substring **antes** do `\r` no
fim da linha. Exemplo: a linha `"compilacao\r\n"` é lida como
`"compilacao\r"`. A busca `strstr("compilacao\r", "compilacao")`
retorna o ponteiro para o início de `"compilacao"` (não para o
`\r`). Então a linha **casa**, e é impressa.

O `\r` só atrapalharia se o padrão incluísse o **fim** da linha
(como `C$`, que exigiria regex). Como o `grep` busca substring
literal, o `\r` no fim é inofensivo.

### Por que manter CRLF é didático

1. **O `\r` existe e é invisível**. O aluno vê a saída "limpa" no
   terminal, mas o `\r` está lá (verificável com `certutil -dump`
   ou `Format-Hex`). Isso ensina que "o que você vê" ≠ "o que
   está no arquivo".
2. **O `-U` é opção, não obrigação**. O `grep` funciona com CRLF.
   O `-U` é para quem quer a saída **sem** `\r`. Documentar isso
   é mais rico que "converter e fingir que o CRLF não existe".
3. **A lição "CRLF é um problema real"** fica registrada. O aluno
   que trabalha com Windows vai encontrar CRLF em todo lugar.
   Saber que o `grep` funciona (mas o `cat -E` mostra `$` no
   lugar errado) é conhecimento prático.

### O que o `.gitattributes` faz

O `.gitattributes` da raiz tem `projects/*/tests/*.txt -text`,
que diz ao Git: "trate como binário, não converta EOL". Isso
preserva o CRLF no clone, em qualquer plataforma. Quem clonar em
Linux recebe CRLF (não LF), e os gabaritos batem.

### Se você quiser testar com LF

Crie um `tests/lf.txt` com o mesmo conteúdo (convertido no editor:
Sublime → View → Line Endings → Unix), e compare as saídas. Os
gabaritos de **contagem** (`-c`, `-v`, `-n`) são os **mesmos**. A
diferença é só o `\r` invisível no fim das linhas impressas.

### Lição

> "O arquivo está em CRLF" não é um detalhe: é uma **decisão**.
> Manter CRLF nos fixtures do `grep` (que funciona com eles) e
> LF nos fixtures do `wc` (que conta bytes) é **intencional**.
> Cada projeto escolhe os fixtures que fazem sentido para o que
> quer demonstrar.

## Lições gerais deste projeto

1. **`malloc`/`realloc`/`free`** são a base da alocação dinâmica.
   `realloc` exige ponteiro temporário (senão, memory leak).
2. **Doubling** dá O(1) amortizado. Crescer 1 byte é O(n²).
3. **`size_t`** é o tipo certo para tamanhos e índices. `int` é
   errado, e a maioria dos livros não explica.
4. **Dois conceitos parecidos** (`show_filename` e `-l`) podem
   ter semânticas diferentes. Misturar gera bug.
5. **EOF ≠ linha vazia**. `0` é válido; `-1` é EOF. Distinguir
   os dois é obrigatório em loops com `>= 0`.
6. **`tolower` com `unsigned char`** evita UB.
7. **`strcasestr` própria** garante portabilidade.
8. **Código de retorno é contrato**. `grep` usa 0/1/2.
9. **CRLF é uma decisão**. Manter CRLF nos fixtures do `grep` é
   intencional, e documentar o `\r` invisível é mais didático que
   converter e fingir que ele não existe.
