# Projetos do CCS

Sumário dos softwares da coleção. Cada projeto é autocontido:
tem seu próprio `README.md`, `notes.md` e testes.

## Sumário

| # | Projeto | Categoria | Conceitos principais |
|---|---|---|---|
| 001 | [wc](001_wc/) | Utilitário de linha de comando | Streams, `EOF`, máquina de estados, portabilidade |
| 002 | [cat](002_cat/) | Utilitário de linha de comando | Parsing de flags, máquina de estados, lookahead, composição |
| 003 | [grep](003_grep/) | Utilitário de linha de comando | Alocação dinâmica (`malloc`/`realloc`), `size_t`, busca literal, código de retorno POSIX |
| 004 | [list](004_linked_list/) | Estrutura de dados | Ponteiros intensivos, `malloc`/`free` estruturados, `struct Node **`, CLI com `isatty` |
| 005 | [getopt](005_getopt/) | Utilitário de linha de comando | Parsing de flags POSIX, variáveis globais, `static` local, flags agrupadas |
| 006 | [hexdump](006_hexdump/) | Ferramenta de sistema | Leitura binária (`fread`), formatação hex, ASCII imprimível, `fseek`, `ferror` |
| 007 | [string_utils](007_string_utils/) | Utilitário de linha de comando | `malloc`/`free`, `memcpy`, aritmética de ponteiros, `ptrdiff_t`, array de ponteiros |
| 008 | [rle](008_rle/) | Ferramenta de sistema | Compressão RLE, leitura/escrita binária, `_setmode`, `ferror`, detecção de corrupção |

*(novos projetos serão adicionados aqui)*

## Trilha sugerida (para quem está aprendendo)

Se você está começando em C, leia nesta ordem:

1. **[001 — wc](001_wc/)** — pequeno, verificável, cobre o essencial
   de streams e portabilidade.
2. **[002 — cat](002_cat/)** — mais flags, parsing de `argv`,
   máquina de estados mais rica, lookahead de 1 caractere.
3. **[003 — grep](003_grep/)** — alocação dinâmica (`malloc`/`realloc`),
   `size_t`, busca literal, código de retorno POSIX.
4. **[004 — list](004_linked_list/)** — ponteiros intensivos,
   `malloc`/`free` estruturados, `struct Node **`, CLI com `isatty`.
5. **[005 — getopt](005_getopt/)** — parsing de flags POSIX,
   variáveis globais, `static` local, flags agrupadas.
6. **[006 — hexdump](006_hexdump/)** — leitura binária (`fread`),
   formatação hex, ASCII imprimível, `fseek`, `ferror`.
7. **[007 — string_utils](007_string_utils/)** — funções de
   string (`strdup`, `strtrim`, `strsplit`), `malloc`/`free`,
   `memcpy`, aritmética de ponteiros.
8. **[008 — rle](008_rle/)** — compressão RLE, leitura/escrita
   binária, `_setmode`, `ferror`, detecção de corrupção.

*(a trilha cresce junto com a coleção)*

## Categorias

Os projetos são agrupados por **categoria de uso real**, não por
dificuldade. Assim o leitor encontra o que procura.

| Categoria | O que cobre | Projetos |
|---|---|---|
| Utilitários de linha de comando | Ferramentas do dia a dia (wc, cat, grep...) | [001](001_wc/), [002](002_cat/), [003](003_grep/), [005](005_getopt/), [007](007_string_utils/) |
| Estruturas de dados | Listas, árvores, hash tables | [004](004_linked_list/) |
| Ferramentas de sistema | `ls`, `/proc`, syscalls, hexdump, compressão | [006](006_hexdump/), [008](008_rle/) |
| Redes | Sockets, protocolos | *(em breve)* |
| Linguagens e parsers | Interpretadores, compiladores | *(em breve)* |
| Jogos e gráficos em terminal | Snake, Tetris, ASCII art | *(em breve)* |

---

arataca89@gmail.com

Aulas particulares de programação
