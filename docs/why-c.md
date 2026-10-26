# Por que C ainda importa

> "Mas C não é mais usada, né?"

Essa pergunta aparece em quase toda primeira aula. É compreensível:
quem aprende programação hoje vê Python, JavaScript, Go, Rust nas
manchetes. C parece coisa de museu. Este texto é uma resposta
direta a essa percepção — não com saudosismo, mas com fatos.

## Objeção 1: "C é antiga"

C nasceu em 1972, nos Bell Labs. É mais velha que a maioria dos
programadores vivos. Mas idade, em linguagens, não é defeito: é
sinal de que ela resolveu problemas que continuam existindo.

O TCP/IP que move a internet foi implementado em C. O kernel do
Linux é C. O Windows tem camadas enormes em C. O macOS roda sobre
um núcleo (XNU) que é C e C++. O PostgreSQL é C. O SQLite — o banco
de dados mais implantado do mundo, presente em todo celular — é C.
O Git é C. O CPython (interpretador do Python) é C. O V8 (motor
JavaScript do Chrome e do Node.js) é C++. O Nginx é C.

A lista não é curiosidade histórica: é a infraestrutura sobre a
qual as linguagens "modernas" rodam. Python não substituiu C —
Python é escrito em C. Rust não substituiu C — Rust chama C o
tempo todo via FFI. Go tem runtime escrito em Go, mas conversa
com o sistema via syscalls que são C.

## Objeção 2: "C é difícil"

C é pequena. A linguagem inteira cabe num livro de 300 páginas
(o K&R). Não tem classes, herança, genéricos, garbage collector,
mônadas. Tem ponteiros, que assustam — mas ponteiros são apenas
números que apontam para endereços de memória. Difícil é a
**responsabilidade**: em C, você gerencia memória à mão. Isso
assusta porque expõe o que outras linguagens escondem.

Mas "difícil" e "poderosa" andam juntas. C dá controle que nenhuma
linguagem de alto nível dá: você decide onde cada byte mora, quando
é liberado, como o programa fala com o sistema operacional. Quem
aprende C aprende **como o computador funciona** — e isso vale para
qualquer linguagem que a pessoa use depois.

## Objeção 3: "C é insegura"

Verdade parcial. C não tem proteção contra buffer overflow, use-after-free,
null pointer dereference. Mas a insegurança não está na linguagem:
está no **uso**. Um programa C bem escrito, com testes, sanitizers
(ASan, UBSan) e revisão, é tão seguro quanto qualquer outro. O
problema é que C permite escrever código inseguro — e muita gente
escreve.

Rust resolve parte disso com o borrow checker. Mas Rust precisa
de C para falar com o sistema operacional, e uma quantidade enorme
de código C legado continuará rodando por décadas. A resposta
prática não é "abandonar C", é "escrever C com disciplina". Este
repositório é, em parte, sobre essa disciplina.

## Objeção 4: "Vou trabalhar com web/mobile/dados, não preciso de C"

Talvez você nunca escreva C profissionalmente. Mas vai **ler** C:
quando um stack trace do Python apontar para uma extensão C,
quando um erro do Node.js vier do V8, quando o Docker falhar por
causa do kernel, quando o banco de dados estourar memória. Quem
entende C entende **o que está embaixo** — e diagnostica problemas
que outros não conseguem.

Além disso, C é a lingua franca dos sistemas. Se você sabe C,
aprende C++, Rust, Go, Zig com muito menos atrito, porque todas
elas conversam com o mesmo substrato.

## O que C constrói

C é uma linguagem de sistemas: uma ferramenta que constrói
ferramentas. Compiladores, interpretadores, sistemas operacionais,
bancos de dados, servidores, drivers, firmware, jogos, ferramentas
de linha de comando — tudo isso é feito em C, ou em algo que fala
com C.

Se você quer **usar** ferramentas, há linguagens melhores. Se você
quer **construir** ferramentas, C é o caminho mais direto.

## Leitura recomendada

- *The C Programming Language*, Kernighan & Ritchie (1978, 2ª ed. 1988)
- *Modern C*, Jens Gustedt (3ª ed., 2019) — gratuito online
- *Expert C Programming: Deep C Secrets*, Peter van der Linden (1994)
- *21st Century C*, Ben Klemens (2ª ed., 2014)
