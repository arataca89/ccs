# Notas de implementação — 001_wc

Registro do raciocínio por trás do código. Não é tutorial: é o
"porquê" das decisões, incluindo os tropeços.

## Por que `int c` e não `char c`?

`fgetc()` retorna `int`, não `char`. Motivo: precisa poder devolver
todos os 256 valores possíveis de um byte (0–255) **mais** um valor
especial, `EOF`, que é negativo (tipicamente -1). Se você declarar
`char c`, em plataformas onde `char` é unsigned (comum em ARM), o
teste `c != EOF` pode falhar. Esse é o bug clássico de iniciante
em C — e ele é silencioso.

## Por que contamos palavras com máquina de estados?

Não dá para contar palavras contando espaços e somando 1. `"  a  b  "`
tem 4 espaços mas 2 palavras; `""` tem 0 espaços e 0 palavras. A
contagem correta é: cada transição de "fora de palavra" para
"dentro de palavra" incrementa o contador. Isso é um autômato
finito de dois estados — o mesmo modelo que voltará nos parsers
mais adiante no CCS.

## O problema do `\r\n` no Windows

No Windows, arquivos de texto usam `\r\n` como separador de linha.
Se abrirmos com `fopen(path, "r")`, a biblioteca C faz tradução
automática: `\r\n` vira `\n` na leitura. Isso parece bom, mas
quebra a contagem de **bytes** — que é o que o wc mede.

Solução: abrir sempre em modo binário (`"rb"`) e tratar `\r` e `\n`
como caracteres normais. Assim a contagem de bytes é fiel em todas
as plataformas.

## Por que `_setmode` no stdin?

Mesmo problema, mas na entrada padrão. Se o usuário faz
`type arquivo.txt | wc.exe` no Windows, o stdin está em modo texto
por padrão e a tradução `\r\n -> \n` acontece. A chamada
`_setmode(_fileno(stdin), _O_BINARY)` desliga isso. No Linux/macOS
essa chamada não existe — daí o `#ifdef _WIN32`.

## Por que CMake e não Makefile?

Makefile puro exige ferramentas POSIX (make, sh) que não vêm por
padrão no Windows. CMake gera build para MSVC, MinGW, Ninja, Make —
tudo a partir do mesmo arquivo. Como o CCS quer ser portátil, CMake
é a escolha natural.

## Gabaritos medidos, não calculados

Ao preparar os fixtures de teste, tentei prever a contagem de linhas
e palavras "de cabeça" e errei feio (chutei 23 linhas / ~152 palavras
para o `sample-long.txt`; o real foi 27 / 194). Lição: **contagem
de linhas e palavras em C não se adivinha — se mede**. Linhas em
branco contam, quebras de parágrafo contam, e o `wc` não distingue
"linha visual" de "linha lógica": conta `\n`.

O caso do `teste1.txt` é ainda mais ilustrativo. A descrição em prosa
do arquivo sugeria 4 linhas e 11 palavras; o arquivo real tem 3
linhas e 9 palavras (as "linhas 2 e 4" da descrição eram linhas em
branco; a linha 3 termina com `\n`). **Descrição em prosa não é
gabarito.** Gabarito é medição.

Por isso, todo gabarito neste repositório é medido com o próprio
`wc` e registrado no `tests/README.md`. Nunca calculado à mão.

## Portabilidade não é só "compila em todo lugar"

É também "comporta-se de forma coerente em todo lugar". O `wc` é um
caso interessante: no Windows, o nome `wc` está livre e pode ser
instalado no `PATH`. No Linux/macOS, `wc` já existe e é usado por
scripts do sistema — instalar o nosso `wc` com o mesmo nome causaria
conflito. A solução é usar nome alternativo (`wc-ccs`) ou path
completo. Isso não é um defeito do nosso código: é uma realidade
do ecossistema POSIX que todo programa C precisa encarar.

## Paths relativos dependem do diretório atual

`..\tests\teste1.txt` só funciona se você estiver em `build\`.
`build\wc.exe tests\teste1.txt` só funciona se você estiver na raiz
do projeto. Não existe "path certo" em absoluto — existe path certo
**para o diretório de onde você roda**. Por isso o README do projeto
sempre diz **de onde** rodar cada comando.

