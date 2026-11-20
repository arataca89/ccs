# Fixtures de teste do hexdump

Arquivos usados para verificar o comportamento do `hexdump`. Os
gabaritos abaixo foram **medidos** rodando `hexdump` nos arquivos
commitados (TDM-GCC 10.3 / Windows 11, em 16/11/2026). Nenhum valor
foi calculado à mão.

## Fixtures

| Arquivo | Descrição |
|---|---|
| `texto.txt` | Texto curto, **em CRLF** (Windows). 31 bytes. Serve para ver o `\r` (`0d`) e o `\n` (`0a`) no hexdump, e o `.` no ASCII (não-imprimíveis). |
| `binario.bin` | 16 bytes **binários** (0x00–0xFF). Serve para ver bytes não-imprimíveis (`.`) e imprimíveis (`ABCD ~`). |

**Nota sobre CRLF**: o `texto.txt` está em CRLF (criado no Sublime
Text sem conversão para LF). O `hexdump` mostra os bytes **reais**,
então o `0d` (CR) aparece no hex e como `.` no ASCII. Isso é
**intencional**: o hexdump é sobre bytes, não sobre texto. O
`notes.md` registra a decisão.

## Gabaritos: `texto.txt` (sem flags)

Comando:

```cmd
build\hexdump.exe tests\texto.txt
```

Saída esperada (medida):

```
00000000  48 65 6c 6c 6f 2c 20 68  65 78 64 75 6d 70 21 0d |Hello, hexdump!.|
00000010  0a 53 65 67 75 6e 64 61  20 6c 69 6e 68 61 2e    |.Segunda linha.|
```

**Explicação**:

- **31 bytes** (15 + 2 + 14). O `\r\n` está **entre** as duas
  linhas, e **não** há newline no final.
- `0d` = `\r` (CR), `0a` = `\n` (LF). O `\r` está no **fim** da
  linha 1 do arquivo, mas aparece no **fim** da linha 1 do
  hexdump. O `\n` "transborda" para a linha 2 do hexdump.
- No ASCII, `0d` e `0a` viram `.` (não-imprimíveis).
- `48`–`21` = `Hello, hexdump!`; `53`–`2e` = `Segunda linha.`.

## Gabaritos: `binario.bin` (sem flags)

Comando:

```cmd
build\hexdump.exe tests\binario.bin
```

Saída esperada (medida):

```
00000000  00 01 02 03 ff fe fd fc  41 42 43 44 20 7e 7f 80 |........ABCD ~..|
```

**Explicação**:

- **16 bytes** (uma linha completa).
- `00 01 02 03`: bytes de controle → `....`
- `ff fe fd fc`: bytes altos → `....`
- `41 42 43 44`: `ABCD`
- `20`: espaço
- `7e`: `~`
- `7f`: DEL → `.`
- `80`: byte alto → `.`
- ASCII: `........ABCD ~..` (8 pontos + `ABCD` + espaço + `~` + 2 pontos).

## Gabaritos: `-n N` (limitar bytes)

### `-n 16` (texto.txt)

Comando:

```cmd
build\hexdump.exe -n 16 tests\texto.txt
```

Saída esperada (medida):

```
00000000  48 65 6c 6c 6f 2c 20 68  65 78 64 75 6d 70 21 0d |Hello, hexdump!.|
```

**Explicação**: lê só os **primeiros 16 bytes** (uma linha
completa). O `\r` (`0d`) está no fim.

### `-n 8` (binario.bin)

Comando:

```cmd
build\hexdump.exe -n 8 tests\binario.bin
```

Saída esperada (medida):

```
00000000  00 01 02 03 ff fe fd fc                          |........|
```

**Explicação**: lê só os **primeiros 8 bytes**. Os 8 bytes
faltantes viram **padding** (3 espaços cada = 24 espaços), para
alinhar o ASCII. O `|........|` tem 8 pontos.

## Gabaritos: `-s N` (pular bytes)

### `-s 16` (texto.txt)

