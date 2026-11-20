# `argc` e `argv`: os argumentos do `main`

> Todo programa C começa por `main`. E `main` pode receber
> argumentos da linha de comando. `argc` e `argv` são como o
> programa os vê.

## O que são

A assinatura completa do `main` é:

```c
int main(int argc, char *argv[])
```

- **`argc`** (argument count): o **número** de argumentos,
  **incluindo** o nome do programa.
- **`argv`** (argument vector): um **array de strings** (ponteiros
  para `char`), com os argumentos.

**Exemplo**: se você roda

```
./programa alfa beta gama
```

O `argc` é **4**, e o `argv` é:

| Índice | Valor |
|---|---|
| `argv[0]` | `"./programa"` |
| `argv[1]` | `"alfa"` |
| `argv[2]` | `"beta"` |
| `argv[3]` | `"gama"` |
| `argv[4]` | `NULL` |

**Atenção**: `argc` é 4, mas `argv` tem **5** elementos (o último
é `NULL`). Isso é garantido pelo C padrão: `argv[argc] == NULL`.
O `NULL` no final é uma **conveniência** para percorrer o array
sem precisar de `argc`:

```c
for (int i = 0; argv[i] != NULL; i++)
    printf("%s\n", argv[i]);
```

**Mas**: a maioria dos programas usa `argc`, porque é mais
explícito (e mais seguro, se o `NULL` não estiver lá por algum
motivo).

## O `argv[0]`: o nome do programa

O `argv[0]` é o **nome do programa** como o shell o invocou. Isso
é útil para **mensagens de erro**:

```c
fprintf(stderr, "%s: arquivo nao encontrado\n", argv[0]);
```

Se o programa é `./meu_programa`, a mensagem fica `"./meu_programa:
arquivo nao encontrado"`. Se é `/usr/bin/meu_programa`, a mensagem
fica `"/usr/bin/meu_programa: arquivo nao encontrado"`.

**Isso é convenção Unix**: as mensagens de erro começam com o nome
do programa (para o usuário saber **qual** programa falhou, em um
pipeline).

**Cuidado**: o `argv[0]` **não é garantidamente** o nome do
programa. Um programa pode ser invocado com `argv[0]` diferente
(ex.: `execve` permite passar qualquer coisa). Mas, na prática,
é o nome.

**Se `argc == 0`** (raro, mas possível): `argv[0]` é `NULL`. O
programa deve tratar isso (ou ignorar).

## Como o shell passa os argumentos

O shell (bash, cmd, PowerShell) faz o **parsing** da linha de
comando **antes** de chamar o programa. O programa **não vê** a
linha original; vê o `argv` já separado.

**Exemplo**: o usuário digita

```
./programa "ola mundo" alfa
```

O shell:
1. Separa os argumentos por espaço, **respeitando aspas**.
2. Passa para o programa: `argv[0] = "./programa"`, `argv[1] =
   "ola mundo"` (com espaço, por causa das aspas), `argv[2] =
   "alfa"`.

**Sem aspas**, o shell separaria `"ola"` e `"mundo"` em dois
argumentos. **As aspas são do shell**, não do programa. O
programa vê `"ola mundo"` como **uma** string.

**Isso é importante**: o programa **não** precisa se preocupar com
aspas. O shell já resolveu.

## Por que `argv` é `char **`

`argv` é um **array de strings**. Em C, uma string é `char *`
(ponteiro para o primeiro caractere, terminada em `'\0'`). E um
array de strings é `char **` (ponteiro para o primeiro `char *`).

**Visualmente**:

```
argv
  |
  v
+--------+     +---+---+---+---+----+
| argv[0]|---->| . | / | p | r | \0 |
+--------+     +---+---+---+---+----+
| argv[1]|---->| a | l | f | a | \0 |
+--------+     +---+---+---+---+----+
| argv[2]|---->| b | e | t | a | \0 |
+--------+     +---+---+---+---+----+
| NULL   |
+--------+
```

- `argv` é `char **` (ponteiro para `char *`).
- `argv[i]` é `char *` (ponteiro para `char`).
- `argv[i][j]` é `char` (um caractere).

**A declaração `char *argv[]`** é equivalente a `char **argv`.
O `[]` é "açúcar sintático" para ponteiro. O compilador trata as
duas formas da mesma maneira.

## Como percorrer `argv`

### Com `argc` (mais comum)

```c
for (int i = 0; i < argc; i++)
    printf("argv[%d] = %s\n", i, argv[i]);
```

**Vantagem**: explícito, e não depende do `NULL` no final.

### Com `NULL` (mais "C idiomático")

```c
for (char **p = argv; *p != NULL; p++)
    printf("%s\n", *p);
```

**Vantagem**: não precisa de `argc`. **Desvantagem**: depende do
`NULL` no final (que é garantido, mas é uma "convenção").

**No CCS**: usamos `argc` (mais explícito). O `getopt` usa
`argc` e `argv` juntos.

