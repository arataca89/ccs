# `const` com ponteiros: onde o `const` afeta?

> `const` em C não significa "constante" (como em outras
> linguagens). Significa "read-only" (somente leitura). E, com
> ponteiros, **a posição do `const` muda o que é read-only**.

## O problema: `const` com ponteiros confunde

Em C, `const` pode ser aplicado a **duas coisas diferentes** em
uma declaração de ponteiro:

1. **O dado apontado** (o `char`, o `int`, a `struct`).
2. **O ponteiro em si** (o endereço).

A posição do `const` na declaração define **qual dos dois** é
read-only. E a sintaxe é sutil:

```c
const char *p;        /* o char é const; o ponteiro não */
char const *p;        /* idêntico ao anterior */
char * const p;       /* o ponteiro é const; o char não */
const char * const p; /* ambos são const */
```

**As duas primeiras são idênticas** (`const char` e `char const`
são a mesma coisa). A terceira e a quarta têm o `const` **depois**
do `*`, o que muda o significado.

## A regra: leia da direita para a esquerda

A forma mais confiável de entender uma declaração de ponteiro com
`const` é **ler da direita para a esquerda**:

```c
const char *p;
```

Lendo da direita para a esquerda: "`p` é um ponteiro para `char`
const". Ou seja: **o `char` é const**; o ponteiro `p` pode ser
reapontado.

```c
char * const p;
```

Lendo da direita para a esquerda: "`p` é um `const` ponteiro para
`char`". Ou seja: **o ponteiro `p` é const**; o `char` pode ser
modificado.

```c
const char * const p;
```

"`p` é um `const` ponteiro para `char` const". Ou seja: **ambos
são const**.

**Isso é a regra do "clockwise spiral"** (espiral horária) do
livro *Expert C Programming*, mas a versão "da direita para a
esquerda" é mais simples e funciona para a maioria dos casos.

## As quatro combinações

### 1. `char *p` — nada é const

```c
char *p = "abc";
```

- **Pode mudar o ponteiro**: `p = "def";` ✅
- **Pode mudar o char**: `p[0] = 'X';` ✅

**Cuidado**: `"abc"` é uma **string literal**, que é **read-only**
em C (modificá-la é comportamento indefinido). Mas o **tipo** `char
*p` não impede. O `const` é uma **promessa do programador**, não
uma garantia do compilador.

**Isso é uma pegadinha clássica**: `char *p = "abc"; p[0] = 'X';`
compila, mas pode dar **segfault** (a string literal está em
memória read-only). **Sempre** use `const char *` para strings
literais.

### 2. `const char *p` — o char é const

```c
const char *p = "abc";
```

- **Pode mudar o ponteiro**: `p = "def";` ✅
- **Não pode mudar o char**: `p[0] = 'X';` ❌ (erro de compilação)

**Isso é o mais comum**: "ponteiro para dado read-only". O
`getopt` usa `const char *optstring` (a `optstring` é read-only).

**Por que é útil**: a função promete que **não modifica** o dado
apontado. O chamador pode passar uma string literal (que é
read-only) sem medo.

### 3. `char * const p` — o ponteiro é const

```c
char buf[10];
char * const p = buf;
```

- **Não pode mudar o ponteiro**: `p = outra_coisa;` ❌ (erro de
  compilação)
- **Pode mudar o char**: `p[0] = 'X';` ✅

**Isso é raro**: "ponteiro fixo para dado mutável". O ponteiro não
pode ser reapontado, mas o dado pode ser modificado.

**Uso típico**: quando você quer garantir que um ponteiro **sempre
aponte para o mesmo lugar** (ex.: um buffer fixo), mas o conteúdo
pode mudar.

### 4. `const char * const p` — ambos são const

```c
const char * const p = "abc";
```

- **Não pode mudar o ponteiro**: `p = "def";` ❌
- **Não pode mudar o char**: `p[0] = 'X';` ❌

**Isso é o mais restritivo**: "ponteiro fixo para dado
read-only". Útil para constantes globais:

```c
const char * const MENSAGEM = "erro fatal";
```

## `const` em parâmetros de função

Em parâmetros de função, `const` documenta o **contrato**:

```c
void imprime(const char *s);   /* promete não modificar s */
size_t strlen(const char *s);  /* promete não modificar s */
```

**Por que é útil**:

1. **Documenta a intenção**: quem lê o header sabe que `s` não é
   modificado.
2. **Permite passar strings literais**: `imprime("ola")` funciona,
   porque `"ola"` é read-only, e `const char *` aceita read-only.
   Se fosse `char *`, o compilador poderia avisar (dependendo do
   warning).
