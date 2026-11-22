# Compressão de dados

> Comprimir é representar a mesma informação usando **menos
> bytes**. É uma das ideias mais importantes da computação, e está
> em todo lugar: ZIP, PNG, JPEG, MP3, MP4, HTTP, etc.

## O que é compressão

**Compressão** é o processo de transformar dados em uma
representação **menor**, que pode ser **descomprimida** depois
para recuperar os dados originais (ou uma aproximação deles).

**Exemplo simples**:

- **Original**: `AAAAABBBCC` (10 bytes)
- **Comprimido**: `(A,5)(B,3)(C,2)` (6 bytes, se cada par for 2 bytes)
- **Descomprimido**: `AAAAABBBCC` (10 bytes, igual ao original)

**Por que comprimir**:

1. **Armazenamento**: um arquivo menor ocupa menos espaço em disco.
2. **Transmissão**: um arquivo menor demora menos para ser
   enviado pela rede.
3. **Custo**: menos bytes = menos custo de armazenamento e
   banda.

## Compressão sem perda vs. com perda

### Sem perda (lossless)

Os dados originais são **recuperados exatamente**. Nada é perdido.

**Exemplos**:
- **ZIP**, **GZIP**, **7z**: arquivos e pastas.
- **PNG**: imagens (sem perda).
- **FLAC**: áudio (sem perda).
- **RLE**: o algoritmo do `008_rle`.

**Quando usar**: quando os dados **precisam** ser exatos (texto,
código, executáveis, imagens médicas, etc.).

### Com perda (lossy)

Os dados originais **não** são recuperados exatamente. Uma
**aproximação** é aceitável.

**Exemplos**:
- **JPEG**: imagens (perde detalhes que o olho humano não nota).
- **MP3**: áudio (perde frequências que o ouvido humano não
  percebe).
- **MP4**: vídeo (perde detalhes entre frames).

**Quando usar**: quando uma **aproximação** é aceitável, e a
compressão precisa ser **muito** maior (10x, 100x).

**No CCS**, só vamos fazer **compressão sem perda** (RLE). A
compressão com perda é **muito** mais complexa (envolve
psicoacústica, psicovisual, etc.), e fica para o futuro.

## Como a compressão funciona

A ideia central é **explorar redundância**. Dados reais têm
**padrões** que podem ser representados de forma mais eficiente.

**Tipos de redundância**:

1. **Repetição**: `AAAAA` tem 5 repetições de `A`. RLE explora
   isso.
2. **Frequência**: em texto, `e` aparece mais que `z`. Huffman
   explora isso (dá códigos menores para símbolos mais
   frequentes).
3. **Padrões repetidos**: `abcabcabc` tem o padrão `abc`
   repetido. LZ77 explora isso (referencia ocorrências
   anteriores).
4. **Correlação**: em imagens, pixels vizinhos são parecidos.
   JPEG explora isso.

**Cada algoritmo explora um tipo de redundância**. Os mais
eficientes combinam vários (DEFLATE = LZ77 + Huffman).

## RLE (Run-Length Encoding)

**O que é**: substitui **sequências repetidas** de um byte por
`(byte, contagem)`.

**Exemplo**:

```
Original:    A A A A A B B B C C
Comprimido:  (A,5) (B,3) (C,2)
```

**Funciona bem para**: dados com **muitas repetições** (imagens
com fundo uniforme, ícones, etc.).

**Funciona mal para**: dados **aleatórios** (sem repetição). Se
não há repetição, o RLE **aumenta** o tamanho (cada byte vira um
par de 2 bytes).

**Onde é usado**:
- **BMP** (alguns modos), **TIFF**, **PCX**: imagens simples.
- **Fax**: documentos em preto e branco (muitas repetições).
- **Compressão de ícones**: fundos uniformes.

**Vantagens**:
- **Simples** de implementar (é o algoritmo de compressão mais
  simples que existe).
