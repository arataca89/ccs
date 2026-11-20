# 006_hexdump: visualizador hexadecimal

> Reinvenção didática do `hexdump -C`, que mostra o conteúdo de um
> arquivo em hexadecimal e ASCII.

## O que faz

Lê um arquivo (ou stdin) e mostra o conteúdo em **hex + ASCII**,
16 bytes por linha, com o offset no início. É o formato canônico
do `hexdump -C`:

```
$ hexdump arquivo.bin
00000000  48 65 6c 6c 6f 2c 20 68  65 78 64 75 6d 70 21 0a |Hello, hexdump!.|
00000010  53 65 67 75 6e 64 61 20  6c 69 6e 68 61 2e       |Segunda linha.|
```

## Onde isso é usado na vida real

- **Debugging de arquivos binários**: ver os bytes reais de uma
  imagem, um executável, um arquivo de rede.
- **Engenharia reversa**: analisar formatos de arquivo
  desconhecidos.
- **Análise forense**: ver o que tem dentro de um arquivo suspeito.
- **Desenvolvimento de protocolos**: ver os bytes de um pacote de
  rede.
- **Verificação de encoding**: ver se um arquivo é UTF-8, Latin-1,
  ou outra coisa.
- **Inspeção de CRLF vs. LF**: ver se um arquivo tem `\r\n` ou
  `\n` (o `hexdump` mostra `0d 0a` ou `0a`).

## Conceitos de C abordados

- **Leitura binária** (`fread` em vez de `fgetc`): lê blocos de
  bytes, não um por um.
- **Formatação de bytes** (`printf("%02x", byte)`): hex com 2
  dígitos, zero-padded.
- **Offset** (`%08lx`): hex com 8 dígitos, zero-padded.
- **ASCII imprimível**: verificação manual (`32` a `126`), sem
  `isprint` (para evitar dependência de locale).
- **`fseek`**: pular bytes (só funciona em arquivos buscáveis).
- **`ferror`**: detectar erro de I/O (diferente de EOF).
- **Padding**: alinhar a coluna ASCII quando a última linha tem
  menos de 16 bytes.

## Como compilar

Requer CMake ≥ 3.16 e um compilador C11.

**Linux/macOS**:

    cmake -S . -B build
    cmake --build build

**Windows com TDM-GCC**:

    cmake -S . -B build -G "MinGW Makefiles"
    cmake --build build

## Como executar

**Da raiz do projeto** (`006_hexdump/`):

    build\hexdump.exe arquivo.bin           (Windows)
    ./build/hexdump arquivo.bin             (Linux/macOS)

**Com flags**:

    build\hexdump.exe -n 32 arquivo.bin     (ler só 32 bytes)
    build\hexdump.exe -s 16 arquivo.bin     (pular 16 bytes)
    build\hexdump.exe -h                    (ajuda)

**Lendo de stdin**:

    type arquivo.bin | build\hexdump.exe    (Windows)
    cat arquivo.bin | ./build/hexdump       (Linux/macOS)

## Flags

| Flag | O que faz |
|---|---|
| `-s N` | Pula os primeiros `N` bytes (só funciona em arquivos, não em stdin) |
| `-n N` | Lê no máximo `N` bytes |
| `-h` | Mostra a ajuda e sai |

**Sem flags**: lê o arquivo inteiro (ou stdin) e mostra o
hexdump completo.

## Formato da saída

```
00000000  48 65 6c 6c 6f 2c 20 68  65 78 64 75 6d 70 21 0a |Hello, hexdump!.|
```

- **Offset** (8 dígitos hex): `00000000`, `00000010`, `00000020`...
- **16 bytes em hex** (2 grupos de 8, separados por espaço).
- **ASCII** entre `|...|`: bytes imprimíveis aparecem como
  caracteres; não-imprimíveis viram `.`.

## Código de retorno

| Código | Significado |
|---|---|
| 0 | Sucesso |
| 1 | Erro (arquivo não encontrado, flag inválida, erro de leitura) |

## Nota sobre CRLF e LF

O `hexdump` mostra os **bytes reais**, não o texto. Se o arquivo
tem quebras `\r\n` (Windows), o `\r` (`0d`) aparece no hex, e o
`\n` (`0a`) também. No ASCII, ambos viram `.`. **Isso não é bug**:
é o comportamento correto do `hexdump`.

**No Linux/macOS**, o `cat` copia bytes fielmente, e o `hexdump`
mostra `0d 0a` se o arquivo for CRLF. **No Windows**, o `type`
(quando redirecionado para um pipe) **traduz** `\r\n` para `\n`,
e o `hexdump` mostra só `0a`. Essa diferença é do **shell**, não
do `hexdump`. Ver [tests/README.md](tests/README.md) para os
gabaritos e a explicação.

## Nota sobre `-s` e stdin

O `-s N` usa `fseek`, que **só funciona em arquivos buscáveis**.
Em stdin (pipe), `fseek` falha, e o `hexdump` imprime um erro
("nao foi possivel pular N bytes") e retorna 1. **Isso é
esperado** (o `notes.md` registra).

## Testes

Ver [tests/README.md](tests/README.md). Dois fixtures
(`texto.txt` em CRLF, `binario.bin` com bytes não-imprimíveis),
com gabaritos medidos (sem flags, `-n`, `-s`, `-h`, flag
inválida, stdin).

## Notas de implementação

Ver [notes.md](notes.md). Cobre `fread` vs. `fgetc`, decimal vs.
hex no `is_printable`, a decisão de manter o `texto.txt` em CRLF,
o padrão `if (chamada() != 0)`, e `ferror`.

## Explicação detalhada

Ver [explained.md](explained.md). Explicação **linha por linha**
do `hexdump.c` e do `main.c`, para quem está começando em C.

## Conceitos relacionados

- [Especificadores de formato do `printf`](../../docs/format-specifiers.md)
  — `%02x`, `%08lx`, e os outros usados no `hexdump`.
- [Tabela ASCII](../../docs/ascii-table.md) — os bytes
  imprimíveis (32–126) e não-imprimíveis.
- [Operador ternário](../../docs/conditional-operator.md) — usado
  no `main.c` (`return (rc == 0) ? 0 : 1`).
- [O padrão `if (chamada() != 0)`](../../docs/error-check-pattern.md)
  — usado no `fseek` e no `fread`.
- [`ferror()`](../../docs/ferror.md) — para que serve, quando
  usar.

## Referências

- `hexdump` (Linux): <https://man7.org/linux/man-pages/man1/hexdump.1.html>
- `xxd` (Linux): <https://linux.die.net/man/1/xxd>
- Tabela ASCII: <https://www.asciitable.com/>

---

arataca89@gmail.com

Aulas de programação
