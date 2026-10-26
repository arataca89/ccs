# Guia de estilo do CCS

Regras de código para todos os projetos do CCS. O objetivo não é
"o melhor estilo do mundo" — é **consistência**. Um leitor que
abre o projeto 001 e o projeto 002 deve reconhecer o mesmo jeito
de escrever C.

## Versão da linguagem

- **C11**, com `CMAKE_C_EXTENSIONS OFF`. Nada de extensões de
  compilador (GNU, MSVC) a menos que estritamente necessário e
  documentado com `#ifdef`.
- Compilar limpo com `-Wall -Wextra -Wpedantic` (GCC/Clang) e
  `/W4` (MSVC). Warning não é sugestão.

## Formatação

- **Indentação**: 4 espaços. Não use tabs.
- **Chaves**: sempre na linha seguinte, mesmo para blocos de uma
  linha. Exceção: `struct`, `enum`, `union` podem usar chaves na
  mesma linha se forem de uma linha só.
- **Comprimento de linha**: até 80 colunas. Se passar, quebre.
- **Espaços**: um espaço depois de vírgula, nenhum antes. Um espaço
  ao redor de operadores binários (`a + b`), nenhum ao redor de
  unários (`-a`, `!b`).

```c
/* Sim */
if (x > 0) {
    y = x * 2;
}

/* Não */
if(x>0){
    y=x*2;
}