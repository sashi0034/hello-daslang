#pragma once

struct Double3;

struct Double2 {
    double x;
    double y;

    Double2();
    Double2(double xValue, double yValue);

    Double2 operator+(const Double2& rhs) const;
    Double2 add(const Double2& rhs) const;
    Double2 scale(double factor) const;
    Double3 toDouble3(double zValue) const;
    void print() const;
};

struct Double3 {
    double x;
    double y;
    double z;

    Double3();
    Double3(double xValue, double yValue, double zValue);

    Double3 operator+(const Double3& rhs) const;
    Double3 add(const Double3& rhs) const;
    Double3 scale(double factor) const;
    Double3 transform() const;
    void print() const;
};

Double2 double2_add_op(const Double2& lhs, const Double2& rhs);
Double2 double2_scale_op(const Double2& value, double factor);
Double3 double3_add_op(const Double3& lhs, const Double3& rhs);
Double3 double3_scale_op(const Double3& value, double factor);

void print_double3_from_cpp(const Double3& v);
