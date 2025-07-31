#ifndef PROGRUNNER_H
#define PROGRUNNER_H

/**
 * @brief Processa um comando específico e executa a query correspondente.
 * @param command String contendo o comando a ser processado.
 * @param storage Estrutura de armazenamento contendo todas as tabelas de dados.
 * @param command_number Número do comando, usado para nomear os ficheiros de saída.
 */
void process_command(const char *command, STORAGE *storage, int command_number);

/**
 * @brief Executa o modo principal do programa, processando comandos a partir de um ficheiro.
 * @param csv_path Caminho para o diretório contendo os ficheiros CSV de entrada.
 * @param commands_file Caminho para o ficheiro de comandos a ser processado.
 */
void run_program_mode(const char *csv_path, const char *commands_file);

#endif
