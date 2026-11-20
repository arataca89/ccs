# Padrões de C

Padrões que programadores C experientes usam, mas que os livros
didáticos raramente citam. Cada padrão tem: **o que é**, **por
que é bom**, **exemplo**, e **onde usamos no CCS**.

O arquivo **cresce** conforme o CCS usa novos padrões.

## Índice

1. [`if (chamada() != 0)` — verificação de erro inline](#1-if-chamada--0--verificação-de-erro-inline)
2. [Retorno por ponteiro](#2-retorno-por-ponteiro)
3. [Operador ternário](#3-operador-ternário)

## 1. `if (chamada() != 0)` — verificação de erro inline

### O que é

Chamar uma função e **verificar o retorno na mesma expressão**,
tomando as providências imediatamente se falhar.

```c
if (fseek(in, (long)opts->skip, SEEK_SET) != 0) {
    fprintf(stderr, "hexdump: nao foi possivel pular %lu bytes\n",
            opts->skip);
    return -1;
}
```

### Por que é bom

1. **Combina a chamada e a verificação** numa expressão só. Não
   precisa de uma variável temporária.
2. **O erro é tratado imediatamente**, no mesmo bloco. Não fica
   "esquecido" para depois.
3. **É idiomático**: programadores C experientes escrevem assim
   (kernel do Linux, libc, PostgreSQL, etc.).

### Quando usar

Quando a função retorna **0 em sucesso** e **não-zero em erro**
(como `fseek`, `fclose`, `fread` com verificação de tamanho,
`fwrite`, `fprintf`).

### Variantes

**Para funções que retornam ponteiro** (`malloc`, `fopen`), o
padrão é `if (!ptr) { ... }`:

```c
FILE *in = fopen(path, "rb");
if (!in) {
    fprintf(stderr, "%s: %s: nao foi possivel abrir\n", argv[0], path);
    return 1;
}
```

**Para funções que retornam `-1` em erro** (como `read`), o
padrão é `if (ret == -1) { ... }`.

### O que os livros não citam

Muitos livros mostram:

```c
int ret = fseek(in, offset, SEEK_SET);
if (ret != 0) {
    ...
}
```

Isso é **equivalente**, mas mais verboso. O padrão inline é mais
comum em código real.

### Onde usamos no CCS

- `006_hexdump`: `if (fseek(...) != 0) { ... }`.
- (futuros projetos, conforme formos usando).

## 2. Retorno por ponteiro

### O que é

Uma função que retorna **dois valores**: o status (sucesso/erro)
e o resultado. O status volta pelo `return`; o resultado volta
por um **parâmetro de ponteiro** (o "parâmetro de saída").

```c
static int parse_ulong(const char *s, unsigned long *out)
{
    ...
    *out = v;   /* resultado vai por *out */
    return 0;   /* status vai pelo return */
}
```

### Por que é bom

1. **C não tem retorno múltiplo** (diferente de Python, Go, etc.).
   O padrão permite "retornar" dois valores.
2. **O status é separado do resultado**. O chamador pode verificar
   o status **antes** de usar o resultado.
3. **É idiomático**: `strtol`, `fread`, `scanf`, e muitas funções
   da libc usam esse padrão.

### Como usar

**A função**:

```c
int funcao(params, tipo *out)
{
    if (erro)
        return -1;   /* status: erro */
    *out = resultado;   /* resultado: por ponteiro */
    return 0;   /* status: sucesso */
}
```

**O chamador**:

```c
unsigned long valor;
if (parse_ulong("42", &valor) != 0) {
    /* erro */
}
/* valor tem o resultado */
```

### Quando usar

Quando a função precisa retornar **status + resultado**, e o
resultado não cabe no `return` (ou o `return` já é usado para o
status).

### Alternativas

- **Struct de retorno**: `struct Result { int status; unsigned
  long value; };`. Mais verboso, mas mais explícito. Usado em
  projetos maiores.
- **`errno`**: a função retorna o valor, e seta `errno` em erro.
  Usado por `strtol`, `malloc` (em alguns casos), etc. Mas
  `errno` é global (não thread-safe), e o chamador precisa
  lembrar de verificar.

### Onde usamos no CCS

- `003_grep`: `read_line(FILE *f, char **buf, size_t *cap, ...)`
  — o status vai pelo `return`, o buffer vai por `**buf` e
  `*cap`.
- `004_linked_list`: `parse_int(const char *s, int *out)`.
- `005_getopt`: `flag_expects_arg(...)` retorna o status, e o
  `optarg` é global.
- `006_hexdump`: `parse_ulong(const char *s, unsigned long *out)`.

## 3. Operador ternário

### O que é

Uma **expressão condicional** que retorna um de dois valores,
dependendo de uma condição:

```c
condicao ? valor_se_verdadeiro : valor_se_falso
```

É como um `if/else`, mas **como expressão** (produz um valor).

### Exemplo

```c
return (rc == 0) ? 0 : 1;
```

Isso é equivalente a:

```c
if (rc == 0)
    return 0;
else
    return 1;
```

Mas o ternário é **mais compacto**, e pode ser usado **dentro de
uma expressão** (ex.: `printf("%s\n", cond ? "sim" : "nao")`).

### Por que é bom

1. **Concisão**: para casos simples (`return cond ? a : b`), é
   mais legível que um `if/else` de 4 linhas.
2. **Expressão**: pode ser usado onde um `if` não pode (dentro
   de `printf`, de atribuição, etc.).
3. **É idiomático**: programadores C usam ternário para casos
   simples.

### Quando **não** usar

- **Condições complexas**: se a condição tem `&&`/`||` demais,
  o ternário fica ilegível.
- **Efeitos colaterais**: `cond ? f() : g()` — as duas funções
  são chamadas? **Não**, só uma. Mas se você não sabe disso, o
  código fica confuso.
- **Aninhamento**: `a ? b : c ? d : e` é **ilegível**. Evite.

**Regra prática**: use ternário para **casos simples** (`return
cond ? a : b`, `x = cond ? a : b`). Para casos complexos, use
`if/else`.

### Onde usamos no CCS

- `006_hexdump`: `return (rc == 0) ? 0 : 1;` e
  `fputc(is_printable(c) ? c : '.', out);`.
- (futuros projetos).

---

arataca89@gmail.com

Aulas de programação
