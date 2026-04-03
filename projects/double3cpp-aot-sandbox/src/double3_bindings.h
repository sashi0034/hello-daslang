#pragma once

struct Double3;

struct Double2 {
    double x;
    double y;

    Double2();
    Double2(double xValue, double yValue);

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

    Double3 add(const Double3& rhs) const;
    Double3 scale(double factor) const;
    Double3 transform() const;
    void print() const;
};

void print_double3_from_cpp(const Double3& v);
