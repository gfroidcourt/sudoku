#include "colors.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // for random()

colors_t
colors_full(const size_t size) {
  if (size > MAX_COLORS) {
    // Handle the case where size is greater than MAX_COLORS
    return (colors_t)-1; // All bits set to 1
  } else {
    return (1ULL << size) - 1; // Set bits 0 to (size-1) to 1
  }
}

colors_t
colors_empty(void) {
  return 0ULL; // All bits set to 0
}

colors_t
color_set(const size_t color_id) {
  if (color_id >= MAX_COLORS) {
    // Handle the case where color_id exceeds MAX_COLORS
    return (colors_t)-1; // All bits set to 1
  } else {
    return 1ULL << color_id; // Set the bit at color_id to 1
  }
}

colors_t
colors_add(const colors_t colors, const size_t color_id) {
  if (color_id >= MAX_COLORS) {
    // Handle the case where color_id exceeds MAX_COLORS
    return colors;
  } else {
    return colors | (1ULL << color_id); // Set the bit at color_id to 1
  }
}

colors_t
colors_discard(const colors_t colors, const size_t color_id) {
  if (color_id >= MAX_COLORS) {
    // If color_id is out of bounds, just return the original colors without modification
    return colors;
  } else {
    // Use bitwise AND with the negation of the shifted value to set the specific bit to 0
    return colors & ~(1ULL << color_id);
  }
}

bool
colors_is_in(const colors_t colors, const size_t color_id) {
  if (color_id >= MAX_COLORS) {
    // If color_id is out of bounds, return false
    return false;
  } else {
    // Use bitwise AND with the shifted value to isolate the specific bit
    // Then, compare it to the shifted value to check if the bit is set to 1
    return (colors & (1ULL << color_id)) == (1ULL << color_id);
  }
}

colors_t
colors_negate(const colors_t colors) {
  // Use bitwise NOT to compute the complement of colors
  return ~colors;
}

colors_t
colors_and(const colors_t colors1, const colors_t colors2) {
  // Use bitwise AND to compute the intersection of colors1 and colors2
  return colors1 & colors2;
}

colors_t
colors_or(const colors_t colors1, const colors_t colors2) {
  return colors1 | colors2;
}

colors_t
colors_xor(const colors_t colors1, const colors_t colors2) {
  return colors1 ^ colors2;
}

colors_t
colors_subtract(const colors_t colors1, const colors_t colors2) {
  // colors1\colors2 = colors1 & ~colors2
  return colors1 & ~colors2;
}

bool
colors_is_equal(const colors_t colors1, const colors_t colors2) {
  return colors1 == colors2;
}

bool
colors_is_subset(const colors_t colors1, const colors_t colors2) {
  // If colors1 is a subset of colors2, then the intersection of
  // colors1 and colors2(using bitwise AND) should be equal to colors1.
  return (colors1 & colors2) == colors1;
}

bool
colors_is_singleton(const colors_t colors) {
  // Check if colors is non-zero and only one bit is set to 1
  return colors && !(colors & (colors - 1));
}

size_t
colors_count(const colors_t colors) {
  size_t count = 0;
  colors_t temp = colors;

  while (temp) {
    temp &= (temp - 1); // unset the rightmost set bit
    count++;
  }

  return count;
}

colors_t
colors_rightmost(const colors_t colors) {
  return colors & (-colors);
}

colors_t
colors_leftmost(const colors_t colors) {
  if (colors == 0) {
    return 0;
  }

  colors_t temp = colors;
  while (temp > 1) {
    temp >>= 1;
  }

  return temp;
}

colors_t
colors_random(const colors_t colors) {
  size_t count = colors_count(colors);
  if (count == 0) {
    return 0; // Return empty set if colors is empty
  }

  size_t random_index = rand() % count; // Generate random index
  size_t found_colors = 0;

  for (size_t i = 0; i < MAX_COLORS; i++) {
    if ((colors & (1ULL << i)) != 0) { // Check if the i-th color is set
      if (found_colors == random_index) {
        return (1ULL << i); // Return the randomly chosen color
      }
      found_colors++;
    }
  }
  return 0; // Shouldn't reach here
}
