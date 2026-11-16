# Notas de implementação — 006_hexdump

Registro do raciocínio por trás do código. Não é tutorial: é o
"porquê" das decisões, incluindo os tropeços.

## Por que `fread` e não `fgetc`?

O `hexdump` lê **blocos de 16 bytes** por vez. Duas opções:

**Opção A — `fgetc` em loop**: lê um byte por vez, e acumula 16
bytes antes de imprimir.

**Opção B — `fread` com buffer de 16 bytes**: lê 16 bytes de uma
vez.

**Escolhemos `fread`**, porque:

1. **É o que o `hexdump` real usa**: `fread` lê blocos, e é mais
   eficiente que `fgetc` (que faz uma chamada de função por byte).
2. **É didático**: ensina `fread`, que nenhum projeto do CCS usou
   ainda. O `fgetc` já foi usado no `wc`, `cat`, `grep`.
3. **O código fica mais simples**: `fread(buf, 1, 16, in)` lê 16
   bytes, e retorna quantos leu. Com `fgetc`, você precisa de um
   loop interno para acumular 16 bytes.

**A diferença de performance**: `fread` é ~10x mais rápido que
`fgetc` (porque `fgetc` faz uma chamada de função por byte, e
`fread` faz uma por bloco). Para o escopo do `hexdump` (arquivos
pequenos/médios), a diferença é imperceptível, mas o **princípio**
é importante.

## Por que decimal (`32`/`126`) e não hex (`0x20`/`0x7E`) no `is_printable`?

A faixa imprimível em ASCII é **32 a 126** (decimal), ou
**0x20 a 0x7E** (hex). As duas formas funcionam. Escolhemos
**decimal**, porque:

1. **É mais legível para o aluno iniciante**: `32` e `126` são
   números "naturais", e a tabela ASCII é geralmente mostrada em
   decimal.
2. **Hex é convenção na saída**, mas no **código** decimal é mais
   claro. O `printf` usa hex (`%02x`) porque o `hexdump` real
   usa hex; mas a **verificação** é decimal.
3. **Comentário resolve a ambiguidade**: o código tem
   `/* 32 = espaco (0x20), 126 = ~ (0x7E) */`, que mostra o hex
   para quem quiser.

**Se você preferir hex**, é só trocar. Mas o `notes.md` registra
a decisão (decimal no código, hex na saída).

## Por que `unsigned char` no buffer e no `is_printable`?

```c
unsigned char buf[BLOCK];
static int is_printable(unsigned char c)
```

### O que é um byte?

Um byte tem **8 bits**. Cada bit é 0 ou 1. Com 8 bits, você
representa **256 valores diferentes** (`2^8 = 256`): de **0 a
255**.

### O que é "signed" e "unsigned"?

- **Unsigned** (sem sinal): todos os 8 bits são usados para o
  **valor**. Vai de **0 a 255**.
- **Signed** (com sinal): o **bit mais significativo** (o da
  esquerda) é usado para o **sinal**. 0 = positivo, 1 = negativo.
  Os outros 7 bits são o valor. Vai de **-128 a +127**.

**Visualmente**:

```
Unsigned:  0 0 0 0 0 0 0 0  = 0
           1 1 1 1 1 1 1 1  = 255
           ^
           todos os bits são valor

Signed:    0 0 0 0 0 0 0 0  = 0
           0 1 1 1 1 1 1 1  = 127
           1 0 0 0 0 0 0 0  = -128
           1 1 1 1 1 1 1 1  = -1
           ^
           primeiro bit é sinal
```

### O problema do `char`

Em C, o tipo `char` pode ser **signed** ou **unsigned**,
dependendo da plataforma:

- **No x86 (Linux, Windows)**: `char` é **signed** por padrão.
- **No ARM (alguns casos)**: `char` é **unsigned** por padrão.

**O C padrão não define** se `char` é signed ou unsigned. Isso é
**implementation-defined**. Então, um programa que assume um dos
dois pode **quebrar** na outra plataforma.

### O que acontece com bytes > 127

Se `char` é **signed**, e você lê um byte **0x80** (128 em
decimal, o primeiro byte **acima** da faixa ASCII):

- Os 8 bits são `10000000`.
- Como o primeiro bit é 1 (sinal negativo), o valor
  **interpretado** como signed é **-128** (não 128).
- Se você comparar `c >= 32` (com `c` = -128), o resultado é
  **falso** (porque -128 < 32).
- Mas o **byte** é 128, que é > 32. Então a comparação está
  **errada**.

**Exemplo concreto**:

