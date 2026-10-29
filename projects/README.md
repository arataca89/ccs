# Projetos do CCS

Sumário dos softwares da coleção. Cada projeto é autocontido:
tem seu próprio `README.md`, `notes.md` e testes.

## Sumário

| # | Projeto | Categoria | Conceitos principais |
|---|---|---|---|
| 001 | [wc](001_wc/) | Utilitário de linha de comando | Streams, `EOF`, máquina de estados, portabilidade |
| 002 | [cat](002_cat/) | Utilitário de linha de comando | Parsing de flags, máquina de estados, lookahead, composição |

*(novos projetos serão adicionados aqui)*

## Trilha sugerida (para quem está aprendendo)

Se você está começando em C, leia nesta ordem:

1. **[001 — wc](001_wc/)** — pequeno, verificável, cobre o essencial
   de streams e portabilidade.
2. **[002 — cat](002_cat/)** — mais flags, parsing de `argv`,
   máquina de estados mais rica, lookahead de 1 caractere.

*(a trilha cresce junto com a coleção)*

## Categorias

Os projetos são agrupados por **categoria de uso real**, não por
dificuldade. Assim o leitor encontra o que procura.

| Categoria | O que cobre | Projetos |
|---|---|---|
| Utilitários de linha de comando | Ferramentas do dia a dia (wc, cat, grep...) | 001, 002 |
| Estruturas de dados | Listas, árvores, hash tables | *(em breve)* |
| Redes | Sockets, protocolos | *(em breve)* |
| Linguagens e parsers | Interpretadores, compiladores | *(em breve)* |
| Jogos e gráficos em terminal | Snake, Tetris, ASCII art | *(em breve)* |
| Ferramentas de sistema | `ls`, `/proc`, syscalls | *(em breve)* |