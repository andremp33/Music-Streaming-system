#ifndef TESTRUNNER_H
#define TESTRUNNER_H


/**
 * @brief Compara os resultados com os resultados-esperados.
 */
void compare_output_files(const char *generated_file, const char *expected_file, int *correct_count, int *first_incorrect_line);

/**
 * @brief Inicializa e executa o programa de testes.
 */
void run_test_mode(const char *csv_path, const char *commands_file, const char *expected_output_dir);

#endif