#pragma once

struct Double2 {
    double x;
    double y;
};

struct Double3 {
    double x;
    double y;
    double z;
};

Double2 make_double2(double x, double y);
Double2 add_double2(const Double2& lhs, const Double2& rhs);
Double2 scale_double2(const Double2& v, double s);
void print_double2_from_cpp(const Double2& v);
Double3 lift_double2_to_double3(const Double2& xy, double z);

Double3 make_double3(double x, double y, double z);
Double3 add_double3(const Double3& lhs, const Double3& rhs);
Double3 scale_double3(const Double3& v, double s);
Double3 cpp_transform(const Double3& v);
void print_double3_from_cpp(const Double3& v);
