#include "light.hpp"

#include <iostream>

Vec4 DirectionalLight::get_direction(const Vec4 &point) const
{
    std::ignore = point;
    return direction;
}

Vec4 PointLight::get_direction(const Vec4 &point) const
{
    return normalize(position - point);
}

float PointLight::get_attenuation(const Vec4 &point) const
{
    return 1.0f / magnitude_squared(position - point) * k;
}

Vec4 SpotLight::get_direction(const Vec4 &point) const
{
    return normalize(position - point);
}

float SpotLight::get_attenuation(const Vec4 &point) const
{
    float light_fall_off = std::max(0.0f, dot(normalize(point - position), direction) - angle) / (1.0f - angle);
    return std::pow(light_fall_off, taper);
}

Color Material::get_color(const Vec4 &point, const Vec4 &normal, const Vec3 &uv, LightCollection &lights)
{
    Color diffuse_sum, specular_sum;

    // Compute the sum of the diffuse and specular light from each light source
    for (const Light *light : lights)
    {
        const Color &color = light->get_color();
        const float attenuation = light->get_attenuation(point);
        // https://web.stanford.edu/class/ee267/lectures/lecture3.pdf
        const Vec4 L = light->get_direction(point); // normalized vector pointing twoards the light source
        const Vec4 &N = normal;                     // normalized surface normal
        const Vec4 V = normalize(point);            // normalized vector pointing towards the viewer
        const Vec4 R = L - N * dot(N, L) * 2;       // normalized reflection on surface normal

        diffuse_sum += color * std::max(0.0f, dot(N, L)) * attenuation;
        specular_sum += color * std::pow(std::max(0.0f, dot(R, V)), shininess) * attenuation;
    }

    // Use the texture's color if there is one
    Color diffuse_color = textured ? texture.get_pixel(uv.x, uv.y) : diffuse;

    Color color;
    // Phong lighting model: Sum of ambient, diffuse, and specular light
    color = ambient * lights.get_ambient() + diffuse_color * diffuse_sum + specular * specular_sum;
    color.r = std::min(1.0f, color.r);
    color.g = std::min(1.0f, color.g);
    color.b = std::min(1.0f, color.b);
    return color;
}

void Material::load_file(const std::string &file_name)
{
    std::fstream file;
    std::string line;

    file.open(file_name);

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            if (ss.eof())
                continue;

            std::string key;
            ss >> key;

            if (key == "#") // ignore comments
                continue;

            if (key == "Ns") // specular highlight focus
            {
                ss >> shininess;
                continue;
            }

            if (key == "Ka") // ambient color
            {
                float r, g, b;
                ss >> r >> g >> b;
                ambient = {r, g, b};
                continue;
            }

            if (key == "Kd") // diffuse color
            {
                float r, g, b;
                ss >> r >> g >> b;
                diffuse = {r, g, b};
                continue;
            }

            if (key == "Ks") // specular color
            {
                float r, g, b;
                ss >> r >> g >> b;
                specular = {r, g, b};
                continue;
            }

            if (key == "map_Kd") // texture image
            {
                std::string path;
                ss >> path;
                texture.load_file(path);
                textured = true;
            }
        }
        file.close();
    }
    else
    {
        std::cerr << "Error: Unable to open file " << file_name << std::endl;
    }
}

void draw_line(Image &image, Vec3 &start, Vec3 &end)
{
    float u, v, du, dv, step;
    du = end.x - start.x;
    dv = end.y - start.y;

    if (std::abs(du) >= std::abs(dv))
        step = std::abs(du);
    else
        step = std::abs(dv);

    du = du / step;
    dv = dv / step;
    u = start.x;
    v = start.y;
    int i = 0;
    while (i <= step)
    {
        image.set_pixel(std::round(u), std::round(v), {1, 1, 1});
        u = u + du;
        v = v + dv;
        ++i;
    }
}

