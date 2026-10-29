# Fixtures de teste do cat

Arquivos usados para verificar o comportamento do `cat`. Diferente
do `001_wc` (que conta), o `cat` **transforma** a saída — então os
gabaritos são as **saídas esperadas por flag**, não contagens.

| Arquivo | Descrição |
|---|---|
| `teste1.txt` | Caso pequeno: linhas com conteúdo, linhas em branco, linha com tab. Termina com `\n`. |
| `sample-long.txt` | Texto longo sobre a criação do C (herdado do `001_wc`). |
| `sample-crlf.txt` | Texto sobre padronização do C, **sem `\n` final** (herdado do `001_wc`). |

## Saídas esperadas

Os gabaritos abaixo foram **medidos** rodando `cat` nos arquivos
commitados, no ambiente TDM-GCC 10.3 / Windows 11, em [data].
Nenhum valor foi calculado à mão.

### `teste1.txt`

Conteúdo (termina com `\n`):

```
linha 1 quatro palavras

linha 3 cinco palavras cinco
linha com tab:  um tab apos o ':'
```

**`cat teste1.txt`** — cópia fiel (idêntica ao conteúdo).

**`cat -n teste1.txt`**:

```
     1  linha 1 quatro palavras
     2  
     3  linha 3 cinco palavras cinco
     4  linha com tab:  um tab apos o ':'
```

**`cat -b teste1.txt`** (com `-U` se o arquivo for CRLF):

```
     1  linha 1 quatro palavras

     2  linha 3 cinco palavras cinco
     3  linha com tab:  um tab apos o ':'
```

**`cat -E teste1.txt`** (com `-U` se CRLF):

```
linha 1 quatro palavras$
$
linha 3 cinco palavras cinco$
linha com tab:  um tab apos o ':'$
```

**`cat -T teste1.txt`**:

```
linha 1 quatro palavras

linha 3 cinco palavras cinco
linha com tab:^Ium tab apos o ':'
```

**`cat -s teste1.txt`** (com `-U` se CRLF)

```
linha 1 quatro palavras

linha 3 cinco palavras cinco

linha com tab:  um tab apos o ':'
```

## Como rodar os testes

**Da raiz do projeto** (`002_cat/`):

    build\cat.exe tests\teste1.txt -n          (Windows)
    ./build/cat tests/teste1.txt -n            (Linux/macOS)

Se você estiver **dentro de `build/`**, use `..\tests\...`:

    cat ..\tests\teste1.txt -n

## Gabaritos medidos, não calculados

Os números e saídas acima foram obtidos rodando `cat` nos arquivos
commitados. Se você rodar e der diferente, o arquivo foi modificado
— ver `.gitattributes` na raiz do repositório.

**Atenção ao CRLF**: os arquivos `teste1.txt`, e
`sample-long.txt` podem estar em CRLF (Windows) ou LF, dependendo
de como foram salvos. O gabarito de `-E`, `-b` e `-s` **depende
disso**. Para os testes de transformação (`-E`, `-b`, `-s`),
recomenda-se usar `-U` junto, ou converter os fixtures para LF.
Ver `notes.md` do `002_cat` para o histórico dessa decisão.

## Por que os fixtures do `002_cat` diferem dos do `001_wc`

O `wc` conta bytes; o `cat` transforma saída. Cada projeto escolhe
fixtures que fazem sentido para **o que quer demonstrar**:

- O `wc` precisa de arquivos com tamanhos e contagens **medíveis**.
- O `cat` precisa de arquivos com **linhas em branco** (`-s`),
  **tabs** (`-T`), e **CRLF** (`-U`).

A diferença é intencional: cada projeto é autocontido.
