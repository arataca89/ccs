# Testes do rle

O `008_rle` não tem fixtures de arquivo tradicionais (como o
`001_wc` ou o `006_hexdump`). Os testes são **comandos de linha**,
e a saída esperada é o que o `rle` produz. Os gabaritos abaixo
foram **medidos** rodando `rle` (TDM-GCC 10.3 / Windows 11, em
[data]). Nenhum valor foi calculado à mão.

## Fixtures

| Arquivo | Descrição |
|---|---|
| `tests/teste.txt` | Texto curto, **19 bytes** (`A`×8, `B`×3, `C`×8), **sem** `\r\n` no final. Serve para testar a compressão. |
| `tests/square.bmp` | Imagem BMP com **fundo uniforme** (quadrado preto), 208.218 bytes. Serve para mostrar que o RLE comprime **muito bem** dados com repetição. |
| `tests/smile2.bmp` | Imagem BMP com **fundo preto + smile amarelo**, 208.218 bytes. Caso **intermediário**: tem repetição (fundo) e variação (smile). |
| `tests/Smile.bmp` | Imagem BMP com **pixels variados** (smiley), 12.342 bytes. Serve para mostrar que o RLE **não comprime** dados com variação. |

**Nota**: o `teste.txt` foi criado no Sublime Text, **sem**
newline no final. Se você criar com `echo` (Windows), ele terá
`\r\n` no final (21 bytes), e o teste será diferente. O
`notes.md` registra.

## Gabaritos

Para verificar o Hexdump você pode usar o programa criado no projeto `006_hexdump`.

### Compressão (texto)

```cmd
build\rle.exe -c tests\teste.txt > teste.rle
```

**Saída**: o arquivo `teste.rle`, com **6 bytes**.

**Hexdump do `teste.rle`**:

```
00000000  41 08 42 03 43 08                                |A.B.C.|
```

**Explicação**:

- `41 08` = `(A, 8)` (8 repetições de `A`).
- `42 03` = `(B, 3)` (3 repetições de `B`).
- `43 08` = `(C, 8)` (8 repetições de `C`).

**Compressão**: 19 bytes → 6 bytes (68% de redução).

### Descompressão (texto)

```cmd
build\rle.exe -d teste.rle > teste_volta.txt
```

**Saída**: o arquivo `teste_volta.txt`, com **19 bytes**
(idêntico ao original).

**Hexdump do `teste_volta.txt`**:

```
00000000  41 41 41 41 41 41 41 41  42 42 42 43 43 43 43 43 |AAAAAAAABBBCCCCC|
00000010  43 43 43                                         |CCC|
```

**Verificação**: `teste_volta.txt` é **idêntico** a `teste.txt`.
Use `fc teste.txt teste_volta.txt` (Windows) ou `diff` (Linux)
para confirmar.

### Teste com imagem BMP (fundo uniforme)

O RLE comprime **muito bem** imagens com fundo uniforme. Testamos
com um BMP de um quadrado preto (`square.bmp`, 208.218 bytes):

```cmd
build\rle.exe -c tests\square.bmp > square.rle
```

**Tamanhos**:

| Arquivo | Tamanho |
|---|---|
| `square.bmp` (original) | 208.218 bytes |
| `square.rle` (comprimido) | 2.548 bytes |

**Resultado**: **98,8% de redução** (208.218 → 2.548). O RLE
encontrou milhares de pixels pretos consecutivos, e comprimiu
cada sequência em um par `(byte, 255)`.

**Descompressão** (verifica que o arquivo volta idêntico):

```cmd
build\rle.exe -d square.rle > square_volta.bmp
fc /b tests\square.bmp square_volta.bmp
```

**Saída esperada**: `FC: nenhuma diferença encontrada`.

### Caso intermediário: fundo preto + smile amarelo

Testamos também com um BMP de **fundo preto** com um **smile
amarelo** dentro (`smile2.bmp`, 208.218 bytes):

```cmd
build\rle.exe -c tests\smile2.bmp > smile2.rle
```

**Tamanhos**:

| Arquivo | Tamanho |
|---|---|
| `smile2.bmp` (original) | 208.218 bytes |
| `smile2.rle` (comprimido) | 43.698 bytes |

**Resultado**: **79% de redução** (208.218 → 43.698).
Como este arquivo tem pixels variados a compressão foi menor que no arquivo anterior.

### Contraste: imagem com pixels variados

Testamos também com um smiley (`Smile.bmp`, 12.342 bytes), que
tem muitos pixels variados, muito mais detalhes, degrade, etc.:

```cmd
build\rle.exe -c tests\Smile.bmp > smile.rle
```

**Tamanhos**:

| Arquivo | Tamanho |
|---|---|
| `Smile.bmp` (original) | 12.342 bytes |
| `smile.rle` (comprimido) | 21.398 bytes |

**Resultado**: **73% de aumento** (12.342 → 21.398). Os pixels
variam, e não há sequências longas do mesmo byte. Cada byte vira
um par `(byte, 1)` (2 bytes), e o arquivo **dobra** de tamanho.

