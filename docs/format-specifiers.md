# Especificadores de formato do `printf`

> `printf` formata a saída. Os **especificadores de formato**
> (os `%` seguidos de letras) dizem **como** cada valor é
> impresso. A maioria dos livros mostra só `%d` e `%s`; este
> texto cobre os que o CCS usa.

## O que é um especificador de formato

Um especificador começa com `%` e termina com uma **letra** que
indica o **tipo** do valor:

```c
printf("idade: %d\n", 42);   /* %d = int */
```

O `printf` substitui `%d` pelo valor de `42`, formatado como
decimal.

## Estrutura de um especificador

```
%[flags][largura][.precisão][comprimento]tipo
```

Exemplo: `%08lx`

- `%` — início.
- `0` — **flag**: preenche com zeros (não espaços).
- `8` — **largura**: 8 caracteres no total.
- `l` — **comprimento**: `long` (não `int`).
- `x` — **tipo**: hexadecimal minúsculo.

Resultado: `0000000a` (se o valor for 10).

## Os tipos mais usados

### `%d` / `%i` — inteiro decimal (com sinal)

```c
printf("%d\n", 42);      /* 42 */
printf("%d\n", -42);     /* -42 */
printf("%d\n", 0);       /* 0 */
```

**`int`** (com sinal). `%d` e `%i` são equivalentes no `printf`
(a diferença é no `scanf`).

### `%u` — inteiro decimal (sem sinal)

```c
printf("%u\n", 42u);     /* 42 */
printf("%u\n", 0u);      /* 0 */
```

**`unsigned int`** (sem sinal). Se você passar um `int` negativo,
o resultado é **lixo** (o `printf` interpreta os bits como
unsigned).

### `%ld` / `%lu` — `long` / `unsigned long`

```c
printf("%ld\n", 42L);    /* 42 */
printf("%lu\n", 42UL);   /* 42 */
```

**`long`** / **`unsigned long`**. O `l` (comprimento) é
**obrigatório** — se você passar `long` com `%d`, o
comportamento é **indefinido** (em algumas plataformas, funciona
por acaso; em outras, imprime lixo).

### `%lld` / `%llu` — `long long` / `unsigned long long`

```c
printf("%lld\n", 42LL);  /* 42 */
printf("%llu\n", 42ULL); /* 42 */
```

**`long long`** (64 bits, garantido). O `ll` (comprimento) é
obrigatório.

### `%zu` — `size_t`

```c
printf("%zu\n", (size_t)42);   /* 42 */
```

**`size_t`**. O `z` (comprimento) é o **específico** para
`size_t`. Use **sempre** que o valor for `size_t` (como o retorno
de `strlen`, `sizeof`, etc.). Se você usar `%d` ou `%lu`, o
comportamento pode ser **indefinido** (depende da plataforma).

### `%x` / `%X` — hexadecimal

```c
printf("%x\n", 255);     /* ff */
printf("%X\n", 255);     /* FF */
```

**`unsigned int`** em hex (minúsculo ou maiúsculo). Para `long`,
use `%lx`; para `long long`, `%llx`.

### `%o` — octal

```c
printf("%o\n", 8);       /* 10 */
```

**`unsigned int`** em octal (base 8). Pouco usado, mas existe.

### `%f` / `%e` / `%g` — ponto flutuante

```c
printf("%f\n", 3.14);    /* 3.140000 */
printf("%.2f\n", 3.14);  /* 3.14 (2 casas decimais) */
```

**`double`** (não `float` — o `printf` promove `float` para
`double`). O `%f` mostra com 6 casas decimais por padrão. O
`.2` (precisão) mostra 2 casas.

### `%c` — caractere

```c
printf("%c\n", 'A');     /* A */
printf("%c\n", 65);      /* A (65 é o código ASCII de 'A') */
```

**`char`** (ou `int` com o código ASCII). Imprime **um**
caractere.

### `%s` — string

```c
printf("%s\n", "ola");   /* ola */
```

**`char *`** (string terminada em `'\0'`). Imprime até o `'\0'`.

### `%%` — porcentagem literal

```c
printf("100%%\n");       /* 100% */
```

**Não consome argumento**. Imprime um `%` literal.

### `%p` — ponteiro

```c
printf("%p\n", (void *)&x);   /* endereço em hex */
```

**`void *`** (ponteiro genérico). Imprime o endereço em hex
(geralmente `0x...`). Útil para debug.

## Flags (modificadores)

### `0` — preencher com zeros

```c
printf("%05d\n", 42);    /* 00042 */
```

Sem o `0`, o preenchimento é com **espaços**:

```c
printf("%5d\n", 42);     /*    42 */
```

### `-` — alinhar à esquerda

```c
printf("%-5d|\n", 42);   /* 42   | */
```

Sem o `-`, o alinhamento é à **direita** (padrão).

### `+` — mostrar sinal (mesmo positivo)

```c
printf("%+d\n", 42);     /* +42 */
printf("%+d\n", -42);    /* -42 */
```

### `#` — prefixo (0x para hex, 0 para octal)

```c
printf("%#x\n", 255);    /* 0xff */
printf("%#o\n", 8);      /* 010 */
```

### ` ` (espaço) — espaço antes de positivo

```c
printf("% d\n", 42);     /*  42 */
printf("% d\n", -42);    /* -42 */
```

Similar ao `+`, mas usa **espaço** em vez de `+`.

## Largura e precisão

### Largura — número mínimo de caracteres

