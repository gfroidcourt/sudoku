#include "grid.h"

#include <colors.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Internat structure (hidden from outside) for a sudoku grid*/
struct _grid_t {
  size_t size;
  colors_t** cells;
};

static bool
grid_check_char(const grid_t* grid, const char c) {
  switch (grid->size) {
    case 1:
      return (c == '1' || c == '_');
    case 4:
      return (c >= '1' && c <= '4') || c == '_';
    case 9:
      return (c >= '1' && c <= '9') || c == '_';
    case 16:
      return ((c >= '1' && c <= '9') || (c >= 'A' && c <= 'G')) || c == '_';
    case 25:
      return ((c >= '1' && c <= '9') || (c >= 'A' && c <= 'P')) || c == '_';
    case 36:
      return ((c >= '1' && c <= '9') || (c >= 'A' && c <= 'Z')) || c == '_'
             || c == '@';
    case 49:
      return ((c >= '1' && c <= '9') || (c >= 'A' && c <= 'Z') || c == '@')
             || (c >= 'a' && c <= 'm') || c == '_';
    case 64:
      return ((c >= '1' && c <= '9') || (c >= 'A' && c <= 'Z')
              || (c >= 'a' && c <= 'z') || c == '@' || c == '&' || c == '*')
             || c == '_';
    default:
      return false;
  }
}

static void
grid_free(grid_t* grid) {
  for (size_t i = 0; i < grid->size; ++i) {
    free(grid->cells[i]);
  }
  free(grid->cells);
  free(grid);
}

static void
grid_print(const grid_t* grid, FILE* fd) {
  for (size_t i = 0; i < grid->size; ++i) {
    for (size_t j = 0; j < grid->size; ++j) {
      fprintf(fd, "%c", grid->cells[i][j]);
      if (j == grid->size - 1) {
        continue;
      }
      fprintf(fd, "%c", ' ');
    }
    fprintf(fd, "\n");
  }
}

bool
check_grid_size(size_t size) {
  switch (size) {
    case 1:
    case 4:
    case 9:
    case 16:
    case 25:
    case 36:
    case 49:
    case 64:
      return true;
    default:
      return false;
  }
}