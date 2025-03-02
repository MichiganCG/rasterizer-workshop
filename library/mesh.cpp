#include "mesh.hpp"

#include <optional>
#include <iostream>

std::vector<std::string> split_by(const std::string &str, char delimiter)
{
    std::stringstream ss(str);
    std::string word;
    std::vector<std::string> words;

    while (std::getline(ss, word, delimiter))
    {
        words.push_back(word);
    }
    return words;
}

void Mesh::load_file(const std::string &file_name)
{
    std::fstream file;
    std::string line, current;

    file.open(file_name);

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            ss >> current;

            if (current == "v") // geometric vertices
            {
                float x, y, z;
                ss >> x >> y >> z;
                vertices.push_back({x, y, z});
                continue;
            }

            if (current == "vt") // texture coordinates
            {
                continue;
            }

            if (current == "vn") // vertex normals
            {
                float x, y, z;
                ss >> x >> y >> z;
                normals.push_back({x, y, z, 0});
                continue;
            }

            if (current == "f") // face element
            {
                std::vector<int> verts;
                while (ss >> current)
                {
                    std::vector<std::string> indices = split_by(current, '/');
                    verts.push_back(std::stoi(indices[0]) - 1);
                }

                // Split polygon into triangles using Fan Triangulation
                // https://en.wikipedia.org/wiki/Fan_triangulation
                for (size_t i = 1; i < verts.size() - 1; ++i)
                    triangles.push_back({this, verts[0], verts[i], verts[i + 1]});
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

inline bool infront(const Vec3 &point, const Plane &plane) { return dot(plane.normal, point - plane.point) >= 0; }

std::vector<Vec3> sutherland_hodgman(std::vector<Vec3> &input_list, const std::vector<Plane> &clipping_planes)
{
    std::vector<Vec3> out_list = input_list;

    for (const Plane &plane : clipping_planes)
    {
        std::swap(input_list, out_list);
        out_list.clear();

        if (input_list.size() == 0)
            return out_list;

        Vec3 *start = &input_list[input_list.size() - 1];
        for (size_t j = 0; j < input_list.size(); ++j)
        {
            Vec3 *end = &input_list[j];

            std::optional<Vec3> hit = intersect_plane(plane.point, plane.normal, *start, *end);
            if (infront(*start, plane))
            {
                if (infront(*end, plane))
                    out_list.push_back(*end);
                else
                    out_list.push_back(*hit);
            }
            else if (infront(*end, plane))
            {
                out_list.push_back(*hit);
                out_list.push_back(*end);
            }
            start = end;
        }
    }
    return out_list;
}

void DDA(Image &image, Vec2 &start, Vec2 &end)
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

Vec2 get_barycentric(Vec2 p, Vec2 &t1, Vec2 &t2, Vec2 &t3)
{
    Vec2 v0 = t2 - t1, v1 = t3 - t1, v2 = p - t1;
    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float denom = d00 * d11 - d01 * d01; // Area of full triangle
    Vec2 out;
    out.v = (d11 * d20 - d01 * d21) / denom;
    out.w = (d00 * d21 - d01 * d20) / denom;
    out.u = 1.0f - out.v - out.w;
    return out;
}

void draw_barycentric(Image &image, DepthBuffer &depth, Vec2 &t1, Vec2 &t2, Vec2 &t3)
{
    uint32_t minu = std::min({t1.u, t2.u, t3.u}), minv = std::min({t1.v, t2.v, t3.v});
    uint32_t maxu = std::max({t1.u, t2.u, t3.u}), maxv = std::max({t1.v, t2.v, t3.v});

    for (uint32_t v = minv; v < maxv; ++v)
    {
        for (uint32_t u = minu; u < maxu; ++u)
        {
            Vec2 bary = get_barycentric({u, v}, t1, t2, t3);

            if (bary.u >= 0 && bary.v >= 0 && bary.w >= 0)
            {
                float z = 1 / (bary.u / t1.w + bary.v / t2.w + bary.w / t3.w);
                if (z < depth.at(u, v))
                {
                    depth.at(u, v) = z;

                    Color color(bary.u, bary.v, bary.w);
                    image.set_pixel(u, v, color);
                }
            }
        }
    }
}