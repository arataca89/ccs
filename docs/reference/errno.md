# `<errno.h>` — códigos de erro

Macros para códigos de erro de funções da libc. Usado
indiretamente (mencionado no `004` e `005` como melhoria
futura).

## Variável

### `errno`

**Header**: `<errno.h>`

**O que é**: uma **variável global** (thread-local) que é setada
por funções da libc quando ocorre um erro. O valor é um código
de erro (ex.: `ERANGE`, `EINVAL`).

**Onde usamos**: **não usamos** ainda (mas o `notes.md` do `004`
e `005` menciona como melhoria futura para `strtol`).

**Cuidados**: `errno` **não é resetado** automaticamente. Antes
de chamar a função, faça `errno = 0`. Depois, verifique se
mudou. **Não** use `errno` para verificar sucesso de funções que
não a setam (ex.: `printf`).

**Ver também**: [`ERANGE`](#erange), [`strtol`](stdlib.md#strtol).

## Códigos

### `ERANGE`

**Header**: `<errno.h>`

**O que é**: código de erro para **resultado fora do intervalo**
(overflow/underflow). `strtol` seta `errno = ERANGE` quando o
valor não cabe em `long`.

**Onde usamos**: **não usamos** ainda.

**Cuidados**: para `strtol`, verifique `errno == ERANGE` **e**
os limites de `int` (`INT_MIN`, `INT_MAX` em `<limits.h>`), se
o resultado for armazenado em `int`.

**Ver também**: [`errno`](#errno), [`strtol`](stdlib.md#strtol).

---

arataca89@gmail.com

Aulas de programação
