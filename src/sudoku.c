#include "sudoku.h"

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

typedef struct {
  size_t size;
  char** cells;
} grid_t;

const size_t MAX_GRID_SIZE = 128;

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

static int
count_non_space_characters(char* s) {
  int count = 0;
  char* str = s;
  while (*str != '\0') {
    if (*str != ' ' && *str != '\t' && *str != '\n') {
      count++;
    }
    str++;
  }
  return count;
}

static grid_t*
file_parser(char* filename) {
  FILE* file = fopen(filename, "r");

  if (file == NULL) {
    err(EXIT_FAILURE, "Error opening file: %s", filename);
    return NULL;
  }

  char first_row[MAX_GRID_SIZE];

  if (fgets(first_row, MAX_GRID_SIZE, file) == NULL) {
    warnx("warning: '%s' is empty!", filename);
    exit(EXIT_FAILURE);
  }

  // if the first row is empty line or starts with a comment ignore it and take the next line
  while (first_row[0] == '\n' || first_row[0] == '#') {
    fgets(first_row, MAX_GRID_SIZE, file) == NULL;
  }

  size_t size = count_non_space_characters(first_row);

  if (!check_grid_size(size)) {
    warnx("warning: '%s' wrong grid size: %ld!", filename, size);
    exit(EXIT_FAILURE);
  }

  // Allocate memory for the grid
  grid_t* grid = grid_alloc(size);
  grid->size = size;

  // Copy the first row to the grid
  for (size_t i = 0; i < size * 2 - 1; i += 2) {
    // Remove spaces
    if (!check_char(grid, first_row[i])) {
      warnx("warning: '%s' wrong character '%c' at line 1!", filename,
            first_row[i]);
      exit(EXIT_FAILURE);
    }
    grid->cells[0][i / 2] = first_row[i];
  }

  // Parse the rest of the file and store the values in the grid
  int row = 1;
  int col = 0;
  char c;

  while ((c = fgetc(file)) != EOF) {
    switch (c) {
      case ' ':
      case ('\t'):
        continue;

      case '#':
        // Skip the rest of the line
        while ((c = fgetc(file)) != EOF && c != '\n')
          ;
        break;

      case '\n':
        if (col > 0) {
          row++;
        }
        col = 0;
        break;

      default:
        if (!check_char(grid, c)) {
          warnx("warning: '%s' wrong character '%c' at line %d!", filename, c,
                row + 1);
          exit(EXIT_FAILURE);
        }

        if (col >= grid->size) {
          warnx(
              "warning: '%s' line '%d' is malformed! (wrong number of columns)",
              filename, row + 1);
          exit(EXIT_FAILURE);
        }

        if (row >= grid->size) {
          warnx("warning: '%s' is malformed! (wrong number of lines)",
                filename);
          exit(EXIT_FAILURE);
        }

        grid->cells[row][col] = c;
        col++;
    }
  }

  if (row < grid->size) {
    warnx("warning: grid has %d missing line(s)", filename, grid->size - row);
    exit(EXIT_FAILURE);
  }

  grid_print(grid, output);

  grid_free(grid);

  fclose(file);

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