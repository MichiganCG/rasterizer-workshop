/* This file is part of the Michigan Computer Graphics rasterization workshop.
 * Copyright (C) 2025  Aidan Rhys Donley
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "library.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../thirdparty/stb/stb_image.h"
#include "../thirdparty/stb/stb_image_write.h"

#include <algorithm>
#include <atomic>
#include <iostream>
#include <random>
#include <stdexcept>
#include <thread>
#include <vector>

using Random = std::default_random_engine;
thread_local std::unique_ptr<Random> thread_random;

Color &Color::operator=(const Color &other)
{
    if (this == &other)
        return *this;
    r = other.r;
    g = other.g;
    b = other.b;
    return *this;
}

Color &Color::operator+=(const Color &rhs)
{
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    return *this;
}

Color &Color::operator-=(const Color &rhs)
{
    r -= rhs.r;
    g -= rhs.g;
    b -= rhs.b;
    return *this;
}

Color &Color::operator*=(const Color &rhs)
{
    r *= rhs.r;
    g *= rhs.g;
    b *= rhs.b;
    return *this;
}

Color &Color::operator*=(const float rhs)
{
    r *= rhs;
    g *= rhs;
    b *= rhs;
    return *this;
}

Color operator+(Color lhs, const Color &rhs) { return (lhs += rhs); }
Color operator-(Color lhs, const Color &rhs) { return (lhs -= rhs); }
Color operator*(Color lhs, const Color &rhs) { return (lhs *= rhs); }
Color operator*(Color lhs, const float rhs) { return (lhs *= rhs); }

Color Image::get_pixel(float x, float y) const
{
    return pixels[get_index(
        static_cast<uint32_t>(x * width),
        static_cast<uint32_t>(y * width)
    )];
}

void Image::write_file(const std::string &path) const
{
    std::vector<uint8_t> data;
    data.reserve(width * height * 3);

    auto convert_single = [](float value)
    {
        // Gamma correction and clamp
        float corrected = std::sqrt(std::max(0.0f, std::min(value, 1.0f)));
        return static_cast<uint8_t>(corrected * std::numeric_limits<uint8_t>::max());
    };

    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)
        {
            auto &pixel = pixels[y * width + x];
            data.push_back(convert_single(pixel.r));
            data.push_back(convert_single(pixel.g));
            data.push_back(convert_single(pixel.b));
        }
    }

    auto casted_width = static_cast<int>(width);
    auto casted_height = static_cast<int>(height);
    int result = stbi_write_png(path.c_str(), casted_width, casted_height, 3, data.data(), 0);
    if (result == 0)
        throw std::runtime_error("Error in STB library when outputting image.");
}

void Image::load_file(const std::string &path)
{
    int w, h, n;
    int result = stbi_info(path.c_str(), &w, &h, &n);
    if (result == 0)
        throw std::runtime_error("Error in STB library when reading image.");

    width = static_cast<uint32_t>(w);
    height = static_cast<uint32_t>(h);
    pixels.reserve(width * height);

    // input between [0, 255]
    auto convert_single = [](int value)
    {
        float corrected = value / 255.0f;
        return corrected * corrected; // Gamma correction
    };

    uint8_t *data = stbi_load(path.c_str(), &w, &h, &n, 3);
    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)
        {
            size_t index = y * width + x;
            auto &pixel = pixels[index];
            pixel.r = convert_single(data[index * 3 + 0]);
            pixel.g = convert_single(data[index * 3 + 1]);
            pixel.b = convert_single(data[index * 3 + 2]);
        }
    }
}

Image DepthBuffer::get_image() const
{
    Image image{width, height};
    for (uint32_t u = 0; u < width; ++u)
        for (uint32_t v = 0; v < height; ++v)
            image.set_pixel(u, v, Color{at(u, v)});
    return image;
}

static Random *make_random_engine(uint32_t seed)
{
    auto random = std::make_unique<Random>(seed);
    Random *result = random.get();
    thread_random = std::move(random);
    return result;
}

float random_float()
{
    Random *random = thread_random.get();
    if (random == nullptr)
        random = make_random_engine(0);
    std::uniform_real_distribution<float> distribution;
    return distribution(*random);
}

void parallel_for(uint32_t begin, uint32_t end, const std::function<void(uint32_t)> &action, bool show_progress)
{
    auto print_done = []()
    { std::printf("\r       \rdone\n"); };

    if (end == begin)
    {
        if (show_progress)
            print_done();
        return;
    }

    if (end < begin)
        std::swap(begin, end);

    uint32_t workers = std::thread::hardware_concurrency();
    workers = std::min(std::max(workers, 1U), end - begin);

    std::vector<std::thread> threads;
    std::atomic<uint32_t> current = begin;

    for (uint32_t i = 0; i < workers; ++i)
    {
        auto entry = [i, end, &current, &action]()
        {
            make_random_engine(i);

            while (true)
            {
                uint32_t index = current++;
                if (index >= end)
                    break;
                action(index);
            }
        };

        auto entry_print = [begin, end, &current, &action]()
        {
            make_random_engine(0);

            while (true)
            {
                uint32_t index = current++;
                if (index >= end)
                    break;

                uint32_t done = index - begin;
                uint32_t total = end - begin;
                std::printf("\r%5.2f %%", static_cast<float>(done) / total * 100.0f);
                std::cout << std::flush;

                action(index);
            }
        };

        if (i > 0 || not show_progress)
            threads.emplace_back(entry);
        else
            threads.emplace_back(entry_print);
    }

    for (auto &thread : threads)
        thread.join();
    if (show_progress)
        print_done();
}