```c
char c = 0x80;   /* 128 em decimal, mas char é signed, então vira -128 */
if (c >= 32) {
    printf("imprimivel\n");   /* NÃO imprime, porque c é -128 */
} else {
    printf("nao imprimivel\n");   /* imprime, mas o byte é 128 */
}
```

**Nota**: `0x80` (128) **não é** um caractere ASCII imprimível (a
ASCII vai até 127). Mas o `hexdump` mostra **qualquer** byte
(0–255), e o ASCII mostra `.` para **todos** os não-imprimíveis.
O problema é que, com `char` signed, o **128** vira **-128**, e a
**comparação** `c >= 32` dá errado. O `is_printable` diria "não
imprimível" (o que está **certo** por acaso, mas por **motivo
errado**).

**Onde isso realmente quebra**: se o `hexdump` fizesse outra
verificação, como `c <= 126` (para ver se é imprimível), com
`char` signed e `c = -128`, `-128 <= 126` é **verdadeiro** (o que
está **errado**, porque o byte 128 **não** é <= 126). Aí o `.`
não seria impresso, e o hexdump mostraria lixo.

### A solução: `unsigned char`

Se você declarar `unsigned char`, o byte 0x80 é interpretado como
**128** (não -128), e a comparação `c >= 32 && c <= 126` funciona
**corretamente**:

```c
unsigned char c = 0x80;   /* 128, sem sinal */
if (c >= 32 && c <= 126) {
    printf("imprimivel\n");   /* NÃO imprime (128 > 126) */
} else {
    printf("nao imprimivel\n");   /* imprime (correto) */
}
```

**Por isso o `hexdump` usa `unsigned char`** no buffer e no
`is_printable`. Bytes são **0 a 255**, não **-128 a 127**.
`unsigned char` garante isso, em **qualquer** plataforma.

### Resumo

| Tipo | Faixa | Byte 0x80 vira | `0x80 >= 32`? | `0x80 <= 126`? |
|---|---|---|---|---|
| `char` (signed) | -128 a 127 | -128 | **falso** (errado) | **verdadeiro** (errado) |
| `unsigned char` | 0 a 255 | 128 | **verdadeiro** (certo) | **falso** (certo) |

**Regra prática**: para **bytes** (não texto), use
**`unsigned char`**. Para texto, `char` (com a ressalva de que
pode ser signed, mas para texto ASCII puro, tanto faz).

## Por que o `texto.txt` está em CRLF (e não LF)?

O `texto.txt` foi criado no Sublime Text, **sem** converter para
LF. Então está em **CRLF** (Windows). Isso foi **decisão
consciente** (não descuido).

**Por que CRLF é didático**:

1. **O `hexdump` mostra os bytes reais**: o `0d` (`\r`) aparece
   no hex, e vira `.` no ASCII. O aluno vê que o `\r` existe, e
   que é não-imprimível.
2. **A lição "CRLF vs. LF"** fica registrada: o mesmo texto, em
   CRLF e LF, tem hexdumps **diferentes**. O `hexdump` é sobre
   bytes, não sobre texto.
3. **O `notes.md` do `002_cat`** já discute CRLF; o `006` mostra
   na prática.

**Se você quiser testar com LF**, converta o `texto.txt` (Sublime:
View → Line Endings → Unix), e o arquivo terá 30 bytes (sem o
`0d`). O hexdump será diferente. Ver `tests/README.md` para os
dois casos.

## O padrão `if (chamada() != 0) { erro; }`

```c
if (fseek(in, (long)opts->skip, SEEK_SET) != 0) {
    fprintf(stderr, "hexdump: nao foi possivel pular %lu bytes\n",
            opts->skip);
    return -1;
}
```

**Esse padrão** é comum em C: chama a função, e **verifica o
retorno na mesma linha**. Se falhar, toma as providências
(imprime erro, retorna).

**Por que é elegante**:

1. **Combina a chamada e a verificação** numa expressão só. Não
   precisa de uma variável temporária.
2. **O erro é tratado imediatamente**, no mesmo bloco. Não fica
   "esquecido" para depois.
3. **É idiomático**: programadores C experientes escrevem assim.

**O que os livros não citam**: muitos livros mostram `if (x < 0)
{ ... }` separado da chamada. O padrão `if (chamada() != 0)` é
mais compacto, e é o que se vê em código real (kernel, libc,
etc.).

**Cuidado**: o padrão funciona bem quando a função retorna
**0 em sucesso** e **não-zero em erro** (como `fseek`, `fclose`,
`fread` com verificação de tamanho). Para funções que retornam
**ponteiro** (`malloc`, `fopen`), o padrão é `if (!ptr) { ... }`.

