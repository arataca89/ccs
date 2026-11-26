# Cores ANSI

> Cores no terminal são feitas com **ANSI escape codes**:
> sequências de bytes que o terminal interpreta como comandos, não
> como texto. Este texto explica o que são, como funcionam, e como
> usá-las em C.

## O que são ANSI escape codes

**ANSI escape codes** são sequências de caracteres que começam com
`ESC` (`\033` ou `\x1b`) seguido de `[`, e terminam com uma letra.
O terminal **não** mostra esses caracteres; ele os **interpreta**
como comandos (mudar cor, mover cursor, limpar tela, etc.).

**Exemplo** (texto vermelho):

```
\033[31mTexto vermelho\033[0m
```

- `\033[31m`: muda a cor do texto para vermelho.
- `Texto vermelho`: o texto.
- `\033[0m`: reseta as cores (volta ao padrão).

**O que o terminal mostra**: `Texto vermelho` (em vermelho).

**O que o programa escreve**: `\033[31mTexto vermelho\033[0m` (com
os códigos).

## A estrutura de um código ANSI

```
\033[<comando>m
```

- `\033` (ou `\x1b`): o caractere `ESC` (27).
- `[`: o colchete.
- `<comando>`: o número (ou números, separados por `;`).
- `m`: a letra que indica "mudar atributo" (cor, negrito, etc.).

**Exemplo**: `\033[1;31m` (negrito + vermelho).

## Cores básicas (foreground)

| Código | Cor |
|---|---|
| `\033[30m` | Preto |
| `\033[31m` | Vermelho |
| `\033[32m` | Verde |
| `\033[33m` | Amarelo |
| `\033[34m` | Azul |
| `\033[35m` | Magenta |
| `\033[36m` | Ciano |
| `\033[37m` | Branco |
| `\033[39m` | Cor padrão (reset do foreground) |

## Cores brilhantes (foreground)

| Código | Cor |
|---|---|
| `\033[90m` | Preto brilhante (cinza) |
| `\033[91m` | Vermelho brilhante |
| `\033[92m` | Verde brilhante |
| `\033[93m` | Amarelo brilhante |
| `\033[94m` | Azul brilhante |
| `\033[95m` | Magenta brilhante |
| `\033[96m` | Ciano brilhante |
| `\033[97m` | Branco brilhante |

**Nota**: no Linux, as cores brilhantes podem parecer diferentes
(depende do tema do terminal). Em alguns terminais, o "brilhante" é
implementado como **negrito** (`\033[1m`) + a cor normal.

## Cores de fundo (background)

| Código | Cor |
|---|---|
| `\033[40m` | Preto |
| `\033[41m` | Vermelho |
| `\033[42m` | Verde |
| `\033[43m` | Amarelo |
| `\033[44m` | Azul |
| `\033[45m` | Magenta |
| `\033[46m` | Ciano |
| `\033[47m` | Branco |
| `\033[100m` a `\033[107m` | Versões brilhantes |

## Outros atributos

| Código | O que faz |
|---|---|
| `\033[0m` | Reset (volta ao padrão) |
| `\033[1m` | Negrito |
| `\033[2m` | Faint (mais claro) |
| `\033[3m` | Itálico |
| `\033[4m` | Sublinhado |
| `\033[5m` | Piscando (raro) |
| `\033[7m` | Inverso (troca foreground/background) |
| `\033[8m` | Invisível |
| `\033[9m` | Riscado |

## Como usar em C

### `printf` com `\033`

```c
printf("\033[31mTexto vermelho\033[0m\n");
```

**No Windows**, o `\033` (escape) **não** é interpretado por padrão.
Precisa habilitar o **modo VT** (Virtual Terminal) com
`SetConsoleMode`. O `enable_ansi` (no `terminal.c`) faz isso.

### `#define` para as cores

Em vez de escrever `\033[31m` em todo lugar, use `#define`:

```c
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
```

Aí:

```c
printf(COLOR_RED "Texto vermelho" COLOR_RESET "\n");
```

**Vantagem**: mais legível, e fácil de mudar.

## Limpar a tela e mover o cursor

Além de cores, os ANSI escape codes podem:

