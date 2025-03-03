#include "mesh.hpp"

#include <optional>
#include <iostream>

void Mesh::load_file(const std::string &file_name)
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

            if (key == "v") // geometric vertices
            {
                float x, y, z;
                ss >> x >> y >> z;
                vertices.push_back({x, y, z});
                continue;
            }

            if (key == "vt") // texture coordinates
            {
                float u, v;
                ss >> u >> v;
                textures.push_back({u, v});
                continue;
            }

            if (key == "vn") // vertex normals
            {
                float x, y, z;
                ss >> x >> y >> z;
                normals.push_back({x, y, z, 0});
                continue;
            }

            if (key == "f") // face element
            {
                std::vector<std::string> elements;

                size_t size = 0;

                while (!ss.eof())
                {
                    std::string element;
                    ss >> element;
                    elements.push_back(element);
                    ++size;
                }

                Face face(this, size);

                for (size_t i = 0; i < size; ++i)
                {
                    // Cases: v, v/t, v//n, v/t/n
                    std::stringstream element_ss(elements[i]);

                    int vertex_index;
                    element_ss >> vertex_index;
                    face.vertex_indices[i] = vertex_index - 1;

                    if (!element_ss.eof() && element_ss.peek() == '/')
                    {
                        // Cases: v/t, v//n, v/t/n
                        char ignore;
                        element_ss >> ignore;

                        if (!element_ss.eof() && element_ss.peek() != '/')
                        {
                            // Cases: v/t, v/t/n
                            int texture_index;
                            element_ss >> texture_index;
                            face.texture_indices[i] = texture_index - 1;
                        }

                        if (!element_ss.eof() && element_ss.peek() == '/')
                        {
                            // Cases: v//n, v/t/n
                            int normal_index;
                            element_ss >> ignore >> normal_index;
                            face.normal_indices[i] = normal_index - 1;
                        }
                    }
                }

                faces.push_back(face);
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

// Checks if the point is on the same side of the plane as the normal.
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

void draw_barycentric(Image &image, DepthBuffer &depth, Triangle &triangle)
{
    Vec2 &v0 = triangle.v0, &v1 = triangle.v1, &v2 = triangle.v2;

    uint32_t minu = std::min({v0.u, v1.u, v2.u});
    uint32_t maxu = std::max({v0.u, v1.u, v2.u});
    uint32_t minv = std::min({v0.v, v1.v, v2.v});
    uint32_t maxv = std::max({v0.v, v1.v, v2.v});

    uint32_t du = maxu - minu + 1;
    uint32_t dv = maxv - minv + 1;

    Vec2 edge0 = v1 - v0, edge1 = v2 - v0;
    float d00 = dot(edge0, edge0);
    float d01 = dot(edge0, edge1);
    float d11 = dot(edge1, edge1);

    float area = d00 * d11 - d01 * d01;

    float z0 = 1.0f / v0.w, z1 = 1.0f / v1.w, z2 = 1.0f / v2.w;

    for (uint32_t i = 0; i < du * dv; ++i)
    {
        uint32_t u = i % du + minu;
        uint32_t v = i / du + minv;
        Vec2 pixel(u, v);

        Vec2 edge2 = pixel - v0;
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

                Color color(a, b, c);
                image.set_pixel(u, v, color);
            }
        }
    }
}