#ifndef __Body__
#define __Body__
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <random>
#include "Vec.cpp"

using namespace std;

#ifndef __Err__
#define __Err__
const double err=0.000001;
#endif

struct colour{
    short r;
    short g;
    short b;
    colour():r(0),g(0),b(0){};
    colour(short r, short g, short b): r(r),g(g),b(b){};
    colour& operator*= (double x){
        r*=x;
        g*=x;
        b*=x;
        r=min(short(255),r);
        g=min(short(255),g);
        b=min(short(255),b);
        return *this;
    }
    colour operator+ (colour& a){
        colour y(min(r+a.r,255),min(g+a.g,255),min(b+a.b,255));
        return y;
    }
    friend colour operator/(colour& a, int b){
        a.r/=b;
        a.g/=b;
        a.b/=b;
        return a;
    }
};

class Body
{
public:
    colour col;
    Body() : col(0, 255, 0) {}
    Body(colour c) : col(c) {}
    virtual bool intersect(Ray ray) = 0;
    virtual Point intersection(Ray ray) = 0;
    virtual Ray normal(Point x) = 0;
};

class Sfera : public Body
{
public:
    double r;
    Point s;
    Sfera(Point s, double r) : s(s), r(r){}
    Sfera(Point s, double r, colour c) : Body(c), s(s), r(r){}
    bool intersect(Ray ray) override;
    Point intersection(Ray ray) override;
    Ray normal(Point x)
    {
        return Ray(s, x);
    }
};

bool Sfera::intersect(Ray ray)
{
    Vec dir(ray.o, s);
    if(dot(dir,ray)<0) return false;
    double norma = dir.norm;
    dir.normalize();
    double cos = dot(ray, dir);
    double x = norma * sqrt(1 - cos * cos);
    if (x < r)
        return true;
    return false;
}

Point Sfera::intersection(Ray ray)
{
    Vec dir(ray.o, s);
    double norma = dir.norm;
    dir.normalize();
    double cos1 = dot(ray, dir);
    Vec z(ray);
    Vec z2(ray);
    z *= norma * cos1;
    Vec x(s, z.pointing());
    double cos2 = x.norm / r;
    double y = r * sqrt(1 - cos2 * cos2);
    z2 *= (norma * cos1 - y);
    return z2.pointing();
}

class Plane : public Body
{
    Point inter;

public:
    Ray normalny;
    Point a, b, c;
    Plane(Point a, Point b, Point c) : a(a), b(b), c(c)
    {
        normalny = cross(Vec(a, b), Vec(a, c));
        //cout<< normalny.x<<" "<< normalny.y<<" "<< normalny.z<<"\n";
        normalny.o = a;
    }
    // Plane (short r, short g, short b){}
    Ray normal(Point x)
    {
        return normalny;
    }
    bool intersect(Ray ray) override;
    Point intersection(Ray ray) override
    {
        return inter;
    }
};

bool Plane::intersect(Ray ray)
{
    Vec e1(a, b);
    Vec e2(a, c);
    //cout<<e1.x<<" "<<e1.y<<" "<<e1.z<<"\n";
    Vec cross_e2 = cross(ray, e2);
    double det = dot(e1, cross_e2);
    //cout<<det<<"\n";
    if (det < err && det > -err)
        return false;
    double inv_det = 1 / det;
    //cout<<inv_det<<"\n";
    Vec s(a, ray.o);
    double u = inv_det * dot(s, cross_e2);
    Vec cross_e1 = cross(s, e1);
    double v = inv_det * dot(ray, cross_e1);
    if ((u < 0 && -u > err) || (v < 0 && -v > err) || u + v > 1 + err){
        //cout<<"test1";
        return false;
    }
    double t = inv_det * dot(e2, cross_e1);
    if (t < err){
        //cout<<"test2";
        return false;
    }
    ray *= t;
    inter = ray.pointing();
    return true;
    /*
    double d;
    Point inter;
    Vec temp(ray);
    d=dot(Ray(normal.o,ray.o),normal)/dot(ray,normal);
    temp*=d;
    inter=temp.pointing();
    if(inter.x-plane.o.x<)*/
}

#endif