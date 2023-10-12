#ifndef COLORS_H
#define COLORS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Define the maximum color number
#define MAX_COLORS 64

// Define colors_t as uint64_t
typedef uint64_t colors_t;

/**
 * @brief Set to '1' all bits within range from 0 to size and 'O' all others.
 * 
 * @param size The number of colors to include in the set.
 * @return A set of colors with all bits set to 1.
 */
colors_t colors_full(const size_t size);

/**
 * @return Simply retun 0ULL. 
 */
colors_t colors_empty(void);

/**
 * @brief Set to '1' the color encoded at the index color_id, all others are '0'.
 * 
 * @param color_id The index of the color to set.
 * @return A set of colors with only the specified color set to 1.
 */
colors_t colors_set(const size_t color_id);

/**
 * @brief Set the given color index to '1' in colors.
 * 
 * @param colors The original set of colors.
 * @param color_id The index of the color to add.
 * @return A new set of colors with the specified color added.
 */
colors_t colors_add(const colors_t colors, const size_t color_id);

/**
 * @brief Set the given color index to '0' in colors and return it.
 * 
 * @param colors The original set of colors.
 * @param color_id The index of the color to remove.
 * @return A new set of colors with the specified color removed. 
 */
colors_t colors_discard(const colors_t colors, const size_t color_id);

/**
 * @brief Check if the color index is set to '1' or not. 
 * 
 * @param colors The set of colors to check.
 * @param color_id The index of the color to check for.
 * @return true if the color is set to '1', false otherwise.
 */
bool colors_is_in(const colors_t colors, const size_t color_id);

/**
 * @brief Bitwise negate the colors_t and return it
 * 
 * @param colors The set of colors to negate.
 * @return A new set of colors with all bits flipped.
 */
colors_t colors_negate(const colors_t colors);

/**
 * @brief Compute the intersection between two colors_t.
 * 
 * @param colors1 The first set of colors.
 * @param colors2 The second set of colors.
 * @return A set of colors representing the intersection of the two input sets.
 */
colors_t colors_and(const colors_t colors1, const colors_t colors2);

/**
 * @brief Compute the union between two colors_t.
 * 
 * @param colors1 The first set of colors.
 * @param colors2 The second set of colors.
 * @return A new set of colors representing the union of the two input sets.
 */
colors_t colors_or(const colors_t colors1, const colors_t colors2);

/**
 * @brief Compute the XOR of two colors_t.
 * 
 * @param colors1 The first set of colors.
 * @param colors2 The second set of colors.
 * @return A new set of colors with the 
 */
colors_t colors_xor(const colors_t colors1, const colors_t colors2);

/**
 * @brief Compute substraction of two sets of colors. 
 * 
 * @param colors1 The set of colors from which colors2 will be subtracted.
 * @param colors2 The set of colors to subtract from colors1.
 * @return A new set of colors representing the difference between the two input sets.
 */
colors_t colors_subtract(const colors_t colors1, const colors_t colors2);

/**
 * @brief Check the equality of two colors_t.
 * 
 * @param colors1 The first set of colors.
 * @param colors2 The second set of colors.
 * @return true if the two sets of colors are equal, false otherwise.
 */
bool colors_is_equal(const colors_t colors1, const colors_t colors2);

/**
 * @brief Test the inclusion of colors1 in colors2.
 * 
 * @param colors1 The set of colors to check.
 * @param colors2 The reference set.
 * @return true if colors1 is a subset of colors2, false otherwise.
 */
bool colors_is_subset(const colors_t colors1, const colors_t colors2);

/**
 * @brief Check if there is only one color in colors.
 * 
 * @param colors The set of colors to check.
 * @return true if the set contains only one color, false otherwise.
 */
bool colors_is_singleton(const colors_t colors);

/**
 * @brief Count the number of colors enclosed in the set.
 * 
 * @param colors The set of colors to count.
 * @return The number of colors enclosed in the set.
 */
size_t colors_count(const colors_t colors);

/**
 * @brief Retrieves the rightmost color in a set.
 * 
 * @param colors The set of colors to check.
 * @return The rightmost color in the set (least significant bit).
 */
colors_t colors_rightmost(const colors_t colors);

/**
 * @brief Retrieves the leftmost color in a set.
 * 
 * @param colors The set of colors to check.
 * @return The leftmost color in the set (most significant bit).
 */
colors_t colors_leftmost(const colors_t colors);

/**
 * @brief Pick up a random color in the set.
 * 
 * @param colors The set of colors to choose from.
 * @return A randomly chosen color from the set if not empty, 0 otherwise.
 */
colors_t colors_random(const colors_t colors);

#endif /* COLORS_H */
