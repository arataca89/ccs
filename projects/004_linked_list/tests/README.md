# Fixtures de teste do list

Arquivos usados para verificar o comportamento do `list`. Os
gabaritos abaixo foram **medidos** rodando `list` nos arquivos
commitados (TDM-GCC 10.3 / Windows 11, em 08/11/2026). Nenhum valor
foi calculado à mão.

## Fixtures

| Arquivo | Descrição |
|---|---|
| `comandos.txt` | Sessão completa, sem erros. Testa `push`, `append`, `print`, `size`, `remove`, `find`, `free`, `quit`. |
| `comandos_erros.txt` | Comandos inválidos. Testa mensagens de erro (argumento faltando, lixo, comando desconhecido). O programa **não** aborta. |

## Gabaritos: `comandos.txt`

Comando:

```cmd
build\list.exe < tests\comandos.txt
```

Saída esperada:

```
30
20
10
40
50
5
removido: 20
30
10
40
50
4
encontrado: 40
nao encontrado: 99
lista liberada
0
```

**Explicação**:

| Comando | Efeito |
|---|---|
| `push 10`, `push 20`, `push 30` | Lista `30, 20, 10` (push insere no início) |
| `append 40`, `append 50` | Lista `30, 20, 10, 40, 50` |
| `print` | Imprime os 5 valores |
| `size` | `5` |
| `remove 20` | `removido: 20`, lista `30, 10, 40, 50` |
| `print` | 4 valores |
| `size` | `4` |
| `find 40` | `encontrado: 40` |
| `find 99` | `nao encontrado: 99` |
| `free` | `lista liberada`, lista vazia |
| `print` | (nada — lista vazia) |
| `size` | `0` |
| `quit` | Sai |

**Nota**: `push` insere no **início**, `append` no **fim**. Por
isso a ordem `30, 20, 10, 40, 50` (push inverte, append mantém).

## Gabaritos: `comandos_erros.txt`

Comando:

```cmd
build\list.exe < tests\comandos_erros.txt > saida.txt 2>&1
type saida.txt
```

**Atenção ao `2>&1`**: as mensagens de erro vão para **stderr**.
Sem o `2>&1`, elas aparecem no terminal mas **não** no arquivo
`saida.txt`. Para capturar tudo (como o usuário vê), use `2>&1`.

Saída esperada:

```
list: push precisa de um inteiro
list: push precisa de um inteiro
list: push precisa de um inteiro
list: append precisa de um inteiro
list: remove precisa de um inteiro
list: find precisa de um inteiro
list: comando desconhecido: 'xyz'
Tente 'help' para ajuda.
10
```

**Explicação**:

| Comando | Erro |
|---|---|
| `push` (sem argumento) | `arg == NULL` → "push precisa de um inteiro" |
| `push abc` | `parse_int("abc")` falha → "push precisa de um inteiro" |
| `push 10 extra` | `parse_int("10 extra")` falha (lixo depois) → "push precisa de um inteiro" |
| `append` (sem argumento) | "append precisa de um inteiro" |
| `remove` (sem argumento) | "remove precisa de um inteiro" |
| `find` (sem argumento) | "find precisa de um inteiro" |
| `xyz` | comando desconhecido + sugestão de `help` |
| `push 10` | OK (insere 10) |
| `print` | `10` |
| `quit` | Sai |

**O programa não aborta** em erros. Continua lendo o próximo
comando. Isso é mais amigável que sair com erro (o usuário pode
corrigir e continuar).

## Como rodar

**Da raiz do projeto** (`004_linked_list/`):

    build\list.exe < tests\comandos.txt                  (Windows)
    ./build/list < tests/comandos.txt                    (Linux/macOS)

**Modo interativo** (sem redirecionamento — o prompt `> ` aparece):

    build\list.exe
    > push 10
    > print
    10
    > quit

## Nota sobre CRLF

Os fixtures estão em **CRLF** (Windows), criados no Sublime Text.
O `strip_newline` remove `\n` e `\r` do final da linha, então
funciona em CRLF e LF. Se você converter para LF, os gabaritos
**não mudam** (o `strip_newline` remove os dois).

## Nota sobre stdout vs. stderr

As mensagens de erro (`list: ...`) vão para **stderr**. As
mensagens de sucesso (`removido: ...`, `encontrado: ...`, os
valores da lista) vão para **stdout**.

Isso é convenção Unix: stdout é para a **saída normal**, stderr
para **erros**. Scripts podem redirecionar um sem o outro
(`list < comandos.txt > saida.txt` captura só o sucesso; os
erros aparecem no terminal).

Para capturar **tudo** (como o usuário vê), use `2>&1`.

---

arataca89@gmail.com

Aulas de programação
