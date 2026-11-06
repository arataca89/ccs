# Complexidade: por que O(n) importa

> Quando dizemos que um algoritmo é O(n), queremos dizer que o
> tempo que ele leva **cresce proporcionalmente ao tamanho da
> entrada**. Se a entrada dobra, o tempo dobra. Simples assim.

## O problema: "rápido" não é uma medida

Se você perguntar "esse código é rápido?", a resposta depende de
**muitas coisas**:

- A máquina (um PC de 2024 é mais rápido que um de 2004).
- O compilador (GCC com `-O2` é mais rápido que sem otimização).
- O sistema operacional (Linux vs. Windows).
- O que mais está rodando na máquina naquele momento.

Essas coisas **variam**. Não dá para comparar algoritmos com base
em "levou 3 segundos na minha máquina".

**Complexidade** é uma medida **independente de máquina**: ela
descreve **como o tempo cresce** quando a entrada cresce. Não
importa se a máquina é rápida ou lenta; o **crescimento** é o
mesmo.

## A ideia central: como o tempo cresce

Imagine que você tem uma lista de `n` números, e quer saber se um
número `x` está na lista. Duas formas:

**Forma 1: percorrer a lista** (busca linear)

```c
for (int i = 0; i < n; i++) {
    if (v[i] == x)
        return 1;
}
return 0;
```

Para cada elemento, você faz **uma comparação**. Se a lista tem
`n` elementos, você faz até `n` comparações. **O tempo cresce
proporcionalmente a `n`.**

**Forma 2: busca binária** (só se a lista estiver ordenada)

```c
int lo = 0, hi = n - 1;
while (lo <= hi) {
    int mid = (lo + hi) / 2;
    if (v[mid] == x) return 1;
    if (v[mid] < x) lo = mid + 1;
    else hi = mid - 1;
}
return 0;
```

A cada passo, você **descarta metade** da lista. Se a lista tem
`n` elementos, você faz cerca de `log₂(n)` comparações. **O tempo
cresce com o logaritmo de `n`.**

**A diferença é enorme**:

| `n` | Busca linear (n) | Busca binária (log₂ n) |
|---|---|---|
| 10 | 10 | ~4 |
| 100 | 100 | ~7 |
| 1.000 | 1.000 | ~10 |
| 1.000.000 | 1.000.000 | ~20 |

Para 1 milhão de elementos, a busca linear faz **1 milhão** de
comparações; a busca binária faz **20**. É 50.000 vezes mais
rápida. **Isso é o que a notação O captura.**

## A notação O: o que significa

`O(f(n))` (lê-se "ó de f de n") significa: **o tempo cresce, no
máximo, proporcionalmente a `f(n)`**, quando `n` é grande.

Alguns exemplos comuns, do **melhor** para o **pior**:

| Notação | Nome | Significado | Exemplo |
|---|---|---|---|
| **O(1)** | Constante | Tempo não depende de `n` | Acessar `v[i]`; inserir no início de uma lista ligada |
| **O(log n)** | Logarítmica | Tempo cresce com o log de `n` | Busca binária; `bsearch` |
| **O(n)** | Linear | Tempo cresce proporcional a `n` | Percorrer um array; busca linear; `strlen` |
| **O(n log n)** | Linearítmica | Tempo cresce com `n` × log `n` | `qsort`, `mergesort` |
| **O(n²)** | Quadrática | Tempo cresce com o quadrado de `n` | Loop duplo; `bubble sort`; comparação ingênua de strings |
| **O(n³)** | Cúbica | Tempo cresce com o cubo de `n` | Multiplicação de matrizes ingênua |
| **O(2ⁿ)** | Exponencial | Tempo dobra a cada elemento | Fibonacci recursivo ingênuo; força bruta em subconjuntos |
| **O(n!)** | Fatorial | Tempo explode | Permutações; caixeiro-viajante por força bruta |

**Regra prática**: quanto **mais devagar** a função cresce, **melhor**. `O(1)` é o melhor; `O(n!)` é o pior.

## Como identificar a complexidade (sem teoria)

Você **não precisa** de cálculo para identificar a complexidade de
um código. Basta **contar os loops** e **ver como eles dependem de `n`**.

### Regra 1: sem loop, é O(1)

```c
int x = v[5];        /* O(1) — acesso direto */
int y = a + b;       /* O(1) — operação aritmética */
```

Se o código **não tem loop** (e não chama função que tem), é
`O(1)`. O tempo não depende de `n`.

### Regra 2: um loop que vai até `n`, é O(n)

```c
for (int i = 0; i < n; i++)   /* O(n) */
    soma += v[i];
```

Um loop que percorre `n` elementos faz `n` iterações. **O(n).**

### Regra 3: dois loops aninhados, é O(n²)

```c
for (int i = 0; i < n; i++)        /* O(n) */
    for (int j = 0; j < n; j++)    /* O(n) */
        soma += v[i] * v[j];        /* total: O(n²) */
```

Loop dentro de loop: multiplica. **O(n²).**

### Regra 4: loop que divide por 2, é O(log n)

```c
while (n > 1) {      /* O(log n) */
    n = n / 2;
}
```

Se o loop **divide** o problema pela metade a cada iteração,
é `O(log n)`. (Busca binária é o exemplo clássico.)

