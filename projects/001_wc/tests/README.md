# Fixtures de teste do wc

| Arquivo | Linhas | Palavras | Bytes |
|---|---|---|---|
| `teste1.txt` | 3 | 9 | 57 |
| `sample-long.txt` | 27 | 194 | 1244 |
| `sample-crlf.txt` | 25 | 187 | 1179 |

## Gabaritos medidos

Os números acima foram obtidos rodando `wc` nos arquivos commitados
(TDM-GCC 10.3 / Windows 11, em [data]). Se você rodar e der diferente,
o arquivo foi modificado — ver `.gitattributes` na raiz do repositório.

## Como rodar os testes

**Da raiz do projeto** (`001_wc/`):

    build\wc.exe tests\teste1.txt          (Windows, TDM-GCC)
    ./build/wc tests/teste1.txt            (Linux/macOS)

Se você estiver **dentro de `build/`**, use `..\tests\...`:

    wc ..\tests\teste1.txt

## Por que `sample-crlf.txt` reporta uma linha a menos

O arquivo termina sem `\n` na última linha. O `wc` conta quebras de
linha, não "linhas visuais". Comportamento idêntico ao `wc` POSIX.