Comando:

```cmd
build\hexdump.exe -s 16 tests\texto.txt
```

Saída esperada (medida):

```
00000010  0a 53 65 67 75 6e 64 61  20 6c 69 6e 68 61 2e    |.Segunda linha.|
```

**Explicação**: pula os primeiros 16 bytes, e o offset começa em
`00000010` (16 em hex). O `0a` (`\n` do fim da linha 1) é o
primeiro byte mostrado, e vira `.` no ASCII.

### `-s 8` (binario.bin)

Comando:

```cmd
build\hexdump.exe -s 8 tests\binario.bin
```

Saída esperada (medida):

```
00000008  41 42 43 44 20 7e 7f 80                          |ABCD ~..|
```

**Explicação**: pula os primeiros 8 bytes, e o offset começa em
`00000008`. Os 8 bytes restantes são `ABCD ~..` no ASCII, com
padding à esquerda (os 8 bytes pulados não aparecem).

## Gabaritos: `-h` (help)

Comando:

```cmd
build\hexdump.exe -h
```

Saída: o texto do help (ver `print_help` no `main.c`). Mostra as
opções, os exemplos, e a nota sobre stdin.

## Gabaritos: flag inválida (`-x`)

Comando:

```cmd
build\hexdump.exe -x tests\texto.txt
```

Saída esperada (medida):

```
build\hexdump.exe: opcao invalida -- 'x'
Tente 'build\hexdump.exe -h' para ajuda.
```

**Exit code**: 1 (erro).

**Explicação**: o `main` imprime a mensagem de erro em `stderr`,
e a sugestão de `-h` em `stderr` também (ambas aparecem juntas no
terminal).

## Gabaritos: stdin

Comando:

```cmd
type tests\texto.txt | build\hexdump.exe
```

Saída esperada (medida):

```
00000000  48 65 6c 6c 6f 2c 20 68  65 78 64 75 6d 70 21 0a |Hello, hexdump!.|
00000010  53 65 67 75 6e 64 61 20  6c 69 6e 68 61 2e       |Segunda linha.|
```

**Atenção**: a saída do **stdin** é **diferente** da saída do
**arquivo** (`hexdump tests\texto.txt`). No arquivo, o `\r`
(`0d`) aparece; no stdin, **não**. Por quê?

**O `type` do Windows traduz `\r\n` para `\n`** quando
redireciona para um pipe. O `hexdump` recebe só `\n` (`0a`), e
mostra isso. **Não é bug do hexdump**: é o shell traduzindo.

**No Linux/macOS**, o `cat texto.txt | hexdump` **não** traduz
(mostra `0d 0a`), porque o `cat` copia bytes fielmente. Isso é
uma **diferença de plataforma** que vale registrar.

**Nota sobre `-s` e stdin**: `type texto.txt | hexdump -s 16`
**falha**, porque o `fseek` não funciona em stdin (não é
buscável). O `hexdump` imprime "nao foi possivel pular 16 bytes"
e retorna 1. Isso é **esperado** (o `notes.md` registra).

## Como rodar

**Da raiz do projeto** (`006_hexdump/`):

    build\hexdump.exe tests\texto.txt           (Windows)
    ./build/hexdump tests/texto.txt             (Linux/macOS)

## Nota sobre CRLF e LF

O `texto.txt` está em **CRLF** (Windows). O `hexdump` mostra os
bytes reais (`0d 0a`), e o ASCII mostra `.` para eles. Se você
converter o `texto.txt` para **LF** (Sublime: View → Line
Endings → Unix), o arquivo terá **30 bytes** (sem o `0d`), e o
hexdump será diferente. **Isso é uma lição**: o mesmo texto, em
CRLF e LF, tem hexdumps **diferentes** (o `\r` aparece ou não).
O `hexdump` é sobre **bytes**, não sobre texto. O `notes.md`
registra a decisão de manter o `texto.txt` em CRLF (para mostrar
o `\r`).

---

arataca89@gmail.com

Aulas de programação
