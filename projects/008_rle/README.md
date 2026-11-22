# 008_rle: compressão Run-Length Encoding

> Implementação didática do algoritmo de compressão mais simples
> que existe: RLE (Run-Length Encoding).

## O que faz

Comprime e descomprime arquivos usando **RLE**: substitui
sequências repetidas de um byte por um par `(byte, contagem)`.

```
Original:    A A A A A B B B C C
Comprimido:  (A,5) (B,3) (C,2)
```

**Modos**:

- **`-c`** (compress): comprime um arquivo.
- **`-d`** (decompress): descomprime um arquivo.
- **`-h`** (help): mostra a ajuda.

**Sem arquivo**: lê de stdin.

## Onde isso é usado na vida real

- **BMP**, **TIFF**, **PCX**: imagens simples (com fundo
  uniforme).
- **Fax**: documentos em preto e branco (muitas repetições).
- **Compressão de ícones**: fundos uniformes.
- **Primeiro estágio de compressores** (antes de LZ77/Huffman).

**Limitação**: RLE só comprime bem dados com **muitas
repetições**. Para dados aleatórios, o arquivo **aumenta** (cada
byte vira um par de 2 bytes).

## Exemplos de compressão

O RLE comprime **proporcionalmente à repetição**. Testamos com
três imagens BMP:

| Imagem | Original | Comprimido | Resultado |
|---|---|---|---|
| `square.bmp` (fundo uniforme) | 208.218 | 2.548 | **98,8% de redução** |
| `smile2.bmp` (fundo preto + smile amarelo) | 208.218 | 43.698 | **79% de redução** |
| `Smile.bmp` (smile em fundo variado) | 12.342 | 21.398 | **73% de aumento** |

**RLE funciona bem** em dados com **muitas repetições** (fundo
uniforme, fax, ícones). **Funciona mal** em dados com **variação**
(imagens reais, texto, etc.).

**Em projetos futuros**, voltaremos à compressão com algoritmos
mais avançados (LZ77, Huffman, DEFLATE), seguindo a filosofia
"do simples para o complicado".

## Conceitos de C abordados

- **Leitura/escrita binária** (`fgetc`/`fputc` em modo
  `"rb"`/`"wb"`).
- **`_setmode` no Windows**: desliga a tradução de EOL
  (`\n` ↔ `\r\n`) em `stdout`/`stdin`.
- **Algoritmo RLE**: contar sequências, emitir pares.
- **`ferror`**: distinguir EOF de erro.
- **Convenção `out, in`**: destino antes da origem (como
  `strcpy(dest, src)`).
- **Detecção de corrupção**: número ímpar de bytes, contagem
  zero.

## Como compilar

Requer CMake ≥ 3.16 e um compilador C11.

**Linux/macOS**:

    cmake -S . -B build
    cmake --build build

**Windows com TDM-GCC**:

    cmake -S . -B build -G "MinGW Makefiles"
    cmake --build build

## Como executar

**Da raiz do projeto** (`008_rle/`):

    build\rle.exe -c entrada.txt > saida.rle    (Windows)
    ./build/rle -c entrada.txt > saida.rle      (Linux/macOS)

**Comandos**:

    build\rle.exe -c arquivo.txt > arquivo.rle
    build\rle.exe -d arquivo.rle > arquivo.txt
    type arquivo.txt | build\rle.exe -c > arquivo.rle
    build\rle.exe -h

## Flags

| Flag | O que faz |
|---|---|
| `-c` | Comprime (padrão) |
| `-d` | Descomprime |
| `-h` | Mostra a ajuda e sai |

**Sem flags**: comprime (padrão).

## Formato do arquivo comprimido

O `rle_compress` gera uma sequência de **pares de bytes**:

```
[byte][contagem][byte][contagem]...
```

- **`byte`**: o valor repetido (1 byte, 0–255).
- **`contagem`**: quantas vezes repetir (1 byte, 1–255).

**Sem cabeçalho** (número mágico, versão, tamanho original). O
arquivo é só os pares. O `notes.md` registra a decisão.

**Limitação**: a contagem é 1 byte (máximo 255). Sequências >
255 são divididas em vários pares.

## Código de retorno

| Código | Significado |
|---|---|
| 0 | Sucesso |
| 1 | Erro (arquivo não encontrado, flag inválida, arquivo corrompido) |

## Nota sobre `_setmode` (Windows)

O `rle` escreve **bytes** (binário). No Windows, o `cmd` **traduz**
`\n` para `\r\n` em redirecionamentos (`>`), a menos que o
`stdout` esteja em **modo binário**.

O `setup_binary_stdio` no `main.c` desliga a tradução
(`_setmode(_fileno(stdout), _O_BINARY)`). Sem ele, o arquivo
comprimido fica **corrompido** (o `\n` do par vira `\r\n`, e o
número de bytes fica ímpar).

**No Linux/macOS**, não há tradução (o modo é sempre binário), e
o `_setmode` não existe (o `#ifdef _WIN32` isola).

Ver [notes.md](notes.md) para o histórico do bug.

## Nota sobre a convenção `out, in`

Os protótipos são:

```c
int rle_compress(FILE *out, FILE *in);
int rle_decompress(FILE *out, FILE *in);
```

**O destino vem antes da origem**, seguindo a convenção da
biblioteca C (`strcpy(dest, src)`, `memcpy(dest, src, n)`). Ver
[notes.md](notes.md) para a justificativa.

## Testes

Ver [tests/README.md](tests/README.md). Os testes são **comandos
de linha** (não há fixtures de arquivo tradicionais), com
gabaritos medidos. Inclui testes com imagens BMP (fundo uniforme,
caso intermediário, e pixels variados).

## Notas de implementação

Ver [notes.md](notes.md). Cobre o algoritmo RLE, o formato do
arquivo, a convenção `out, in`, o `_setmode`, a detecção de
corrupção, e os testes com imagens BMP.

## Explicação detalhada

Ver [explained.md](explained.md). Explicação **linha por linha**
do `rle.c` e do `main.c`.

## Conceitos relacionados

- [Compressão de dados](../../docs/compression.md) — o que é
  compressão, RLE, LZ77, Huffman, DEFLATE, e compressão com
  perda.
- [`ferror()`](../../docs/ferror.md) — para que serve, quando
  usar.
- [Especificadores de formato do `printf`](../../docs/format-specifiers.md)
  — usados no `main.c`.
- [Design de CLI](../../docs/cli-design.md) — flags, exit codes,
  stdin.

## Referências

- *Introduction to Data Compression*, Khalid Sayood.
- *The Data Compression Book*, Mark Nelson — introdução prática,
  com código em C.
- RLE na Wikipedia: <https://en.wikipedia.org/wiki/Run-length_encoding>

---

arataca89@gmail.com

Aulas de programação