```c
printf("%8d\n", 42);     /*       42 (8 caracteres, alinhado à direita) */
```

### Precisão — número de casas decimais (float) ou máximo de caracteres (string)

```c
printf("%.2f\n", 3.14159);   /* 3.14 */
printf("%.3s\n", "abcdef");  /* abc (máximo 3 caracteres) */
```

### `*` — largura/precisão como argumento

```c
printf("%*d\n", 8, 42);      /* 8 é a largura, 42 é o valor */
```

Isso permite largura dinâmica (ex.: calcular a largura da coluna
em runtime).

## Combinações usadas no CCS

### `%08lx` (no `hexdump`)

```c
fprintf(out, "%08lx  ", offset);
```

- `0` — preencher com zeros.
- `8` — 8 caracteres.
- `l` — `long`.
- `x` — hex minúsculo.

Resultado: `0000000a` (se `offset` for 10).

### `%02x` (no `hexdump`)

```c
fprintf(out, "%02x ", buf[i]);
```

- `0` — preencher com zeros.
- `2` — 2 caracteres.
- `x` — hex minúsculo.

Resultado: `0a` (se `buf[i]` for 10), `ff` (se for 255).

### `%zu` (no `grep`, `list`)

```c
printf("%zu\n", list_size(list));
```

- `z` — `size_t`.
- `u` — unsigned decimal.

Resultado: `5` (se o tamanho for 5).

### `%lu` (no `grep`, `hexdump`)

```c
printf("%lu\n", matches);
```

- `l` — `long`.
- `u` — unsigned decimal.

Resultado: `2` (se `matches` for 2).

### `%s` (em todos os projetos)

```c
printf("%s\n", buf);
```

- `s` — string.

Resultado: o conteúdo da string.

## Erros comuns

### 1. Usar `%d` para `long`

```c
long x = 42;
printf("%d\n", x);   /* ERRADO: %d espera int, não long */
```

**Correto**: `printf("%ld\n", x);`.

**Por que é perigoso**: em algumas plataformas (32-bit), `int` e
`long` têm o mesmo tamanho, e funciona. Em outras (64-bit), `long`
é maior, e o `printf` lê **só parte** do valor (ou lê lixo).
**Comportamento indefinido**.

### 2. Usar `%d` para `size_t`

```c
size_t n = strlen(s);
printf("%d\n", n);   /* ERRADO: %d espera int, não size_t */
```

**Correto**: `printf("%zu\n", n);`.

**Por que é perigoso**: `size_t` é unsigned, e pode ser maior que
`int`. O `printf` interpreta os bits como `int`, e o resultado é
**lixo** (para valores grandes).

### 3. Esquecer o `l` ou `ll` para `long`/`long long`

```c
long x = 42;
printf("%d\n", x);    /* ERRADO: falta o l */
printf("%ld\n", x);   /* CERTO */
```

### 4. Passar o tipo errado para `%s`

```c
int x = 42;
printf("%s\n", x);   /* ERRADO: %s espera char *, não int */
```

**Por que é perigoso**: o `printf` trata `x` como ponteiro, e
tenta ler uma string em um endereço inválido. **Segfault** (ou
lixo).

### 5. Esquecer o `%%` para porcentagem

```c
printf("100%\n");    /* ERRADO: % seguido de \n é inválido */
printf("100%%\n");   /* CERTO: %% imprime % */
```

## Como o `printf` sabe o tipo?

O `printf` **não sabe** o tipo dos argumentos (C não tem
reflexão). Ele **confia** no especificador de formato. Se você
passa `%d` para um `long`, o `printf` lê **4 bytes** (o tamanho
de `int`), e ignora os outros 4 (se `long` for 8 bytes). O
resultado é **lixo**.

**Por isso o compilador avisa** (com `-Wall -Wextra`): se o
especificador não bate com o tipo, o GCC/Clang emitem warning.
**Sempre** compile com warnings, e **corrija** os que aparecerem.

## Resumo

| Especificador | Tipo | Exemplo |
|---|---|---|
| `%d` / `%i` | `int` | `printf("%d", 42)` |
| `%u` | `unsigned int` | `printf("%u", 42u)` |
| `%ld` / `%lu` | `long` / `unsigned long` | `printf("%ld", 42L)` |
| `%lld` / `%llu` | `long long` / `unsigned long long` | `printf("%lld", 42LL)` |
| `%zu` | `size_t` | `printf("%zu", strlen(s))` |
| `%x` / `%X` | `unsigned int` (hex) | `printf("%x", 255)` |
| `%o` | `unsigned int` (octal) | `printf("%o", 8)` |
| `%f` / `%e` / `%g` | `double` | `printf("%f", 3.14)` |
| `%c` | `char` | `printf("%c", 'A')` |
| `%s` | `char *` | `printf("%s", "ola")` |
| `%%` | (nenhum) | `printf("100%%")` |
| `%p` | `void *` | `printf("%p", ptr)` |

**Regra prática**: o especificador **precisa** bater com o tipo
do argumento. Na dúvida, consulte a tabela, ou compile com
`-Wall -Wextra` (o compilador avisa).

## Leitura recomendada

- `man 3 printf` (Linux/macOS) — a referência completa.
- *The C Programming Language*, Kernighan & Ritchie — seção 7.2
  (`printf`).
- *Modern C*, Jens Gustedt — capítulo sobre I/O formatado.

---

arataca89@gmail.com

Aulas de programação
