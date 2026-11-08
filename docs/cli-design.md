# Design de CLI: flags, comandos e modo interativo

> Um programa de linha de comando (CLI) tem **interface**. E
> interface se **projeta**. Este texto cobre os três estilos
> principais, com exemplos do CCS.

## Por que design de CLI importa

Um programa CLI é usado por **humanos** e por **scripts**. Os dois
têm necessidades diferentes:

- **Humanos** querem comandos curtos, help claro, mensagens de erro
  úteis.
- **Scripts** querem saída previsível, exit codes consistentes,
  ausência de prompts.

Um bom design de CLI atende aos dois. Um mau design frustra os dois.

## Estilo 1: flags (Unix clássico)

O programa faz **uma coisa**, e as **flags** modificam **como** ele
faz.

```
cat -nE arquivo.txt
grep -in "padrao" arquivo.txt
```

**Características**:

- **Flags curtas** (`-n`, `-E`) ou **longas** (`--number`,
  `--show-ends`).
- **Flags agrupadas**: `-nE` é o mesmo que `-n -E`.
- **Flags antes ou depois** dos argumentos: `cat arquivo -n` é
  válido (embora incomum).
- **`-` = stdin**: `cat -` lê de stdin.
- **`-h` ou `--help`**: mostra ajuda.
- **Exit codes**: 0 = sucesso, 1 = falha (ou 2 = erro, no `grep`).

**Quando usar**: quando o programa faz **uma coisa** (copiar,
buscar, contar), e as flags são **modificadores**. É o estilo da
maioria dos utilitários Unix (`cat`, `grep`, `ls`, `cp`, `mv`).

**Vantagem**: familiar para quem usa Unix. Fácil de combinar em
pipelines (`grep "x" arquivo | wc -l`).

**Desvantagem**: se o programa faz **muitas coisas diferentes**,
as flags viram uma sopa (`-a -b -c -d -e ...`), e fica difícil
lembrar o que cada uma faz.

## Estilo 2: comandos (subcomandos)

O programa faz **várias coisas**, e o **primeiro argumento** é o
**comando**.

```
git commit -m "mensagem"
git push origin main
list push 42
list print
```

**Características**:

- **Primeiro argumento** é o comando (`commit`, `push`, `print`).
- **Argumentos seguintes** são parâmetros do comando.
- **Flags** podem existir **por comando** (`git commit -m` vs.
  `git push -u`).
- **`help`** é um comando (`list help`), não uma flag.

**Quando usar**: quando o programa tem **muitos comandos
diferentes**, e as flags não dão conta. É o estilo de `git`,
`docker`, `cargo`, `npm`.

**Vantagem**: escala. Cada comando tem suas próprias flags e
argumentos, e o help pode ser **por comando** (`git help commit`).

**Desvantagem**: mais verboso (`git commit -m "x"` vs. `commit "x"`).
E o parsing é mais complexo (primeiro argumento é comando, resto é
por comando).

## Estilo 3: interativo (REPL)

O programa **não recebe argumentos** (ou recebe poucos), e o
usuário digita comandos **em uma sessão**.

```
$ list
> push 10
> push 20
> print
20
10
> quit
```

**Características**:

- **Prompt** (`> `) antes de cada comando.
- **Estado persiste** entre comandos (a lista, no `list`).
- **`quit` ou `exit`** para sair.
- **`help`** mostra os comandos disponíveis.
- **Batch**: se a entrada vem de um arquivo (`list < comandos.txt`),
  o **prompt não aparece**, e a saída é limpa.

**Quando usar**: quando o programa **mantém estado** (uma lista,
uma conexão, um editor), e o usuário quer **explorar** ou
**manipular** esse estado aos poucos. É o estilo de `python` (REPL),
`sqlite3`, `gdb`, `bc`.

**Vantagem**: exploratório. O usuário vê o estado, testa comandos,
e vê o resultado imediatamente.

