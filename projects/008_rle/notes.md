# Notas de implementação — 008_rle

Registro do raciocínio por trás do código. Não é tutorial: é o
"porquê" das decisões, incluindo os tropeços.

## Por que RLE?

RLE (Run-Length Encoding) é o algoritmo de compressão mais simples
que existe. Substitui sequências repetidas de um byte por um par
`(byte, contagem)`.

**Por que escolhemos RLE**:

1. **Simples**: é o algoritmo de compressão mais fácil de
   implementar. Não precisa de árvore (Huffman), janela deslizante
   (LZ77), ou matemática avançada (DCT).
2. **Didático**: ensina o conceito de compressão (explorar
   redundância) sem a complexidade dos algoritmos reais.
3. **Binário**: lê e escreve bytes (diferente dos projetos
   anteriores, que eram de texto). Ensina `fgetc`/`fputc` em modo
   binário.
4. **Categoria vazia**: cobre a categoria "Compressão" (ou
   "Ferramentas de sistema"), que estava vazia.

## O formato do arquivo comprimido

O `rle_compress` gera uma sequência de **pares de bytes**:

```
[byte][contagem][byte][contagem]...
```

- **`byte`**: o valor repetido (1 byte, 0–255).
- **`contagem`**: quantas vezes repetir (1 byte, 1–255).

**Exemplo**: `AAAAABBBCC` comprime para `[A][5][B][3][C][2]`.

**Decisões**:

- **Sem cabeçalho**: o arquivo comprimido é só os pares. Não há
  "número mágico" nem versão. Um cabeçalho (com "RLE1" e o
  tamanho original) seria mais robusto, mas fica para depois.
- **Contagem em 1 byte**: máximo 255. Sequências > 255 são
  divididas em vários pares. A contagem em 2 bytes (até 65535)
  seria mais eficiente, mas mais complexa.
- **Sem compressão de sequências de 1**: se um byte aparece uma
  vez, o par tem 2 bytes (o byte + a contagem). O arquivo
  **aumenta** (100% de aumento) para dados sem repetição. Isso é
  esperado no RLE.

## Por que `out` antes de `in`

Os protótipos são:

```c
int rle_compress(FILE *out, FILE *in);
int rle_decompress(FILE *out, FILE *in);
```

**O destino vem antes da origem**, seguindo a convenção da
biblioteca C:

- `strcpy(dest, src)` — copia `src` para `dest`.
- `memcpy(dest, src, n)` — copia `src` para `dest`.
- `strcat(dest, src)` — concatena `src` em `dest`.
- `sprintf(dest, format, ...)` — escreve em `dest`.

A lógica é que a função **modifica o primeiro parâmetro** (o
destino), e lê o segundo (a origem). A analogia com atribuição
(`A = B`) ajuda a lembrar: o destino vem primeiro.

**Isso é uma decisão consciente**: como `FILE *` não tem "direção"
intrínseca (é só um stream), a ordem `out, in` fica um pouco
estranha de ler (`rle_compress(out, in)` parece que estamos
comprimindo `out`). Mas **consistência com a biblioteca** é mais
importante que a leitura literal.

## Por que `_setmode` no `main.c`

O `rle` escreve **bytes** (binário), não texto. No Windows, o
`stdout` é **text mode** por padrão, e o `cmd` **traduz** `\n`
para `\r\n` na escrita (e `\r\n` para `\n` na leitura).

Isso **corrompe** o arquivo comprimido (que é binário). O
`_setmode(_fileno(stdout), _O_BINARY)` desliga a tradução.

### O sintoma

Com um arquivo original de 21 bytes (`A`×8, `B`×3, `C`×8, `\r`,
`\n`), o `rle_compress` gerou 10 bytes (`41 08 42 03 43 08 0d
01 0a 01`). Mas o `teste.rle` (via `> teste.rle`) ficou com **11
bytes**:

```
00000000  41 08 42 03 43 08 0d 01  0d 0a 01                |A.B.C......|
```

O último par (`0a 01` do `\n`) virou `0d 0a 01` — o `cmd`
traduziu o `0a` para `0d 0a`. O `rle_decompress` detectou
corrupção (número ímpar de bytes).