### Regra 5: loop que chama função O(n), é O(n²) no total

```c
for (int i = 0; i < n; i++)    /* O(n) iterações */
    strlen(s);                  /* O(n) cada */
/* total: O(n²) */
```

**Cuidado**: uma função `O(n)` chamada dentro de um loop `O(n)`
dá `O(n²)`. Isso é **fácil de esquecer**.

### Regra 6: loops sequenciais (não aninhados) somam, não multiplicam

```c
for (int i = 0; i < n; i++) { ... }   /* O(n) */
for (int i = 0; i < n; i++) { ... }   /* O(n) */
/* total: O(n) + O(n) = O(2n) = O(n) */
```

Dois loops **um depois do outro** (não aninhados) somam. Como
constantes são ignoradas (`O(2n)` = `O(n)`), o total é `O(n)`.

## Como comparar duas soluções

Quando você tem duas formas de resolver o mesmo problema, **compare
a complexidade**:

| Se uma é... | e a outra é... | A melhor é... |
|---|---|---|
| O(1) | O(n) | **O(1)** (não depende de `n`) |
| O(log n) | O(n) | **O(log n)** (cresce muito mais devagar) |
| O(n) | O(n²) | **O(n)** (para `n = 1000`, é 1000 vs. 1.000.000) |
| O(n²) | O(n log n) | **O(n log n)** (para `n = 1000`, é ~10.000 vs. 1.000.000) |
| O(n log n) | O(2ⁿ) | **O(n log n)** (para `n = 30`, é ~150 vs. 1 bilhão) |

**Regra prática**: se uma solução tem complexidade **menor**, ela
é **melhor** para `n` grande. Para `n` pequeno, a diferença pode
ser irrelevante (e a solução "pior" pode até ser mais rápida, por
ter menos overhead). Mas **assintoticamente** (para `n` grande), a
de menor complexidade ganha.

## Onde a complexidade aparece no CCS

### `001_wc`: contar palavras — O(n)

```c
while ((c = fgetc(f)) != EOF) { ... }   /* O(n), n = tamanho do arquivo */
```

Percorre o arquivo uma vez. **O(n).**

### `002_cat`: copiar bytes — O(n)

```c
while ((c = fgetc(f)) != EOF) { ... }   /* O(n) */
```

Percorre o arquivo uma vez. **O(n).**

### `002_cat -s`: squeeze — O(n)

Mesmo com o `-s`, percorre o arquivo uma vez. **O(n).**

### `003_grep`: busca literal — O(n × m)

```c
while (read_line(...) >= 0) {          /* O(n) linhas */
    strstr(buf, pattern);              /* O(m) por linha, m = tamanho do padrão */
}
```

Percorre `n` linhas, e para cada linha, busca o padrão (`strstr`
é `O(tamanho_da_linha × tamanho_do_padrão)`). No pior caso,
**O(n × m)** (ou `O(n²)` se `m` for proporcional a `n`).

**Uma otimização possível** (para um projeto futuro): o algoritmo
de **Boyer-Moore** faz a busca em `O(n/m)`, muito mais rápido
para padrões longos. Mas o `strstr` é `O(n × m)`, e é suficiente
para o escopo do `003_grep`.

### `004_linked_list` (futuro): inserir no início — O(1)

```c
void list_push(struct Node **head, int value) {
    struct Node *n = node_create(value);   /* O(1) */
    n->next = *head;                       /* O(1) */
    *head = n;                             /* O(1) */
}
```

Inserir no início **não percorre a lista**. **O(1).**

### `004_linked_list` (futuro): inserir no fim — O(n)

```c
void list_append(struct Node **head, int value) {
    /* ... */
    struct Node *cur = *head;
    while (cur->next != NULL)   /* O(n) — percorre até o fim */
        cur = cur->next;
    cur->next = n;
}
```

Inserir no fim **percorre a lista**. **O(n).**

**Isso é o trade-off** da lista ligada: `push` (início) é `O(1)`,
`append` (fim) é `O(n)`. Um array é o contrário: inserir no fim é
`O(1)`, inserir no início é `O(n)`.

## O que **não** é complexidade

Complexidade **não** mede:

- **Tempo absoluto** (segundos). Mede **como o tempo cresce**.
- **Memória** (isso é "complexidade de espaço", um conceito
  separado, mas parecido).
- **Constantes**. `O(2n)` = `O(n)`. Constantes são ignoradas.
- **Pior caso vs. melhor caso**. `O(f(n))` geralmente é o **pior
  caso**. (Há também `Θ` para "exatamente", `Ω` para "no mínimo",
  mas isso é teoria que você não precisa agora.)

## Resumo em uma frase

> **Complexidade é como o tempo cresce quando a entrada cresce.**
> `O(1)` é o melhor (não cresce). `O(n)` cresce proporcionalmente.
> `O(n²)` cresce com o quadrado. `O(2ⁿ)` explode. Quanto menor,
> melhor.

## Leitura recomendada (se quiser aprofundar)

- *Algorithms in C*, Robert Sedgewick — capítulos 2 e 3 (análise
  de algoritmos).
- *Introduction to Algorithms*, Cormen et al. — capítulo 3
  (crescimento de funções), mas é denso.
- *Grokking Algorithms*, Aditya Bhargava — introdução ilustrada,
  sem matemática pesada.
