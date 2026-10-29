# Notas de implementação — 002 cat

Registro do raciocínio por trás do código. Não é tutorial: é o
"porquê" das decisões, incluindo os tropeços. Bugs são documentados
com sintoma, diagnóstico, soluções consideradas e escolha final.

## Por que `int c` e não `char c`?

Mesmo motivo do `001_wc`: `fgetc()` retorna `int`, não `char`,
porque precisa representar todos os 256 valores de byte **mais** o
`EOF` (negativo). Se você declarar `char c`, em plataformas onde
`char` é unsigned, o teste `c != EOF` pode falhar silenciosamente.

## Por que `cat` usa `fgetc` e não `fread` com buffer?

`fread` com buffer de 64 KB seria ~10x mais rápido. Mas exige
gerenciar `size_t nread`, tratar leitura parcial, e o código fica
mais complexo. Para o propósito do CCS (ensinar), `fgetc` é mais
claro. O `wc` já usou a mesma abordagem. Se performance importasse,
o projeto seria sobre performance, não sobre C.

## Por que duas passadas sobre `argv`?

O `cat` real aceita flags **depois** de arquivos:

```
cat arquivo.txt -n
```

Para que `-n` tenha efeito sobre `arquivo.txt` mesmo vindo depois,
precisamos **coletar todas as flags antes** de processar qualquer
arquivo. Daí duas passadas:

1. `parse_flags`: percorre `argv`, liga flags, ignora arquivos.
2. `main`: percorre `argv` de novo, processa arquivos na ordem.

Isso evita `malloc` (não precisamos de array auxiliar) e preserva
a ordem dos arquivos naturalmente. `argv` é um array comum — pode
ser percorrido quantas vezes quisermos.

## Por que parsing manual e não `getopt`?

`getopt` não é C padrão. No Linux/macOS vem de `<getopt.h>` (POSIX);
no Windows, MinGW tem, mas MSVC **não**. Usar `getopt` quebraria
portabilidade. Além disso, o parsing manual é didático: ensina o
aluno a ler `argv` caractere por caractere, coisa que ele vai fazer
em todo projeto CLI.

O parsing manual também permite **flags agrupadas** (`-nE`), que é
convenção Unix. O `switch` percorre cada caractere depois do `-`.

## Por que `arg[0] != '-' || arg[1] == '\0'` para "não é flag"?

Cobre dois casos:

- `arg[0] != '-'`: não começa com `-`, é arquivo.
- `arg[1] == '\0'`: é `-` sozinho, que é **stdin**, não flag.

Se usássemos `&&`, o `-` sozinho viraria "flag sem nome" e seria
ignorado — quebrando `cat -`.

## Por que `-b` sobrepõe `-n`?

O `cat` POSIX especifica: se ambos forem passados, `-b` ganha.
A sobreposição é feita **depois** de processar todas as flags,
não "a última flag ganha":

```c
if (opts->number_nonblank)
    opts->number_all = 0;
```

Assim, `cat -n -b` e `cat -b -n` dão o mesmo resultado.

## O caso do `$` no lugar errado (ou: como um bug de plataforma virou lição)

### O sintoma

Ao testar `cat -E` no Windows, a saída visual era:

```
$    1  linha 1 quatro palavras
$    2
$    3  linha 3 cinco palavras cinco
```

O `$` do `-E` deveria aparecer **no fim** de cada linha, não no
começo. O `-n` sozinho funcionava.

### O diagnóstico

O primeiro passo foi **ver os bytes**, não o texto. Terminais
mentem: expandem tabs, escondem `\r`, interpretam sequências de
escape. Usamos `certutil -dump` no arquivo de saída:

```
0000  6c 69 6e 68 61 20 31 20  71 75 61 74 72 6f 20 70   linha 1 quatro p
0010  61 6c 61 76 72 61 73 0d  24 0a 0d 24 0a 6c 69 6e   alavras.$..$.lin
0020  68 61 20 33 20 63 69 6e  63 6f 20 70 61 6c 61 76   ha 3 cinco palav
0030  72 61 73 20 63 69 6e 63  6f 0d 24 0a               ras cinco.$.
```

A sequência crítica é `0d 24 0a`: **`\r` (`0d`) antes do `$` (`24`),
antes do `\n` (`0a`)**.

