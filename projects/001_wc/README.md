# 001_wc: contador de linhas, palavras e bytes

> Reinvenção didática de um dos utilitários mais antigos do Unix (1971).

## O que faz

Lê um ou mais arquivos (ou a entrada padrão) e informa quantas
**linhas**, **palavras** e **bytes** cada um contém. Ao final, se
houver mais de um arquivo, mostra o total.

```
$ wc texto.txt
      42      187     1024 texto.txt
```

## Onde isso é usado na vida real

- É um dos primeiros utilitários do Unix (v1, 1971), escrito por
  Dennis Ritchie e Ken Thompson.
- Aparece em scripts de build, CI/CD, análise de logs e pipelines
  de bioinformática.
- A ideia "contar coisas em streams" é a base de ferramentas como
  `awk`, `grep -c`, `sort | uniq -c`.
- Praticamente todo sistema Unix-like (Linux, macOS, BSDs) traz uma
  versão em C.

## Conceitos de C abordados

- `fgetc`, `EOF` e por que `int c`, não `char c`.
- Leitura de streams (`FILE *`).
- Máquina de estados mínima (transição dentro/fora de palavra).
- Modularização (`wc.c` + `wc.h` + `main.c`).
- Tratamento de erro (`ferror`, `fopen` retornando `NULL`).
- Argumentos de linha de comando (`argc`, `argv`).
- Convenção `"-"` = stdin.
- Portabilidade: `#ifdef _WIN32`, modo binário, `\r\n` vs `\n`.
- Build system multiplataforma (CMake).

## Como compilar

Requer CMake ≥ 3.16 e um compilador C11 (GCC, Clang ou MSVC).

**Linux/macOS**:

    cmake -S . -B build
    cmake --build build

**Windows com TDM-GCC**:

    cmake -S . -B build -G "MinGW Makefiles"
    cmake --build build

O executável fica em `build/wc` (Linux/macOS) ou `build/wc.exe`
(Windows).

## Como executar

**Da raiz do projeto** (`001_wc/`):

    build\wc.exe tests\teste1.txt          (Windows)
    ./build/wc tests/teste1.txt            (Linux/macOS)

**Lendo da entrada padrão**:

    echo "ola mundo" | build\wc.exe        (Windows)
    echo "ola mundo" | ./build/wc          (Linux/macOS)

**Vários arquivos** (mostra o total ao final):

    build\wc.exe tests\teste1.txt tests\sample-long.txt

## Instalando no sistema (opcional)

**Windows**: copie `build\wc.exe` para uma pasta que esteja no `PATH`
(por exemplo `C:\Users\<voce>\bin`, adicionada às variáveis de
ambiente). Depois disso, `wc arquivo.txt` funciona de qualquer lugar.
O Windows não tem um `wc` nativo, então o nome está livre.

**Linux/macOS**: **não** substitua o `wc` do sistema — ele já existe
em `/usr/bin/wc` e é usado por scripts. Se quiser instalar o seu,
use um nome alternativo (por exemplo `wc-ccs`) ou chame pelo path
completo. Essa é uma lição de portabilidade: no Windows, `wc` é
"terra virgem"; no Linux, já tem dono.

## Testes

Ver [tests/README.md](tests/README.md). Três fixtures, com gabaritos
medidos, cobrem: contagem pequena verificável, texto longo, e arquivo
sem quebra de linha final.

## Notas de implementação

Ver [notes.md](notes.md).

## Referências

- POSIX `wc`: <https://pubs.opengroup.org/onlinepubs/9699919799/utilities/wc.html>
- História do `wc`: <https://en.wikipedia.org/wiki/Wc_(Unix)>