| Código | O que faz |
|---|---|
| `\033[2J` | Limpa a tela |
| `\033[H` | Move o cursor para o topo (1,1) |
| `\033[2J\033[H` | Limpa a tela **e** move o cursor para o topo |
| `\033[<linha>;<coluna>H` | Move o cursor para a posição |
| `\033[K` | Limpa do cursor até o fim da linha |

**No `snake.c`**, o `game_render` usa `\033[2J\033[H` para limpar a
tela e redesenhar o tabuleiro a cada frame.

## Portabilidade

### Linux/macOS

Funciona **nativamente** (todo terminal moderno suporta ANSI).

### Windows 10+

Funciona **se** o modo VT (Virtual Terminal) estiver habilitado.
Isso é feito com `SetConsoleMode`:

```c
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
DWORD mode = 0;
GetConsoleMode(h, &mode);
mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
SetConsoleMode(h, mode);
```

**No `snake.c`**, o `enable_ansi` (no `terminal.c`) faz isso.

### Windows antigo (7, 8)

Não suporta ANSI. Mas o CCS é para Windows 10/11.

### Redirecionamento

Se o `stdout` for **redirecionado** (ex.: `snake > log.txt`), os
códigos ANSI **não** são interpretados (o arquivo terá os bytes
`\033[31m` literalmente). **Jogos** não devem ser redirecionados
(eles precisam do terminal).

## Cores no `009_snake`

O `snake.c` usa as seguintes cores:

| Elemento | Cor | Código |
|---|---|---|
| Cabeça (`@`) | Verde brilhante | `\033[92m` |
| Corpo (`+`) | Verde | `\033[32m` |
| Comida (`*`) | Vermelho brilhante | `\033[91m` |
| Borda (`#`) | Cinza | `\033[90m` |
| Pontuação | Amarelo | `\033[93m` |
| Game over | Vermelho brilhante | `\033[91m` |

**No `snake.h`**:

```c
#define COLOR_RESET   "\033[0m"
#define COLOR_HEAD    "\033[92m"
#define COLOR_BODY    "\033[32m"
#define COLOR_FOOD    "\033[91m"
#define COLOR_WALL    "\033[90m"
#define COLOR_SCORE   "\033[93m"
```

## Cores com fundo

Para usar **fundo** colorido, combine foreground e background:

```c
printf("\033[41;97mTexto branco em fundo vermelho\033[0m\n");
```

- `41`: fundo vermelho.
- `97`: texto branco brilhante.

## Resumo

| O que | Código |
|---|---|
| **Reset** | `\033[0m` |
| **Vermelho** | `\033[31m` |
| **Verde** | `\033[32m` |
| **Amarelo** | `\033[33m` |
| **Azul** | `\033[34m` |
| **Branco** | `\033[37m` |
| **Cinza** | `\033[90m` |
| **Vermelho brilhante** | `\033[91m` |
| **Verde brilhante** | `\033[92m` |
| **Amarelo brilhante** | `\033[93m` |
| **Branco brilhante** | `\033[97m` |
| **Negrito** | `\033[1m` |
| **Limpar tela** | `\033[2J` |
| **Cursor para o topo** | `\033[H` |

**Regras**:

1. **Sempre** termine com `\033[0m` (reset), senão as cores "vazam"
   para o resto da saída.
2. **Use `#define`** para as cores (mais legível).
3. **No Windows 10+**, habilite o modo VT (`enable_ansi`).
4. **Não use ANSI** se o `stdout` for redirecionado (o arquivo
   terá os bytes literais).

## Onde isso aparece no CCS

- **`009_snake`**: as cores do tabuleiro (cabeça, corpo, comida,
  borda, pontuação, game over).
- **Futuro**: qualquer programa de terminal que queira cores
  (editores, visualizadores, jogos).

## Leitura recomendada

- ANSI escape code (Wikipedia):
  <https://en.wikipedia.org/wiki/ANSI_escape_code>
- Cores ANSI (tabela):
  <https://en.wikipedia.org/wiki/ANSI_escape_code#Colors>
- `SetConsoleMode` (Windows):
  <https://learn.microsoft.com/en-us/windows/console/setconsolemode>

---

arataca89@gmail.com

Aulas de programação