void draw_barycentric(Image &image, DepthBuffer &depth, Color &color, const VertexData &vertex0, const VertexData &vertex1, const VertexData &vertex2)
{
    const Vec3 &s0 = vertex0.screen_coordinate, &s1 = vertex1.screen_coordinate, &s2 = vertex2.screen_coordinate;

    uint32_t minu = std::clamp(std::min({s0.x, s1.x, s2.x}), 0.0f, image.get_width() - 1.0f);
    uint32_t maxu = std::clamp(std::max({s0.x, s1.x, s2.x}), 0.0f, image.get_width() - 1.0f);
    uint32_t minv = std::clamp(std::min({s0.y, s1.y, s2.y}), 0.0f, image.get_height() - 1.0f);
    uint32_t maxv = std::clamp(std::max({s0.y, s1.y, s2.y}), 0.0f, image.get_height() - 1.0f);

    Vec3 edge0 = s1 - s0, edge1 = s2 - s0;
    float d00 = dot(edge0, edge0);
    float d01 = dot(edge0, edge1);
    float d11 = dot(edge1, edge1);
    float area = d00 * d11 - d01 * d01;

    float z0 = 1.0f / s0.z, z1 = 1.0f / s1.z, z2 = 1.0f / s2.z;

    for (uint32_t u = minu; u <= maxu; ++u)
    {
        for (uint32_t v = minv; v <= maxv; ++v)
        {
            Vec3 pixel(u + 0.5f, v + 0.5f);

            Vec3 edge2 = pixel - s0;
            float d20 = dot(edge2, edge0);
            float d21 = dot(edge2, edge1);

            float b = (d11 * d20 - d01 * d21) / area;
            float c = (d00 * d21 - d01 * d20) / area;
            float a = 1.0f - b - c;

            if (a >= 0 && b >= 0 && c >= 0)
            {
                float inverse_z = a * z0 + b * z1 + c * z2;
                float z = 1 / inverse_z;

                if (z <= depth.at(u, v))
                {
                    depth.at(u, v) = z;

                    image.set_pixel(u, v, color);
                }
            }
        }
    }
}

void draw_barycentric(Image &image, DepthBuffer &depth, Material &mat, LightCollection &lights, const VertexData &vertex0, const VertexData &vertex1, const VertexData &vertex2)
{
    const Vec4 &p0 = vertex0.world, &p1 = vertex1.world, &p2 = vertex2.world;
    const Vec4 &n0 = vertex0.normal, &n1 = vertex1.normal, &n2 = vertex2.normal;
    const Vec3 &s0 = vertex0.screen_coordinate, &s1 = vertex1.screen_coordinate, &s2 = vertex2.screen_coordinate;
    const Vec3 &t0 = vertex0.texture_coordinate, &t1 = vertex1.texture_coordinate, &t2 = vertex2.texture_coordinate;

    uint32_t minu = std::clamp(std::min({s0.x, s1.x, s2.x}), 0.0f, image.get_width() - 1.0f);
    uint32_t maxu = std::clamp(std::max({s0.x, s1.x, s2.x}), 0.0f, image.get_width() - 1.0f);
    uint32_t minv = std::clamp(std::min({s0.y, s1.y, s2.y}), 0.0f, image.get_height() - 1.0f);
    uint32_t maxv = std::clamp(std::max({s0.y, s1.y, s2.y}), 0.0f, image.get_height() - 1.0f);

    Vec3 edge0 = s1 - s0, edge1 = s2 - s0;
    float d00 = dot(edge0, edge0);
    float d01 = dot(edge0, edge1);
    float d11 = dot(edge1, edge1);
    float area = d00 * d11 - d01 * d01;

    float z0 = 1.0f / s0.z, z1 = 1.0f / s1.z, z2 = 1.0f / s2.z;

    for (uint32_t u = minu; u <= maxu; ++u)
    {
        for (uint32_t v = minv; v <= maxv; ++v)
        {
            Vec3 pixel(u + 0.5f, v + 0.5f);

            Vec3 edge2 = pixel - s0;
            float d20 = dot(edge2, edge0);
            float d21 = dot(edge2, edge1);

            float b = (d11 * d20 - d01 * d21) / area;
            float c = (d00 * d21 - d01 * d20) / area;
            float a = 1.0f - b - c;

            // Check if this pixel is in the triangle
            if (a >= 0 && b >= 0 && c >= 0)
            {
                float az = a * z0, bz = b * z1, cz = c * z2;
                float z = 1 / (az + bz + cz);

                // Check if this pixel is the closest so far
                if (z <= depth.at(u, v))
                {
                    // Set the depth buffer with the closer value
                    depth.at(u, v) = z;

                    // Interpolate across all of the vertex values
                    Vec4 point = (p0 * az + p1 * bz + p2 * cz) * z;
                    Vec3 texture = (t0 * az + t1 * bz + t2 * cz) * z;
                    Vec4 normal = normalize(n0 * az + n1 * bz + n2 * cz);

                    // Set the color using the material and light
                    Color color = mat.get_color(point, normal, texture, lights);
                    image.set_pixel(u, v, color);
                }
            }
        }
    }
}
