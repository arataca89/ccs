# Tabela ASCII

> ASCII é o padrão que mapeia **números** (0–127) a **caracteres**
> (letras, dígitos, pontuação, controle). Todo programador C
> precisa conhecer, pelo menos, a faixa imprimível (32–126) e os
> caracteres de controle mais usados (`\n`, `\t`, `\0`).

## O que é ASCII

**ASCII** (American Standard Code for Information Interchange) é
um padrão de 1963 que define **128 caracteres** (0–127), cada um
com um **código numérico**:

- `0` a `31` (e `127`): **caracteres de controle** (não
  imprimíveis). Ex.: `\n` (10), `\t` (9), `\0` (0).
- `32` a `126`: **caracteres imprimíveis**. Ex.: espaço (32),
  `A` (65), `a` (97), `~` (126).

**Em C**, um `char` é um número (0–127, ou 0–255 se estendido).
Quando você escreve `'A'`, o compilador traduz para `65`. Quando
você imprime com `%c`, o `printf` traduz `65` de volta para `A`.

## A faixa imprimível (32–126)

| Código | Caractere | Código | Caractere | Código | Caractere |
|---|---|---|---|---|---|
| 32 | (espaço) | 64 | `@` | 96 | `` ` `` |
| 33 | `!` | 65 | `A` | 97 | `a` |
| 34 | `"` | 66 | `B` | 98 | `b` |
| 35 | `#` | 67 | `C` | 99 | `c` |
| 36 | `$` | 68 | `D` | 100 | `d` |
| 37 | `%` | 69 | `E` | 101 | `e` |
| 38 | `&` | 70 | `F` | 102 | `f` |
| 39 | `'` | 71 | `G` | 103 | `g` |
| 40 | `(` | 72 | `H` | 104 | `h` |
| 41 | `)` | 73 | `I` | 105 | `i` |
| 42 | `*` | 74 | `J` | 106 | `j` |
| 43 | `+` | 75 | `K` | 107 | `k` |
| 44 | `,` | 76 | `L` | 108 | `l` |
| 45 | `-` | 77 | `M` | 109 | `m` |
| 46 | `.` | 78 | `N` | 110 | `n` |
| 47 | `/` | 79 | `O` | 111 | `o` |
| 48 | `0` | 80 | `P` | 112 | `p` |
| 49 | `1` | 81 | `Q` | 113 | `q` |
| 50 | `2` | 82 | `R` | 114 | `r` |
| 51 | `3` | 83 | `S` | 115 | `s` |
| 52 | `4` | 84 | `T` | 116 | `t` |
| 53 | `5` | 85 | `U` | 117 | `u` |
| 54 | `6` | 86 | `V` | 118 | `v` |
| 55 | `7` | 87 | `W` | 119 | `w` |
| 56 | `8` | 88 | `X` | 120 | `x` |
| 57 | `9` | 89 | `Y` | 121 | `y` |
| 58 | `:` | 90 | `Z` | 122 | `z` |
| 59 | `;` | 91 | `[` | 123 | `{` |
| 60 | `<` | 92 | `\` | 124 | `\|` |
| 61 | `=` | 93 | `]` | 125 | `}` |
| 62 | `>` | 94 | `^` | 126 | `~` |
| 63 | `?` | 95 | `_` | | |

**Padrões úteis**:

- **Dígitos** (`0`–`9`): 48–57. **Contíguos**.
- **Maiúsculas** (`A`–`Z`): 65–90. **Contíguas**.
- **Minúsculas** (`a`–`z`): 97–122. **Contíguas**.
- **Diferença maiúscula/minúscula**: 32 (`'a' - 'A' == 32`).
  Isso permite converter com `c + 32` (mas use `tolower`).

**Por que isso importa**: se `'0'` é 48 e `'9'` é 57, então
`c - '0'` converte um dígito para o valor numérico (ex.: `'5' -
'0'` = 5). Isso é usado em parsers.

## Caracteres de controle (0–31 e 127)

| Código | Nome | Escape em C | O que faz |
|---|---|---|---|
| 0 | NUL | `\0` | Terminador de string |
| 7 | BEL | `\a` | Bipe (raro) |
| 8 | BS | `\b` | Backspace |
| 9 | HT | `\t` | Tab |
| 10 | LF | `\n` | Nova linha (Unix) |
| 11 | VT | `\v` | Tab vertical (raro) |
| 12 | FF | `\f` | Form feed (raro) |
| 13 | CR | `\r` | Retorno de carro (Windows: `\r\n`) |
| 27 | ESC | `\e` (não padrão) | Escape (terminal) |
| 127 | DEL | — | Delete (não imprimível) |

**Os mais usados no CCS**:

- **`\0`** (0): termina strings. **Todo** programa C lida com
  isso.
- **`\n`** (10): nova linha. O `wc` conta `\n`, o `cat -E` mostra
  `$`, o `grep` busca em linhas.
- **`\t`** (9): tab. O `cat -T` mostra `^I`.
- **`\r`** (13): retorno de carro. No Windows, `\r\n` é o
  separador de linha. O `cat -U` remove o `\r`.

## ASCII estendido (128–255)

**ASCII puro** vai só até 127. Os códigos **128–255** são
**extensões** (não fazem parte do ASCII original):

- **Latin-1** (ISO-8859-1): 128–255 mapeiam para caracteres
  acentuados e símbolos (ex.: 233 = `é`).
- **Windows-1252**: similar ao Latin-1, com algumas diferenças
  (ex.: 128 = `€`).
- **UTF-8**: 128–255 são **parte** de caracteres multi-byte
  (ex.: `é` = `0xC3 0xA9` em UTF-8).

**No `hexdump`**: o ASCII mostra `.` para **todos** os bytes
> 126 (incluindo 128–255), porque o `hexdump -C` assume ASCII
puro. Um byte `0xE9` (233) vira `.`, mesmo que em Latin-1 seja
`é`. **Isso é intencional**: o hexdump mostra bytes, não texto
em um encoding específico.

## Como usar a tabela no código

### Verificar se é dígito

```c
if (c >= '0' && c <= '9')   /* 48 a 57 */
    /* é dígito */
