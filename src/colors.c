#include "colors.h"

#include "string.h"
#include <stdbool.h>
#include <stdio.h>

colors_t
colors_full(const size_t size) {
  static const int BITS_PER_BYTE = 8;
  if (size == 0) {
    return 0ULL;
  }

  if (size >= sizeof(colors_t) * BITS_PER_BYTE) {
    return (colors_t)-1;
  }

  return (1ULL << size) - 1;
}

colors_t
colors_empty(void) {
  return 0ULL;
}

colors_t
colors_set(const size_t color_id) {
  if (color_id >= MAX_COLORS) {
    return 0ULL;
  }

  return 1ULL << color_id;
}

colors_t
colors_add(const colors_t colors, const size_t color_id) {
  if (color_id >= MAX_COLORS) {
    return colors;
  }

  return colors | (1ULL << color_id);
}

colors_t
colors_discard(const colors_t colors, const size_t color_id) {
  if (color_id >= MAX_COLORS) {
    return colors;
  }

  return colors & ~(1ULL << color_id);
}

bool
colors_is_in(const colors_t colors, const size_t color_id) {
  if (color_id >= MAX_COLORS) {
    return false;
  }

  return (colors & (1ULL << color_id)) != 0;
}

colors_t
colors_negate(const colors_t colors) {
  return ~colors;
}

colors_t
colors_and(const colors_t colors1, const colors_t colors2) {
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
  return colors1 & ~colors2;
}

bool
colors_is_equal(const colors_t colors1, const colors_t colors2) {
  return colors1 == colors2;
}

bool
colors_is_subset(const colors_t colors1, const colors_t colors2) {
  return (colors1 & colors2) == colors1;
}

bool
colors_is_singleton(const colors_t colors) {
  return colors && !(colors & (colors - 1));
}

size_t
colors_count(const colors_t colors) {
  size_t count = 0;
  colors_t temp = colors;

  while (temp) {
    temp = temp & (temp - 1);
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

  colors_t mask = 1ULL << (MAX_COLORS - 1);
  for (size_t i = 0; i < MAX_COLORS; i++) {
    if (colors & mask) {
      return mask;
    }
    mask >>= 1;
  }

  return 0;
}

static colors_t
prng(uint64_t* seed) {
  uint64_t A = 6364136223846793005ULL;
  uint64_t C = 1ULL;
  *seed = *seed * A + C;
  return *seed;
}

/**
 * @brief Returns a random color from the provided set of colors.
 */
colors_t
colors_random(const colors_t colors) {
  static uint64_t seed;

  size_t count = colors_count(colors);

  if (count == 0) {
    return 0;
  }

  uint64_t randomNumber = prng(&seed);
  size_t random_index = randomNumber % count;
  size_t found_colors = 0;

  for (size_t i = 0; i < MAX_COLORS; i++) {
    if ((colors & (1ULL << i)) != 0) {
      if (found_colors == random_index) {
        return (1ULL << i);
      }
      found_colors++;
    }
  }
  return 0;
}

bool
subgrid_consistency(colors_t* subgrid[], const size_t size) {
  colors_t all_colors_combined = colors_empty();

  for (size_t i = 0; i < size; i++) {
    // Check for empty cells
    if (*subgrid[i] == colors_empty()) {
      return false;
    }

    // Check for two singleton sharing colors
    if (colors_is_singleton(*subgrid[i])) {
      for (size_t j = 0; j < i; j++) {
        if (colors_is_singleton(*subgrid[j])
            && colors_is_equal(*subgrid[i], *subgrid[j])) {
          return false;
        }
      }
    }

    // Combine all colors
    all_colors_combined = colors_or(all_colors_combined, *subgrid[i]);
  }

  // Check if each color appears at least once
  if (colors_count(all_colors_combined) < size) {
    return false;
  }

  return true;
}

static bool
cross_hatching_heuristics(colors_t* subgrid[], const size_t size) {
  bool result = false;
  for (size_t i = 0; i < size; i++) {
    if (colors_is_singleton(*subgrid[i])) {
      for (size_t j = 0; j < size; j++) {
        if (i != j && colors_is_subset(*subgrid[i], *subgrid[j])) {
          *subgrid[j] = colors_subtract(*subgrid[j], *subgrid[i]);
          result = true;
        }
      }
    }
  }
  return result;
}

static bool
lone_number_heuristic(colors_t* subgrid[], size_t size) {
  int position = 0;
  bool result = false;
  for (size_t i = 0; i < size; i++) {
    int cpt = 0;
    for (size_t j = 0; j < size; j++) {
      if (colors_is_in(*subgrid[j], i)) {
        position = j;
        cpt++;
      }
      if (cpt > 1) {
        break;
      }
    }
    if (cpt == 1 && !colors_is_singleton(*subgrid[position])) {
      *subgrid[position] = colors_set(i);
      result = true;
    }
  }

  return result;
}

static bool
naked_subset_heuristic(colors_t* subgrid[], size_t size) {
  bool result = false;

  for (size_t i = 0; i < size; i++) {
    int cpt = 0;
    int color_count = colors_count(*subgrid[i]);
    for (size_t j = 0; j < size; j++) {
      if (*subgrid[i] == *subgrid[j]) {
        cpt++;
      }
    }
    if (cpt == color_count) {
      for (size_t j = 0; j < size; j++) {
        if (*subgrid[i] != *subgrid[j]) {
          colors_t temp = *subgrid[j];
          *subgrid[j] = colors_subtract(*subgrid[j], *subgrid[i]);
          if (temp != *subgrid[j]) {
            result = true;
          }
        }
      }
    }
  }
  return result;
}

bool
subgrid_heuristics(colors_t* subgrid[], const size_t size) {

  return cross_hatching_heuristics(subgrid, size)
         || lone_number_heuristic(subgrid, size)
         || naked_subset_heuristic(subgrid, size);
}
