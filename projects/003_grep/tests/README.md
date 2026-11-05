# Fixtures de teste do grep

Arquivos usados para verificar o comportamento do `grep`. Os
gabaritos abaixo foram **medidos** rodando `grep` nos arquivos
commitados (TDM-GCC 10.3 / Windows 11, em [data]). Nenhum valor
foi calculado à mão.

## Fixtures

| Arquivo | Descrição |
|---|---|
| `compilacao.txt` | Texto sobre o processo de compilação em C, 15 linhas. Tem palavras em CAIXA ALTA (`COMPILACAO`, `ASSEMBLY`, `MONTAGEM`, `LIGACAO`, `GCC`) para testar `-i`. |
| `historia.txt` | Texto sobre a história do C, 15 linhas. Tem palavras em CAIXA ALTA (`HARDWARE`, `MARCO`, `ANSI`, `ISO`, `PYTHON`) para testar `-i`. |

**Os dois fixtures estão em CRLF (Windows)** — 14 quebras `\r\n`
cada (a linha 15 termina com `\r\n`). Isso é intencional: o
`grep` funciona com CRLF (a busca literal é robusta ao `\r` no
fim da linha), e documentar o CRLF é mais didático que converter
e fingir que ele não existe.

Se você clonar o repositório em Linux/macOS, o `.gitattributes`
com `-text` preserva o CRLF (não converte para LF). Os gabaritos
abaixo batem em qualquer plataforma.

## Gabaritos: `compilacao.txt`

| Comando | Saída esperada | Nº de linhas |
|---|---|---|
| `grep "compilacao" compilacao.txt` | `O processo de compilacao em C` | 1 |
| `grep -i "compilacao" compilacao.txt` | linha 1 + linha 6 | 2 |
| `grep -n "compilacao" compilacao.txt` | `1:O processo de compilacao em C` | 1 |
| `grep -c "compilacao" compilacao.txt` | `1` | 1 |
| `grep -v "compilacao" compilacao.txt` | 13 linhas (todas menos 1 e 6) | 13 |
| `grep -i "assembly" compilacao.txt` | linhas 6 e 14 | 2 |
| `grep -l "compilacao" compilacao.txt` | `tests\compilacao.txt` | 1 |

**Saída literal de `grep -i "compilacao" compilacao.txt`**:

```
O processo de compilacao em C
A segunda e a COMPILACAO propriamente dita, que gera ASSEMBLY.
```

**Saída literal de `grep -i "assembly" compilacao.txt`**:

```
A segunda e a COMPILACAO propriamente dita, que gera ASSEMBLY.
E -S mostra o ASSEMBLY gerado.
```

**Nota sobre o `\r`**: as linhas impressas têm um `\r` invisível
no fim (antes do `\n`). O terminal não mostra, mas ele está lá
(ver `certutil -dump` ou `Format-Hex`). Para ver a saída **sem**
`\r`, use `-U`:

    grep -U -i "compilacao" compilacao.txt

## Gabaritos: `historia.txt`

| Comando | Saída esperada | Nº de linhas |
|---|---|---|
| `grep -i "python" historia.txt` | `Hoje, C influencia C++, Java, Go, Rust e PYTHON.` | 1 |
| `grep -i "hardware" historia.txt` | `permitisse controle direto sobre o HARDWARE.` | 1 |
| `grep "C" historia.txt` | 9 linhas (título + 8) | 9 |
| `grep -c "C" historia.txt` | `9` | 9 |
| `grep -v "C" historia.txt` | 6 linhas (todas menos as 9) | 6 |

## Múltiplos arquivos

`grep "C" compilacao.txt historia.txt` — **16 linhas** (8 de
`compilacao.txt` + 8 de `historia.txt`), cada uma prefixada com o
nome do arquivo:

```
tests\compilacao.txt:O processo de compilacao em C
tests\compilacao.txt:Compilar um programa C envolve quatro etapas principais.
...
tests\historia.txt:A historia da linguagem C
tests\historia.txt:C foi criada por Dennis Ritchie nos Bell Labs, em 1972.
...
```

O prefixo (`arquivo:`) só aparece quando há **múltiplos** arquivos.
Com **um** arquivo, não há prefixo.

## Código de retorno

| Comando | `ERRORLEVEL` |
|---|---|
| `grep "compilacao" compilacao.txt` (achou) | 0 |
| `grep "python" compilacao.txt` (não achou) | 1 |
| `grep "x" nao_existe.txt` (erro) | 2 |

## Como rodar

**Da raiz do projeto** (`003_grep/`):

    build\grep.exe "compilacao" tests\compilacao.txt       (Windows)
    ./build/grep "compilacao" tests/compilacao.txt         (Linux/macOS)

## Por que os fixtures são CRLF (e não LF)

O `grep` funciona com CRLF porque a busca literal (`strstr`) acha
a substring **antes** do `\r` no fim da linha. O `\r` só
atrapalharia se o padrão incluísse o fim da linha (como `C$`, que
não temos — não há regex).

Manter CRLF é **didático**:

- O aluno vê que o `\r` existe (invisível no terminal) e que o
  `grep` funciona mesmo assim.
- O `-U` é uma **opção** para quem quer a saída sem `\r`, não
  uma obrigação.
- A lição "CRLF é um problema real, e a solução é explícita"
  fica registrada nos testes.

Se você quiser testar com arquivos LF, crie um `tests/lf.txt` com
o mesmo conteúdo (convertido no editor) e compare as saídas.
A diferença é só o `\r` invisível — os gabaritos de contagem são
os mesmos.