```

Ou, mais portável:

```c
if (isdigit((unsigned char)c))
    /* é dígito */
```

### Converter dígito para número

```c
int valor = c - '0';   /* '5' - '0' = 5 */
```

### Converter maiúscula para minúscula

```c
char minuscula = c + 32;   /* 'A' + 32 = 'a' */
```

Ou, mais portável:

```c
char minuscula = tolower((unsigned char)c);
```

### Verificar se é imprimível

```c
if (c >= 32 && c <= 126)   /* faixa imprimível */
    /* é imprimível */
```

É o que o `006_hexdump` faz no `is_printable`.

## ASCII vs. Unicode

**ASCII** é um subconjunto de **Unicode** (os primeiros 128
code points são iguais). **UTF-8** é uma codificação de Unicode
que usa 1 a 4 bytes por caractere:

- **ASCII** (0–127): 1 byte (igual ao ASCII).
- **Acentos** (128–2047): 2 bytes (ex.: `é` = `0xC3 0xA9`).
- **Outros** (2048+): 3 ou 4 bytes (ex.: emoji).

**No CCS**: os projetos usam **ASCII puro** (0–127) para os
fixtures, para evitar a complexidade do UTF-8. O `hexdump` mostra
bytes, não caracteres Unicode. O `notes.md` do `006_hexdump`
registra a decisão.

## Por que o `hexdump` mostra `.` para não-imprimíveis

O `hexdump -C` (e o nosso `006_hexdump`) mostra o ASCII entre
`|...|`, com `.` para bytes não-imprimíveis. **Por quê**:

1. **Legibilidade**: se mostrasse o byte cru, o terminal
   interpretaria (ex.: `\n` pularia linha, `\t` faria tab, `\r`
   voltaria o cursor). A saída ficaria bagunçada.
2. **Convenção**: o `hexdump -C` faz assim desde os anos 1980.
3. **Segurança**: bytes de controle podem ter efeitos no
   terminal (ex.: `\x1b` é escape). Mostrar `.` é seguro.

**A faixa imprimível** é 32–126. Fora disso, `.`.

## Resumo

| Faixa | O que é | Exemplos |
|---|---|---|
| 0–31 | Controle | `\0` (0), `\t` (9), `\n` (10), `\r` (13) |
| 32–126 | Imprimível | espaço (32), `0`–`9` (48–57), `A`–`Z` (65–90), `a`–`z` (97–122), `~` (126) |
| 127 | DEL | (não imprimível) |
| 128–255 | Estendido | Latin-1, Windows-1252, UTF-8 (multi-byte) |

**Regras práticas**:

1. **`'0'` é 48**, não 0. `c - '0'` converte dígito para número.
2. **`'A'` é 65**, `'a'` é 97. A diferença é 32.
3. **A faixa imprimível é 32–126**. Fora disso, não imprimível.
4. **Use `isdigit`, `isalpha`, `isprint`** (de `<ctype.h>`) para
   portabilidade. A verificação manual (`c >= '0' && c <= '9'`)
   funciona, mas não cobre locales.
5. **`\0` termina strings**. Todo programa C lida com isso.
6. **ASCII é subconjunto de UTF-8**. Para ASCII puro, 1 byte = 1
   caractere. Para acentos, UTF-8 usa 2+ bytes.

## Leitura recomendada

- Tabela ASCII completa: <https://www.asciitable.com/>
- *The C Programming Language*, Kernighan & Ritchie — seção 2.3
  (constantes de caractere).
- `man 7 ascii` (Linux) — a tabela no terminal.

---

arataca89@gmail.com

Aulas de programação
