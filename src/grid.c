#include "grid.h"

#include <colors.h>

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    return NULL;
  }

  grid_t* grid = malloc(sizeof(grid_t));
  if (grid == NULL) {
    return NULL;
  }

  grid->size = size;
  grid->cells = malloc(size * sizeof(colors_t*));
  if (grid->cells == NULL) {
    return NULL;
  }

  for (size_t i = 0; i < size; i++) {
    grid->cells[i] = malloc(size * sizeof(colors_t));
    if (grid->cells[i] == NULL) {
      return NULL;
    }
  }

  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      grid->cells[i][j] = colors_full(size);
    }
  }
  return grid;
}

void
grid_free(grid_t* grid) {
  if (!grid) {
    return;
  }

  for (size_t i = 0; i < grid->size; i++) {
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

  size_t grid_size = grid->size;

  for (size_t i = 0; i < grid_size; i++) {
    for (size_t j = 0; j < grid_size; j++) {
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

static colors_t
convert_character_to_color(char character, size_t grid_size) {

  for (size_t i = 0; i < grid_size; i++) {
    if (color_table[i] == character) {
      return colors_set(i);
    }
  }
  return colors_full(grid_size);
}

static char*
convert_color_to_character(colors_t color) {

  char* string_to_return = malloc(colors_count(color) + 1);
  if (!string_to_return) {
    return NULL;
  }

  size_t index = 0;

  for (size_t i = 0; i < MAX_COLORS; i++) {
    if (colors_is_in(color, i)) {
      string_to_return[index] = color_table[i];
      index++;
    }
  }

  string_to_return[index] = '\0';

  return string_to_return;
}

char*
grid_get_cell(const grid_t* grid, const size_t row, const size_t column) {
  if (!grid || row >= grid->size || column >= grid->size) {
    return NULL;
  }

  if (grid->cells[row][column] == colors_full(grid->size)) {
    char* string_to_return = malloc(2);

    if (!string_to_return) {
      return NULL;
    }

    string_to_return[0] = EMPTY_CELL;
    string_to_return[1] = '\0';

    return string_to_return;
  }

  return convert_color_to_character(grid->cells[row][column]);
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

  grid->cells[row][column] = convert_character_to_color(color, grid->size);
}

bool
grid_is_solved(grid_t* grid) {
  for (size_t i = 0; i < grid->size; i++) {
    for (size_t j = 0; j < grid->size; j++) {
      if (!colors_is_singleton(grid->cells[i][j])) {
        return false;
      }
    }
  }
  return true;
}

static void
fill_subgrids(grid_t* grid, colors_t* subgrids[grid->size * 3][grid->size]) {
  size_t size = grid->size;
  size_t block_size = sqrt(size);
  int subgrid_count = 0;

  // Check rows
  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < size; j++) {
      subgrids[subgrid_count][j] = &(grid->cells[i][j]);
    }
    subgrid_count++;
  }

  // Check cols
  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < size; j++) {
      subgrids[subgrid_count][j] = &(grid->cells[j][i]);
    }
    subgrid_count++;
  }

  // Check blocks
  for (size_t i = 0; i < size; ++i) {
    size_t start_row = (i / block_size) * block_size;
    size_t start_col = (i % block_size) * block_size;
    size_t k = 0;
    for (size_t j = start_row; j < start_row + block_size; ++j) {
      for (size_t l = start_col; l < start_col + block_size; ++l) {
        subgrids[subgrid_count][k++] = &(grid->cells[j][l]);
      }
    }
    subgrid_count++;
  }
}

bool
grid_is_consistent(grid_t* grid) {
  size_t size = grid->size;
  colors_t* subgrids[size * 3][size];

  fill_subgrids(grid, subgrids);

  for (size_t i = 0; i < size * 3; i++) {
    if (!subgrid_consistency(subgrids[i], size)) {
      return false;
    }
  }

  return true;
}