## O `argc` e o `argv` no `getopt`

O `getopt` recebe `argc` e `argv`:

```c
int getopt(int argc, char *const argv[], const char *optstring);
```

**Por que `char *const argv[]`**:

- **`argv[]`**: array de `char *`.
- **`const`** depois do `*`: os **ponteiros** do array são const
  (o `getopt` não pode fazer `argv[0] = outra_coisa`).
- **Os chars** não são const (o `getopt` poderia modificar, mas
  não modifica).

Ver [const-pointers.md](const-pointers.md) para a diferença entre
`const char *`, `char * const`, e `const char * const`.

**O `optind`** é o índice do próximo argumento não-flag. Depois do
`getopt`, o chamador usa `argv[optind..argc-1]` para os arquivos:

```c
while ((opt = getopt(argc, argv, "vo:")) != -1) { ... }
for (int i = optind; i < argc; i++)
    printf("arquivo: %s\n", argv[i]);
```

## O que acontece se o programa modificar `argv`

O programa **pode** modificar `argv` (os chars, não os ponteiros,
se for `char *const argv[]`). Mas **não deve**:

1. **Modificar os chars** (`argv[1][0] = 'X'`) é **permitido** pelo
   tipo `char *const argv[]`, mas **confunde** o usuário (o
   argumento era `"alfa"`, e vira `"Xlfa"`).
2. **Modificar os ponteiros** (`argv[1] = "outra"`) **não é
   permitido** pelo tipo `char *const argv[]` (o compilador
   reclama).

**Convenção**: `argv` é **read-only** na prática. O programa lê,
não modifica. Se precisar de uma cópia modificável, faça uma
cópia:

```c
char *copia = strdup(argv[1]);   /* POSIX; ou implemente a sua */
/* ... modifica copia ... */
free(copia);
```

**No CCS**: nenhum projeto modifica `argv`. O `getopt` **lê**
`argv`, mas não modifica.

## Onde `argc`/`argv` aparecem no CCS

Todos os projetos com CLI usam `argc`/`argv`:

- **`001_wc`**: `int main(int argc, char **argv)` — processa
  arquivos.
- **`002_cat`**: `int main(int argc, char **argv)` — parsing
  manual de flags.
- **`003_grep`**: `int main(int argc, char **argv)` — parsing
  manual de flags.
- **`004_linked_list`**: `int main(void)` — **não** usa
  `argc`/`argv` (lê comandos de stdin).
- **`005_getopt`**: `int main(int argc, char *argv[])` — usa o
  `getopt` para parsing.

**Por que o `004` não usa**: o `list` é **interativo** (lê
comandos de stdin, não da linha de comando). O `argc`/`argv` não
faz sentido para ele. Isso é uma **decisão de design** (ver
[cli-design.md](cli-design.md)).

## O `envp`: o terceiro argumento (opcional)

Além de `argc` e `argv`, o `main` pode receber um **terceiro**
argumento: `envp` (environment pointer), o array de variáveis de
ambiente.

```c
int main(int argc, char *argv[], char *envp[])
```

**`envp`** é um array de strings no formato `"NOME=valor"`. Ex.:
`"PATH=/usr/bin:/bin"`, `"HOME=/home/user"`.

**Isso é uma extensão** (não é C padrão; é POSIX/Unix). No
Windows, também existe (com `_environ`). **No CCS**, não usamos
`envp` (não precisamos de variáveis de ambiente). Mas vale saber
que existe.

**Alternativa portável**: `getenv("PATH")` retorna o valor de uma
variável de ambiente (C padrão, em `<stdlib.h>`). É mais simples
que percorrer `envp`.

## Resumo

| Conceito | O que é |
|---|---|
| **`argc`** | Número de argumentos, **incluindo** o nome do programa |
| **`argv`** | Array de strings (`char **`), com os argumentos |
| **`argv[0]`** | Nome do programa (como o shell o invocou) |
| **`argv[argc]`** | `NULL` (garantido pelo C padrão) |
| **`envp`** | Variáveis de ambiente (extensão, opcional) |

**Regras práticas**:

1. **Use `argc`** para percorrer (mais explícito).
2. **`argv[0]`** é o nome do programa (use em mensagens de erro).
3. **Não modifique `argv`** (leia, não escreva).
4. **Aspas são do shell**, não do programa.
5. **`argc` é 1** se o programa não tem argumentos (só o nome).
6. **`argc` pode ser 0** (raro; `argv[0]` é `NULL`).

## Leitura recomendada

- *The C Programming Language*, Kernighan & Ritchie — seção 5.10
  ("Command-Line Arguments").
- *Modern C*, Jens Gustedt — capítulo sobre `main` e argumentos.
- POSIX `execve`:
  <https://pubs.opengroup.org/onlinepubs/9699919799/functions/execve.html>

---

arataca89@gmail.com

Aulas de programação
