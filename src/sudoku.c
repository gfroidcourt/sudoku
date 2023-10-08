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
    perror("Error opening file");
    return NULL;
  }

  char first_row[MAX_GRID_SIZE];
  size_t idx = 0;
  char ch;

  while ((ch = fgetc(file)) != EOF && ch != '\n') {
    if (ch == '#') { // Ignore the rest of the line after '#'
      while ((ch = fgetc(file)) != EOF && ch != '\n')
        ;
      break;
    }
    if (ch != ' ' && ch != '\t') {
      if (idx >= MAX_GRID_SIZE) {
        fprintf(stderr, "Error: Row has too many columns.\n");
        fclose(file);
        return NULL;
      }
      first_row[idx++] = ch;
    }
  }

  grid_t* grid = grid_alloc(idx);

  if (!grid) {
    fprintf(stderr, "Error allocating memory for grid.\n");
    fclose(file);
    return NULL;
  }
  for (size_t i = 0; i < idx; i++) {
    if (!grid_check_char(grid, first_row[i])) {
      fprintf(stderr, "Error: Invalid character '%c' in grid.\n", first_row[i]);
      grid_free(grid);
      fclose(file);
      return NULL;
    }
    grid_set_cell(grid, 0, i, first_row[i]);
  }

  size_t row_count = 1;
  while ((ch = fgetc(file)) != EOF) {
    if (ch == '\n') {
      row_count++;
    }
  }

  if (row_count != grid_get_size(grid)) {
    fprintf(stderr, "Error: Grid does not have the right number of rows.\n");
    grid_free(grid);
    fclose(file);
    return NULL;
  }

  fclose(file);
  grid_print(grid, stdout); // Display the parsed grid
  return grid;
}

int
main(int argc, char* argv[]) {
  int optc;
  bool all = false;
  bool unique = false;
  bool generate = false;
  int result;

  const struct option long_options[] = {
      {"help", no_argument, NULL, 'h'},
      {"all", no_argument, NULL, 'a'},
      {"version", no_argument, NULL, 'V'},
      {"generate", optional_argument, NULL, 'g'},
      {"unique", no_argument, NULL, 'u'},
      {"output", required_argument, NULL, 'o'},
      {"verbose", no_argument, NULL, 'v'}};

  output = stdout;
  optc = getopt_long(argc, argv, "havg::uo:V", long_options, NULL);
  char* program_name = basename(argv[0]);

  while (optc != -1) {
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
        exit(EXIT_SUCCESS);

      case 'o':
        output = fopen(optarg, "w");
        if (output == NULL) {
          err(EXIT_FAILURE, "Error opening file: %s", optarg);
        }
        break;

      case 'a':
        if (generate) {
          warnx("warning: option 'all' conflict with generator mode, "
                "disabling it !");
        }
        all = true;
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
        if (!check_grid_size(atoi(optarg))) {
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

  if (output != stdout) {
    fclose(output);
  }

  for (int i = optind; i < argc; ++i) {
    grid_t* grid = file_parser(argv[i]);
  }
}