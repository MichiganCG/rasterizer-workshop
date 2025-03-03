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

Color Material::get_color(Vec3 &point, Vec3 &normal, std::vector<DirectionalLight> &lights)
{
    if (normal.x == 0 && normal.y == 0 && normal.z == 0)
        return Color(1);

    Color diffuse_sum, specular_sum, color;

    for (auto &light : lights)
    {
    diffuse_sum += light.color * std::max(0.0f, dot(normal, light.direction));
    specular_sum += light.color * std::pow(std::max(0.0f, dot(normal, normalize(point + light.direction))), shininess);

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

void fill_barycentric(Image &image, DepthBuffer &depth, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2)
{
    uint32_t minu = std::min({p0.u, p1.u, p2.u});
    uint32_t maxu = std::max({p0.u, p1.u, p2.u});
    uint32_t minv = std::min({p0.v, p1.v, p2.v});
    uint32_t maxv = std::max({p0.v, p1.v, p2.v});

    Vec2 edge0 = p1 - p0, edge1 = p2 - p0;
    float d00 = dot(edge0, edge0);
    float d01 = dot(edge0, edge1);
    float d11 = dot(edge1, edge1);

    float area = d00 * d11 - d01 * d01;

    float z0 = 1.0f / p0.w, z1 = 1.0f / p1.w, z2 = 1.0f / p2.w;

    for (uint32_t u = minu; u <= maxu; ++u)
    {
        for (uint32_t v = minv; v <= maxv; ++v)
        {
            Vec2 pixel(u, v);

            Vec2 edge2 = pixel - p0;
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

                    Color color(1);
                    image.set_pixel(u, v, color);
                }
            }
        }
    }
}

void draw_barycentric(Image &image, DepthBuffer &depth, Material &mat, std::vector<DirectionalLight> &lights, Triangle &triangle)
{
    const Vec2 &p0 = triangle.point(0), &p1 = triangle.point(1), &p2 = triangle.point(2);
    const Vec3 &v0 = triangle.vertex(0), &v1 = triangle.vertex(1), &v2 = triangle.vertex(2);
    const Vec3 &n0 = triangle.normal(0), &n1 = triangle.normal(1), &n2 = triangle.normal(2);

    uint32_t minu = std::min({p0.u, p1.u, p2.u});
    uint32_t maxu = std::max({p0.u, p1.u, p2.u});
    uint32_t minv = std::min({p0.v, p1.v, p2.v});
    uint32_t maxv = std::max({p0.v, p1.v, p2.v});

    Vec2 edge0 = p1 - p0, edge1 = p2 - p0;
    float d00 = dot(edge0, edge0);
    float d01 = dot(edge0, edge1);
    float d11 = dot(edge1, edge1);

    float area = d00 * d11 - d01 * d01;

    float z0 = 1.0f / p0.w, z1 = 1.0f / p1.w, z2 = 1.0f / p2.w;

    for (uint32_t u = minu; u <= maxu; ++u)
    {
        for (uint32_t v = minv; v <= maxv; ++v)
        {
            Vec2 pixel(u, v);

            Vec2 edge2 = pixel - p0;
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

                    Vec3 point = (v0 * a + v1 * b + v2 * c);
                    Vec3 normal = normalize(n0 * a + n1 * b + n2 * c);
                    Color color = mat.get_color(point, normal, lights);
                    image.set_pixel(u, v, color);
                }
            }
        }
    }
}