**Desvantagem**: difícil de automatizar (precisa de `expect`, ou de
um modo batch). E o estado persiste entre comandos, o que pode
confundir (um comando afeta o próximo).

## Como escolher

| Se o programa... | Use... | Exemplo |
|---|---|---|
| Faz **uma coisa**, e as flags modificam **como** | **Flags** | `cat`, `grep`, `wc` |
| Faz **várias coisas diferentes** | **Comandos** | `git`, `docker` |
| **Mantém estado** e o usuário quer **explorar** | **Interativo** (com modo batch) | `list` (CCS), `python` (REPL) |

**Nota**: os estilos **não são exclusivos**. O `git` tem comandos
**e** flags. O `list` tem comandos **e** modo interativo. O
importante é **consistência**: escolha um estilo dominante e siga.

## Exit codes: o contrato com scripts

O **exit code** (código de retorno) é a forma como o programa
comunica **sucesso ou falha** para o shell e para scripts.

**Convenção Unix**:

| Código | Significado |
|---|---|
| **0** | Sucesso |
| **1** | Falha genérica (ou "não encontrou", em `grep`) |
| **2** | Erro de uso (flag inválida, arquivo não encontrado) |
| **> 2** | Códigos específicos do programa |

**No CCS**:

- **`wc`**: 0 (sucesso), 1 (erro de leitura).
- **`cat`**: 0 (sucesso), 1 (erro de leitura/escrita).
- **`grep`**: 0 (achou), 1 (não achou), 2 (erro). **POSIX**.
- **`list`**: 0 (sucesso), 1 (erro de alocação).

**Por que importa**: scripts usam `if grep -q padrao arquivo; then
...`. O `-q` (quiet) do `grep` real retorna 0 se achou, 1 se não
achou, **sem imprimir nada**. O exit code é a **única** forma de
saber o resultado.

**No `004_linked_list`**: o `list` retorna 0 em sucesso, 1 em erro
de alocação. Não há "não achou" no exit code (o `find` imprime
"nao encontrado", mas o exit code é 0). Isso é uma **simplificação**
do escopo; o `notes.md` registra a decisão.

## O `-h` (help): obrigatório em todo CLI

Todo programa CLI deve ter **help**. Sem ele, o usuário não
descobre as flags/comandos.

**Formato**:

```
Uso: <programa> [OPCOES] [ARGUMENTOS]

Descrição curta do que o programa faz.

Opções:
  -a    o que -a faz
  -b    o que -b faz

Comandos (se for estilo 2):
  comando1    o que faz
  comando2    o que faz

Exemplos:
  <programa> -a arquivo
  <programa> comando1 arg

Notas:
  (qualquer coisa importante que o usuário deva saber)
```

**No CCS**: `cat -h`, `grep -h`, `list help` mostram o help. O
`wc` não tem help (é simples demais — só conta). Isso é uma
**decisão de escopo**; o `notes.md` do `001_wc` registra.

## Prompt e `isatty`: interativo vs. batch

Um programa que **lê de stdin** pode ser:

- **Interativo**: o usuário digita no terminal. Faz sentido mostrar
  um **prompt** (`> `), porque o usuário precisa saber que o
  programa está esperando.
- **Batch**: a entrada vem de um arquivo (`programa < comandos.txt`)
  ou de um pipe (`echo "x" | programa`). **Não** faz sentido
  mostrar prompt, porque a saída fica poluída (o prompt se
  mistura com os resultados).

**Como distinguir**: `isatty(fileno(stdin))` retorna `1` se o
stdin é um **terminal**, `0` se é arquivo/pipe.

```c
int interactive = isatty(fileno(stdin));

while (1) {
    if (interactive) {
        printf("> ");
        fflush(stdout);   /* prompt sem \n precisa de fflush */
    }
    if (fgets(line, sizeof line, stdin) == NULL)
        break;
    /* ... processa ... */
}
```

**Pontos importantes**:

