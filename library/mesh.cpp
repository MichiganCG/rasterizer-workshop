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
                            face.texture_indices.push_back(texture_index - 1);
                        }

                        if (!element_ss.eof() && element_ss.peek() == '/')
                        {
                            // Cases: v//n, v/t/n
                            int normal_index;
                            element_ss >> ignore >> normal_index;
                            face.normal_indices.push_back(normal_index - 1);
                        }
                    }
                }

                faces.push_back(face);
                continue;
            }
        }
        file.close();

        fix_normals();
    }
    else
    {
        std::cerr << "Error: Unable to open file " << file_name << std::endl;
    }
}

void Mesh::fix_normals()
{
    for (auto &face : faces)
    {
        if (face.normal_indices.size() == 0)
        {
            const Vec4 &v0 = face.get_vertex(0), &v1 = face.get_vertex(1), &v2 = face.get_vertex(2);

            // Compute face normal
            Vec4 normal = normalize(cross(v1 - v0, v2 - v0));
            normal.w = 0;

            int index = (int)normals.size();
            normals.push_back(normal);

            for (size_t i = 0; i < face.size; ++i)
                face.normal_indices.push_back(index);
        }
    }
}

std::vector<Vec4> clipping_planes = {
    //{-1, 0, 0},
    //{1, 0, 0},
    //{0, -1, 0},
    //{0, 1, 0},
    {0, 0, -1},
    {0, 0, 1},
};

void sutherland_hodgman_clip(std::vector<Vertex> &vertex_list)
{
    std::vector<Vertex> out_list = vertex_list;

    for (const Vec4 &plane : clipping_planes)
    {
        std::swap(vertex_list, out_list);
        out_list.clear();

        if (vertex_list.size() == 0)
            return;

        Vertex *start = &vertex_list[vertex_list.size() - 1];
        for (size_t j = 0; j < vertex_list.size(); ++j)
        {
            Vertex *end = &vertex_list[j];

            float d0 = dot(start->point, plane);
            float d1 = dot(end->point, plane);

            if (d0 > 0)
            {
                if (d1 > 0)
                {
                    out_list.push_back(*end);
                }
                else
                {
                    Vertex intersect;
                    float a = d0 / (d0 - d1);
                    intersect.point = start->point * (1.0f - a) + end->point * (a);
                    intersect.normal = start->normal * (1.0f - a) + end->normal * (a);
                    out_list.push_back(intersect);
                }
            }
            else if (d1 > 0)
            {
                Vertex intersect;
                float a = d0 / (d0 - d1);
                intersect.point = start->point * (1.0f - a) + end->point * (a);
                intersect.normal = start->normal * (1.0f - a) + end->normal * (a);
                out_list.push_back(intersect);
                out_list.push_back(*end);
            }
            start = end;
        }
    }
    std::swap(vertex_list, out_list);
}
