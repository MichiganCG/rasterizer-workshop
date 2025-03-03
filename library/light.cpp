#include "light.hpp"

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

void draw_barycentric(Image &image, DepthBuffer &depth, Vec2 &v0, Vec2 &v1, Vec2 &v2)
{
    uint32_t minu = std::min({v0.u, v1.u, v2.u});
    uint32_t maxu = std::max({v0.u, v1.u, v2.u});
    uint32_t minv = std::min({v0.v, v1.v, v2.v});
    uint32_t maxv = std::max({v0.v, v1.v, v2.v});

    Vec2 edge0 = v1 - v0, edge1 = v2 - v0;
    float d00 = dot(edge0, edge0);
    float d01 = dot(edge0, edge1);
    float d11 = dot(edge1, edge1);

    float area = d00 * d11 - d01 * d01;

    float z0 = 1.0f / v0.w, z1 = 1.0f / v1.w, z2 = 1.0f / v2.w;

    for (uint32_t u = minu; u <= maxu; ++u)
    {
        for (uint32_t v = minv; v <= maxv; ++v)
        {
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

                    Color color(1);
                    image.set_pixel(u, v, color);
                }
            }
        }
    }
}

void draw_barycentric(Image &image, DepthBuffer &depth, Vec2 &v0, Vec2 &v1, Vec2 &v2, Vec3 &n0, Vec3 &n1, Vec3 &n2)
{
    uint32_t minu = std::min({v0.u, v1.u, v2.u});
    uint32_t maxu = std::max({v0.u, v1.u, v2.u});
    uint32_t minv = std::min({v0.v, v1.v, v2.v});
    uint32_t maxv = std::max({v0.v, v1.v, v2.v});

    Vec2 edge0 = v1 - v0, edge1 = v2 - v0;
    float d00 = dot(edge0, edge0);
    float d01 = dot(edge0, edge1);
    float d11 = dot(edge1, edge1);

    float area = d00 * d11 - d01 * d01;

    float z0 = 1.0f / v0.w, z1 = 1.0f / v1.w, z2 = 1.0f / v2.w;

    for (uint32_t u = minu; u <= maxu; ++u)
    {
        for (uint32_t v = minv; v <= maxv; ++v)
        {
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

                    Vec3 normal = normalize(n0 * a + n1 * b + n2 * c);
                    float d = std::max(0.01f, dot(normal, Vec3::UP));

                    Color color(d);
                    image.set_pixel(u, v, color);
                }
            }
        }
    }
}