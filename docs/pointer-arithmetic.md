# Aritmética de ponteiros

> Ponteiros são endereços. "Aritmética de ponteiros" é somar,
> subtrair e comparar endereços. Parece assustador, mas é uma das
> coisas mais úteis (e mais mal explicadas) de C.

## O que é um ponteiro

Um ponteiro é uma **variável que guarda um endereço de memória**.
Em C, cada variável tem um endereço (um número), e um ponteiro
guarda esse número.

```c
int x = 42;
int *p = &x;   /* p guarda o endereço de x */
```

**Visualmente**:

```
x:  [42]        (endereço 1000, por exemplo)
p:  [1000]      (p aponta para x)
```

`p` vale `1000` (o endereço de `x`). `*p` vale `42` (o valor
apontado).

## O que é aritmética de ponteiros

C permite **somar**, **subtrair** e **comparar** ponteiros. Mas
com uma regra importante: a aritmética é em **elementos**, não em
**bytes**.

### Soma: `p + 1`

`p + 1` é o ponteiro para o **próximo elemento** (não o próximo
byte). O que é "próximo elemento" depende do **tipo** de `p`:

- Se `p` é `char *`, `p + 1` avança **1 byte**.
- Se `p` é `int *`, `p + 1` avança **4 bytes** (o tamanho de um
  `int`).
- Se `p` é `double *`, `p + 1` avança **8 bytes**.

**Exemplo**:

```c
int v[] = { 10, 20, 30 };
int *p = v;        /* p aponta para v[0] */

printf("%d\n", *p);       /* 10 */
printf("%d\n", *(p + 1)); /* 20 */
printf("%d\n", *(p + 2)); /* 30 */
```

**Visualmente** (cada `int` tem 4 bytes):

```
v:  [10][20][30]      (endereços 1000, 1004, 1008)
     ^
     p

p + 1 aponta para o endereço 1004 (o próximo int)
p + 2 aponta para o endereço 1008
```

**Por que não é `p + 4`** (para avançar 4 bytes): o compilador
**multiplica** o deslocamento pelo tamanho do tipo. `p + 1` é
"próximo `int`", não "próximo byte". Isso é **fundamental** — se
você quer avançar bytes, use `char *`.

### Subtração: `p2 - p1`

`p2 - p1` é a **distância** entre dois ponteiros, em
**elementos** (não bytes). Retorna um `ptrdiff_t` (signed).

**Exemplo**:

```c
int v[] = { 10, 20, 30 };
int *p1 = &v[0];
int *p2 = &v[2];

printf("%ld\n", p2 - p1);   /* 2 (dois elementos de distância) */
```

**Visualmente**:

```
v:  [10][20][30]      (endereços 1000, 1004, 1008)
     ^        ^
     p1       p2

p2 - p1 = 2 (dois ints de distância)
```

**Em bytes**, a distância é 8 (2 × 4), mas `p2 - p1` retorna
**2** (elementos). Se você quer a distância em bytes, multiplique
pelo `sizeof`:

```c
size_t bytes = (size_t)(p2 - p1) * sizeof(int);   /* 8 */
```

### O tipo `ptrdiff_t`

Quando você subtrai dois ponteiros (`p2 - p1`), o resultado é do
tipo **`ptrdiff_t`** (definido em `<stddef.h>`).

**O que é**: um tipo inteiro **com sinal** (`signed`), usado
para representar a **diferença** entre dois ponteiros. É o tipo
"irmão" do `size_t` (que é unsigned, para tamanhos).

**Por que `signed`**: a diferença pode ser **negativa** (se
`p2 < p1`). O `ptrdiff_t` precisa representar valores negativos.

**Por que existe**: o tamanho de um `ptrdiff_t` depende da
plataforma (como o `size_t`). Em 32-bit, é tipicamente 32 bits;
em 64-bit, 64 bits. Usar `int` para diferença de ponteiros
**quebra** em plataformas onde o `int` é menor que o ponteiro.

**Exemplo**:

```c
int v[] = { 10, 20, 30 };
int *p1 = &v[0];
int *p2 = &v[2];

ptrdiff_t diff = p2 - p1;   /* 2 */
printf("%td\n", diff);      /* %td é o especificador para ptrdiff_t */
```
**Cuidados**:

- **`%td`** é o especificador de formato para `ptrdiff_t` (o
  `t` é de "ptrdiff_t"). Não use `%d` (pode dar warning).
- **`ptrdiff_t` é signed**. Se você quer `size_t` (unsigned),
  faça o cast **explícito**: `size_t len = (size_t)(end - start
  + 1);` (como no `strtrim`).
- **Só subtraia** ponteiros que apontam para o **mesmo array**
  (senão, comportamento indefinido).

**No `strtrim`**: `end - start` retorna `ptrdiff_t`; o `+ 1`
mantém `ptrdiff_t`; o `(size_t)` converte para `size_t` (porque
`malloc` espera `size_t`).

### Incremento e decremento: `p++`, `p--`

`p++` avança para o próximo elemento. `p--` recua.

```c
int v[] = { 10, 20, 30 };
int *p = v;
p++;          /* p aponta para v[1] */
printf("%d\n", *p);   /* 20 */
p--;          /* p volta para v[0] */
```

**Isso é muito usado** em loops:

```c
for (int *p = v; p < v + 3; p++)
    printf("%d\n", *p);   /* 10, 20, 30 */
```

### Comparação: `p1 < p2`, `p1 == p2`

Você pode comparar ponteiros com `<`, `>`, `<=`, `>=`, `==`,
`!=`. Mas **só faz sentido** se os dois ponteiros apontam para
**elementos do mesmo array** (ou um passou do fim).

