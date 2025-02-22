#pragma once

#include "stb_image_write.h"

#include <cmath>
#include <tuple>
#include <string>
#include <vector>
#include <numbers>
#include <cstdint>
#include <functional>

constexpr float Infinity = std::numeric_limits<float>::infinity();
constexpr float Pi = std::numbers::pi_v<float>;

struct Color
{
	Color(float r, float g, float b) : r(r), g(g), b(b) {}
	explicit Color(float value = 0.0f) : r(value), g(value), b(value) {}

	float r, g, b;
};

/**
 * Returns if a value is very close to zero.
 * @param epsilon The threshold used to make this decision.
 */
inline bool almost_zero(float value, float epsilon = 8E-7f) { return -epsilon < value && value < epsilon; }

/**
 * Takes the square root of a number while avoiding negative numbers from rounding errors.
 * @return The square root of value if value is positive, otherwise zero.
 */
inline float safe_sqrt(float value) { return value <= 0.0f ? 0.0f : std::sqrt(value); }

/**
 * @return A random floating point value between 0 (inclusive) and 1 (exclusive).
 */
float random_float();

class Image
{
public:
	Image(uint32_t width, uint32_t height) : width(width), height(height), pixels(width * height) {}

	Color get_pixel(uint32_t x, uint32_t y) const { return pixels[get_index(x, y)]; }
	void set_pixel(uint32_t x, uint32_t y, Color color) { pixels[get_index(x, y)] = color; }

	/**
	 * Outputs this image as a PNG image file.
	 */
	void write_file(const std::string &path) const;

private:
	uint32_t get_index(uint32_t x, uint32_t y) const { return x + width * y; }

	uint32_t width;
	uint32_t height;
	std::vector<Color> pixels;
};

/**
 * Returns the luminance value of a color.
 * This can be thought of as the visually perceived brightness.
 */
inline float get_luminance(Color color) { return color.r * 0.212671f + color.g * 0.715160f + color.b * 0.072169f; }

/**
 * Returns whether a color is almost black.
 */
inline bool almost_black(Color color) { return almost_zero(get_luminance(color)); }

/**
 * @return Whether a color value is valid (i.e. not NaN).
 */
inline bool is_invalid(Color color) { return not std::isfinite(color.r + color.g + color.b); }

/**
 * Executes an action in parallel, taking advantage of multiple threads.
 * Also optionally prints the execution progress in standard out.
 * @param begin The first index to execute (inclusive).
 * @param end One past the last index to execute (exclusive).
 * @param action The action to execute in parallel.
 */
void parallel_for(uint32_t begin, uint32_t end, const std::function<void(uint32_t)> &action, bool show_progress = true);
