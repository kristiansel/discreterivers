#ifndef APPCONSTRAINTS_H
#define APPCONSTRAINTS_H

#include <cstddef>

namespace appconstraints {

static const unsigned int min_window_width_abs  = 700;
static const unsigned int min_window_height_abs = 500;

static constexpr float min_ui_scale_factor = 0.2f;
static constexpr float max_ui_scale_factor = 5.0f;

static constexpr std::size_t n_actors_max = 1000;

}

#endif // APPCONSTRAINTS_H
