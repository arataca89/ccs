# 003_grep: busca de padrão em linhas

> Reinvenção didática do `grep`, com busca literal, flags de
> filtragem e leitura de linha com alocação dinâmica.

## O que faz

Busca um **padrão literal** nas linhas de um ou mais arquivos (ou
da entrada padrão), e imprime as linhas que casam. Com flags, filtra,
conta, numera, ou mostra só o nome do arquivo.

```
$ grep "compilacao" texto.txt
O processo de compilacao em C
A segunda e a COMPILACAO propriamente dita, que gera ASSEMBLY.
```

## Onde isso é usado na vida real

- É um dos utilitários mais usados do Unix, presente em todo
  sistema Unix-like (Linux, macOS, BSDs).
- A ideia "filtrar linhas por padrão" é a base de ferramentas como
  `awk`, `sed`, `rg` (ripgrep), `ag` (the silver searcher).
- Aparece em scripts de build, análise de logs, pipelines de
  processamento de texto, debugging.
- A implementação do `grep` original (1973) é um dos exemplos
  clássicos de código C bem escrito.

## Conceitos de C abordados

- **Alocação dinâmica** (`malloc`, `realloc`, `free`) para ler
  linhas de tamanho variável.
- **`size_t`** para tamanhos e capacidades (o tipo que `sizeof`,
  `malloc` e `strlen` usam).
- O padrão **doubling** (crescer o buffer × 2) para complexidade
  amortizada O(1).
- O erro clássico de `realloc` (`buf = realloc(buf, ...)` perde o
  ponteiro original).
- **Busca literal** com `strstr`, e implementação de
  `strcasestr` (case-insensitive) que não é C padrão.
- **Parsing manual de flags** (`-i`, `-v`, `-c`, `-n`, `-l`, `-U`, `-h`).
- **Código de retorno POSIX** (0 = achou, 1 = não achou, 2 = erro).
- **`tolower` com `unsigned char`** (evitar UB com caracteres > 127).

## Como compilar

Requer CMake ≥ 3.16 e um compilador C11.

**Linux/macOS**:

    cmake -S . -B build
    cmake --build build

**Windows com TDM-GCC**:

    cmake -S . -B build -G "MinGW Makefiles"
    cmake --build build

## Como executar

**Da raiz do projeto** (`003_grep/`):

    build\grep.exe "compilacao" tests\compilacao.txt    (Windows)
    ./build/grep "compilacao" tests/compilacao.txt      (Linux/macOS)

**Com flags**:

    build\grep.exe -i "compilacao" tests\compilacao.txt
    build\grep.exe -n "compilacao" tests\compilacao.txt
    build\grep.exe -c "compilacao" tests\compilacao.txt

**Múltiplos arquivos** (prefixa o nome):

    build\grep.exe "C" tests\compilacao.txt tests\historia.txt

**Lendo de stdin**:

    echo compilacao | build\grep.exe "compilacao"

**Ajuda**:

    build\grep.exe -h

## Flags

| Flag | O que faz |
|---|---|
| `-i` | Ignora maiúsculas/minúsculas |
| `-v` | Inverte: mostra linhas que **não** casam |
| `-c` | Conta linhas que casam (não imprime) |
| `-n` | Mostra número da linha |
| `-l` | Mostra só o nome do arquivo, se houver match |
| `-U` | Remove `\r` no fim de linha (converte CRLF em LF) |
| `-h` | Mostra ajuda e sai |

Flags podem ser agrupadas (`-in` = `-i -n`).

## Código de retorno

| Código | Significado |
|---|---|
| 0 | Pelo menos uma linha casou |
| 1 | Nenhuma linha casou |
| 2 | Erro (arquivo não encontrado, flag inválida) |

Isso é o comportamento do `grep` POSIX, e é **diferente** do `wc`
e do `cat` (que retornam 0/1).

## Nota sobre arquivos CRLF (Windows)

O `grep` busca bytes fielmente. Se o arquivo tem quebras `\r\n`
(Windows), o `\r` aparece na saída (invisível no terminal), e o
efeito de "voltar o cursor" pode ser notado com `-E` (que não
temos) ou com `certutil -dump` no arquivo de saída. **Isso não é
bug**: é o comportamento do `grep` POSIX.

**Os fixtures deste projeto (`tests/compilacao.txt` e
`tests/historia.txt`) estão em CRLF de propósito.** O `grep`
funciona com eles (a busca literal é robusta ao `\r` no fim). Se
você quer ver a saída **sem** `\r`, use `-U`:

    grep -U "padrao" arquivo.txt

Ou converta o arquivo antes com `dos2unix`. Ver [notes.md](notes.md)
para o histórico dessa decisão.

## Testes

Ver [tests/README.md](tests/README.md). Dois fixtures
(`compilacao.txt`, `historia.txt`), em CRLF, com gabaritos medidos.

## Notas de implementação

Ver [notes.md](notes.md). Cobre a decisão de `malloc`/`realloc`,
o bug do `-l` (filename vs show_filename), a distinção EOF vs
linha vazia, uma explicação sobre `size_t`, e a decisão de manter
os fixtures em CRLF.

## Conceitos relacionados

- [Máquina de estados](../../docs/state-machine.md) — usada no
  `wc` e no `cat`; o `grep` usa máquina de estados implícita no
  `read_line` (leitura caractere a caractere).
- [Lookahead](../../docs/lookahead.md) — o `grep -U` usa
  lookahead de 1 para distinguir `\r\n` de `\r` sozinho.

## Referências

- POSIX `grep`: <https://pubs.opengroup.org/onlinepubs/9699919799/utilities/grep.html>
- GNU grep: <https://www.gnu.org/software/grep/manual/grep.html>
- História do `grep`: <https://en.wikipedia.org/wiki/Grep>
