# Implementações da disciplina de Criptografia

CEFET-RJ, UnED Petrópolis (2023.1)

## Tarefa 1: Algoritmo de Encriptação baseado no AES-128

Criar um CLI em `C` para encriptação e decriptação de arquivos utilizando as seguintes configurações:

- Método de Encriptação: `AES-128`
- Padding: `ANSI.X9.23`
- Modo de Encriptação: `PCBC`

## Tarefa 2: Criptoanálise Linear

A tarefa consiste em, dada uma tabela SBOX e um algoritmo simples, realizar as seguintes etapas:

- Gerar a tabela de _bias_.
- Encontrar as melhores expressões lineares para a chave.
- Implemente um ataque usando as expressões obtidas explorando a estatística das expressões lineares.

O algoritmo (hipotético) analisado será:

- **Data:** Mensagem $m_i$ com L bytes e a chave $(k_0, k_1, k_2, k_3)$ com 4 bytes.
- **Result:** O texto cifrado $c_i$.

```c
i = 0;
while (i < L) {
    w[i] = m[i] ^ k[i] mod 4;
    c[i] = SBOX[w[i]];
    i = i + 1;
}
```