- **Rápido** (compressão e descompressão são O(n)).
- **Sem perda** (recupera exatamente).

**Desvantagens**:
- **Comprime pouco** (só sequências longas).
- **Pode aumentar** o tamanho (se não há repetição).

**No `008_rle`**: vamos implementar RLE com pares `(byte,
contagem)`, contagem em 1 byte (0–255). Sequências > 255 são
divididas em vários pares.

## LZ77 (Lempel-Ziv 77)

**O que é**: substitui **padrões repetidos** por **referências**
a ocorrências anteriores.

**Exemplo**:

```
Original:    abcabcabc
Comprimido:  abc(3,3)   ; "repita 3 bytes a partir de 3 posições atrás"
```

**Como funciona**: mantém uma **janela deslizante** dos últimos N
bytes. Quando encontra um padrão que já apareceu, emite
`(distância, comprimento)` em vez do padrão.

**Onde é usado**:
- **DEFLATE** (ZIP, GZIP, PNG): LZ77 + Huffman.
- **LZMA** (7z, XZ): LZ77 + modelo de contexto.
- **LZ4** (rápido, para jogos e bancos de dados).

**Vantagens**: comprime **muito** bem (melhor que RLE).
**Desvantagens**: mais complexo que RLE.

## Huffman

**O que é**: dá **códigos menores** para símbolos **mais
frequentes**, e códigos maiores para os menos frequentes.

**Exemplo** (texto em inglês):

- `e` aparece 12% das vezes → código de 3 bits.
- `z` aparece 0,1% das vezes → código de 10 bits.

**Como funciona**: constrói uma **árvore binária** (árvore de
Huffman) com os símbolos, onde os mais frequentes ficam mais
perto da raiz (códigos menores).

**Onde é usado**:
- **DEFLATE** (ZIP, GZIP, PNG): Huffman + LZ77.
- **JPEG**: Huffman (para os coeficientes DCT).
- **MP3**: Huffman (para os coeficientes MDCT).

**Vantagens**: ótimo para dados com **frequências desiguais**.
**Desvantagens**: precisa de **duas passadas** (contar
frequências, depois codificar), ou de uma tabela de frequências
no arquivo.

## DEFLATE (LZ77 + Huffman)

**O que é**: a combinação de **LZ77** (para padrões repetidos) e
**Huffman** (para frequências desiguais). É o algoritmo do ZIP,
GZIP, PNG, etc.

**Como funciona** (alto nível):

1. **LZ77**: substitui padrões repetidos por referências.
2. **Huffman**: codifica os símbolos (literais e referências) com
   códigos de tamanho variável.

**Onde é usado**:
- **ZIP**, **GZIP**: arquivos.
- **PNG**: imagens.
- **HTTP**: `Content-Encoding: gzip`.
- **PDF**: streams comprimidos.

**Vantagens**: comprime **muito** bem (10x–100x em texto).
**Desvantagens**: mais complexo que RLE (e mais lento).

**No CCS**: DEFLATE fica para o futuro. É **muito** mais complexo que RLE.

## Compressão com perda (JPEG, MP3, MP4)

### JPEG (imagens)

**Como funciona** (alto nível):

1. Converte a imagem de RGB para YCbCr (luminância + crominância).
2. **Subamostra** a crominância (o olho humano é menos sensível a
   cor que a luminância).
3. Divide a imagem em blocos 8x8, e aplica **DCT** (Discrete
   Cosine Transform).
4. **Quantiza** os coeficientes (perde detalhes de alta
   frequência, que o olho não nota).
5. **Comprime** os coeficientes com Huffman.

**Resultado**: imagens 10x–100x menores, com perda **quase**
imperceptível.

### MP3 (áudio)

**Como funciona** (alto nível):

1. Divide o áudio em frames.
2. Aplica **MDCT** (Modified Discrete Cosine Transform).
3. **Modelo psicoacústico**: descarta frequências que o ouvido
   humano **não** percebe (mascaramento).
