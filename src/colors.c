#include "colors.h"

#include "string.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // for random()

colors_t
colors_full(const size_t size) {
  if (size == 0) {
    return 0ULL;
  }

  if (size >= sizeof(colors_t) * 8) {
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

colors_t
colors_random(const colors_t colors) {
  size_t count = colors_count(colors);

  if (count == 0) {
    return 0;
  }

  size_t random_index = rand() % count;
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
subgrid_consistency(colors_t subgrid[], const size_t size) {
  colors_t all_colors = colors_empty();

  for (size_t i = 0; i < size; ++i) {
    if (subgrid[i] == colors_empty()) {
      return false; // Empty cell found
    }

    if (colors_is_singleton(subgrid[i])
        && colors_is_in(all_colors, subgrid[i])) {
      return false; // Duplicate singleton found
    }

    all_colors = colors_or(all_colors, subgrid[i]);
  }

  // Check if each color appears at least once
  for (size_t i = 0; i < size; ++i) {
    if (!colors_is_in(all_colors, colors_set(i))) {
      return false; // A color is missing
    }
  }

  return true; // Subgrid is consistent
}

bool
subgrid_is_solved(colors_t subgrid[], const size_t size) {
  colors_t seen_colors = 0;

  for (size_t i = 0; i < size; ++i) {
    if (!colors_is_singleton(subgrid[i])) {
      return false;
    }

    colors_t singleton_color = subgrid[i];
    if (seen_colors & singleton_color) {
      return false;
    }

    seen_colors |= singleton_color; // Mark the color as found
  }

  return true;
}