O que estava acontecendo:

1. O arquivo de teste estava em **CRLF** (`\r\n`).
2. O `cat` abre em modo **binário** (`"rb"`), então lê `\r` e `\n`
   como dois caracteres separados.
3. O `-E` transforma **só o `\n`** em `$\n`, deixando o `\r` intacto.
4. A sequência emitida é `\r$\n`.
5. No terminal, `\r` faz o **cursor voltar ao início da linha**.
   O `$` é impresso na coluna 0, **sobrescrevendo** o primeiro
   caractere do número.
6. Visualmente, parece que o `$` está no começo. Mas os bytes
   mostram que ele está **depois** do `\r` e **antes** do `\n`.

**Não era bug do `cat.c`**: o código copiava bytes fielmente. O
`cat` real (GNU coreutils) faz exatamente a mesma coisa. O efeito
visual é do terminal, não do programa.

### Decisões consideradas

**1. Abrir em modo texto no Windows (`"r"` em vez de `"rb"`)**

A libc traduziria `\r\n → \n` na leitura, e o `\r` sumiria.

*Rejeitado*: quebra a cópia fiel de binários (um `.jpg` com `\r\n`
no meio seria corrompido). Cria divergência Windows/Linux. E não é
o que o `cat` real faz — o `cat` é uma ferramenta de bytes.

**2. `#ifdef _WIN32` convertendo `\r\n → \n` na saída**

*Rejeitado*: mesmos problemas do item 1, mais um: o aluno não
aprende *por que* funciona — só vê uma diferença de plataforma.
Portabilidade não é "cada plataforma faz o que quiser"; é "o mesmo
comportamento em todo lugar".

**3. Converter os fixtures para LF**

*Rejeitado como solução única*: resolve o teste, mas não o uso real
(o usuário Windows vai abrir arquivos CRLF e ver o mesmo efeito).

**4. Manter o comportamento fiel e adicionar flag `-U` (opt-in)**

*Escolhido*. O `cat` copia bytes (contrato POSIX). A flag `-U`
converte `\r\n → \n` **na saída**, só quando o usuário pede. O
padrão continua fiel.

### A implementação do `-U`

Converter `\r\n → \n` sem estragar `\r` sozinho exige **lookahead
de 1 caractere**. A estratégia:

- Ao ver `\r`, **não emitir nada ainda**. Marcar `last_was_cr`.
- No próximo caractere:
  - Se for `\n`, o `\r` era parte de CRLF: descartar o `\r` e
    processar o `\n` normalmente.
  - Se for outra coisa, o `\r` era sozinho: emitir o `\r` pendente
    e processar o caractere atual.
- Se o arquivo terminar com `\r` (EOF após `\r`), o `\r` ficou
  pendente: emitir após o loop.

Esse padrão — estado pendente + lookahead — é a base de todo
parser. O `wc` e o `cat -s` já tinham máquina de estados; agora o
aluno vê **lookahead**, que é um passo adiante.

**Bug sutil durante a implementação**: a primeira versão não
tratava `\r` no fim do arquivo. O `while` terminava, e o `\r`
ficava "na mão" — nunca emitido. A correção foi o bloco após o
`while`. **Esquecer de tratar o estado pendente na saída do loop
é um bug clássico** — vale lembrar.

### Por que essa decisão

- **Fidelidade**: `cat` copia bytes. Converter silenciosamente
  quebraria binários e divergiria do `cat` real.
- **Portabilidade**: o mesmo código, o mesmo comportamento, em
  qualquer plataforma. Sem `#ifdef` para EOL.
- **Didática**: o aluno aprende que conversão de EOL é uma
  **decisão explícita**, não um automatismo. E aprende a usar
  `certutil -dump` (ou `xxd`, `od -c`) para ver os bytes reais
  quando a saída visual engana.
- **Usabilidade**: o usuário Windows que quer saída limpa tem a
  flag `-U`, sem precisar de `dos2unix`.

## O bug do `-s`: estado compartilhado mal isolado

### O sintoma

Com `cat -s` (colapsar linhas em branco múltiplas), a saída
mostrava **três** linhas em branco onde deveria haver **uma**.
O `-s` não suprimia nada.

### O diagnóstico

