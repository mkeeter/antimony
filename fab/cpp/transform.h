#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <string>

struct Transform
{
    Transform(std::string x_forward,
              std::string y_forward,
              std::string z_forward,
              std::string x_reverse,
              std::string y_reverse,
              std::string z_reverse);

    Transform(std::string x_forward,
              std::string y_forward,
              std::string x_reverse,
              std::string y_reverse);

    const std::string x_forward, y_forward, z_forward;
    const std::string x_reverse, y_reverse, z_reverse;
};

#endif // TRANSFORM_H
