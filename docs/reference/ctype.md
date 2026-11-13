# `<ctype.h>` — classificação de caracteres

Funções para classificar e converter caracteres. Usado no
`003_grep` (para `tolower`) e no `005_getopt` (para `tolower`).

## Classificação

### `isdigit`

**Header**: `<ctype.h>`

**Assinatura**: `int isdigit(int c);`

**O que faz**: retorna não-zero se `c` é um **dígito decimal**
(`'0'` a `'9'`), zero caso contrário.

**Retorno**: não-zero (verdadeiro) ou 0 (falso).

**Onde usamos**: **não usamos** ainda (mas é comum em parsers).

**Cuidados**: o argumento é `int` (não `char`), e deve ser um
valor de `unsigned char` ou `EOF`. Passar `char` diretamente
pode dar **comportamento indefinido** para valores > 127. Use
`isdigit((unsigned char)c)`.

**Ver também**: [`isalpha`](#isalpha), [`isspace`](#isspace).

### `isspace`

**Header**: `<ctype.h>`

**Assinatura**: `int isspace(int c);`

**O que faz**: retorna não-zero se `c` é um **espaço em branco**
(espaço, `\t`, `\n`, `\v`, `\f`, `\r`), zero caso contrário.

**Retorno**: não-zero ou 0.

**Onde usamos**: **não usamos** ainda (o `001_wc` faz a
verificação manual, para evitar dependência de locale).

**Cuidados**: mesmo que `isdigit` — use
`isspace((unsigned char)c)`.

**Ver também**: [`isdigit`](#isdigit), [`isalpha`](#isalpha).

### `isalpha`

**Header**: `<ctype.h>`

**Assinatura**: `int isalpha(int c);`

**O que faz**: retorna não-zero se `c` é uma **letra** (`a-z`,
`A-Z`), zero caso contrário.

**Retorno**: não-zero ou 0.

**Onde usamos**: **não usamos** ainda.

**Cuidados**: mesmo que `isdigit` — use
`isalpha((unsigned char)c)`.

**Ver também**: [`isdigit`](#isdigit), [`isspace`](#isspace).

### `isprint`

**Header**: `<ctype.h>`

**Assinatura**: `int isprint(int c);`

**O que faz**: retorna não-zero se `c` é um **caractere
imprimível** (incluindo espaço), zero caso contrário.

**Retorno**: não-zero ou 0.

**Onde usamos**: **não usamos** ainda (o `006_hexdump` vai usar
a verificação manual `c >= 0x20 && c <= 0x7E`, para
portabilidade).

**Cuidados**: `isprint` depende de **locale**. Para ASCII puro,
a verificação manual é mais previsível.

**Ver também**: [`isalpha`](#isalpha).

## Conversão

### `tolower`

**Header**: `<ctype.h>`

**Assinatura**: `int tolower(int c);`

**O que faz**: se `c` é uma **letra maiúscula**, retorna a
minúscula correspondente. Senão, retorna `c` inalterado.

**Retorno**: o caractere convertido (ou `c` inalterado).

**Onde usamos**: `003` (`strcasestr_impl`), `005` (não
diretamente, mas o `getopt` não usa `tolower`).

**Cuidados**: mesmo que `isdigit` — use
`tolower((unsigned char)c)`. Passar `char` diretamente pode dar
**comportamento indefinido** para valores > 127 (ex.: acentos
em UTF-8). Ver
[notes.md do 003_grep](../../projects/003_grep/notes.md#tolower-com-unsigned-char).

**Ver também**: [`toupper`](#toupper).

### `toupper`

**Header**: `<ctype.h>`

**Assinatura**: `int toupper(int c);`

**O que faz**: se `c` é uma **letra minúscula**, retorna a
maiúscula correspondente. Senão, retorna `c` inalterado.

**Retorno**: o caractere convertido (ou `c` inalterado).

**Onde usamos**: **não usamos** ainda.

**Cuidados**: mesmo que `tolower` — use
`toupper((unsigned char)c)`.

**Ver também**: [`tolower`](#tolower).
