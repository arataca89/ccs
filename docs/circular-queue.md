# Fila circular

> Uma fila circular é uma fila com um número **fixo** de posições,
> onde o fim "dá a volta" para o início. É usada quando o número
> máximo de elementos é conhecido, e o custo de inserir/remover
> precisa ser **O(1)**.

## O problema: filas comuns gastam memória

Uma **fila** é uma estrutura **FIFO** (First In, First Out): o
primeiro que entra é o primeiro que sai (como uma fila de banco).

**Implementação comum** (array linear):

```
fila: [A][B][C][ ][ ][ ]
       ^        ^
       início   fim
```

- **Inserir**: coloca no `fim`, e avança o `fim`.
- **Remover**: tira do `início`, e avança o `início`.

**Problema**: quando o `fim` chega ao final do array, **não há mais
espaço**, mesmo que o `início` tenha "avançado" (posições vazias
no começo). Exemplo:

```
fila: [ ][ ][C][D][E][ ]
              ^        ^
              início   fim
```

- O `fim` está na última posição.
- O `início` está na posição 2.
- Há **2 posições vazias** no começo (0 e 1).
- Mas o `fim` **não pode** inserir (chegou ao final do array).

**Soluções**:

1. **Mover os elementos** para o começo (O(n) — caro).
2. **Realocar** o array (O(n) — caro).
3. **Fila circular** (O(1) — grátis).

## A fila circular

A fila circular trata o array como um **círculo**: quando o `fim`
chega ao final, ele "dá a volta" para o início (posição 0).

**Visualização**:

```
Array: [ ][ ][C][D][E][ ]
        0  1  2  3  4  5

Como um círculo:

        0
     5     1
     4     2
        3

O fim (5) "dá a volta" para a posição 0.
```

**Inserir** (em uma fila circular):

```c
fim = (fim + 1) % capacity;
```

**Remover**:

```c
inicio = (inicio + 1) % capacity;
```

**O `% capacity`** faz o índice "dar a volta" (quando chega em
`capacity`, volta para 0).

## Exemplo passo a passo

**Fila circular** (capacity = 5, `inicio = 0`, `fim = -1`, `length = 0`):

### Inserir A

```c
fim = (fim + 1) % 5;   // fim = ( -1 + 1) % 5 = 0
fila[0] = 'A';
length = 1;
```

```
fila: [A][ ][ ][ ][ ]
       ^
       inicio = 0, fim = 0
```

### Inserir B

```c
fim = (0 + 1) % 5 = 1;
fila[1] = 'B';
length = 2;
```

```
fila: [A][B][ ][ ][ ]
       ^  ^
       inicio = 0, fim = 1
```

### Inserir C

```c
fim = (1 + 1) % 5 = 2;
fila[2] = 'C';
length = 3;
```

```
fila: [A][B][C][ ][ ]
       ^     ^
       inicio = 0, fim = 2
```

### Remover (tira A)

```c
inicio = (0 + 1) % 5 = 1;
length = 2;
```

```
fila: [ ][B][C][ ][ ]
          ^  ^
          inicio = 1, fim = 2
```

### Inserir D

```c
fim = (2 + 1) % 5 = 3;
fila[3] = 'D';
length = 3;
```

```
fila: [ ][B][C][D][ ]
          ^     ^
          inicio = 1, fim = 3
```

### Inserir E

```c
fim = (3 + 1) % 5 = 4;
fila[4] = 'E';
length = 4;
```

```
fila: [ ][B][C][D][E]
          ^        ^
          inicio = 1, fim = 4
```

### Inserir F (dá a volta!)

```c
fim = (4 + 1) % 5 = 0;   // deu a volta para 0!
fila[0] = 'F';
length = 5;
```

```
fila: [F][B][C][D][E]
       ^  ^
       fim = 0, inicio = 1
```

**O fim "deu a volta"** para a posição 0 (que estava vazia, porque
o A foi removido). **Isso é a fila circular**.

## Como percorrer uma fila circular

Para percorrer os elementos (do início ao fim), **não** dá para usar
`for (i = 0; i < length; i++)` diretamente (porque o início pode
não ser 0).

**A fórmula**:

```c
for (int i = 0; i < length; i++) {
    int idx = (inicio + i) % capacity;
    /* fila[idx] é o i-ésimo elemento */
}
```

**Exemplo** (fila = `[F][B][C][D][E]`, inicio = 1, length = 5,
capacity = 5):

