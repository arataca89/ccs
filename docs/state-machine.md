# Máquina de estados

> Um programa que "lembra" o que aconteceu antes precisa de estado.
> Máquina de estados é a forma mais simples e mais comum de organizar
> essa memória.

## O que é

Uma **máquina de estados** (ou autômato finito) é um modelo de
computação com:

- Um conjunto finito de **estados**.
- Um **estado inicial**.
- Regras de **transição**: dado o estado atual e um **evento**
  (entrada), qual é o próximo estado.
- Opcionalmente, **ações** associadas a transições ou a estados.

A ideia central: o comportamento do programa **depende do estado
atual**. O mesmo evento pode causar ações diferentes, dependendo
de onde a máquina está.

## Por que isso aparece em C

C não tem estado embutido. Você declara uma variável `int` e pronto:
é estado. Isso é ao mesmo tempo simples (uma variável) e perigoso
(uma variável mal usada vira bug). Máquina de estados é uma
**disciplina** para usar essas variáveis de forma correta.

Em C, uma máquina de estados costuma ser:

- Uma variável `int` ou `enum` para o estado atual.
- Um `switch` sobre o estado, ou um `if/else` encadeado.
- Transições explícitas (atribuições ao estado).

## Exemplo 1: contar palavras (`001_wc`)

No `wc`, a contagem de palavras é uma máquina de **dois estados**:

- **FORA_DE_PALAVRA**: o último caractere lido foi espaço (ou é o
  início do arquivo).
- **DENTRO_DE_PALAVRA**: o último caractere lido foi não-espaço.

O "evento" é cada caractere lido. As transições:

| Estado atual | Evento | Próximo estado | Ação |
|---|---|---|---|
| FORA | espaço | FORA | — |
| FORA | não-espaço | DENTRO | `words++` |
| DENTRO | espaço | FORA | — |
| DENTRO | não-espaço | DENTRO | — |

Em C:

```c
int in_word = 0;   /* estado: 0 = FORA, 1 = DENTRO */
while ((c = fgetc(f)) != EOF) {
    int is_space = (c == ' ' || c == '\n' || c == '\t');
    if (is_space) {
        in_word = 0;
    } else if (!in_word) {
        in_word = 1;
        words++;
    }
}
```

Duas coisas importantes:

1. **O estado é explícito**: `in_word` não é "número de palavras
   menos alguma coisa". É o estado da máquina.
2. **A ação depende da transição**: `words++` só acontece na
   transição FORA → DENTRO. É isso que evita contar "  a  b  "
   como 4 palavras (os espaços não incrementam).

## Exemplo 2: colapsar linhas em branco (`002_cat -s`)

No `cat -s`, o estado é:

- **ÚLTIMA_LINHA_FOI_VAZIA**: a linha anterior era em branco.
- **ÚLTIMA_LINHA_TINHA_CONTEÚDO**: a linha anterior tinha conteúdo.

O evento é "chegou um `\n`". Transições:

| Estado | Evento | Próximo | Ação |
|---|---|---|---|
| TINHA_CONTEÚDO | `\n` | TINHA_CONTEÚDO ou VAZIA (depende do próximo) | emite `\n` |
| VAZIA | `\n` | VAZIA | **suprime** (não emite) |
| VAZIA | conteúdo | TINHA_CONTEÚDO | emite |

A sutileza: a linha **atual** é vazia ou não? Isso só se sabe
**depois** de ler o primeiro caractere. Daí a variável
`at_line_start` no `cat.c`: ela guarda "estamos no início de uma
linha", e a transição para "linha tem conteúdo" acontece no
primeiro caractere não-`\n`.

## Exemplo 3: um lexer 

Lexers (analisadores léxicos) são máquinas de estados. Um lexer
simples para reconhecer números e identificadores:

- **INICIAL**: aguardando próximo token.
- **EM_NUMERO**: lendo dígitos.
- **EM_IDENTIFICADOR**: lendo letras/dígitos/underscore.

Eventos são caracteres. Transições:

| Estado | Evento | Próximo | Ação |
|---|---|---|---|
| INICIAL | dígito | EM_NUMERO | inicia número |
| INICIAL | letra | EM_IDENTIFICADOR | inicia identificador |
| EM_NUMERO | dígito | EM_NUMERO | continua |
| EM_NUMERO | outro | INICIAL | emite token NUMERO |
| EM_IDENTIFICADOR | letra/dígito/`_` | EM_IDENTIFICADOR | continua |
| EM_IDENTIFICADOR | outro | INICIAL | emite token IDENT |

Isso é a base de **todo** compilador e interpretador. 