## `ferror`: para que serve, quando usar, onde colocar

```c
if (ferror(in)) {
    fprintf(stderr, "hexdump: erro de leitura\n");
    return -1;
}
```

**`ferror`** verifica se houve **erro de I/O** no stream
(diferente de **EOF**). O `fread` retorna 0 tanto em EOF quanto
em erro; o `ferror` distingue os dois.

**Quando usar**: **depois** do loop de leitura, para verificar se
o loop terminou por EOF (normal) ou por erro (problema).

**Onde colocar**: **após** o `while`, antes do `return`. Não
dentro do loop (o `ferror` só faz sentido no final).

**A diferença de `feof`**: `feof` verifica se o **EOF** foi
atingido; `ferror` verifica se houve **erro**. Os dois podem ser
verdadeiros (se o erro ocorreu no fim). O `fread` retorna 0 em
ambos; o `ferror` diz se foi erro.

**No `hexdump`**: o `ferror` está após o `while`, e retorna `-1`
se houve erro. O `main` converte em exit code 1.

## O que acontece se o arquivo não for múltiplo de 16?

A última linha tem **menos de 16 bytes**. O `fread` retorna
`n < 16`, e o `print_line` imprime **padding** (3 espaços por
byte faltante) para alinhar o ASCII.

**Exemplo**: arquivo com 5 bytes (`"Hello"`):

```
00000000  48 65 6c 6c 6f                              |Hello|
```

Os 11 bytes faltantes viram 33 espaços, alinhando o `|Hello|`.

**Por que o padding é importante**: sem ele, o ASCII ficaria
**desalinhado** entre as linhas, e o hexdump ficaria ilegível.

## O que acontece com `-s` em stdin?

O `-s N` usa `fseek`, que **só funciona em arquivos buscáveis**.
Em stdin (pipe), `fseek` falha, e o `hexdump` imprime erro
("nao foi possivel pular N bytes") e retorna 1.

**Isso é esperado**: o `hexdump` real também falha com `-s` em
stdin. O `notes.md` registra a limitação.

**Alternativa** (para projetos futuros): ler e **descartar** os
primeiros N bytes, em vez de `fseek`. Mas isso é mais lento (lê
tudo), e o `hexdump` real usa `fseek`.

## O que acontece com o `type` do Windows e o CRLF?

No Windows, o `type arquivo.txt | hexdump` **traduz** `\r\n` para
`\n` no pipe. O `hexdump` recebe só `\n` (`0a`), e mostra isso.
**Não é bug do hexdump**: é o shell traduzindo.

**No Linux/macOS**, o `cat arquivo.txt | hexdump` **não** traduz
(mostra `0d 0a`), porque o `cat` copia bytes fielmente.

**Isso é uma diferença de plataforma** que vale registrar:

- **Windows**: `type` (e `cmd` em geral) traduz EOL em pipes.
- **Linux/macOS**: `cat` (e o shell) não traduz.

**No `tests/README.md`**, os dois casos (arquivo e stdin) estão
documentados, com os gabaritos medidos.

## Lições gerais deste projeto

1. **`fread` lê blocos**, `fgetc` lê bytes. Para hexdump (e
   qualquer leitura em bloco), `fread` é mais eficiente.
2. **Decimal no código, hex na saída**: a verificação
   (`is_printable`) é mais legível em decimal; a formatação
   (`printf`) é convenção em hex.
3. **`unsigned char` para bytes**: `char` signed pode dar
   resultados errados para valores > 127. `unsigned char` garante
   0–255.
4. **O padrão `if (chamada() != 0)`**: combina chamada e
   verificação numa expressão. É idiomático, mas os livros não
   citam.
5. **`ferror` distingue EOF de erro**: o `fread` retorna 0 em
   ambos; o `ferror` diz se foi erro.
6. **Padding alinha o ASCII**: sem ele, a última linha (menos de
   16 bytes) fica desalinhada.
7. **CRLF é uma decisão**: o `texto.txt` em CRLF mostra o `\r`
   (`0d`) no hexdump, o que é didático. O mesmo texto em LF tem
   hexdump diferente.
8. **`fseek` só funciona em arquivos buscáveis**: stdin (pipe)
   não é buscável. O `-s` falha em stdin, e isso é esperado.
9. **O shell pode traduzir EOL**: o `type` do Windows traduz
   `\r\n` → `\n` em pipes. O `cat` do Linux não. O `hexdump`
   mostra o que **recebe**, não o que está no arquivo original.
