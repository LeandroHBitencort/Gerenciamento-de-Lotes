# Sistema de Gerenciamento de Lotes em C

Sistema desenvolvido em C para importação, armazenamento e g erenciamento de dados a partir de arquivos externos.

## Funcionalidades
- Importação de dados via arquivos .txt
- Armazenamento em arquivo .csv
- Controle de IDs persistentes
- Exclusão de registros por lote
- Listagem de arquivos disponíveis

## Conceitos aplicados
- Manipulação de arquivos (leitura e escrita)
- Alocação dinâmica de memória
- Estruturas (struct)
- Processamento de dados externos
- Controle de estado com arquivos auxiliares

## Como funciona
O sistema permite importar arquivos contendo dados, armazenados em um arquivo CSV que simula um banco de dados simples e gerenciar os registros posteriormente.

## Observações
- Os dados são armazenados no arquivo `dados.csv`
- O controle de IDs é feito pelo arquivo `last_id.id`

## Como executar

1. Compile o código:
   gcc nome_do_arquivo.c -o programa

2. Execute:
   ./programa

3. Certifique-se de ter arquivos `.txt` no diretório para importação

## Autores

Projeto desenvolvido em grupo para atividade acadêmica.

- Leandro Bitencort
- Arnaldo Goulart
- Jafe Vinicius
- Joãoo Victor Haddad
- Matheus Henrique Parila
- Matheus Mendes
