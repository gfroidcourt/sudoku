#include "grid.h"

#include <colors.h>

#include <string.h>

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Internat structure (hidden from outside) for a sudoku grid*/
struct _grid_t {
  size_t size;
  colors_t** cells;
};

bool
grid_check_char(const grid_t* grid, const char c) {
  if (!grid) {
    return false;
  }

  switch (grid->size) {
    case 64:
      return (c >= '1' && c <= '9') || (c >= 'A' && c <= 'Z') || c == '@'
             || (c >= 'a' && c <= 'z') || c == '&' || c == '*' || c == '_';
    case 49:
      return (c >= '1' && c <= '9') || (c >= 'A' && c <= 'Z') || c == '@'
             || (c >= 'a' && c <= 'm') || c == '_';
    case 36:
      return (c >= '1' && c <= '9') || (c >= 'A' && c <= 'Z') || c == '@'
             || c == '_';
    case 25:
      return (c >= '1' && c <= '9') || (c >= 'A' && c <= 'P') || c == '_';
    case 16:
      return (c >= '1' && c <= '9') || (c >= 'A' && c <= 'G') || c == '_';
    case 9:
      return (c >= '1' && c <= '9') || c == '_';
    case 4:
      return (c >= '1' && c <= '4') || c == '_';
    case 1:
      return c == '1' || c == '_';
    default:
      return false;
  }
}

grid_t*
grid_alloc(size_t size) {
  if (size == 0 || !grid_check_size(size)) {
    fprintf(stderr, "Invalid grid size: %zu\n", size);
    return NULL;
  }

  grid_t* grid = malloc(sizeof(grid_t));
  if (grid == NULL) {
    return NULL;
  }

  grid->size = size;
  grid->cells = malloc(size * sizeof(colors_t*));
  if (grid->cells && NULL) {
    return NULL;
  }

  for (size_t i = 0; i < size; ++i) {
    grid->cells[i] = malloc(size * sizeof(colors_t));
    if (grid->cells == NULL) {
      return NULL;
    }
  }

  return grid;
}

void
grid_free(grid_t* grid) {
  if (!grid) {
    return;
  }

  for (size_t i = 0; i < grid->size; ++i) {
    free(grid->cells[i]);
  }

  free(grid->cells);
  free(grid);
}

void
grid_print(const grid_t* grid, FILE* fd) {
  if (!grid || !fd) {
    return;
  }

  for (size_t i = 0; i < grid_get_size(grid); i++) {
    for (size_t j = 0; j < grid_get_size(grid); j++) {
      char* cell_content = grid_get_cell(grid, i, j);
      if (cell_content) {
        fprintf(fd, "%s ", cell_content);
        free(cell_content);
      } else {
        fprintf(fd, "%c ", EMPTY_CELL);
      }
    }
    fprintf(fd, "\n");
  }
}

bool
grid_check_size(const size_t size) {
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

grid_t*
grid_copy(const grid_t* grid) {
  if (!grid) {
    return NULL;
  }

  grid_t* new_grid = grid_alloc(grid->size);

  if (!new_grid) {
    return NULL;
  }

  for (size_t i = 0; i < grid->size; i++) {
    for (size_t j = 0; j < grid->size; j++) {
      new_grid->cells[i][j] = grid->cells[i][j];
    }
  }

  return new_grid;
}

char*
grid_get_cell(const grid_t* grid, const size_t row, const size_t column) {
  if (!grid || row >= grid->size || column >= grid->size) {
    return NULL;
  }

  char cell_content = grid->cells[row][column];

  char* cell_str = (char*)malloc(2 * sizeof(char));
  if (!cell_str) {
    return NULL;
  }

  cell_str[0] = cell_content;
  cell_str[1] = '\0';

  return cell_str;
}

size_t
grid_get_size(const grid_t* grid) {
  if (!grid) {
    return 0;
  }
  return grid->size;
}

void
grid_set_cell(grid_t* grid, const size_t row, const size_t column,
              const char color) {
  if (!grid || row >= grid->size || column >= grid->size) {
    return;
  }

  grid->cells[row][column] = color;
}

bool
subgrid_apply(grid_t* grid,
              bool (*func)(colors_t subgrid[], const size_t size)) {
  size_t size = grid_get_size(grid);
  colors_t subgrid[size];

  // Apply func to each row
  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < size; ++j) {
      subgrid[j] = grid_get_cell(grid, i, j);
    }
    if (!func(subgrid, size)) {
      return false;
    }
  }

  // Apply func to each column
  for (size_t j = 0; j < size; ++j) {
    for (size_t i = 0; i < size; ++i) {
      subgrid[i] = grid_get_cell(grid, i, j);
    }
    if (!func(subgrid, size)) {
      return false;
    }
  }

  // Apply func to each block
  size_t block_size = sqrt(size);
  for (size_t bi = 0; bi < block_size; ++bi) {
    for (size_t bj = 0; bj < block_size; ++bj) {
      size_t index = 0;
      for (size_t i = 0; i < block_size; ++i) {
        for (size_t j = 0; j < block_size; ++j) {
          subgrid[index++] =
              grid_get_cell(grid, bi * block_size + i, bj * block_size + j);
        }
      }
      if (!func(subgrid, size)) {
        return false;
      }
    }
  }

  return true;
}

bool
grid_is_solved(grid_t* grid) {
  return subgrid_apply(grid, &subgrid_is_solved);
}

bool
grid_is_consistent(grid_t* grid) {
  return subgrid_apply(grid, &subgrid_consistency);
}