O `-s` testa `if (opts->squeeze_blank && c == '\n' && st.last_was_blank)`.
Para suprimir, `last_was_blank` precisa ser 1 quando o segundo `\n`
consecutivo chega. Mas a variável **nunca era 1** no momento do teste.

A causa: `last_was_blank` estava sendo usada com **dois significados
ao mesmo tempo**:

1. "O último caractere lido foi `\n`" (usado na atualização de
   estado).
2. "A linha anterior era vazia" (usado pelo `-s`).

São coisas diferentes! Uma linha é vazia quando **dois `\n`
consecutivos** são vistos. O primeiro `\n` termina a linha com
conteúdo; o segundo representa uma linha vazia.

Além disso, o bloco de **numeração** resetava `at_line_start = 0`
**antes** da atualização de estado. Então, quando a atualização
tentava usar `at_line_start` para decidir se a linha era vazia,
a informação já tinha se perdido.

### Primeira tentativa (falhou)

Tentei:

```c
if (c == '\n') {
    st.last_was_blank = st.at_line_start;
    st.at_line_start = 1;
} else {
    st.at_line_start = 0;
    st.last_was_blank = 0;
}
```

A ideia: se o `\n` chegou quando `at_line_start` ainda era 1, a
linha era vazia. Mas o bloco de numeração **já tinha resetado**
`at_line_start = 0` antes. Então `last_was_blank` era sempre 0.
**O `-s` continuou sem suprimir.**

### A solução

Capturar `at_line_start` **no início da iteração**, antes de
qualquer processamento:

```c
while ((c = fgetc(input)) != EOF) {
    int was_at_line_start = st.at_line_start;

    /* ... -U, -s, numeração, emit_char ... */

    if (c == '\n') {
        st.last_was_blank = was_at_line_start;
        st.at_line_start = 1;
    } else {
        st.at_line_start = 0;
        st.last_was_blank = 0;
    }
}
```

Agora `was_at_line_start` guarda a informação **antes** de a
numeração resetar `at_line_start`. Se o `\n` chegou quando a linha
ainda estava no início, ela era vazia. Correto.

### Por que essa decisão

- **Separação de conceitos**: `at_line_start` indica "estamos no
  início de uma linha" (para numeração); `last_was_blank` indica
  "a linha anterior era vazia" (para squeeze). Cada um com seu
  papel.
- **Captura antes de modificar**: a informação "a linha estava
  vazia" precisa ser capturada **antes** de qualquer bloco que
  mexa em `at_line_start`.
- **Simplicidade**: uma variável local, uma linha a mais. Não
  precisou reestruturar o loop.

### Lição geral

> Reutilizar uma variável para dois conceitos é fonte clássica de
> bug. `last_was_blank` tentava ser "último foi `\n`" e "linha
> anterior era vazia" ao mesmo tempo. Quando os dois conceitos
> divergem (numa linha vazia, os dois são 1; mas em qual momento?),
> o bug aparece. **Uma variável, um conceito.**

## O `-s` com CRLF: por que ainda falha sem `-U`

Mesmo após a correção, `-s` sozinho num arquivo CRLF **não**
colapsa as linhas em branco como esperado. A sequência real é
`...cinco\r\n\r\n\r\n\r\nlinha...`. O `-s` testa `c == '\n'`,
mas o `\r` **reseta `last_was_blank` para 0** (no bloco `else`).
Então, quando o `\n` chega, `last_was_blank` é 0 e não suprime.

**Isso não é bug**: é o comportamento do `cat` POSIX com CRLF.
O `cat` real no Linux faz o mesmo. A solução é `-U`, que remove
o `\r` antes do squeeze:

```
cat -Us arquivo.txt   →  colapsa corretamente
```

### Lição geral

> CRLF é um problema real, não um detalhe. Ferramentas que operam
> em "linhas" (como `sed`, `awk`, `grep -c`, e nosso `-s`) precisam
> de LF puro para funcionar como esperado. A solução não é fazer
> cada ferramenta adivinhar CRLF, é **converter** antes (`-U`,
> `dos2unix`). O `-U` é a ferramenta para isso.

## Por que "sem argumentos mostra help" em vez de ler de stdin?

