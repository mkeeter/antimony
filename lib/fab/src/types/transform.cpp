#include "fab/types/transform.h"

Transform::Transform(
        std::string x_forward, std::string y_forward, std::string z_forward,
        std::string x_reverse, std::string y_reverse, std::string z_reverse)
    : x_forward(x_forward), y_forward(y_forward), z_forward(z_forward),
      x_reverse(x_reverse), y_reverse(y_reverse), z_reverse(z_reverse)
{
    // Nothing to do here
}

Transform::Transform(std::string x_forward, std::string y_forward,
                     std::string x_reverse, std::string y_reverse)
    : Transform(x_forward, y_forward, "", x_reverse, y_reverse, "")
{
    // Nothing to do here
}