4. **Quantiza** e **comprime** com Huffman.

**Resultado**: áudio 10x menor, com perda **quase**
imperceptível.

### MP4 (vídeo)

**Como funciona** (alto nível):

1. Divide o vídeo em **frames** (I, P, B).
2. **I-frames**: frames completos (como JPEG).
3. **P-frames**: frames que referenciam o anterior (só as
   diferenças).
4. **B-frames**: frames que referenciam o anterior e o próximo.
5. **Comprime** com Huffman.

**Resultado**: vídeo 100x–1000x menor, com perda **quase**
imperceptível.

**No CCS**: compressão com perda fica para o futuro distante
(envolve matemática avançada, psicoacústica, etc.).

## Onde a compressão aparece no CCS

- **`008_rle`**: RLE (compressão sem perda, simples).
- **`deflate`** (futuro): DEFLATE (LZ77 + Huffman).
- **`zip`** (futuro): formato ZIP (usa DEFLATE).

## Comparação dos algoritmos

| Algoritmo | Compressão | Velocidade | Complexidade | Onde é usado |
|---|---|---|---|---|
| **RLE** | Baixa (2x–5x) | Muito rápida | Muito baixa | BMP, TIFF, PCX, fax |
| **LZ77** | Média (3x–10x) | Rápida | Média | DEFLATE, LZMA, LZ4 |
| **Huffman** | Média (2x–8x) | Rápida | Média | DEFLATE, JPEG, MP3 |
| **DEFLATE** | Alta (10x–100x) | Média | Alta | ZIP, GZIP, PNG |
| **LZMA** | Muito alta (10x–200x) | Lenta | Muito alta | 7z, XZ |
| **JPEG** | Alta (10x–100x) | Média | Alta (com perda) | Imagens |
| **MP3** | Alta (10x) | Média | Alta (com perda) | Áudio |
| **MP4** | Muito alta (100x–1000x) | Média | Muito alta (com perda) | Vídeo |

## Como escolher um algoritmo

- **Dados com muitas repetições** (imagens simples, fax): **RLE**.
- **Dados gerais** (texto, código, arquivos): **DEFLATE** (ZIP,
  GZIP).
- **Máxima compressão** (backup, distribuição): **LZMA** (7z,
  XZ).
- **Velocidade** (jogos, bancos de dados): **LZ4**.
- **Imagens** (fotografias): **JPEG** (com perda) ou **PNG** (sem
  perda).
- **Áudio**: **MP3**, **AAC**, **FLAC** (sem perda).
- **Vídeo**: **MP4** (H.264, H.265), **WebM**.

## Resumo

| Conceito | O que é |
|---|---|
| **Compressão** | Representar a mesma informação com menos bytes |
| **Sem perda** | Recupera exatamente (ZIP, PNG, RLE) |
| **Com perda** | Recupera uma aproximação (JPEG, MP3, MP4) |
| **Redundância** | Padrões que podem ser explorados (repetição, frequência, correlação) |
| **RLE** | Substitui sequências repetidas por `(byte, contagem)` |
| **LZ77** | Substitui padrões repetidos por referências |
| **Huffman** | Códigos menores para símbolos mais frequentes |
| **DEFLATE** | LZ77 + Huffman (ZIP, GZIP, PNG) |

**Regra prática**: para dados com **muitas repetições**, RLE. Para
dados **gerais**, DEFLATE. Para **máxima compressão**, LZMA. Para
**velocidade**, LZ4.

## Leitura recomendada

- *Introduction to Data Compression*, Khalid Sayood — o
  livro-texto clássico.
- *Data Compression: The Complete Reference*, David Salomon — a
  referência completa.
- *The Data Compression Book*, Mark Nelson — introdução prática,
  com código em C.
- `zlib` (biblioteca): <https://zlib.net/> — a implementação de
  DEFLATE mais usada.

---

arataca89@gmail.com

Aulas de programação