- **`isatty`** (POSIX) e **`_isatty`** (Windows) detectam terminal.
  O `#ifdef _WIN32` resolve a portabilidade (ver
  [notes.md do 004](../../projects/004_linked_list/notes.md)).
- **`fileno(stdin)`** retorna o descritor de arquivo do `stdin`
  (`0` no Unix). O `isatty` espera um descritor, não um `FILE *`.
- **`fflush(stdout)`** é **obrigatório** após o prompt. Sem ele,
  o prompt fica no buffer (porque não tem `\n`), e não aparece
  antes do `fgets`. O usuário vê o cursor piscando **sem prompt**.

**Isso é uma sutileza de I/O** que vale registrar: `stdout` é
**bufferizado** (line-buffered em terminal, full-buffered em pipe).
Prompts sem `\n` precisam de `fflush`.

## Parsing de flags: manual vs. `getopt`

Em C, há duas formas de fazer parsing de flags:

### `getopt` (POSIX)

```c
int opt;
while ((opt = getopt(argc, argv, "nEis")) != -1) {
    switch (opt) {
    case 'n': ... break;
    case 'E': ... break;
    /* ... */
    }
}
```

**Vantagem**: testado, cobre casos de borda (flags agrupadas, `-`
sozinho, `--` para parar).

**Desvantagem**: **não é C padrão**. No Linux/macOS, está em
`<unistd.h>`. No Windows/MSVC, **não existe** (MinGW tem, MSVC
não). Usar `getopt` **quebra** a portabilidade do CCS.

### Parsing manual

```c
for (int i = 1; i < argc; i++) {
    const char *arg = argv[i];
    if (arg[0] != '-' || arg[1] == '\0')
        break;
    for (const char *p = arg + 1; *p; p++) {
        switch (*p) {
        case 'n': ... break;
        case 'E': ... break;
        /* ... */
        }
    }
}
```

**Vantagem**: **C padrão**, portável. Didático: o aluno vê como o
parsing funciona por dentro.

**Desvantagem**: mais código. Casos de borda (flags agrupadas,
`-` sozinho) precisam ser tratados manualmente.

**No CCS**: usamos **parsing manual**, porque `getopt` não é
portável (ver [notes.md do 002_cat](../../projects/002_cat/notes.md)
e [notes.md do 003_grep](../../projects/003_grep/notes.md)). O
`docs/style-guide.md` registra a decisão.

## Mensagens de erro: úteis, não crípticas

Uma mensagem de erro **boa** diz:

1. **O que deu errado** ("opcao invalida").
2. **Onde** ("'z'").
3. **Como corrigir** ("Tente 'grep -h' para ajuda.").

**Exemplo ruim**:

```
error
```

**Exemplo bom**:

```
grep: opcao invalida -- 'z'
Tente 'grep -h' para ajuda.
```

**No CCS**: todas as mensagens de erro seguem esse padrão. O
`notes.md` de cada projeto registra as decisões.

## Resumo

| Estilo | Quando usar | Exemplo |
|---|---|---|
| **Flags** | Uma coisa, flags modificam | `cat -n`, `grep -i` |
| **Comandos** | Várias coisas | `git commit`, `list push` |
| **Interativo** | Estado, exploração | `list`, `python` |

**Regras**:

1. **Exit codes consistentes** (0 = sucesso, 1 = falha, 2 = erro).
2. **`-h`/`help` obrigatório**.
3. **Prompt só se `isatty`** (batch fica limpo).
4. **`fflush(stdout)` após prompt** (sem `\n`, precisa de flush).
5. **Parsing manual** (portabilidade).
6. **Mensagens de erro úteis** (o que, onde, como corrigir).

## Leitura recomendada

- *The Art of Unix Programming*, Eric Raymond — capítulo 10
  ("Command-Line Interfaces").
- *The Pragmatic Programmer*, Hunt & Thomas — seção sobre
  "Good-Enough Software" (inclui CLIs).
- POSIX Utility Conventions:
  <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html>
