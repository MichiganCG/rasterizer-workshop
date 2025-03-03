#include "light.hpp"

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
        }
        file.close();
    }
    else
    {
        std::cerr << "Error: Unable to open file " << file_name << std::endl;
    }
}

Color Material::get_color(Vec3 &point, Vec3 &normal, LightCollection &lights)
{
    if (normal.x == 0 && normal.y == 0 && normal.z == 0)
        return Color(1);

    Color diffuse_sum, specular_sum, color;

    for (Light *light : lights)
    {
        diffuse_sum += light->color * std::max(0.0f, dot(normal, light->direction));
        specular_sum += light->color * std::pow(std::max(0.0f, dot(normal, normalize(point + light->direction))), shininess);
    }

    color = ambient + diffuse * diffuse_sum + specular * specular_sum;
    color.r = std::min(1.0f, color.r);
    color.g = std::min(1.0f, color.g);
    color.b = std::min(1.0f, color.b);
    return color;
}

void draw_line(Image &image, Vec2 &start, Vec2 &end)
{
    float u, v, du, dv, step;
    du = end.u - start.u;
    dv = end.v - start.v;

    if (std::abs(du) >= std::abs(dv))
        step = std::abs(du);
    else
        step = std::abs(dv);

    du = du / step;
    dv = dv / step;
    u = start.u;
    v = start.v;
    int i = 0;
    while (i <= step)
    {
        image.set_pixel(std::round(u), std::round(v), {1, 1, 1});
        u = u + du;
        v = v + dv;
        ++i;
    }
}

void draw_barycentric(Image &image, DepthBuffer &depth, Color &color, const Vertex &vertex0, const Vertex &vertex1, const Vertex &vertex2)
{
    const Vec2 &s0 = vertex0.screen, &s1 = vertex1.screen, &s2 = vertex2.screen;

    uint32_t minu = std::clamp(std::min({s0.u, s1.u, s2.u}), 0.0f, image.get_width() - 1.0f);
    uint32_t maxu = std::clamp(std::max({s0.u, s1.u, s2.u}), 0.0f, image.get_width() - 1.0f);
    uint32_t minv = std::clamp(std::min({s0.v, s1.v, s2.v}), 0.0f, image.get_height() - 1.0f);
    uint32_t maxv = std::clamp(std::max({s0.v, s1.v, s2.v}), 0.0f, image.get_height() - 1.0f);

    Vec2 edge0 = s1 - s0, edge1 = s2 - s0;
    float d00 = dot(edge0, edge0);
    float d01 = dot(edge0, edge1);
    float d11 = dot(edge1, edge1);
    float area = d00 * d11 - d01 * d01;

    float z0 = 1.0f / s0.w, z1 = 1.0f / s1.w, z2 = 1.0f / s2.w;

    for (uint32_t u = minu; u <= maxu; ++u)
    {
        for (uint32_t v = minv; v <= maxv; ++v)
        {
            Vec2 pixel(u + 0.5f, v + 0.5f);

            Vec2 edge2 = pixel - s0;
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

void draw_barycentric(Image &image, DepthBuffer &depth, Material &mat, LightCollection &lights, const Vertex &vertex0, const Vertex &vertex1, const Vertex &vertex2)
{
    const Vec3 &p0 = vertex0.point, &p1 = vertex1.point, &p2 = vertex2.point;
    const Vec3 &n0 = vertex0.normal, &n1 = vertex1.normal, &n2 = vertex2.normal;
    const Vec2 &s0 = vertex0.screen, &s1 = vertex1.screen, &s2 = vertex2.screen;

    uint32_t minu = std::clamp(std::min({s0.u, s1.u, s2.u}), 0.0f, image.get_width() - 1.0f);
    uint32_t maxu = std::clamp(std::max({s0.u, s1.u, s2.u}), 0.0f, image.get_width() - 1.0f);
    uint32_t minv = std::clamp(std::min({s0.v, s1.v, s2.v}), 0.0f, image.get_height() - 1.0f);
    uint32_t maxv = std::clamp(std::max({s0.v, s1.v, s2.v}), 0.0f, image.get_height() - 1.0f);

    Vec2 edge0 = s1 - s0, edge1 = s2 - s0;
    float d00 = dot(edge0, edge0);
    float d01 = dot(edge0, edge1);
    float d11 = dot(edge1, edge1);
    float area = d00 * d11 - d01 * d01;

    float z0 = 1.0f / s0.w, z1 = 1.0f / s1.w, z2 = 1.0f / s2.w;

    for (uint32_t u = minu; u <= maxu; ++u)
    {
        for (uint32_t v = minv; v <= maxv; ++v)
        {
            Vec2 pixel(u + 0.5f, v + 0.5f);

            Vec2 edge2 = pixel - s0;
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

                    Vec3 point = p0 * a + p1 * b + p2 * c;
                    Vec3 normal = normalize(n0 * a + n1 * b + n2 * c);
                    Color color = mat.get_color(point, normal, lights);
                    image.set_pixel(u, v, color);
                }
            }
        }
    }
}
