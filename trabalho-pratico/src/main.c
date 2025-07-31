#include "../include/utils.h"
#include "../include/interactive.h"
#include "../include/Queries/querie6.h"
#include "../include/testRunner.h"
#include "../include/progRunner.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>

#ifdef DEBUG_FLAG
const char *debug_flag = DEBUG_FLAG;
#else
const char *debug_flag = "disabled";
#endif

int main(int argc, char *argv[]) {
    printf("Debug flag is: %s\n", debug_flag);

    if (argc == 3 && strcmp(argv[0], "./programa-principal") == 0) {
        const char *csv_path = argv[1];
        const char *commands_file = argv[2];

        #ifdef DEBUG_FLAG
        printf("Running programa-principal\nCSV Path: %s\nCommands File: %s\n", csv_path, commands_file);
        #endif

        run_program_mode(csv_path, commands_file);

    } else if (argc == 4 && strcmp(argv[0], "./programa-testes") == 0) {
        const char *csv_path = argv[1];
        const char *commands_file = argv[2];
        const char *expected_output_dir = argv[3];

        #ifdef DEBUG_FLAG
        printf("Running programa-testes\nCSV Path: %s\nCommands File: %s\nExpected Output Dir: %s\n",
               csv_path, commands_file, expected_output_dir);
        #endif

        run_test_mode(csv_path, commands_file, expected_output_dir);

    } else if (argc == 1 && strcmp(argv[0], "./programa-interativo") == 0) {
        setbuf(stdout, NULL);  // Ensure output is immediately flushed

        #ifdef DEBUG_FLAG
        printf("Running programa-interativo\n");
        #endif

        run_interactive_mode();

    } else {
        printf("Usage:\n");
        printf("  ./programa-principal <CSV path> <commands file>\n");
        printf("  ./programa-testes <CSV path> <commands file> <expected output dir>\n");
        printf("  ./programa-interativo\n");
        return 1;
    }

    return 0;
}
