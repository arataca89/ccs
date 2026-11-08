# `static`: três significados, uma palavra

> Em C, `static` faz **três coisas diferentes**, dependendo de onde
> é usado. A maioria dos livros explica só uma (ou nenhuma), e o
> aluno fica confuso. Este texto cobre as três.

## Por que `static` confunde

Em C, a mesma palavra-chave tem significados **diferentes** em
**contextos diferentes**:

- Em uma **função**, `static` muda a **visibilidade** (linker).
- Em uma **variável global**, `static` muda a **visibilidade** (linker).
- Em uma **variável local**, `static` muda o **tempo de vida** (persistência).

São três conceitos **não relacionados**, que compartilham a mesma
palavra. Isso é uma herança histórica de C (a palavra `static` foi
escolhida por razões que faziam sentido em 1972, mas hoje confunde).

O importante: **o contexto define o significado**.

## Uso 1: `static` em funções (o mais comum no CCS)

```c
/* list.c */
static void print_help(void) { ... }       /* só visível em list.c */
static int parse_int(const char *s, ...) { ... }  /* idem */

void list_push(List *l, int v) { ... }     /* visível em qualquer .c */
```

Uma função `static` só é visível **dentro do arquivo `.c`** onde foi
definida. Outros arquivos **não conseguem** chamá-la, mesmo que
tenham o `#include` do header.

**Por que usar**:

1. **Encapsulamento**: a função é um "detalhe interno" do módulo.
   Ninguém fora precisa saber que ela existe. Se você mudar a
   assinatura ou remover, ninguém quebra.
2. **Evita colisão de nomes**: se dois arquivos `.c` têm uma função
   `print_help`, sem `static` o linker reclama ("multiple definition").
   Com `static`, cada uma é local ao seu arquivo, e não há conflito.
3. **Otimização**: o compilador pode **inlinar** funções `static`
   mais agressivamente, porque sabe que elas não são chamadas de
   fora. (Com `-O2`, o GCC faz isso.)
4. **Documentação**: `static` sinaliza "isso é interno". Quem lê o
   `.c` sabe que não precisa se preocupar com compatibilidade.

**Convenção do CCS**: toda função que **não** está no `.h` é
`static`. O `.h` só declara o que é público.

### O que muda no linker

Sem `static`, uma função tem **símbolo global** (visível para o
linker, e para outros `.o`). Com `static`, o símbolo é **local**
(visível só dentro do `.o`).

Visualmente:

```
Sem static:  list.o -> símbolo "parse_int" (global)
Com static:  list.o -> símbolo "parse_int" (local, não exportado)
```

Isso é o que permite dois arquivos terem a mesma função `static`
sem conflito. O linker trata cada uma como "privada" do seu `.o`.

## Uso 2: `static` em variáveis globais

```c
/* list.c */
static int debug = 0;   /* só visível em list.c */
```

Uma variável global `static` só é visível **dentro do arquivo `.c`**.
Outros arquivos **não conseguem** acessá-la, mesmo com `extern`.

**Por que usar**:

- **Encapsulamento**: a variável é um "estado interno" do módulo.
- **Evita colisão**: dois arquivos podem ter `static int debug` sem
  conflito.

**Sem `static`**, uma variável global é visível para **todo** o
programa (se declarada `extern` em outro lugar). Isso é uma fonte
comum de bugs (duas partes do programa mexendo na mesma variável
sem saber).

**Convenção do CCS**: variáveis globais são **raras** (preferimos
passar parâmetros). Quando existem, são `static` (internas ao
módulo).

## Uso 3: `static` em variáveis locais (o mais confuso)

```c
void contador(void)
{
    static int n = 0;   /* persiste entre chamadas! */
    n++;
    printf("%d\n", n);
}
```

Uma variável local `static` **persiste** entre chamadas da função.
Na primeira chamada, `n = 0` (inicializada uma vez). Na segunda,
`n` ainda é 1. Na terceira, 2. **O valor não é perdido** quando a
função retorna.

**Por que isso confunde**: uma variável local "normal" é criada na
pilha a cada chamada, e destruída ao retornar. Uma `static` é
criada **uma vez** (no segmento de dados do programa), e persiste
até o fim do programa.

**Quando usar**:

- **Contadores** (ex.: quantas vezes a função foi chamada).
- **Cache** (ex.: guardar o último resultado).
- **Inicialização única** (ex.: inicializar algo na primeira
  chamada, e reusar nas seguintes).

**Quando NÃO usar**:

- **Quase sempre**. Variáveis `static` locais são um **estado
  global disfarçado**. Elas dificultam testes (o estado persiste
  entre testes), e dificultam raciocinar sobre o código (o
  comportamento da função depende de chamadas anteriores).

**Convenção do CCS**: variáveis `static` locais são **evitadas**.
Se precisar de estado, prefira uma `struct` passada por parâmetro
(como `List *list` no `004_linked_list`).

## Resumo

| Onde | O que faz | Quando usar |
|---|---|---|
| **Função** | Só visível no `.c` | Helpers internos (convenção do CCS) |
| **Variável global** | Só visível no `.c` | Estado interno do módulo (raro) |
| **Variável local** | Persiste entre chamadas | Cache, contador (evitar) |

**Regra prática**: se você vê `static` em um projeto C, olhe o
**contexto**:

- Antes de uma **função** → é encapsulamento (a função é interna).
- Antes de uma **variável global** → é encapsulamento (a variável
  é interna).
- Antes de uma **variável local** (dentro de uma função) → é
  persistência (a variável mantém o valor entre chamadas).

## `static` no `004_linked_list`

No `004_linked_list`, usamos `static` em **três funções** do
`main.c`:

```c
static void print_help(void) { ... }
static void strip_newline(char *s) { ... }
static int parse_int(const char *s, int *out) { ... }
```

São helpers internos do `main.c` — não precisam ser exportados
para outros arquivos. `static` deixa isso explícito.

**Por que não estão no `list.h`**: porque não são parte da
**interface** da lista. São detalhes do `main.c` (como imprimir
ajuda, como remover `\n`, como converter string em `int`). Se
outro arquivo precisasse delas, aí sim iriam para um header. Mas
não precisa.

## `static` no `001_wc`, `002_cat`, `003_grep`

Todos os projetos do CCS usam `static` para helpers internos:

- **`001_wc`**: `wc_count` é público (no `.h`); o resto é `static`.
- **`002_cat`**: `emit_line_number`, `emit_char` são `static`.
- **`003_grep`**: `grow_buffer`, `strcasestr_impl` são `static`.

**Isso é consistência**: o `.h` só declara o que é público. O
resto é `static`.

## Um erro comum: esquecer o `static` e ter colisão

Se você tem dois arquivos `.c` com uma função `helper` (sem
`static`), o linker reclama:

```
multiple definition of `helper'
```

**Solução**: adicionar `static` em **uma** (ou ambas) as funções.
Aí cada uma é local ao seu `.c`, e não há conflito.

**Isso é comum** em projetos que crescem: duas pessoas escrevem
`helper` em arquivos diferentes, e o build quebra. `static` evita
isso.

## Leitura recomendada

- *The C Programming Language*, Kernighan & Ritchie — seção 4.6
  (`static` em funções e variáveis).
- *Modern C*, Jens Gustedt — capítulo sobre ligação (linkage).
- `man static` (não existe; `static` é palavra-chave, não função).