status_t
grid_heuristics(grid_t* grid) {
  size_t size = grid->size;
  colors_t* subgrids[size * 3][size];
  bool grid_changed = true;

  fill_subgrids(grid, subgrids);

  while (grid_changed) {
    grid_changed = false;
    for (size_t i = 0; i < size * 3; i++) {
      if (subgrid_heuristics(subgrids[i], size)) {
        grid_changed = true;
      }
    }
  }

  if (!grid_is_consistent(grid)) {
    return grid_inconsistent;
  }

  if (grid_is_solved(grid)) {
    return grid_solved;
  }

  return grid_unsolved;
}

bool
grid_choice_is_empty(const choice_t choice) {
  return choice.color == colors_empty();
}

void
grid_choice_apply(grid_t* grid, const choice_t choice) {
  if (grid != NULL && choice.row < grid->size && choice.column < grid->size) {
    grid->cells[choice.row][choice.column] = choice.color;
  }
}

void
grid_choice_discard(grid_t* grid, const choice_t choice) {
  if (grid != NULL && choice.row < grid->size && choice.column < grid->size) {
    grid->cells[choice.row][choice.column] =
        colors_subtract(grid->cells[choice.row][choice.column], choice.color);
  } else {
    return;
  }
}

void
grid_choice_print(const choice_t choice, FILE* fd) {
  if (fd == NULL) {
    return;
  }
  char* color = convert_color_to_character(choice.color);
  fprintf(fd, "Choice at grid[%zu][%zu] = '%s' and choice is '%c'.\n",
          choice.row, choice.column, color, color[strlen(color) - 1]);
  free(color);
}

choice_t
grid_choice(grid_t* grid) {
  if (grid == NULL) {
    return (choice_t){0, 0, colors_empty()};
  }

  size_t min_colors_count = MAX_COLORS + 1;
  choice_t choice = {0, 0, colors_empty()};

  for (size_t row = 0; row < grid->size; ++row) {
    for (size_t column = 0; column < grid->size; ++column) {
      colors_t cell_colors = grid->cells[row][column];

      if (!colors_is_singleton(cell_colors)) {
        size_t current_count = colors_count(cell_colors);

        if (current_count > 0 && current_count < min_colors_count) {
          min_colors_count = current_count;
          choice.row = row;
          choice.column = column;
          choice.color = cell_colors;
        }
      }
    }
  }

  if (!grid_choice_is_empty(choice)) {
    choice.color = colors_rightmost(choice.color);
  }

  return choice;
}

grid_t*
grid_solver_internal(grid_t* grid, _mode_t mode, int* solution_count) {
  if (grid == NULL) {
    return NULL;
  }

  status_t status = grid_heuristics(grid);
  if (status == grid_solved) {
    if (mode == mode_all) {
      grid_print(grid, stdout);
      printf("\n");
      (*solution_count)++;
    }
    return (mode == mode_first) ? grid : NULL;
  } else if (status == grid_inconsistent) {
    return NULL;
  }

  choice_t choice = grid_choice(grid);
  if (grid_choice_is_empty(choice)) {
    return NULL;
  }

  grid_t* copy = grid_copy(grid);
  if (copy == NULL) {
    return NULL;
  }

  grid_choice_apply(copy, choice);

  grid_t* result = grid_solver_internal(copy, mode, solution_count);
  if (result != NULL && grid_is_solved(result)) {
    if (mode == mode_all) {
      grid_free(copy);
      return grid_solver_internal(grid, mode, solution_count);
    }
    grid_free(grid);
    return result;
  }

  grid_choice_discard(grid, choice);
  grid_free(copy);

  return grid_solver_internal(grid, mode, solution_count);
}

grid_t*
grid_solver(grid_t* grid, _mode_t mode) {
  int solution_count = 0;
  grid_t* result = grid_solver_internal(grid, mode, &solution_count);

  if (mode == mode_all) {
    printf("Number of solutions: %i \n", solution_count);
  }

  return result;
}