O `cat` POSIX, sem argumentos, lê de stdin. Você digita `cat` e
Enter, ele fica esperando entrada; você precisa de `Ctrl+D`
(Linux/macOS) ou `Ctrl+Z` (Windows) para sair.

No Windows, `Ctrl+Z` é pouco conhecido. Mostrar o help é mais
amigável para quem nunca usou `cat`. Mas é uma **decisão de
design**, não o comportamento POSIX. Documentada no help:

> "Sem argumentos, mostra esta ajuda (o cat POSIX leria de stdin;
> optamos por mostrar a ajuda por ser mais amigavel no Windows)."

### Lição geral

> "Amigável" e "compatível com POSIX" nem sempre coincidem. Quando
> divergem, a escolha precisa ser **consciente** e **documentada**.
> O usuário que conhece o `cat` real não deve ser surpreendido
> sem aviso.

## Por que `-h` usa `exit(0)` e o `default` usa `return -1`?

`exit` é legítimo em **casos terminais**: `-h` mostra help e sai,
não há mais nada a fazer. `exit(0)` é claro: sucesso, fim.

Já uma flag inválida é um **erro recuperável** (o usuário pode
corrigir e rodar de novo). `parse_flags` retorna `-1`, e `main`
faz `return 1`. Isso permite testar `parse_flags` isoladamente
(no futuro) e mantém o controle do fluxo no `main`.

**Regra**: `exit` em caso terminal (help, versão), `return` em
caso recuperável (erro de flag, arquivo não encontrado).

## Por que `-U` é processado antes de `-s`, `-n`, `-E`?

A ordem no loop é:

1. `-U` (conversão CRLF)
2. `-s` (squeeze)
3. `-n`/`-b` (numeração)
4. `-E`/`-T` (transformação de caractere)

A ordem **importa**:

- `-U` remove `\r`, então `-s` e `-n` veem só `\n`. Sem isso, o
  `\r` atrapalha (como vimos).
- `-s` decide se a linha é suprimida **antes** da numeração. Se
  a numeração viesse antes, uma linha vazia suprimida ainda
  consumiria um número.
- `-E`/`-T` transformam o caractere **na saída**, depois de todas
  as decisões. Se viessem antes, o `$` do `-E` atrapalharia a
  detecção de `\n`.

### Lição geral

> A ordem das transformações é parte do design. Cada flag é uma
> camada de um pipeline; a ordem define o que cada camada "vê".
> Documentar a ordem é documentar o design.

## Lições gerais deste projeto

1. **Quando a saída de um programa "parece errada", veja os bytes,
   não o texto.** Terminais mentem. `certutil -dump`, `xxd`,
   `od -c`, `Format-Hex` mostram a verdade.
2. **Comportamento "tecnicamente correto" e "visualmente agradável"
   nem sempre coincidem.** A solução não é escolher um dos dois,
   é dar ao usuário **controle** (flag opt-in).
3. **Conversão de EOL é uma decisão, não um automatismo.** O
   `#ifdef _WIN32` que traduz `\r\n → \n` esconde essa decisão do
   usuário. A flag `-U` a expõe.
4. **Lookahead de 1** é o padrão para processar sequências de 2
   caracteres (`\r\n`, `//`, `/*`, `->`). Estado pendente +
   próximo caractere resolve sem buffer.
5. **Estado pendente após o loop** é uma fonte comum de bug.
   Sempre que há máquina de estados, pergunte: "o que acontece se
   o input terminar no meio de uma transição?".
6. **Uma variável, um conceito.** Reutilizar `last_was_blank` para
   dois significados causou o bug do `-s`.
7. **Capture a informação antes de modificá-la.** O bloco de
   numeração resetava `at_line_start` antes da atualização de
   estado. Capturar no início da iteração resolveu.
8. **`exit` em caso terminal, `return` em caso recuperável.**
9. **A ordem das transformações é parte do design.** Documentar a
   ordem é documentar o pipeline.
10. **"Amigável" e "compatível" podem divergir.** Quando divergem,
    a escolha precisa ser consciente e documentada.

## Conceitos relacionados

- [Máquina de estados](../../docs/state-machine.md) — usada em
  `-s` (squeeze) e no `wc`.
- [Lookahead](../../docs/lookahead.md) — usada em `-U` (CRLF).
