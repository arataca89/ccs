# Testes do getopt (demo)

O `005_getopt` não tem fixtures de arquivo. Os testes são
**comandos de linha**, e a saída esperada é o que o `demo`
imprime. Os gabaritos abaixo foram **medidos** rodando `demo`
(TDM-GCC 10.3 / Windows 11, em 10/11/2026. Nenhum valor foi
calculado à mão.

## Como rodar

**Da raiz do projeto** (`005_getopt/`):

    build\demo.exe -v                      (Windows)
    ./build/demo -v                        (Linux/macOS)

## Gabaritos

### Sem flags

```cmd
build\demo.exe
```

Saída:

```
verbose = 0
output  = (nenhum)
arquivos: (nenhum)
```

### `-v` (uma vez)

```cmd
build\demo.exe -v
```

Saída:

```
verbose = 1
output  = (nenhum)
arquivos: (nenhum)
```

### `-vv` (repetida)

```cmd
build\demo.exe -vv
```

Saída:

```
verbose = 2
output  = (nenhum)
arquivos: (nenhum)
```

**Por que 2**: `-vv` é `-v -v` (flags agrupadas). Cada `v`
incrementa `verbose`. O `getopt` processa os dois caracteres.

### `-o` com argumento separado

```cmd
build\demo.exe -o saida.txt
```

Saída:

```
verbose = 0
output  = saida.txt
arquivos: (nenhum)
```

**Por que `saida.txt`**: o `optarg` é o próximo argumento
(`argv[optind]`), porque o `-o` espera argumento (tem `:` na
`optstring`).

### `-o` com argumento colado

```cmd
build\demo.exe -osaida.txt
```

Saída:

```
verbose = 0
output  = saida.txt
arquivos: (nenhum)
```

**Por que funciona**: o `getopt` vê `-osaida.txt`, processa o
`o`, e como `*nextchar != '\0'` (ainda tem `saida.txt`), usa o
resto como `optarg`.

### Flags agrupadas com argumento

```cmd
build\demo.exe -vo saida.txt arquivo1 arquivo2
```

Saída:

```
verbose = 1
output  = saida.txt
arquivos:
  arquivo1
  arquivo2
```

**Por que**: `-vo` é `-v` + `-o`. O `v` não espera argumento
(retorna `'v'`). O `o` espera, e como não há nada colado
(`*nextchar == '\0'`), pega o próximo argumento (`saida.txt`).
Depois, `optind` aponta para `arquivo1`, e o loop de arquivos
imprime `arquivo1` e `arquivo2`.

### `--` (para o parsing)

```cmd
build\demo.exe -v -- arquivo1
```

Saída:

```
verbose = 1
output  = (nenhum)
arquivos:
  arquivo1
```

**Por que**: o `--` para o parsing de flags. O `getopt` retorna
`-1`, e `optind` aponta para `arquivo1` (o `--` foi consumido,
mas não é flag).

### `-h` (help)

```cmd
build\demo.exe -h
```

Saída: o texto do help (ver `print_help` no `main.c`). Como é
longo, não vou colar aqui. O importante: mostra as opções, os
exemplos, e a nota sobre flags agrupadas e `--`.

### `-x` (flag inválida)

```cmd
build\demo.exe -x
```

Saída (stderr):

```
build\demo.exe: opcao invalida -- 'x'
```

E (stdout):

```
Tente 'build\demo.exe -h' para ajuda.
```

**Exit code**: 1 (erro).

**Por que a mensagem vem do `getopt`**: o `getopt` imprime a
mensagem de erro (porque `opterr != 0`). O `main` adiciona a
sugestão de `-h` e retorna 1.

**Para silenciar a mensagem do `getopt`**: setar `opterr = 0`
antes do loop. O `notes.md` registra.

## Nota sobre `opterr`

O `getopt` imprime mensagens de erro por padrão (`opterr = 1`).
Se o chamador quiser silenciar (e imprimir a sua própria
mensagem), seta `opterr = 0` antes do loop:

```c
opterr = 0;
while ((opt = getopt(...)) != -1) { ... }
```

**No `demo`**, não setamos `opterr = 0`, então o `getopt` imprime
a mensagem de erro. O `main` adiciona a sugestão de `-h`.

## Nota sobre thread-safety

O `getopt` usa variáveis **globais** (`optarg`, `optind`,
`optopt`, `opterr`). Isso **não é thread-safe**: duas threads
chamando `getopt` ao mesmo tempo corrompem o estado.

**Para o escopo do `005`**, não há threads, então não é problema.
Mas o [notes.md](../notes.md) registra a limitação, e menciona a alternativa
(struct de estado em vez de globais) como melhoria futura.

