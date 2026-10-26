# Linha do tempo de C

Uma cronologia da linguagem, dos antecedentes aos dias atuais.
Não é exaustiva: foca nos marcos que explicam **por que** C é como é.

## Antes de C (1960–1972)

- **1960 — ALGOL 60**: primeira linguagem com sintaxe estruturada
  em blocos (`begin ... end`). Influenciou quase tudo que veio depois.
- **1964 — CPL (Combined Programming Language)**: projeto ambicioso
  do Cambridge, nunca terminado.
- **1967 — BCPL (Basic CPL)**: simplificação do CPL por Martin
  Richards. Introduziu `//` para comentários (que C só adotaria
  em C99!) e a ideia de "a linguagem não tem tipos, tudo é palavra".
- **1969 — B**: Ken Thompson, nos Bell Labs, simplifica BCPL para
  caber num PDP-7. B é tipicamente sem tipos, e isso se mostra um
  problema.
- **1969–1970 — Unix**: Thompson e Dennis Ritchie escrevem o Unix
  em assembly para o PDP-7.

## Nascimento e infância (1972–1978)

- **1972 — C**: Dennis Ritchie cria C nos Bell Labs. A grande
  inovação sobre B: **tipos**. `int`, `char`, `float`, ponteiros
  tipados. O nome "C" é a próxima letra depois de B (que era
  a próxima depois de A, de ALGOL... a linhagem é quase alfabética).
- **1973 — Unix reescrito em C**: marco histórico. Pela primeira
  vez, um sistema operacional é portável — basta recompilar em
  outra máquina. Isso muda tudo.
- **1978 — K&R**: Brian Kernighan e Dennis Ritchie publicam *The
  C Programming Language*. O livro é, de fato, a primeira
  especificação da linguagem — o "padrão K&R".

## Padronização (1983–1999)

- **1983 — ANSI X3J11**: comitê formado para padronizar C. A
  linguagem estava fragmentada em dialetos de fabricante.
- **1989 — C89 / ANSI C**: ratificado como ANSI X3.159-1989.
- **1990 — C90 / ISO C**: ISO adota o mesmo padrão como
  ISO/IEC 9899:1990. C89 e C90 são, na prática, o mesmo padrão.
- **1995 — C95**: emenda menor (wide chars, digrafos).
- **1999 — C99**: a maior revisão até então. Introduziu:
  - `//` comentários de linha (roubado de C++ e BCPL)
  - Declaração de variáveis no meio de blocos
  - `long long` (inteiros de 64 bits)
  - `stdint.h` (tipos de tamanho fixo: `int32_t`, etc.)
  - Arrays de tamanho variável (VLA)
  - `inline`
  - `_Bool` / `stdbool.h`
  - Números complexos (`complex.h`)

## Século XXI (2000–hoje)

- **2011 — C11**: threads (`threads.h`), atomics (`stdatomic.h`),
  alinhamento (`_Alignas`), `_Generic`, funções anônimas. Também
  removeu `gets()` (finalmente).
- **2018 — C17 / C18**: revisão menor, correções e clarificações.
  Nenhuma feature nova.
- **2024 — C23**: a revisão mais recente. Introduziu:
  - `nullptr` (finalmente, um `NULL` decente)
  - `constexpr`
  - `typeof`
  - `auto` para inferência de tipo (sim, em C!)
  - Atributos `[[...]]` padronizados
  - `bool`, `true`, `false` como palavras-chave
  - `#embed` (incluir arquivos binários no código)
  - Melhorias em `printf` (formatos binários `%b`)

## Por que essa linha do tempo importa

Cada revisão resolveu um problema real de quem escrevia C. C99
veio porque programadores precisavam de inteiros de 64 bits e
de `//`. C11 veio porque threads viraram onipresentes. C23 veio
porque `NULL` sempre foi uma dor.

Entender **por que** cada feature existe é mais útil que decorar
**quando** ela apareceu. E entender que C evolui — devagar, com
cuidado, mas evolui — desfaz a ideia de que é uma linguagem
"parada no tempo".