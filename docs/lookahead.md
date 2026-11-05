# Lookahead

> Às vezes, para decidir o que fazer com o caractere atual, você
> precisa **ver o próximo**. Lookahead é a técnica de "olhar um
> pouco à frente" antes de agir.

## O que é

**Lookahead** (literalmente "olhar à frente") é a técnica de
examinar os próximos caracteres (ou tokens) da entrada **antes**
de decidir o que fazer com o atual.

O caso mais comum é **lookahead de 1**: você precisa ver **um**
caractere à frente para decidir. Exemplos:

- `\r\n`: o `\r` só é "quebra de linha do Windows" se o próximo
  for `\n`. Se for outra coisa, o `\r` é sozinho.
- `//`: a barra só inicia comentário de linha se a próxima também
  for barra.
- `/*`: idem, para comentário de bloco.
- `->`: o hífen só é "seta" se o próximo for `>`.
- `==`: o `=` só é "comparação" se o próximo for `=`.

Em todos esses casos, você não pode decidir com **um** caractere
só. Precisa de dois.

## Por que isso aparece em C

C lê entrada **sequencialmente**, um caractere (ou byte) de cada
vez. `fgetc` retorna **um** caractere. Não há "espiar o próximo"
nativo. Então você tem que **simular** lookahead com estado.

O padrão clássico é:

1. Ler o caractere.
2. Se ele **pode** iniciar uma sequência de dois, guardar o estado
   (ex.: `last_was_cr = 1`) e **não emitir nada ainda**.
3. No próximo caractere, decidir:
   - Se completa a sequência (ex.: `\n` após `\r`), agir de acordo.
   - Se não completa (ex.: `x` após `\r`), "desfazer" o adiamento
     (emitir o `\r` pendente) e processar o `x` normalmente.

## Exemplo 1: `\r\n` (`002_cat -U`)

Sem lookahead, `-U` não funciona. Você lê `\r`, e não sabe se é:

- `\r\n` (CRLF do Windows) → deve converter em `\n`.
- `\r` sozinho (raro, mas existe em arquivos antigos de Mac) →
  deve preservar.

A solução com estado + lookahead:

```c
int last_was_cr = 0;
while ((c = fgetc(f)) != EOF) {
    if (c == '\r') {
        last_was_cr = 1;
        continue;   /* não emite ainda; decide no próximo */
    }
    if (last_was_cr) {
        last_was_cr = 0;
        if (c != '\n') {
            emit('\r');   /* era \r sozinho: emite o pendente */
        }
        /* se c == '\n', o \r era de CRLF: descarta */
    }
    emit(c);
}
/* EOF: se last_was_cr == 1, o \r ficou pendente; emitir */
if (last_was_cr) emit('\r');
```

**Pontos importantes**:

1. **O `\r` é adiado**, não emitido imediatamente. É o "estado
   pendente".
2. **O próximo caractere resolve o adiamento**: se for `\n`,
   descarta o `\r`; se for outro, emite o `\r` e processa o outro.
3. **O EOF também resolve**: se o arquivo termina com `\r`, o
   `\r` fica pendente após o `while`. **Esquecer isso é bug
   clássico.**

## Exemplo 2: comentários `//` e `/*`

Um lexer que reconhece comentários:

```c
int last_was_slash = 0;
while ((c = fgetc(f)) != EOF) {
    if (last_was_slash) {
        last_was_slash = 0;
        if (c == '/') {
            /* comentário de linha: consome até \n */
            while ((c = fgetc(f)) != EOF && c != '\n')
                ;
            continue;
        } else if (c == '*') {
            /* comentário de bloco: consome até */
            int prev = 0;
            while ((c = fgetc(f)) != EOF) {
                if (prev == '*' && c == '/') break;
                prev = c;
            }
            continue;
        } else {
            /* era só uma barra: emite e processa c */
            emit('/');
        }
    }
    if (c == '/') {
        last_was_slash = 1;
        continue;
    }
    emit(c);
}
```

**Pontos importantes**:

1. **`/` é adiado**: não sabemos se é divisão, comentário de linha
   ou de bloco até ver o próximo.
2. **O próximo resolve**: `/` → comentário de linha; `*` →
   comentário de bloco; outro → divisão.
3. **Comentário de bloco precisa de lookahead de 2** (`*/`), não
   de 1. O estado `prev` guarda o caractere anterior.

## Exemplo 3: números vs. identificadores

Um lexer simples:

```c
/* Se o primeiro caractere é dígito, é número.
 * Se é letra ou _, é identificador.
 * Mas "123abc" é erro (ou número seguido de identificador). */
```

