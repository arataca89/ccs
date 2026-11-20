# 004_list: lista ligada simples

> Implementação didática de uma lista ligada simples (singly linked
> list) de inteiros, com uma CLI para manipular.

## O que faz

Mantém uma lista ligada de inteiros em memória, e aceita comandos
via stdin (modo interativo ou batch):

```
$ list
> push 10
> push 20
> append 30
> print
20
10
30
> size
3
> remove 10
removido: 10
> print
20
30
> quit
```

## Onde isso é usado na vida real

- **Pilhas e filas**: a lista ligada é a implementação clássica.
- **Tabelas hash**: encadeamento de colisões (cada bucket é uma
  lista ligada).
- **Grafos**: listas de adjacência.
- **Compiladores**: tabela de símbolos, árvore sintática.
- **Sistemas operacionais**: lista de processos prontos, lista de
  blocos livres.
- **Editores de texto**: buffer de linhas.
- **Histórico de navegação**: botão "voltar".
- **Blockchain**: cada bloco aponta para o anterior.

## Conceitos de C abordados

- **Ponteiros de forma intensiva**: cada nó tem `struct Node *next`.
- **Auto-referência**: `struct Node` contém um ponteiro para si mesma.
- **`malloc`/`free` estruturados**: alocar um nó, liberar um nó,
  liberar a lista inteira.
- **Ponteiro para ponteiro** (`struct Node **pp`): para inserir e
  remover sem caso especial para a cabeça.
- **O erro clássico de `free`**: liberar sem salvar o `next`
  (use-after-free).
- **`struct` como container** (`struct List { Node *head; }`):
  permite evoluir sem mudar as funções.
- **`const` como contrato**: `find`, `print`, `size` não modificam
  a lista.
- **`size_t`**: para contagem (como no `grep`).
- **CLI**: modo interativo vs. batch, prompt, `isatty`, `fflush`.
- **`strtol`**: para converter string em `int` com validação.

## Como compilar

Requer CMake ≥ 3.16 e um compilador C11.

**Linux/macOS**:

    cmake -S . -B build
    cmake --build build

**Windows com TDM-GCC**:

    cmake -S . -B build -G "MinGW Makefiles"
    cmake --build build

## Como executar

**Modo interativo** (o prompt `> ` aparece):

    build\list.exe          (Windows)
    ./build/list            (Linux/macOS)

E digite os comandos:

    > push 10
    > push 20
    > print
    20
    10
    > quit

**Modo batch** (lê de um arquivo, sem prompt):

    build\list.exe < testes\comandos.txt    (Windows)
    ./build/list < tests/comandos.txt      (Linux/macOS)

## Comandos

| Comando | O que faz |
|---|---|
| `push N` | Insere `N` no **início** da lista (O(1)) |
| `append N` | Insere `N` no **fim** da lista (O(n)) |
| `remove N` | Remove o primeiro nó com valor `N` |
| `find N` | Busca `N` (imprime `encontrado` ou `nao encontrado`) |
| `print` | Imprime a lista (um valor por linha) |
| `size` | Imprime o número de nós |
| `free` | Libera a lista (e recria uma vazia) |
| `help` | Mostra a ajuda |
| `quit` | Sai do programa |

Linhas em branco e linhas começando com `#` são ignoradas.

**Por que `push` é O(1) e `append` é O(n)?** Porque `push` insere
no início (não percorre a lista), e `append` insere no fim
(percorre até o último nó). Se quiséssemos `append` O(1),
precisaríamos de um ponteiro para o `tail` na `struct List`. Fica
para um projeto futuro. Ver [notes.md](notes.md) e
[docs/complexity.md](../../docs/complexity.md).

## Código de retorno

| Código | Significado |
|---|---|
| 0 | Sucesso |
| 1 | Erro (alocação, lista não criada) |

**Nota**: o `find` que não encontra imprime `nao encontrado` em
stdout, mas o exit code é 0 (o programa rodou com sucesso; a busca
é que não achou). Isso é uma **simplificação** do escopo; o
`grep` real usa exit code 1 para "não achou". Ver
[notes.md](notes.md).

## Modo interativo vs. batch

O `list` lê comandos de stdin. Se o stdin é um **terminal**, o
prompt `> ` aparece (modo interativo). Se o stdin é um
**arquivo/pipe**, o prompt **não** aparece (modo batch), e a saída
fica limpa para scripts.

Isso é feito com `isatty(fileno(stdin))`. Ver [notes.md](notes.md)
e [docs/cli-design.md](../../docs/cli-design.md).

## Testes

Ver [tests/README.md](tests/README.md). Dois fixtures
(`comandos.txt`, `comandos_erros.txt`), com gabaritos medidos.

## Notas de implementação

Ver [notes.md](notes.md). Cobre `isatty`/`fileno`, `strtol` com
validação, `fgets` vs. `read_line`, a CLI, e as lições gerais.

## Conceitos relacionados

- [Listas ligadas](../../docs/linked-list.md) — o conceito, com
  exemplos em C e os erros mais comuns.
- [Complexidade](../../docs/complexity.md) — por que `push` é O(1)
  e `append` é O(n).
- [Design de CLI](../../docs/cli-design.md) — modo interativo vs.
  batch, prompt, `isatty`.
- [Por que usar funções `static`](../../docs/static.md) — os
  helpers internos do `main.c` são `static`.

## Referências

- *The C Programming Language*, Kernighan & Ritchie — capítulo 6
  (structs) tem um exemplo de lista ligada (tabela de símbolos).
- *Algorithms in C*, Robert Sedgewick — capítulo sobre listas.
- *Introduction to Algorithms*, Cormen et al. — capítulo 10
  (estruturas de dados elementares).

---

arataca89@gmail.com

Aulas de programação
