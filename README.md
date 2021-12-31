# project-unification 🇧🇷 🤖⚽
Demonstração do framework de unificação de categorias de futebol de robôs, pensado para introduzir novatos na infraestrutura de software alto-nível adotado pela equipe, tendo uma grande base de funções úteis e adaptações aos casos de uso para desenvolver a cognição em software de futebol de robôs.

## Descrição
### Arquitetura

### MouseAndKeyBoard
- GoToPoint(tecla ?):
- RotatePoint(tecla ?):
- AnglePID(tecla ?):

## Dependências
- Compilador g++ 9.3 ou superior, como presente no Ubuntu >= 20.04 ou similares.
- VSCode, editor utilizado pela equipe e com plugins fornece total supporte ao projeto.

## Setup de ambiente
- Para instalar as dependências essenciais do projeto execute no terminal:
```bash
$ cd scripts
$ sudo ./setup.py --essentials
```
- Para configurar o VSCode:
  - Abra a pasta do projeto no VScode
  - Abra o terminal integrado do VSCode
  - Execute os seguintes comandos no terminal
  ```bash
  $ cd scripts
  $ ./vscode-cpp-essentials.sh
  ```
  - Ative o formmatter seguindo os passos:
   - Navegue pela interface nas opções: File > Preferences > settings
   - Busque pelo campo "Editor: Default Formatter" em settings e selecione o clang-format como formatter.
