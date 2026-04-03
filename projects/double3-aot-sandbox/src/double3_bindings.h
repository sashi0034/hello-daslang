#pragma once

struct Double3 {
    double x;
    double y;
    double z;
};

Double3 make_double3(double x, double y, double z);
Double3 add_double3(const Double3& lhs, const Double3& rhs);
Double3 scale_double3(const Double3& v, double s);
Double3 cpp_transform(const Double3& v);
void print_double3_from_cpp(const Double3& v);
