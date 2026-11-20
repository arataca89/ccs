# Notas de implementação — 007_string_utils

Registro do raciocínio por trás do código. Não é tutorial: é o
"porquê" das decisões, incluindo os tropeços.

## Por que implementar essas funções do zero?

As funções `strdup` e `strndup` são POSIX (não C padrão), e
`strtrim`, `str_reverse`, `strtolower`, `strtoupper`, `strsplit`,
`strjoin` não são padrão nem POSIX (são "comuns", cada projeto
implementa a sua).

**Por que implementar**:

1. **Didático**: ensina como as funções funcionam por dentro
   (`malloc`, `memcpy`, aritmética de ponteiros).
2. **Portabilidade**: `strdup` e `strndup` não existem no
   Windows/MSVC (lá, são `_strdup` e `_strndup`). Implementar as
   nossas garante portabilidade.
3. **Útil**: `strtrim`, `strsplit`, `strjoin` são funções que
   todo programa C acaba precisando. Tê-las no CCS é útil para
   projetos futuros.

## O conflito de `strrev` com o MinGW

### O sintoma

Ao compilar o `007_string_utils` no Windows com TDM-GCC, o
compilador reclamou:

```
error: conflicting types for 'strrev'
note: previous declaration of 'strrev' was here
```

### O diagnóstico

O MinGW (TDM-GCC) **já tem** uma função `strrev` na `<string.h>`,
com assinatura **diferente**:

```c
char *strrev(char *s);   /* MinGW: retorna char * */
```

A nossa era:

```c
void strrev(char *s);   /* nossa: retorna void */
```

O compilador vê duas declarações com retornos diferentes, e
reclama.

**Por que isso acontece**: `strrev` **não é C padrão** (nem
POSIX). É uma extensão do MSVC (e do MinGW, que a implementa por
compatibilidade). Cada plataforma pode ter a sua. No
Linux/macOS, `strrev` **não existe** (a nossa não conflita).

### A solução

Renomear a nossa função para `str_reverse`. Isso evita o
conflito, e deixa claro que é uma função **nossa** (não a do
MinGW).

**Mudanças**:
- `include/strings.h`: `void str_reverse(char *s);`
- `src/strings.c`: `void str_reverse(char *s) { ... }`
- `src/main.c`: `str_reverse(r);`
- `reference.md`, `explained.md`, `README.md`: menções
  atualizadas.

### Lição

> Nomes de funções que **não são C padrão** (nem POSIX) podem
> conflitar com extensões de plataforma. Para evitar, use um
> prefixo (`str_`, `my_`, `ccs_`) ou um nome mais descritivo
> (`str_reverse` em vez de `strrev`).

## Por que `strtrim` usa aritmética de ponteiros

No `strtrim`:

```c
size_t len = (size_t)(end - start + 1);
```

**`end` e `start`** são ponteiros para `char`, apontando para
posições **dentro da string**.

**`end - start`**: a distância entre os dois ponteiros, em
**elementos** (chars). Como `char` tem 1 byte, a distância em
bytes também é a mesma. Retorna `ptrdiff_t` (signed).

**`+ 1`**: o número de caracteres de `start` a `end`,
**inclusive**. O `+1` é porque `end` aponta para o último
caractere (não o próximo).

**`(size_t)`**: converte para `size_t` (o tipo que `malloc`
espera, e evita warning de signed/unsigned).

Ver [`docs/pointer-arithmetic.md`](../../docs/pointer-arithmetic.md)
para a explicação completa.

## Por que `strsplit` aloca um array de ponteiros

O `strsplit` retorna `char **` (array de strings terminado em
`NULL`), como `argv`. Isso permite percorrer com:

```c
for (char **p = tokens; *p; p++)
    printf("%s\n", *p);
```

**Por que terminar em `NULL`**: convenção do `argv`. O chamador
não precisa saber o número de tokens (basta percorrer até
`NULL`).

**Quem libera**: o chamador precisa liberar **cada string** e o
**array**:

```c
for (char **p = tokens; *p; p++)
    free(*p);
free(tokens);
```

**Isso é chato**, mas é o padrão. O `reference.md` documenta.

## Por que `strsplit("")` retorna um token vazio

Se `s` for `""` (string vazia), o `strsplit` retorna um array
com **um** elemento (`""`) e o `NULL`:

```c
char **tokens = strsplit("", ',');
/* tokens[0] = "", tokens[1] = NULL */
```

**Por que**: consistente com `strtok` (que retorna um token
vazio para string vazia). E é mais simples que retornar um array
com zero elementos (que exigiria um caso especial).

## Por que `strsplit("a,,b", ',')` gera um token vazio no meio

```c
char **tokens = strsplit("a,,b", ',');
/* tokens[0] = "a", tokens[1] = "", tokens[2] = "b" */
```

**Por que**: "dividir por delimitador" significa que cada
delimitador separa dois tokens. Dois delimitadores consecutivos
geram um token vazio entre eles. É o comportamento de `strtok`
(também gera token vazio).

**Se você não quer tokens vazios**: filtre depois (com uma
função que remove strings vazias). Mas o `strsplit` **não**
filtra (é uma decisão de design).

## Por que `strjoin` com `sep = NULL` trata como `""`

```c
if (!sep)
    sep = "";
```

**Por que**: `NULL` é um valor inválido para `sep`. Tratar como
string vazia é mais simples que retornar `NULL` (erro). O
chamador não precisa se preocupar com `NULL`.

## Por que o `main` faz `strdup` antes de `str_reverse`/`strtolower`/`strtoupper`

```c
char *r = strdup(argv[2]);
str_reverse(r);
```

**Por que**: `str_reverse`, `strtolower`, `strtoupper` modificam
a string **in-place**. Mas `argv[2]` pode ser **read-only** (o
shell pode passar strings em memória read-only). Modificar
`argv[2]` diretamente é **comportamento indefinido**.

**A solução**: fazer uma **cópia mutável** (`strdup`) e modificar
a cópia. O `reference.md` explica (seção "Strings mutáveis vs.
imutáveis").

## Por que o `main` aloca `parts` no `join` (e não passa `&argv[3]`)

O `main` poderia passar `&argv[3]` diretamente para `strjoin`
(porque `argv` é terminado em `NULL`). Mas o código aloca um
array `parts` por clareza didática (mostra como construir um
array terminado em `NULL`).

**Versão mais simples** (que também funciona):

```c
char *r = strjoin(&argv[3], sep);
```

**Decisão**: manter a versão com `parts` no código (didático), e
registrar a alternativa no `notes.md`.

## Lições gerais deste projeto

1. **`malloc`/`free`** para alocação dinâmica de strings. Sempre
   verifique `NULL`, sempre libere.
2. **`memcpy`** para cópia de bytes (mais eficiente que `strcpy`
   para blocos grandes).
3. **Aritmética de ponteiros** (`end - start + 1`) para calcular
   tamanhos. Ver [`docs/pointer-arithmetic.md`](../../docs/pointer-arithmetic.md).
4. **Array de ponteiros** (`char **`) para listas de strings.
   Termine em `NULL` (como `argv`).
5. **Strings mutáveis vs. imutáveis**: funções in-place exigem
   string mutável. O chamador precisa garantir.
6. **Nomes de funções não-padrão podem conflitar** com extensões
   de plataforma. Use prefixo (`str_`) ou nome descritivo.
7. **`unsigned char` em `tolower`/`toupper`** evita comportamento indefinido.
8. **`strtol` com validação** (no `main`) para converter números.

---

arataca89@gmail.com

Aulas de programação