Aqui o lookahead é **semântico**, não de caractere: você lê o
primeiro caractere, decide o **tipo** do token, e depois consome
até o fim do token. Não é lookahead de 1 no sentido estrito, mas
é a mesma ideia: **decidir com base no que vem depois**.

## Lookahead de N

Para N > 1, a técnica é a mesma, mas com um buffer:

```c
int buf[N];
int n_buf = 0;

int peek(int k) {
    /* retorna o k-ésimo caractere à frente, preenchendo o buffer */
    while (n_buf <= k) {
        buf[n_buf++] = fgetc(f);
    }
    return buf[k];
}

int consume(void) {
    int c = peek(0);
    /* desloca o buffer */
    for (int i = 1; i < n_buf; i++)
        buf[i-1] = buf[i];
    n_buf--;
    return c;
}
```

Isso é o que parsers de linguagens reais fazem (ex.: o parser do
GCC usa lookahead de 1 token, mas o lexer pode precisar de mais
para strings, números, etc.).

**No CCS**, lookahead de 1 basta para `cat -U`. Lookahead de 2
aparece em `/* */`. Lookahead maior só em projetos de parser
completo.

## Alternativa: `ungetc`

A libc tem `ungetc(c, f)`, que "devolve" um caractere ao stream.
Com ela, você pode fazer lookahead de 1 sem estado:

```c
int c = fgetc(f);
if (c == '\r') {
    int next = fgetc(f);
    if (next == '\n') {
        /* CRLF: processa como \n */
    } else {
        ungetc(next, f);   /* devolve o próximo */
        /* processa \r sozinho */
    }
}
```

**Vantagem**: código mais direto.

**Desvantagem**: `ungetc` garante **apenas 1** caractere de
"devolução" (C padrão). Para lookahead de 2+, não serve. E o
estado fica implícito no stream, o que pode confundir em código
com múltiplas funções lendo do mesmo `FILE *`.

**No CCS**, preferimos o padrão de estado explícito (`last_was_cr`)
porque:

- Funciona para lookahead de N > 1.
- O estado fica **visível** no código (variável), não escondido
  no stream.
- Ensina o padrão que parsers reais usam.

`ungetc` é uma alternativa válida para casos simples, e vale
conhecer.

## Erros comuns

1. **Esquecer o estado pendente após o loop.** Se o input termina
   no meio do lookahead (ex.: arquivo termina com `\r`, ou com
   `/`), o estado fica "na mão". **Sempre trate o EOF.**

2. **Emitir o caractere adiado duas vezes.** Se você adia o `\r`,
   e depois emite o `\r` pendente **e** o `\r` original, duplica.
   Controle o estado com cuidado.

3. **Confundir lookahead de 1 com lookahead de N.** `\r\n` é 1.
   `/* */` é 2 (precisa de `*` **e** `/`). `"""` (strings Python)
   é 3. Cada caso tem seu N.

4. **Usar `ungetc` mais de uma vez.** C padrão garante só 1.
   Se precisar de mais, use buffer explícito.

## Onde lookahead aparece

- **Lexers** (comentários, strings, números, operadores compostos).
- **Parsers** (decidir entre alternativas gramaticais).
- **Protocolos** (delimitadores de mensagem, escapes).
- **Processamento de texto** (`\r\n`, `\t`, entidades HTML `&amp;`).
- **Compressão** (LZ77 usa lookahead para encontrar repetições).
- **Editores** (auto-complete, syntax highlighting).

Sempre que a decisão depende de "o que vem depois", lookahead
está presente.

## Leitura recomendada

- *Compilers: Principles, Techniques, and Tools* (o "Dragão"),
  Aho et al. — capítulo 3 (análise léxica) cobre lookahead em
  lexers.
- *flex & bison*, John Levine — ferramentas que geram lexers e
  parsers; lookahead é central.
- *Parsing Techniques: A Practical Guide*, Grune & Jacobs —
  referência avançada, cobre lookahead de N.
```

---

## Resumo do que criar agora

```
docs/
├── state-machine.md      ← novo
└── lookahead.md          ← novo
```

E, no `README.md` da raiz, adicionar à seção "Documentos":

```markdown
- [Máquina de estados](docs/state-machine.md)
- [Lookahead](docs/lookahead.md)
```

E, no `notes.md` do `002_cat`, adicionar links no início ou no
final, apontando para os conceitos:


## Conceitos relacionados

- [Máquina de estados](../../docs/state-machine.md) — usada em
  `-s` (squeeze) e no `wc`.
- [Lookahead](../../docs/lookahead.md) — usada em `-U` (CRLF).