| `i` | `inicio + i` | `% 5` | Elemento |
|---|---|---|---|
| 0 | 1 | 1 | B |
| 1 | 2 | 2 | C |
| 2 | 3 | 3 | D |
| 3 | 4 | 4 | E |
| 4 | 5 | **0** | F |

**Ordem**: B, C, D, E, F (do início ao fim). ✅

## No `snake.c`

A cobra é uma **fila circular**:

```c
typedef struct {
    Point *body;       /* array de pontos */
    int capacity;      /* tamanho do array */
    int head;          /* índice da cabeça */
    int tail;          /* índice da cauda */
    int length;        /* comprimento atual */
    Direction dir;
} Snake;
```

**Mapeamento**:

- **`body`**: o array de pontos.
- **`capacity`**: o tamanho do array (quantas posições cabem).
- **`head`**: o índice da **cabeça** (o "fim" da fila, onde novos
  segmentos são adicionados).
- **`tail`**: o índice da **cauda** (o "início" da fila, onde
  segmentos são removidos).
- **`length`**: o número de segmentos **ocupados** (pode ser menor
  que `capacity`).

**Inserir** (mover a cobra, sem comer):

```c
s->tail = (s->tail + 1) % s->capacity;   // remove a cauda
s->length--;

s->head = (s->head + 1) % s->capacity;   // adiciona a cabeça
s->body[s->head] = new_head;
s->length++;
```

**Inserir** (comer):

```c
s->head = (s->head + 1) % s->capacity;   // adiciona a cabeça
s->body[s->head] = new_head;
s->length++;
// a cauda NÃO é removida (a cobra cresceu)
```

**Percorrer o corpo** (no `is_body`):

```c
for (int i = 0; i < game->snake.length; i++) {
    int idx = (game->snake.tail + i) % game->snake.capacity;
    Point p = game->snake.body[idx];
    if (p.x == x && p.y == y)
        return 1;
}
```

**A linha do `idx`**: o i-ésimo segmento da cobra (começando da
cauda) está no índice `(tail + i) % capacity`.

**Por que `% capacity`**: porque a fila é circular. Se `tail = 3`
e `capacity = 5`, os segmentos estão em 3, 4, **0**, 1, 2 (o `%`
faz o índice "dar a volta").

**Exemplo concreto** (capacity = 5, tail = 3, length = 3):

| `i` | `tail + i` | `% 5` | Posição real |
|---|---|---|---|
| 0 | 3 | 3 | `body[3]` (cauda) |
| 1 | 4 | 4 | `body[4]` |
| 2 | 5 | **0** | `body[0]` (deu a volta!) |

## Por que o Snake usa fila circular

**A cobra cresce e encolhe**:

- **Come**: a cabeça é adicionada, e a cauda **não** é removida
  (a cobra cresce).
- **Move** (sem comer): a cabeça é adicionada, e a cauda é
  removida (a cobra mantém o tamanho).

**Com uma fila comum**:

- **Inserir** no fim: O(1).
- **Remover** do início: O(n) (precisa mover os elementos).

**Com uma fila circular**:

- **Inserir** no fim: O(1).
- **Remover** do início: O(1).

**A diferença é enorme**: em uma cobra de 50 segmentos, a fila
comum faria 50 operações por movimento; a fila circular faz 1.

## Resumo

| Conceito | O que é |
|---|---|
| **Fila** | FIFO (primeiro a entrar, primeiro a sair) |
| **Fila circular** | Fila com número fixo de posições, onde o fim "dá a volta" para o início |
| **`% capacity`** | Faz o índice "dar a volta" (quando chega em `capacity`, volta para 0) |
| **Inserir** | `fim = (fim + 1) % capacity` |
| **Remover** | `inicio = (inicio + 1) % capacity` |
| **Percorrer** | `idx = (inicio + i) % capacity` |

**Regras**:

1. **Só use fila circular** quando o número máximo de elementos é
   **conhecido** (o `capacity` é fixo).
2. **O `% capacity` é essencial** (sem ele, o índice sai do array).
3. **A `length` pode ser menor que `capacity`** (a fila pode estar
   "vazia" em algumas posições).
4. **Para percorrer**, use `(inicio + i) % capacity` (não
   `i` diretamente).

## Onde a fila circular aparece no CCS

- **`009_snake`**: o corpo da cobra é uma fila circular.
- **Futuro**: buffers de áudio, buffers de rede (ring buffers),
  escalonadores de processos, etc.

---

arataca89@gmail.com

Aulas de programação
