#ifndef __Vec__
#define __Vec__

#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <random>
using namespace std;

class Point{
public:
    double x;
    double y;
    double z;
    Point() : x(0), y(0), z(0) {}
    Point(double x, double y, double z) : x(x), y(y), z(z) {}
};

class Vec{
public:
    Point o;
    double x;
    double y;
    double z;
    double norm;
    Vec() : o(Point()), x(0), y(0), z(0) {}
    Vec(Point o, double x, double y, double z) : o(o), x(x), y(y), z(z)
    {
        norm = sqrt(x * x + y * y + z * z);
    }
    Vec(Point o, Point a) : o(o)
    {
        x = a.x - o.x;
        y = a.y - o.y;
        z = a.z - o.z;
        norm = sqrt(x * x + y * y + z * z);
    }
    void normalize()
    {
        x /= norm;
        y /= norm;
        z /= norm;
    }
    Point pointing()
    {
        Point a(o);
        a.x += x;
        a.y += y;
        a.z += z;
        return a;
    }
    Vec operator+=(Vec b)
    {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }
    Vec operator-=(Vec b)
    {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        return *this;
    }
    Vec operator*=(double n)
    {
        x *= n;
        y *= n;
        z *= n;
        return *this;
    }
    // friend Vec operator+(Vec a, Vec b);
};

Vec random_vec(){
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0, 1);
    return Vec(Point(0, 0, 0), distribution(generator), distribution(generator), distribution(generator));
}

class Ray : public Vec{
public:
    Ray() : Vec() {}
    Ray(Point o, double x, double y, double z) : Vec(o, x, y, z)
    {
        (*this).normalize();
    }
    Ray(Ray& a){
        x = a.x;
        y = a.y;
        z = a.z;
        o = a.o;
        norm=x*x+y*y+z*z;
        (*this).normalize();
    }
    Ray(Vec v) : Vec(v)
    {
        (*this).normalize();
    }
    Ray(Point o, Point b) : Vec(o, b)
    {
        // cout<<norm<<"fesf\n";
        (*this).normalize();
    }
    Ray &operator=(Vec a)
    {
        x = a.x;
        y = a.y;
        z = a.z;
        o = a.o;
        norm=x*x+y*y+z*z;
        cout<<x<<" "<<y<<" "<<z<<"\n";
        (*this).normalize();
        return *this;
    }
};

double dot(Vec a, Vec b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec cross(Vec a, Vec b){
    return Vec(Point(0, 0, 0), a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

#endif