#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>

// Define the maximum color number
#define MAX_COLORS 64

// Define colors_t as uint64_t
typedef uint64_t colors_t;

/**
 * @brief Returns a set of colors with all bits set up to the specified size.
 * 
 * @param size The number of colors to include in the set.
 * @return A set of colors with the specified number of bits set.
 */
colors_t colors_full(const size_t size);

/**
 * @brief Returns an empty set of colors.
 * 
 * @return A set of colors with all bits set to 0.
 */
colors_t colors_empty(void);

/**
 * @brief Returns a set of colors with only the specified color set.
 * 
 * @param color_id The index of the color to set.
 * @return A set of colors with only the specified color set.
 */
colors_t color_set(const size_t color_id);

/**
 * @brief Adds a specified color to a set of colors.
 * 
 * @param colors The original set of colors.
 * @param color_id The index of the color to add.
 * @return A new set of colors with the specified color added.
 */
colors_t colors_add(const colors_t colors, const size_t color_id);

/**
 * @brief Removes a specified color from a set of colors.
 * 
 * @param colors The original set of colors.
 * @param color_id The index of the color to remove.
 * @return A new set of colors with the specified color removed.
 */
colors_t colors_discard(const colors_t colors, const size_t color_id);

/**
 * @brief Checks if a specified color is present in a set of colors.
 * 
 * @param colors The set of colors to check.
 * @param color_id The index of the color to check for.
 * @return true if the color is present, false otherwise.
 */
bool colors_is_in(const colors_t colors, const size_t color_id);

/**
 * @brief Returns the complement of a set of colors.
 * 
 * @param colors The set of colors to negate.
 * @return The complement of the set of colors.
 */
colors_t colors_negate(const colors_t colors);

/**
 * @brief Computes the intersection of two sets of colors.
 * 
 * @param colors1 The first set of colors.
 * @param colors2 The second set of colors.
 * @return A new set of colors representing the intersection of the two input sets.
 */
colors_t colors_and(const colors_t colors1, const colors_t colors2);

/**
 * @brief Computes the union of two sets of colors.
 * 
 * @param colors1 The first set of colors.
 * @param colors2 The second set of colors.
 * @return A new set of colors representing the union of the two input sets.
 */
colors_t colors_or(const colors_t colors1, const colors_t colors2);

/**
 * @brief Computes the exclusive union of two sets of colors.
 * 
 * @param colors1 The first set of colors.
 * @param colors2 The second set of colors.
 * @return A new set of colors representing the exclusive union of the two input sets.
 */
colors_t colors_xor(const colors_t colors1, const colors_t colors2);

/**
 * @brief Computes the difference between two sets of colors.
 * 
 * @param colors1 The set of colors from which colors2 will be subtracted.
 * @param colors2 The set of colors to subtract from colors1.
 * @return A new set of colors representing the difference between the two input sets.
 */
colors_t colors_subtract(const colors_t colors1, const colors_t colors2);

/**
 * @brief Checks if two sets of colors are equal.
 * 
 * @param colors1 The first set of colors.
 * @param colors2 The second set of colors.
 * @return true if the two sets of colors are equal, false otherwise.
 */
bool colors_is_equal(const colors_t colors1, const colors_t colors2);

/**
 * @brief Checks if the first set of colors is a subset of the second set.
 * 
 * @param colors1 The set of colors to check.
 * @param colors2 The reference set.
 * @return true if colors1 is a subset of colors2, false otherwise.
 */
bool colors_is_subset(const colors_t colors1, const colors_t colors2);

/**
 * @brief Checks if a set of colors contains only one color.
 * 
 * @param colors The set of colors to check.
 * @return true if the set contains only one color, false otherwise.
 */
bool colors_is_singleton(const colors_t colors);

/**
 * @brief Counts the number of colors in a set.
 * 
 * @param colors The set of colors to count.
 * @return The number of colors in the set.
 */
size_t colors_count(const colors_t colors);

/**
 * @brief Retrieves the rightmost color in a set.
 * 
 * @param colors The set of colors to check.
 * @return The rightmost color in the set.
 */
colors_t colors_rightmost(const colors_t colors);

/**
 * @brief Retrieves the leftmost color in a set.
 * 
 * @param colors The set of colors to check.
 * @return The leftmost color in the set.
 */
colors_t colors_leftmost(const colors_t colors);

/**
 * @brief Returns a random color from a set.
 * 
 * @param colors The set of colors to choose from.
 * @return A randomly chosen color from the set.
 */
colors_t colors_random(const colors_t colors);

#endif /* COLORS_H */
