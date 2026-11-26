# Testes do snake

O `009_snake` é um jogo, então os testes são **comandos de linha**
(e interação com o teclado), não há fixtures de arquivo. Os
gabaritos abaixo foram **medidos** rodando `snake` (TDM-GCC 10.3 /
GCC 16.x / Windows 11 e Linux, em 25/11/2026. Nenhum valor foi
calculado à mão.

## Como rodar

**Da raiz do projeto** (`009_snake/`):

    build\snake.exe [opcoes]        (Windows)
    ./build/snake [opcoes]          (Linux/macOS)

## Gabaritos

### `-h` (help)

```cmd
build\snake.exe -h
```

Saída: o texto do help (ver `print_help` no `main.c`). Mostra os
níveis, as opções, os controles, e exemplos.

### Nível fácil (padrão)

```cmd
build\snake.exe
```

Saída esperada:

- Título "CCS SNAKE" (verde brilhante).
- Linha de parâmetros: `Tabuleiro: NxN | Alvo: 19 | Delay: 500 ms`.
- Tabuleiro com bordas (`#`), cobra (`@` + `+`), comida (`*`).
- Score: `Score: 0 / 19`.

**N**: o tamanho máximo do terminal (100% do máximo).

### Nível médio

```cmd
build\snake.exe -m
```

Saída esperada:

- Título "CCS SNAKE".
- Linha de parâmetros: `Tabuleiro: NxN | Alvo: 39 | Delay: 300 ms`.
- Score: `Score: 0 / 39`.

**N**: 70% do máximo do terminal.

### Nível difícil

```cmd
build\snake.exe -d
```

Saída esperada:

- Título "CCS SNAKE".
- Linha de parâmetros: `Tabuleiro: NxN | Alvo: 59 | Delay: 150 ms`.
- Score: `Score: 0 / 59`.

**N**: 50% do máximo do terminal.

### Flag `-s` (tamanho)

```cmd
build\snake.exe -s 20
```

Saída esperada:

- Título "CCS SNAKE".
- Linha de parâmetros: `Tabuleiro: 20x20 | Alvo: 19 | Delay: 500 ms`.
- Tabuleiro 20×20.

**Nota**: se o terminal não suportar 20×20, o jogo limita ao
máximo e avisa.

### Flag `-t` (alvo)

```cmd
build\snake.exe -t 50
```

Saída esperada:

- Título "CCS SNAKE".
- Linha de parâmetros: `Tabuleiro: NxN | Alvo: 49 | Delay: 500 ms`.
- Score: `Score: 0 / 49`.

**Nota**: se o alvo for maior que o tabuleiro, o jogo avisa e sai.

### Flag `-v` (delay)

```cmd
build\snake.exe -v 200
```

Saída esperada:

- Título "CCS SNAKE".
- Linha de parâmetros: `Tabuleiro: NxN | Alvo: 19 | Delay: 200 ms`.

**Nota**: o delay menor deixa o jogo mais rápido.

### Terminal muito pequeno

Se o terminal for menor que `MIN_SIZE + 6` linhas (ou colunas), o
jogo **não inicia**:

```cmd
build\snake.exe
```

Saída (stderr):

```
snake: terminal muito pequeno (LxC).
Minimo: 16x16.
```

**Exit code**: 1 (erro).

### Alvo maior que o tabuleiro

```cmd
build\snake.exe -s 10 -t 200
```

Saída (stderr):

```
snake: alvo 200 e muito grande para tabuleiro 10x10.
Maximo: 36 comidas.
```

**Exit code**: 1 (erro).

### Flag inválida

```cmd
build\snake.exe -x
```

Saída (stderr):

```
snake: argumento invalido: '-x'
Tente 'snake -h' para ajuda.
```

**Exit code**: 1 (erro).

## Testes manuais (interação)

Os testes acima verificam a **saída inicial** (título, parâmetros,
tabuleiro). Para testar a **jogabilidade**, é preciso interagir:

### Movimento

1. Rode `build\snake.exe` (nível fácil).
2. Aperte as **setas** (ou WASD). A cobra deve mudar de direção.
3. Se a cobra bater na parede, deve aparecer "GAME OVER!".
4. Se a cobra comer a comida, o score deve aumentar.

### Vitória

1. Rode `build\snake.exe -t 5` (alvo 5, mais fácil de vencer).
2. Coma 4 comidas (score 4 / 4).
3. Deve aparecer "VOCE VENCEU!".

### `Q` (sair)

1. Rode `build\snake.exe`.
2. Aperte `Q`. O jogo deve sair (com o score final).

## Nota sobre `_setmode` (Windows)

O `snake` usa `_setmode` (no `main.c`) para configurar
`stdin`/`stdout` como **binários**. Sem isso, o Windows traduz
`\r\n` para `\n`, e o tabuleiro pode ficar "estranho".

**Isso é o mesmo problema do `001_wc`, `002_cat`, `003_grep`,
`008_rle`**.

## Nota sobre as setas

As setas funcionam **diferente** em cada plataforma:

- **Windows**: enviam 2 bytes (`0` ou `0xE0` + código). O
  `read_key` lê os dois e mapeia para `KEY_*`.
- **Linux/macOS**: enviam 3 bytes (`ESC` + `[` + letra). O
  `read_key` lê os três e mapeia para `KEY_*`.

**Teste**: rode o jogo e aperte as setas. A cobra deve mudar de
direção.

## Nota sobre o `usleep` (Linux/macOS)

O `009_snake` usa `CMAKE_C_STANDARD 99` + `CMAKE_C_EXTENSIONS ON`
(por causa do `usleep`). Ver [notes.md](notes.md) para a
justificativa.

## Como rodar

**Da raiz do projeto** (`009_snake/`):

    build\snake.exe [opcoes]        (Windows)
    ./build/snake [opcoes]          (Linux/macOS)

---

arataca89@gmail.com

Aulas de programação