### Lição: compressão depende da proporção de repetição

| Imagem | Original | Comprimido | Resultado | Repetição |
|---|---|---|---|---|
| `square.bmp` (fundo uniforme) | 208.218 | 2.548 | **98,8% de redução** | ~100% |
| `smile2.bmp` (fundo preto + smile amarelo) | 208.218 | 43.698 | **79% de redução** | ~80% |
| `Smile.bmp` (smile em fundo variado) | 12.342 | 21.398 | **73% de aumento** | ~0% |

**RLE comprime proporcionalmente à repetição**: quanto mais
repetição, melhor. O `square.bmp` (quase tudo preto) comprime
98,8%; o `smile2.bmp` (fundo preto + smile) comprime 79%; o
`Smile.bmp` (pixels variados) **aumenta** 73%.

**Isso não é falha do RLE** — é uma **característica** do
algoritmo. Para imagens reais (com muita variação), PNG/JPEG
usam algoritmos melhores (DEFLATE, DCT).

**A descompressão é exata** nos três casos (o arquivo volta
idêntico). O RLE não perde informação.

**Em projetos futuros**, voltaremos à compressão com algoritmos
mais avançados (LZ77, Huffman, DEFLATE), seguindo a filosofia
"do simples para o complicado".

### Arquivo corrompido (número ímpar de bytes)

**Windows (PowerShell)**:

```powershell
[byte[]]$bytes = 0x41, 0x42, 0x43, 0x44, 0x45
[System.IO.File]::WriteAllBytes("$PWD\tests\corrompido.rle", $bytes)
```

**Linux/macOS (bash)**:

```bash
printf '\x41\x42\x43\x44\x45' > tests/corrompido.rle
```

**Teste**:

```cmd
build\rle.exe -d corrompido.rle
```

**Saída** (stderr):

```
rle: arquivo corrompido (falta a contagem)
```

**Exit code**: 1 (erro).

**Explicação**: o arquivo tem 5 bytes (um byte sem a contagem
correspondente). O `rle_decompress` detecta.

### Arquivo corrompido (contagem zero)

**Windows (PowerShell)**:

```powershell
[byte[]]$bytes = 0x41, 0x00
[System.IO.File]::WriteAllBytes("$PWD\tests\corrompido2.rle", $bytes)
```

**Linux/macOS (bash)**:

```bash
printf '\x41\x00' > tests/corrompido2.rle
```

**Verificação**:

```bash
hexdump -C tests/corrompido2.rle
# 00000000  41 00                                             |A.|
```

**Teste**:

```cmd
build\rle.exe -d corrompido2.rle
```

**Saída** (stderr):

```
rle: arquivo corrompido (contagem zero)
```

**Exit code**: 1 (erro).

**Explicação**: o `rle_compress` nunca gera contagem 0. Se o
arquivo tem `count = 0`, é corrupção.

### `-h` (help)

```cmd
build\rle.exe -h
```

**Saída**: o texto do help (ver `print_help` no `main.c`).
Mostra as opções, os exemplos, e a nota sobre stdin.

### Flag inválida

```cmd
build\rle.exe -x
```

**Saída** (stderr):

```
build\rle.exe: opcao invalida -- 'x'
Tente 'build\rle.exe -h' para ajuda.
```

**Exit code**: 1 (erro).

### Stdin

```cmd
type tests\teste.txt | build\rle.exe -c > teste_stdin.rle
```

**Saída**: o arquivo `teste_stdin.rle`, com **6 bytes**
(idêntico ao `teste.rle`).

**Explicação**: o `rle` lê de stdin quando não há arquivo. O
`_setmode` garante que a leitura é binária (sem tradução de
EOL).

## Nota sobre `_setmode` e o `cmd`

O `rle` escreve **bytes** (binário), não texto. No Windows, o
`cmd` **traduz** `\n` para `\r\n` em redirecionamentos (`>`), a
menos que o `stdout` esteja em **modo binário**.

O `setup_binary_stdio` no `main.c` desliga a tradução
(`_setmode(_fileno(stdout), _O_BINARY)`). Sem ele, o arquivo
comprimido fica **corrompido** (o `\n` do par vira `\r\n`, e o
número de bytes fica ímpar).

**Isso é o mesmo problema do `001_wc`, `002_cat`, `003_grep`**.
O `notes.md` registra.

## Nota sobre a contagem de bytes

O `teste.txt` tem **19 bytes** (`A`×8, `B`×3, `C`×8), **sem**
`\r\n` no final. Se você criar com `echo` (Windows), terá
`\r\n` (21 bytes), e o `teste.rle` terá um par a mais
(`(0d, 1)(0a, 1)`).

**Isso é esperado**: o RLE lida com qualquer byte, incluindo
`\r` e `\n`. O `notes.md` registra.

## Como rodar

**Da raiz do projeto** (`008_rle/`):

    build\rle.exe -c tests\teste.txt > teste.rle   (Windows)
    ./build/rle -c tests/teste.txt > teste.rle     (Linux/macOS)

---

arataca89@gmail.com

Aulas de programação
