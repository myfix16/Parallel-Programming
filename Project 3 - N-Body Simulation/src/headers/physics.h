#pragma once

#ifndef __device__
#define __device__
#endif

constexpr auto GRAVITY_CONST = 1e5;
constexpr auto DT = 1e-4;
constexpr auto FLOAT_OP_ERROR = 1e-5;
constexpr auto RADIUS2 = 0.01;
constexpr auto BOUND_X = 4000;
constexpr auto BOUND_Y = 4000;
constexpr auto MAX_MASS = 400;
constexpr auto COLLISION_DIST2 = 4 * RADIUS2;

/*
 * ->: x increases
 * up: y increases
 */

// vector in physics
struct Vector {
    double x;
    double y;

    __device__ Vector operator+(const Vector& rhs) const { return Vector{x + rhs.x, y + rhs.y}; }

    __device__ Vector operator-(const Vector& rhs) const { return Vector{x - rhs.x, y - rhs.y}; }

    __device__ Vector operator*(const double dt) const { return Vector{x * dt, y * dt}; }

    __device__ Vector operator/(const double dt) const { return Vector{x / dt, y / dt}; }

    __device__ Vector operator-() const { return Vector{-x, -y}; }

    Vector& operator=(const Vector& rhs) = default;

    __device__ Vector& operator+=(const Vector& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    __device__ Vector& operator-=(const Vector& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
};

struct Point : Vector {
    __device__ double sqr_dist(const Vector& rhs) const {
        const Vector diff = *this - rhs;
        return diff.x * diff.x + diff.y * diff.y;
    }
};

struct Velocity : Vector {
    Velocity() = default;
    __device__ Velocity(const double vx, const double vy) : Vector{vx, vy} {}
};

using Force = Velocity;

// Get the gravity force that body 2 exerts on body 1
Force get_force(double m1, double m2, const Point& pos1, const Point& pos2);

void handle_wall_collision(Point& pos, Velocity& v);

void do_bounce(double m1, double m2, Point& pos1, Point& pos2, Velocity& v1, Velocity& v2);

void update_position(const std::vector<double>& m, std::vector<Point>& pos,
                     std::vector<Velocity>& v, size_t n, size_t i);

void update_velocity(const std::vector<double>& m, const std::vector<Point>& pos,
                     std::vector<Velocity>& v, size_t n, size_t i);