```c
int v[] = { 10, 20, 30 };
int *p = v;
while (p < v + 3) {
    printf("%d\n", *p);
    p++;
}
```

**`p < v + 3`**: enquanto `p` não chegou ao fim do array
(`v + 3` é "um depois do último").

## O caso do `strtrim`: `end - start + 1`

Agora, o exemplo que motivou este texto:

```c
const char *start = s;       /* aponta para o início (depois de pular espaços) */
const char *end = start + strlen(start) - 1;   /* aponta para o último caractere */

/* ... recua `end` enquanto for espaço ... */

size_t len = (size_t)(end - start + 1);
```

**Passo a passo**:

1. **`start`** e **`end`** são `const char *` (ponteiros para
   `char`).
2. **`end - start`**: a distância entre `end` e `start`, em
   **elementos** (chars). Como `char` tem 1 byte, a distância em
   bytes também é a mesma. Retorna `ptrdiff_t`.
3. **`end - start + 1`**: o número de caracteres de `start` a
   `end`, **inclusive**. O `+1` é porque `end` aponta para o
   último caractere (não o próximo).
4. **`(size_t)`**: converte para `size_t` (o tipo que `malloc`
   espera, e evita warning de signed/unsigned).

**Exemplo**: `s = "  ola  "` (8 caracteres).

- `start` aponta para `'o'` (índice 2).
- `end` aponta para `'a'` (índice 4, depois de recuar).
- `end - start` = 4 - 2 = **2**.
- `end - start + 1` = 2 + 1 = **3**.
- `len` = 3 (`"ola"`).

**Visualmente**:

```
s:      [ ][ ][o][l][a][ ][ ][\0]
índice:  0  1  2  3  4  5  6  7
               ^     ^
               start end

end - start = 2 (elementos)
end - start + 1 = 3 (número de caracteres de start a end)
```

## Por que a aritmética é em elementos, não em bytes

**Porque o compilador sabe o tamanho do tipo**. Se `p` é `int *`,
`p + 1` avança `sizeof(int)` bytes. Isso é **conveniente**: você
não precisa multiplicar manualmente.

**Se você quer bytes**, use `char *`:

```c
char *bytes = (char *)p;
bytes += 1;   /* avança 1 byte */
```

Isso é usado em `memcpy`, `memmove`, etc., que trabalham com
bytes.

## Cuidados

### 1. Só faça aritmética dentro do mesmo array

```c
int a[] = { 1, 2, 3 };
int b[] = { 4, 5, 6 };
int *p = &a[2];
p++;   /* ⚠️ comportamento indefinido: p passou do fim de a */
```

**Aritmética fora do array** (ou além do fim) é
**comportamento indefinido**. O resultado pode ser lixo, ou
crash.

**Exceção**: `p + 1` é válido se `p` aponta para o **último**
elemento (aponta para "um depois do fim", que é válido para
comparação, mas **não** para desreferenciar).

### 2. Não desreferencie um ponteiro inválido

```c
int *p = NULL;
*p = 42;   /* ⚠️ segfault */
```

**Sempre** verifique se o ponteiro é válido (não `NULL`) antes de
desreferenciar.

### 3. Não subtraia ponteiros de arrays diferentes

```c
int a[3], b[3];
int *p = &a[0];
int *q = &b[0];
printf("%ld\n", q - p);   /* ⚠️ comportamento indefinido */
```

**Só subtraia** ponteiros que apontam para o **mesmo array**.

### 4. `ptrdiff_t` é signed

`p2 - p1` retorna `ptrdiff_t` (signed). Se você quer `size_t`
(unsigned), faça o cast **explícito**:

```c
size_t len = (size_t)(end - start + 1);
```

Isso evita warnings de signed/unsigned, e deixa claro que o valor
é não-negativo.

## Onde a aritmética de ponteiros aparece no CCS

- **`007_string_utils`**: `strtrim` (`end - start + 1`),
  `strsplit` (`p - start`).
- **`006_hexdump`**: `buf[i]` (que é `*(buf + i)`).
- **`004_linked_list`**: `cur = cur->next` (que é `(*cur).next`,
  mas não é aritmética de ponteiros).
- **`003_grep`**: `read_line` (avança no buffer).
- **`001_wc`**, **`002_cat`**: loops com `fgetc` (não usam
  aritmética de ponteiros diretamente).

## Resumo

| Operação | O que faz | Exemplo |
|---|---|---|
| `p + 1` | Próximo **elemento** (não byte) | `int *p; p + 1` avança 4 bytes |
| `p - 1` | Elemento anterior | `p - 1` |
| `p2 - p1` | Distância em **elementos** | `&v[2] - &v[0]` = 2 |
| `p++` / `p--` | Avança / recua 1 elemento | `p++` |
| `p1 < p2` | Compara endereços | `p < v + 3` |

**Regras**:

1. Aritmética é em **elementos**, não bytes. `char` tem 1 byte,
   então é a mesma coisa. `int` tem 4, então `p + 1` avança 4
   bytes.
2. **Só** faça aritmética dentro do **mesmo array**.
3. **Não** desreferencie ponteiro inválido (NULL, ou além do
   fim).
4. `p2 - p1` retorna `ptrdiff_t` (signed). Faça cast para
   `size_t` se for usar como tamanho.
5. Para avançar **bytes**, use `char *`.

## Leitura recomendada

- *The C Programming Language*, Kernighan & Ritchie — seção 5.3
  (pointers and arrays) e 5.4 (address arithmetic).
- *Modern C*, Jens Gustedt — capítulo sobre ponteiros.
- *Expert C Programming: Deep C Secrets*, Peter van der Linden
  — capítulo 4 (pointers and arrays).

---

arataca89@gmail.com

Aulas de programação
