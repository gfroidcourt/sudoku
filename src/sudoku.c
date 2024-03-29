#include "sudoku.h"

#include "grid.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <err.h>
#include <getopt.h>
#include <libgen.h>
#include <string.h>

static bool verbose = false;
static FILE* output;

static void
print_help(char* executable_name) {
  printf("Usage:\t%s [a|-o FILE|-v|-V|-h] FILE...\n"
         "\tsudoku -g[SIZE] [-u|-o FILE|-v|-V|-h]\n"
         "Solve or generate Sudoku grids of size: 1, 4, 9, 16, 25, 36, 49, 64\n"
         "\n"
         "-a,--all\t\tsearch for all possible solutions\n"
         "-g[N],--generate[SIZE]\tgenerate a grid of size NxN (default:9)\n"
         "-o FILE,--output=FILE\twrite output to FILE\n"
         "-u,--unique\t\tgenerate a grid with unique solution\n"
         "-v,--verbose\t\tverbose output\n"
         "-V,--version\t\tdisplay version and exit\n"
         "-h,--help\t\tdisplay this help and exit\n",
         executable_name);

  exit(EXIT_SUCCESS);
}

static grid_t*
file_parser(char* filename) {
  FILE* file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Error opening file \"%s\".\n", filename);
    exit(EXIT_FAILURE);
  }

  char row[MAX_GRID_SIZE];
  size_t index = 0;
  int c;

  grid_t* grid = NULL;
  size_t expected_row_length = 0;
  size_t row_count = 0;

  while (true) {
    c = fgetc(file);

    switch (c) {
      case '#':
        while ((c = fgetc(file)) != '\n' && c != EOF)
          ;
        break;

      case ' ':
      case '\t':
        break;

      case '\n':
      case EOF:
        if (index > 0) {
          if (row_count == 0) {
            grid = grid_alloc(index);

            if (!grid) {
              fprintf(stderr, "Error allocating memory for grid.\n");
              fclose(file);
              exit(EXIT_FAILURE);
            }

            expected_row_length = index;
          } else if (index != expected_row_length) {
            fprintf(stderr,
                    "Line %zu is malformed! (wrong number of columns)\n",
                    row_count + 1);
            grid_free(grid);
            fclose(file);
            exit(EXIT_FAILURE);
          }

          for (size_t i = 0; i < index; i++) {
            if (!grid_check_char(grid, row[i])) {
              fprintf(stderr, "Error: Invalid character '%c' on line %zu.\n",
                      row[i], row_count + 1);
              grid_free(grid);
              fclose(file);
              exit(EXIT_FAILURE);
            }
            grid_set_cell(grid, row_count, i, row[i]);
          }

          row_count++;
          index = 0;
        }

        if (c == EOF) {
          goto end_of_file;
        }

        break;

      default:
        if (index >= MAX_GRID_SIZE) {
          fprintf(stderr, "Error: Row on line %zu has too many columns.\n",
                  row_count + 1);
          fclose(file);
          exit(EXIT_FAILURE);
        }
        row[index++] = c;
    }
  }

end_of_file:

  if (!grid || row_count != grid_get_size(grid)) {
    fprintf(stderr, "Error: Incomplete or extra rows in grid.\n");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  fclose(file);
  return grid;
}

int
main(int argc, char* argv[]) {
  int optc;
  bool unique = false;
  bool generate = false;
  int result;
  char* filename = NULL;
  _mode_t mode = mode_first;

  const struct option options[] = {{"help", no_argument, NULL, 'h'},
                                   {"all", no_argument, NULL, 'a'},
                                   {"version", no_argument, NULL, 'V'},
                                   {"generate", optional_argument, NULL, 'g'},
                                   {"unique", no_argument, NULL, 'u'},
                                   {"output", required_argument, NULL, 'o'},
                                   {"verbose", no_argument, NULL, 'v'},
                                   {NULL, 0, NULL, 0}};

  output = stdout;

  char* program_name = basename(argv[0]);

  while ((optc = getopt_long(argc, argv, "havg::uo:V", options, NULL)) != -1) {
    switch (optc) {
      case 'h':
        print_help(program_name);
        break;

      case 'V':
        printf("%s version %d.%d.%d\n"
               "Solve/generate sudoku grids of size: 1, 4, 9, 16, 25, "
               "36, 49, 64\n",
               program_name, VERSION, SUBVERSION, REVISION);
        exit(EXIT_SUCCESS);

      case 'v':
        verbose = true;
        break;

      case 'o':
        filename = optarg;

        if (output == NULL) {
          err(EXIT_FAILURE, "Error opening file: %s", optarg);
        }
        break;

      case 'a':
        if (generate) {
          warnx("warning: option 'all' conflict with generator mode, "
                "disabling it !");
        }
        mode = mode_all;
        printf("search for all possible solutions\n");
        break;

      case 'u':
        if (!generate) {
          warnx("warning: option 'unique' conflict with solver mode, "
                "disabling it !");
        }
        unique = true;
        printf("generate a grid with unique solution\n");
        break;

      case 'g':
        generate = true;

        if (optarg == NULL) {
          printf("generate grid of size 9x9\n");
          exit(EXIT_SUCCESS);
        }

        if (!grid_check_size(atoi(optarg))) {
          errx(EXIT_FAILURE, "error: invalid grid size: %s", optarg);
        }

        printf("generate grid of size %dx%d\n", atoi(optarg), atoi(optarg));
        exit(EXIT_SUCCESS);
        break;

      default:
        errx(EXIT_FAILURE, "error: invalid option: %s", optarg);
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Error: no input file specified.\n");
    fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (filename != NULL) {
    output = fopen(filename, "w");
    if (output == NULL) {
      err(EXIT_FAILURE, "Error opening file: %s", filename);
    }
  }

  for (int i = optind; i < argc; ++i) {
    grid_t* grid = file_parser(argv[i]);

    grid_t* new_grid = grid_solver(grid, mode);

    if (new_grid == NULL && mode == mode_first) {
      fprintf(stderr, "Error: no solution found for grid %s \n", argv[i]);
      grid_free(grid);
      return EXIT_FAILURE;
    }
    grid_print(new_grid, output);
    grid_free(grid);
  }

  if (output != stdout) {
    fclose(output);
  }

  return EXIT_SUCCESS;
}