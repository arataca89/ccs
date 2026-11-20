# Testes do string_utils

O `007_string_utils` não tem fixtures de arquivo. Os testes são
**comandos de linha**, e a saída esperada é o que o `strings`
imprime. Os gabaritos abaixo foram **medidos** rodando `strings`
(TDM-GCC 10.3 / Windows 11, em 20/11/2026). Nenhum valor foi
calculado à mão.

## Como rodar

**Da raiz do projeto** (`007_string_utils/`):

    build\strings.exe COMANDO [ARGUMENTOS...]   (Windows)
    ./build/strings COMANDO [ARGUMENTOS...]     (Linux/macOS)

## Gabaritos

### `dup STR`

```cmd
build\strings.exe dup "ola"
```

Saída:

```
ola
```

**Explicação**: duplica a string `"ola"` (aloca + copia). O
resultado é impresso e liberado.

### `ndup N STR`

```cmd
build\strings.exe ndup 3 "ola mundo"
```

Saída:

```
ola
```

**Explicação**: duplica os primeiros 3 caracteres de
`"ola mundo"`. O resultado é `"ola"`.

### `trim STR`

```cmd
build\strings.exe trim "  ola  "
```

Saída:

```
ola
```

**Explicação**: remove os espaços do início e do fim. O
resultado é `"ola"`.

### `rev STR`

```cmd
build\strings.exe rev "ola"
```

Saída:

```
alo
```

**Explicação**: inverte a string. `"ola"` vira `"alo"`.

**Nota**: o `main` faz `strdup` antes de `str_reverse`, porque
`argv[2]` pode ser read-only. A cópia é mutável.

### `lower STR`

```cmd
build\strings.exe lower "OLA MUNDO"
```

Saída:

```
ola mundo
```

**Explicação**: converte para minúsculas. `"OLA MUNDO"` vira
`"ola mundo"`.

### `upper STR`

```cmd
build\strings.exe upper "ola mundo"
```

Saída:

```
OLA MUNDO
```

**Explicação**: converte para maiúsculas. `"ola mundo"` vira
`"OLA MUNDO"`.

### `split STR DELIM`

```cmd
build\strings.exe split "a,b,c" ,
```

Saída:

```
a
b
c
```

**Explicação**: divide `"a,b,c"` pelo delimitador `,`. Três
tokens: `"a"`, `"b"`, `"c"`. Cada um é impresso em uma linha.

### `join SEP STR...`

```cmd
build\strings.exe join ", " a b c
```

Saída:

```
a, b, c
```

**Explicação**: junta `"a"`, `"b"`, `"c"` com o separador
`", "`. O resultado é `"a, b, c"`.

**Nota**: as strings podem vir entre aspas (`"abc" "def"
"ghi"`) ou sem (`a b c`). O shell remove as aspas, e o `main`
recebe as strings.

### `help`

```cmd
build\strings.exe help
```

Saída: o texto do help (ver `print_help` no `main.c`). Mostra os
comandos, os exemplos, e a nota sobre `join`.

### Comando desconhecido

```cmd
build\strings.exe xyz
```

Saída (stderr):

```
build\strings.exe: comando desconhecido: 'xyz'
Tente 'build\strings.exe help' para ajuda.
```

**Exit code**: 1 (erro).

## Nota sobre `str_reverse` (e não `strrev`)

O MinGW (TDM-GCC) **já tem** uma função `strrev` na `<string.h>`,
com assinatura diferente (`char *strrev(char *)`). Para evitar
conflito, a nossa função foi renomeada para `str_reverse`. O
`notes.md` registra a decisão.

## Nota sobre `join` com `&argv[3]`

O `main.c` poderia passar `&argv[3]` diretamente para `strjoin`
(porque `argv` é terminado em `NULL`). Mas o código aloca um
array `parts` por clareza didática. O `notes.md` registra.

---

arataca89@gmail.com

Aulas de programação