### O diagnóstico

O `cmd` do Windows traduz `\n` para `\r\n` em redirecionamentos
(`>`), a menos que o `stdout` esteja em **modo binário**. Isso
acontece **independentemente da origem do arquivo** (Sublime,
`echo`, `type`, etc.). O problema é a **escrita** do arquivo
comprimido, não a leitura do original.

### A solução

O `setup_binary_stdio` no `main.c`:

```c
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

static void setup_binary_stdio(void)
{
#ifdef _WIN32
    _setmode(_fileno(stdin),  _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}
```

**No Linux/macOS**, não há tradução (o modo é sempre binário), e
o `_setmode` não existe (o `#ifdef _WIN32` isola).

**Isso é o mesmo problema do `001_wc`, `002_cat`, `003_grep`**.
O `006_hexdump` **não** faz (porque só lê). O `007_string_utils`
**não** faz (porque só escreve texto). O `008_rle` **precisa**
fazer (porque escreve binário).

## Por que `count` é `unsigned` e não `int`

No `rle_compress`:

```c
unsigned count = 1;
```

**`count`**: quantas vezes o `previous` apareceu até agora.
Começa em 1, e vai até 255 (o máximo de um byte).

**`unsigned`**: a contagem é não-negativa. `unsigned` deixa claro.

**`fputc((int)count, out)`**: o `fputc` espera `int`, não
`unsigned`. O cast `(int)` evita warning.

## O caso do `count = 0` no `rle_compress`

Quando a sequência atinge 255, o `rle_compress` emite o par e
**zera** `count`:

```c
if (count == MAX_COUNT) {
    if (fputc(previous, out) == EOF) return -1;
    if (fputc((int)count, out) == EOF) return -1;
    count = 0;
}
```

**Por quê**: o `current` que disparou o `count++` **já foi
lido**, mas **ainda não foi emitido**. Se zerarmos `count`, o
próximo `current` (se for igual a `previous`) fará `count++` para
1, e a sequência recomeça.

**Exemplo**: 260 repetições de `A`.

- `count` chega a 255 → emite `(A, 255)`, `count = 0`.
- Próximo `current` (ainda `A`): `current == previous` → `count++`
  → 1.
- Mais 4 repetições: `count` chega a 5.
- Fim do arquivo: emite `(A, 5)`.

**Total**: `(A, 255)(A, 5)` = 260 repetições.

## O caso do arquivo vazio no `rle_compress`

```c
previous = fgetc(in);
if (previous == EOF) {
    if (ferror(in))
        return -1;
    return 0;
}
```

**Se o arquivo está vazio**: `fgetc` retorna `EOF`. O `ferror`
distingue EOF (vazio) de erro. Se for vazio (sem erro), retorna 0
(sucesso, nada a comprimir).

**Por que ler o primeiro byte antes do loop**: o loop precisa de
um `previous` para comparar com o `current`. Sem o primeiro byte,
o `previous` seria indefinido.

## O caso do número ímpar de bytes no `rle_decompress`

```c
while ((byte = fgetc(in)) != EOF) {
    count = fgetc(in);
    if (count == EOF) {
        fprintf(stderr, "rle: arquivo corrompido (falta a contagem)\n");
        return -1;
    }
    ...
}
```

**O loop sempre tenta ler 2 bytes por iteração** (um para
`byte`, outro para `count`). Se o arquivo tem um número **par**
de bytes (como todo arquivo gerado pelo `rle_compress`), o loop
lê pares até o `EOF`. Se tem um número **ímpar**, a última
iteração lê o `byte`, mas o `count` é `EOF` (o arquivo acabou).

**Não é que o loop leia 1 byte e pare**: ele **sempre** tenta
ler 2, e se o segundo não existe, detecta a corrupção.

## O caso do `count = 0` no `rle_decompress`

```c
if (count == 0) {
    fprintf(stderr, "rle: arquivo corrompido (contagem zero)\n");
    return -1;
}
```

**O `rle_compress` nunca gera contagem 0**. Se o arquivo tem
`count = 0`, é corrupção (ou um arquivo gerado por outro
programa).

## Teste com imagens BMP