## Exemplo 4: máquina de estados em rede (protocolos)

Protocolos de rede são máquinas de estados. O handshake do TCP:

- **CLOSED** → (SYN enviado) → **SYN_SENT**
- **SYN_SENT** → (SYN+ACK recebido) → **ESTABLISHED**
- **ESTABLISHED** → (FIN recebido) → **CLOSE_WAIT**
- ...

Cada lado da conexão mantém um estado. O mesmo pacote (ex.: um
`SYN`) tem significado diferente dependendo do estado. Isso é
máquina de estados em escala de rede.

## Como implementar em C

### Padrão 1: `int` + `if/else` (mais simples)

```c
int state = STATE_INITIAL;
while ((c = fgetc(f)) != EOF) {
    if (state == STATE_INITIAL) {
        if (isdigit(c)) {
            state = STATE_IN_NUMBER;
            /* ... */
        }
    } else if (state == STATE_IN_NUMBER) {
        /* ... */
    }
}
```

Bom para 2–3 estados. Fica ruim com muitos.

### Padrão 2: `enum` + `switch` (mais legível)

```c
typedef enum {
    STATE_INITIAL,
    STATE_IN_NUMBER,
    STATE_IN_IDENT,
} State;

State state = STATE_INITIAL;
while ((c = fgetc(f)) != EOF) {
    switch (state) {
    case STATE_INITIAL:
        if (isdigit(c)) state = STATE_IN_NUMBER;
        else if (isalpha(c)) state = STATE_IN_IDENT;
        break;
    case STATE_IN_NUMBER:
        if (!isdigit(c)) {
            emit_number();
            state = STATE_INITIAL;
            /* reprocessa c */
        }
        break;
    /* ... */
    }
}
```

O `switch` deixa claro **quais** estados existem e **o que** cada
um faz. É o padrão mais usado em C.

### Padrão 3: tabela de transição (para máquinas grandes)

```c
typedef struct {
    State next;
    void (*action)(void);
} Transition;

Transition table[NUM_STATES][NUM_EVENTS] = {
    /* ... */
};
```

Para máquinas com dezenas de estados (ex.: parser de XML, protocolo
complexo). Mais difícil de ler, mas escala.

**Para o CCS**, os padrões 1 e 2 bastam. O padrão 3 é para quando
o projeto for **sobre** máquina de estados (ex.: um lexer completo).

## Erros comuns

1. **Esquecer de tratar o estado inicial.** No `cat -U`, o estado
   inicial é `last_was_cr = 0`. Se você esquecer, o primeiro
   caractere pode ser mal interpretado.

2. **Esquecer o estado pendente após o loop.** Se o input termina
   no meio de uma transição, o estado fica "na mão". Ex.: arquivo
   termina com `\r` (sem `\n` depois) — o `\r` precisa ser emitido
   após o `while`. **Esse é um bug clássico.**

3. **Reutilizar uma variável para dois conceitos.** No `cat -s`,
   `last_was_blank` tentava ser "último foi `\n`" e "linha anterior
   era vazia". Bug garantido. **Uma variável, um conceito.**

4. **Não documentar a máquina.** Se o código tem 5 estados e o
   `notes.md` não tem uma tabela de transições, o próximo leitor
   (inclusive você, 6 meses depois) vai penar. **A tabela de
   transições é parte da documentação.**

## Onde máquina de estados aparece

- **Lexers e parsers** (compiladores, interpretadores, JSON, XML).
- **Protocolos de rede** (TCP, HTTP/2, WebSocket).
- **Jogos** (menu, jogando, pausado, game over).
- **UI** (formulário: preenchendo, validando, enviando, sucesso).
- **Processamento de texto** (`wc`, `cat -s`, `grep`, `sed`).
- **Emuladores de CPU** (cada instrução é uma transição).
- **Máquinas de venda** (o exemplo clássico de livro-texto:
  aguardando moeda, selecionando produto, dispensando).

Se você programa em C por tempo suficiente, vai implementar
máquinas de estados **muitas** vezes. Reconhecer o padrão (e dar
nome a ele) é metade do caminho.

## Leitura recomendada

- *Introduction to Automata Theory, Languages, and Computation*,
  Hopcroft, Motwani, Ullman — o livro-texto clássico (denso).
- *Compilers: Principles, Techniques, and Tools* (o "Dragão"),
  Aho et al. — capítulo 3 cobre lexers como máquinas de estados.
- *The Art of Unix Programming*, Eric Raymond — capítulo sobre
  "Textuality" menciona máquinas de estados em ferramentas Unix.
