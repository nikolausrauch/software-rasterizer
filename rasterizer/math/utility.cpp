#include "utility.h"

Vec2 equirectangularUV(const Vec3& dir)
{
    float phi = atan2(dir.x, dir.z);
    float theta = asin(dir.y);
    return Vec2((phi + PI) / (2.0 * PI), (theta + 0.5 * PI) / PI);
}

Vec3 equirectangulatDir(Vec2 pixel, Vec2i size)
{
    float phi = ( pixel.x / size.x * 2.0 * PI ) - PI;
    float theta = ( pixel.y / size.y * PI) - 0.5 * PI;
    return Vec3{std::cos(theta) * std::sin(phi), std::sin(theta), std::cos(theta) * std::cos(phi)};
}
