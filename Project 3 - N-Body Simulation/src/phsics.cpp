#include <cmath>
#include <vector>

#include "physics.h"

Force get_force(const double m1, const double m2, const Point& pos1, const Point& pos2) {
    const double dx = pos2.x - pos1.x, dy = pos2.y - pos1.y;
    const double r_sqr = dx * dx + dy * dy;
    const double r = sqrt(r_sqr);
    const double f = GRAVITY_CONST * m1 * m2 / (r_sqr + FLOAT_OP_ERROR);
    return Force{f * dx / r, f * dy / r};
}

void handle_wall_collision(Point& pos, Velocity& v) {
    if (pos.x < 0) {
        pos.x = 0;
        v.x = -v.x;
    }
    else if (pos.x > BOUND_X) {
        pos.x = BOUND_X;
        v.x = -v.x;
    }
    if (pos.y < 0) {
        pos.y = 0;
        v.y = -v.y;
    }
    else if (pos.y > BOUND_Y) {
        pos.y = BOUND_Y;
        v.y = -v.y;
    }
}

void do_bounce(const double m1, const double m2, Point& pos1, Point& pos2, Velocity& v1,
               Velocity& v2) {
    // simple workaround: invert the speed
    v1.x = -v1.x;
    v1.y = -v1.y;
    v2.x = -v2.x;
    v2.y = -v2.y;

    // move the bodies away from each other
    pos1 += v1 * DT;
    pos2 += v2 * DT;
}

void update_position(const std::vector<double>& m, std::vector<Point>& pos,
                     std::vector<Velocity>& v, const size_t n, const size_t i) {
    // X = X + V * dt
    pos[i] += v[i] * DT;
    // handle wall collision
    handle_wall_collision(pos[i], v[i]);
    // handle collision with other bodies and bounce
    for (size_t j = 0; j < n; ++j) {
        if (i == j) continue;
        if (pos[i].sqr_dist(pos[j]) < COLLISION_DIST2) {
            do_bounce(m[i], m[j], pos[i], pos[j], v[i], v[j]);
        }
    }
}

void update_velocity(const std::vector<double>& m, const std::vector<Point>& pos,
                     std::vector<Velocity>& v, const size_t n, const size_t i) {
    Force f{};
    // sum up all the forces
    for (size_t j = 0; j < n; ++j) {
        if (i == j) continue;
        f += get_force(m[i], m[j], pos[i], pos[j]);
    }
    // v += at
    v[i] += (f / m[i]) * DT;
}