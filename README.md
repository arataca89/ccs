# CCS — C Collection Software

> C não é apenas uma linguagem: é a ferramenta com que se constroem
> ferramentas. Este repositório reúne softwares escritos em C para
> mostrar, com código real, o que a linguagem ainda faz — e continuará
> fazendo.

## O que é isto

O CCS é uma coleção de projetos em C, cada um autocontido, portável
e documentado. O objetivo é didático: mostrar a estudantes e
programadores que C, nascida em 1972, continua sendo a base dos
sistemas que usamos todos os dias — sistemas operacionais, bancos
de dados, servidores web, compiladores, ferramentas de linha de
comando.

Cada projeto traz:

- Código-fonte comentado e modularizado.
- Um `README.md` explicando o que o software faz e onde ele é usado
  na vida real.
- Um `notes.md` com as decisões de implementação, os bugs
  encontrados e os aprendizados.
- Testes em `tests/`, com gabaritos **medidos** (não calculados à
  mão) e um `tests/README.md` explicando cada fixture.

## Índice de projetos

| # | Projeto | Categoria | O que demonstra |
|---|---|---|---|
| 001 | [wc](projects/001_wc/) | Utilitário de linha de comando | Contador de linhas, palavras e bytes. Streams, `EOF`, máquina de estados, portabilidade Windows/Linux. |
| 002 | [cat](projects/002_cat/) | Utilitário de linha de comando | Concatenação e transformação de texto. Parsing manual de flags, máquina de estados, lookahead de 1 caractere, composição de transformações. |

*(novos projetos serão adicionados ao longo do tempo — ver
[sumário detalhado](projects/README.md))*

## Como compilar qualquer projeto

Todos usam CMake. Requer CMake ≥ 3.16 e um compilador C11 (GCC,
Clang ou MSVC).

```bash
cd projects/001_wc      # ou qualquer outro projeto
cmake -S . -B build
cmake --build build
```

No Windows com TDM-GCC, use o gerador MinGW:

```cmd
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

O executável fica em `build/<nome>` (Linux/macOS) ou
`build/<nome>.exe` (Windows). Cada projeto tem seu próprio
`README.md` com instruções específicas.

## Filosofia do repositório

- **Portabilidade de verdade**: o código compila e se comporta de
  forma coerente em Windows, Linux e macOS. Nada de `#ifdef` gratuito.
- **C padrão**: C11, sem extensões de compilador. Se compila limpo
  com `-Wall -Wextra -Wpedantic` (GCC/Clang) e `/W4` (MSVC), é C
  padrão de verdade.
- **Gabaritos medidos**: os resultados esperados dos testes são
  medidos rodando o próprio programa no arquivo commitado, nunca
  calculados à mão.
- **Registro do aprendizado**: cada projeto tem um `notes.md` com
  as dúvidas, os tropeços, os bugs e as descobertas. Aprender C é
  também aprender *como* se aprende C.
- **Decisões documentadas**: quando há trade-off (ex.: converter
  CRLF automaticamente ou só com flag), a escolha é registrada no
  `notes.md` com as alternativas consideradas e o porquê.

## Documentos

Textos transversais, que valem por si (podem ser lidos fora do
contexto de um projeto específico):

- [Por que C ainda importa](docs/why-c.md) — resposta às objeções
  comuns ("C é antiga", "C é difícil", "C é insegura").
- [Linha do tempo de C](docs/history.md) — de ALGOL a C23.
- [Máquina de estados](docs/state-machine.md) — o conceito, com
  exemplos no `wc`, no `cat` e em lexers.
- [Lookahead](docs/lookahead.md) — o conceito, com exemplos no
  `cat -U` e em lexers.
- [Guia de estilo do CCS](docs/style-guide.md) — convenções de
  código para todos os projetos.

## Licença

MIT. Ver [LICENSE](LICENSE).