Testamos o RLE com **três** imagens BMP, cada uma com uma
proporção diferente de repetição:

### `square.bmp` (fundo uniforme)

- **Original**: 208.218 bytes.
- **Comprimido** (`square.rle`): 2.548 bytes.
- **Resultado**: **98,8% de redução**.
- **Por quê**: o quadrado preto tem milhares de pixels pretos
  consecutivos. O RLE comprimiu cada sequência em um par
  `(byte, 255)`.

### `smile2.bmp` (fundo preto + smile amarelo)

- **Original**: 208.218 bytes.
- **Comprimido** (`smile2.rle`): 43.698 bytes.
- **Resultado**: **79% de redução**.
- **Por quê**: o fundo preto (uniforme) comprimiu muito bem, mas
  o smile amarelo (pixels variados) não comprimiu tão bem. É o caso
  **realista**: tem repetição (fundo) e variação (smile).

### `Smile.bmp` (pixels variados)

- **Original**: 12.342 bytes.
- **Comprimido** (`smile.rle`): 21.398 bytes.
- **Resultado**: **73% de aumento**.
- **Por quê**: os pixels variam (muitos detalhes, sorriso, olhos, degradê, etc.), e não há
  sequências longas do mesmo byte. Cada byte vira um par
  `(byte, 1)` (2 bytes), e o arquivo **dobra** de tamanho.

### Lição

**Compressão depende da proporção de repetição**, não do tipo de
arquivo:

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

## Por que não criar o arquivo de saída automaticamente

O `rle` **não** cria `teste.rle` automaticamente. Ele escreve no
`stdout`, e o usuário redireciona (`> teste.rle`). Isso segue a
**convenção Unix**:

- `rle -c teste.txt > teste.rle` — comprime para `teste.rle`.
- `rle -c teste.txt | gzip > teste.rle.gz` — comprime e
  compacta.
- `rle -c teste.txt` — mostra o binário no terminal (raro, mas
  possível).

**Vantagens**:

1. **Composição**: permite `rle | gzip`, `rle | base64`, etc.
2. **Flexibilidade**: o usuário escolhe o nome do arquivo.
3. **Consistência**: os outros projetos do CCS (`001_wc`,
   `002_cat`, `003_grep`, `006_hexdump`) escrevem no `stdout`.
4. **Simplicidade**: o `rle` faz uma coisa (comprimir), não
   "comprimir e criar arquivo".

**Desvantagem**: o usuário precisa redirecionar (mais verboso).

**Ideia futura**: adicionar uma flag `-o saida.rle` (output),
que abre o arquivo com `fopen("saida.rle", "wb")` (modo
binário). Isso mantém a convenção (stdout por padrão), mas
oferece a conveniência. Fica para um upgrade.

## Lições gerais deste projeto

1. **RLE** é o algoritmo de compressão mais simples. Substitui
   sequências repetidas por `(byte, contagem)`. Funciona bem para
   dados com muitas repetições, e mal para dados aleatórios.
2. **`fgetc`/`fputc`** em modo binário (`"rb"`/`"wb"`) para
   leitura/escrita de bytes.
3. **`_setmode`** no Windows desliga a tradução de EOL. Sem ele,
   o `cmd` traduz `\n` para `\r\n` na escrita, e corrompe
   arquivos binários.
4. **Convenção `out, in`**: o destino vem antes da origem,
   seguindo a biblioteca C (`strcpy(dest, src)`).
5. **`ferror`** distingue EOF de erro. Verificar depois do loop.
6. **Arquivo corrompido**: número ímpar de bytes (falta a
   contagem), ou contagem 0 (inválida).
7. **Convenção Unix**: o programa escreve no `stdout`, e o
   usuário redireciona. Isso permite composição (`rle | gzip`).
8. **Não criar arquivo de saída automaticamente**: foge da
   convenção, e mistura responsabilidades. A flag `-o` (futura)
   é uma alternativa.
9. **Compressão depende do conteúdo**: RLE comprime
   proporcionalmente à repetição. Imagens com fundo uniforme
   comprimem muito; imagens com variação, pouco (ou aumentam).

---

arataca89@gmail.com

Aulas de programação
