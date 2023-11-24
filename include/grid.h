#ifndef GRID_H
#define GRID_H

#include "colors.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define MAX_GRID_SIZE 64
#define EMPTY_CELL    '_'

static const char color_table[] = "123456789"
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "@"
                                  "abcdefghijklmnopqrstuvwxyz"
                                  "&*";

/* Sudoku grid (forward declaration to hide the implementation)*/
typedef struct _grid_t grid_t;

typedef enum { grid_solved, grid_unsolved, grid_inconsistent } status_t;

typedef enum { mode_first, mode_all } _mode_t;

typedef struct {
  size_t row;
  size_t column;
  colors_t color;
} choice_t;

/**
 * @brief Allocates memory for a new grid of the specified size.
 *
 * @param size The size of the grid to be allocated.
 * @return A pointer to the newly allocated grid, or NULL on failure.
 */
grid_t* grid_alloc(size_t size);

/**
 * @brief Frees the memory allocated for the provided grid.
 *
 * @param grid The grid to be freed.
 */
void grid_free(grid_t* grid);

/**
 * @brief Prints the content of the provided grid to the specified file
 * descriptor.
 *
 * @param grid The grid to be printed.
 * @param fd The file descriptor where the grid should be printed.
 */
void grid_print(const grid_t* grid, FILE* fd);

/**
 * @brief Checks if the provided size is valid for a Sudoku grid.
 *
 * @param size The size to be checked.
 * @return true if the size is valid, false otherwise.
 */
bool grid_check_size(const size_t size);

/**
 * @brief Checks if the provided character is valid for the specified grid.
 *
 * @param grid The grid in context.
 * @param c The character to be checked.
 * @return true if the character is valid for the grid, false otherwise.
 */
bool grid_check_char(const grid_t* grid, const char c);

/**
 * @brief Creates a deep copy of the provided grid.
 *
 * @param grid The grid to be copied.
 * @return A new grid that is a deep copy of the provided grid, or NULL on
 * failure.
 */
grid_t* grid_copy(const grid_t* grid);

/**
 * @brief Retrieves the content of a specific cell as a string.
 *
 * @param grid The grid from which to retrieve the cell content.
 * @param row The row index of the cell.
 * @param column The column index of the cell.
 * @return A string representing the content of the cell.
 * The caller is responsible for freeing this string.
 */
char* grid_get_cell(const grid_t* grid, const size_t row, const size_t column);

/**
 * @brief Retrieves the size of the provided grid.
 *
 * @param grid The grid whose size is to be retrieved.
 * @return The size of the grid.
 */
size_t grid_get_size(const grid_t* grid);

/**
 * @brief Sets the content of a specific cell to a given color.
 *
 * @param grid The grid in which to set the cell content.
 * @param row The row index of the cell.
 * @param column The column index of the cell.
 * @param color The color to set in the cell.
 */
void grid_set_cell(grid_t* grid, const size_t row, const size_t column,
                   const char color);

/**
 * @brief Checks if the given grid is solved.
 *
 * @param grid The grid to check.
 * @return true if the grid is solved, false otherwise.
 */
bool grid_is_solved(grid_t* grid);

/**
 * @brief Checks if the given grid is consistent.
 *
 * @param grid The grid to check.
 * @return true if the grid is consistent, false otherwise.
 */
bool grid_is_consistent(grid_t* grid);

/**
 * @brief Applies heuristics to the given grid.
 *
 * @param grid The grid to apply heuristics.
 * @return status_t indicating the status of the operation.
 */
status_t grid_heuristics(grid_t* grid);

/**
 * @brief Checks if the given choice is empty.
 *
 * @param choice The choice to check.
 * @return true if the choice is empty, false otherwise.
 */
bool grid_choice_is_empty(const choice_t choice);

/**
 * @brief Applies the given choice to the grid.
 *
 * @param grid The grid to apply the choice.
 * @param choice The choice to apply.
 */
void grid_choice_apply(grid_t* grid, const choice_t choice);

/**
 * @brief Discards the given choice from the grid.
 *
 * @param grid The grid to discard the choice.
 * @param choice The choice to discard.
 */
void grid_choice_discard(grid_t* grid, const choice_t choice);

/**
 * @brief Prints the given choice to the specified file.
 *
 * @param choice The choice to print.
 * @param fd The file to print the choice.
 */
void grid_choice_print(const choice_t choice, FILE* fd);

/**
 * @brief Solves the given grid using the specified mode.
 *
 * @param grid The grid to solve.
 * @param mode The mode to use for solving.
 * @return A pointer to the solved grid.
 */
grid_t* grid_solver(grid_t* grid, _mode_t mode);

#endif /* GRID_H */
