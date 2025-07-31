# 🎵 Sistema de Streaming de Música

Este é um projeto de **Laboratórios de Informática III** que simula um sistema de streaming de música. O sistema processa dados de músicas, artistas, utilizadores e estatísticas, respondendo a consultas definidas. Foca-se em modularidade, encapsulamento, e boas práticas de programação em C.

## 🚀 Como Executar

### Pré-requisitos
- Sistema operacional Linux ou WSL (Windows Subsystem for Linux).
- GCC (Gnu Compiler Collection).
- Bibliotecas:
  - `glib-2.0`
  - `ncurses`

### Compilação
Clone o repositório:
```bash
git clone https://github.com/dium-li3/2425-G77.git
cd 2425-G77
```

Compile o projeto:
```bash
make release   # Compila para modo release
make debug     # Compila para modo debug
```

### Execução
- **Modo Principal**:
  ```bash
  ./programa-principal <caminho para CSVs> <arquivo de comandos>
  ```
- **Modo Interativo**:
  ```bash
  ./programa-interativo
  ```
- **Modo Testes**:
  ```bash
  ./programa-testes <caminho para CSVs> <arquivo de comandos> <diretório esperado>
  ```

### Limpeza
Para remover executáveis e objetos de compilação:
```bash
make clean
```

### Testes de Memória
Execute o programa com `valgrind` para verificar erros de memória:
```bash
make memcheck
```
Feito com ❤️ por  André Miguel Rego Trindade Pinto - A104267 - andremp33(https://github.com/andremp33) e Hélder Tiago Peixoto da Cruz - A104174 - a104174(https://github.com/a104174).
