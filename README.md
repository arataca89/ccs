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
- Um `notes.md` com as decisões de implementação e os aprendizados.
- Testes com gabaritos medidos, para você verificar que funciona.

## Índice de projetos

| # | Projeto | O que demonstra |
|---|---|---|
| 001 | [wc](projects/001_wc/) | Contador de linhas, palavras e bytes. Streams, `EOF`, máquina de estados, portabilidade Windows/Linux. |

*(novos projetos serão adicionados ao longo do tempo)*

## Como compilar qualquer projeto

Todos usam CMake. Requer CMake ≥ 3.16 e um compilador C11 (GCC,
Clang ou MSVC).

```bash
cd projects/001_wc
cmake -S . -B build
cmake --build build
```

No Windows com TDM-GCC, use o gerador MinGW:

```cmd
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

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
  as dúvidas, os tropeços e as descobertas. Aprender C é também
  aprender *como* se aprende C.

## Licença

MIT. Ver [LICENSE](LICENSE).

