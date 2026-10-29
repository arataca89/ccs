# 002_cat: concatenar e transformar arquivos

> Reinvenção didática de um dos utilitários mais antigos do Unix (1971),
> com as flags que tornam o `cat` uma ferramenta de inspeção de texto.

## O que faz

Lê um ou mais arquivos (ou a entrada padrão) e escreve o conteúdo na
saída padrão, aplicando transformações opcionais:

- **`-n`**: numera todas as linhas.
- **`-b`**: numera apenas linhas não vazias (sobrepõe `-n`).
- **`-E`**: mostra `$` no fim de cada linha.
- **`-T`**: mostra tabs como `^I`.
- **`-s`**: colapsa linhas em branco múltiplas em uma.
- **`-U`**: converte quebras `\r\n` (CRLF) em `\n` na saída.

Sem argumentos, mostra a ajuda. Com `-`, lê de stdin explicitamente.

```
$ cat -nE texto.txt
     1	primeira linha$
     2	$
     3	terceira linha$
```

## Onde isso é usado na vida real

- É um dos primeiros utilitários do Unix (v1, 1971), escrito por
  Dennis Ritchie e Ken Thompson.
- É a ferramenta canônica para **inspecionar texto**: ver fim de
  linha (`-E`), tabs (`-T`), numeração (`-n`), colapsar brancos
  (`-s`).
- Aparece em scripts de build, pipelines de processamento de texto,
  debugging de arquivos de configuração.
- A ideia "transformar um stream de bytes em outro" é a base de
  `sed`, `awk`, `tr`, e de toda a filosofia Unix de ferramentas
  pequenas que se compõem.
- Praticamente todo sistema Unix-like (Linux, macOS, BSDs) traz
  uma versão em C.

## Conceitos de C abordados

- Leitura e escrita de bytes (`fgetc`/`fputc`).
- **Parsing manual de flags** (`argv` caractere por caractere,
  flags agrupadas como `-nE`).
- **Máquina de estados** para `-s` (squeeze) e para o controle de
  início de linha.
- **Lookahead de 1 caractere** para `-U` (distinguir `\r\n` de
  `\r` sozinho).
- **Estado pendente após o loop** (o `\r` no fim do arquivo).
- Composição de transformações em **pipeline** (a ordem das flags
  importa).
- `const` como contrato (`const CatOptions *opts`).
- Tratamento de erro (`ferror`, `fopen` retornando `NULL`,
  `fputc` retornando `EOF`).
- Portabilidade: `#ifdef _WIN32` isolado, modo binário.

## Como compilar

Requer CMake ≥ 3.16 e um compilador C11 (GCC, Clang ou MSVC).

**Linux/macOS**:

    cmake -S . -B build
    cmake --build build

**Windows com TDM-GCC**:

    cmake -S . -B build -G "MinGW Makefiles"
    cmake --build build

O executável fica em `build/cat` (Linux/macOS) ou `build/cat.exe`
(Windows).

## Como executar

**Da raiz do projeto** (`002_cat/`):

    build\cat.exe tests\teste1.txt          (Windows)
    ./build/cat tests/teste1.txt            (Linux/macOS)

**Com flags**:

    build\cat.exe -n tests\teste1.txt
    build\cat.exe -nE tests\teste1.txt
    build\cat.exe -Ub tests\teste2.txt

**Lendo da entrada padrão**:

    echo ola mundo | build\cat.exe          (Windows)
    echo ola mundo | ./build/cat            (Linux/macOS)

**Múltiplos arquivos** (concatena na ordem):

    build\cat.exe tests\teste1.txt tests\teste2.txt

**Ajuda**:

    build\cat.exe -h

## Flags

| Flag | O que faz | Observações |
|---|---|---|
| `-n` | Numera todas as linhas | Formato `%6lu\t`, como o `cat` POSIX |
| `-b` | Numera só linhas não vazias | Sobrepõe `-n` |
| `-E` | Mostra `$` no fim de cada linha | Útil para ver `\n` |
| `-T` | Mostra tabs como `^I` | Útil para ver `\t` |
| `-s` | Colapsa linhas em branco múltiplas | Uma linha em branco entre parágrafos |
| `-U` | Converte `\r\n` em `\n` na saída | Ver nota sobre CRLF abaixo |
| `-h` | Mostra ajuda e sai | — |
| `-` | Lê de stdin | Como no `cat` POSIX |

Flags podem ser agrupadas (`-nE` é o mesmo que `-n -E`) ou
separadas (`-n -E`). A ordem entre elas não importa (exceto `-b`
sobrepor `-n`, que é regra final).

## Nota sobre arquivos CRLF (Windows)

O `cat` copia bytes fielmente. Se o arquivo tem quebras `\r\n`
(Windows), o `\r` é preservado — inclusive com `-E`, `-b` e `-s`.
Em um terminal, `\r` faz o cursor voltar ao início da linha, o que
pode dar a impressão de que o `$` do `-E` está no lugar errado, ou
que `-b`/`-s` não funcionam. **Isso não é bug**: é o comportamento
do `cat` POSIX.

Se você quer tratar CRLF como LF, use `-U` junto com essas flags:

    cat -UE arquivo.txt    →  $ no fim de cada linha (correto)
    cat -Ub arquivo.txt    →  numera só linhas não vazias
    cat -Us arquivo.txt    →  colapsa linhas em branco corretamente

Ou converta o arquivo antes com `dos2unix`.

Ver [notes.md](notes.md#o-caso-do--no-lugar-errado-ou-como-um-bug-de-plataforma-virou-lição)
para o histórico completo dessa decisão, incluindo o diagnóstico
com `certutil -dump`.

## Sem argumentos: mostra ajuda (decisão consciente)

O `cat` POSIX, sem argumentos, lê de stdin. Nós escolhemos mostrar
a ajuda, porque `Ctrl+Z` (Windows) e `Ctrl+D` (Linux/macOS) são
pouco conhecidos. **Isso é uma decisão de design**, documentada no
help e no `notes.md`.

## Testes

Ver [tests/README.md](tests/README.md). Os fixtures cobrem:

- `teste1.txt`: caso com múltiplas linhas em branco e tab (para
  `-s` e `-T`).

Gabaritos medidos, não calculados — ver `notes.md` do `001_wc`
para a regra geral.

## Notas de implementação

Ver [notes.md](notes.md). Cobre o histórico de desenvolvimento:
os bugs encontrados (o `$` no lugar errado, o `-s` que não
colapsava), as soluções consideradas, as escolhas feitas e por quê.

## Conceitos relacionados

- [Máquina de estados](../../docs/state-machine.md) — usada em
  `-s` (squeeze) e no controle de início de linha.
- [Lookahead](../../docs/lookahead.md) — usada em `-U` (CRLF).

## Referências

- POSIX `cat`: <https://pubs.opengroup.org/onlinepubs/9699919799/utilities/cat.html>
- GNU coreutils `cat`: <https://www.gnu.org/software/coreutils/manual/html_node/cat-invocation.html>
- História do `cat`: <https://en.wikipedia.org/wiki/Cat_(Unix)>