3. **Detecta erros**: se a função tentar `s[0] = 'X'`, o
   compilador reclama.

**No CCS**: `list_find`, `list_print`, `list_size` recebem
`const List *list` (a lista não é modificada). `getopt` recebe
`const char *optstring` (a `optstring` não é modificada).

## `const` em arrays de ponteiros

O `getopt` usa `char *const argv[]`. Vamos decifrar:

```c
char *const argv[]
```

Lendo da direita para a esquerda: "`argv` é um array de `const`
ponteiros para `char`". Ou seja:

- **`argv`** é um array (o `[]`).
- **Cada elemento** é um `char *const` (ponteiro const para char).
- **O ponteiro** (cada elemento) é const: não pode ser
  reapontado.
- **O char** pode ser modificado (mas o `getopt` não modifica).

**Por que `char *const argv[]` e não `const char *argv[]`**:

- **`char *const argv[]`**: os **ponteiros** do array são const
  (o `getopt` não pode fazer `argv[0] = outra_coisa`). Mas os
  **chars** podem ser modificados.
- **`const char *argv[]`**: os **chars** são const (o `getopt`
  não pode fazer `argv[0][0] = 'X'`). Mas os **ponteiros** podem
  ser reapontados.

O `getopt` **não modifica nem os ponteiros nem os chars**. Então,
na prática, `const char *const argv[]` seria o mais restritivo. Mas
a assinatura POSIX é `char *const argv[]` (por razões históricas),
e o `getopt` **promete** não modificar os chars.

**No `005_getopt`**: a assinatura é `char *const argv[]`, que é a
POSIX. O `notes.md` registra a sutileza.

## Resumo: a tabela

| Declaração | Pode mudar `p`? | Pode mudar `*p`? |
|---|---|---|
| `char *p` | ✅ | ✅ |
| `const char *p` | ✅ | ❌ |
| `char * const p` | ❌ | ✅ |
| `const char * const p` | ❌ | ❌ |

**Regra prática**: leia da direita para a esquerda. O `const`
antes do `*` afeta o **dado**; o `const` depois do `*` afeta o
**ponteiro**.

## Erros comuns

### 1. Achar que `const char *p` impede mudar o ponteiro

```c
const char *p = "abc";
p = "def";   /* ✅ permitido! o ponteiro não é const */
```

**`const char *p`** significa "ponteiro para char const". O
**ponteiro** pode ser reapontado; o **char** não.

### 2. Achar que `char * const p` impede mudar o char

```c
char buf[10];
char * const p = buf;
p[0] = 'X';   /* ✅ permitido! o char não é const */
```

**`char * const p`** significa "ponteiro const para char". O
**ponteiro** não pode ser reapontado; o **char** pode.

### 3. Esquecer o `const` em strings literais

```c
char *p = "abc";   /* ⚠️ perigoso: "abc" é read-only */
p[0] = 'X';        /* ❌ comportamento indefinido (pode dar segfault) */
```

**Sempre** use `const char *` para strings literais:

```c
const char *p = "abc";   /* ✅ seguro */
p[0] = 'X';              /* ❌ erro de compilação (bom!) */
```

### 4. Achar que `const` é "constante"

Em C, `const` **não** significa "constante em tempo de
compilação". Significa "read-only". O valor pode ser inicializado
em tempo de execução:

```c
int x = 42;
const int y = x;   /* ✅ y é read-only, mas o valor veio de x */
```

**Para "constante em tempo de compilação"**, use `#define` ou
`enum`:

```c
#define MAX 100
enum { TAM = 100 };
```

## Onde o `const` aparece no CCS

- **`002_cat`**: `const CatOptions *opts` (as opções não são
  modificadas).
- **`003_grep`**: `const char *pattern`, `const GrepOptions *opts`.
- **`004_linked_list`**: `const List *list` (em `find`, `print`,
  `size`).
- **`005_getopt`**: `char *const argv[]`, `const char *optstring`.

**Convenção do CCS**: `const` em todo parâmetro que a função
**não modifica**. Isso documenta a intenção, e permite passar
dados read-only.

## Leitura recomendada

- *Expert C Programming: Deep C Secrets*, Peter van der Linden —
  capítulo 3 ("The Shocking Truth: C Declarations") tem a regra
  do "clockwise spiral".
- *The C Programming Language*, Kernighan & Ritchie — seção 2.4
  (declarações) menciona `const` de passagem.
- `cdecl` (ferramenta online): <https://cdecl.org/> — traduz
  declarações C para inglês.
