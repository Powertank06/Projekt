#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
using namespace std;

class Point{
public:
    double x;
    double y;
    double z;
    Point(): x(0),y(0),z(0){}
    Point(double x,double y,double z): x(x),y(y),z(z){}
};

class Vec{
public:
    Point o;
    double x;
    double y;
    double z;
    double norm;
    Vec(): o(Point()),x(0),y(0),z(0){}
    Vec(Point o, double x, double y, double z): o(o),x(x),y(y),z(z){
        norm=sqrt(x*x+y*y+z*z);
    }
    Vec(Point o, Point a): o(o){
        x=a.x-o.x;
        y=a.y-o.y;
        z=a.z-o.z;
        norm=sqrt(x*x+y*y+z*z);
    }
    void normalize(){
        x/=norm;
        y/=norm;
        z/=norm;
    }
    Point pointing(){
        Point a(o);
        a.x+=x;
        a.y+=y;
        a.z+=z;
        return a;
    }
    Vec operator+=(Vec b){
        x+=b.x;
        y+=b.y;
        z+=b.z;
        return *this;
    }
    Vec operator-=(Vec b){
        x-=b.x;
        y-=b.y;
        z-=b.z;
        return *this;
    }
    Vec operator*=(double n){
        x*=n;
        y*=n;
        z*=n;
        return *this;
    }
    //friend Vec operator+(Vec a, Vec b);
};

class Ray: public Vec{
public:
    Ray(Vec v): Vec(v){
        (*this).normalize();
    }
    Ray(Point o, Point b): Vec(o,b){
        //cout<<norm<<"fesf\n";
        (*this).normalize();
    }
};

double dot(Vec a, Vec b){
    return a.x*b.x+a.y*b.y+a.z*b.z;
}

class Sfera{
public:
    double r;
    Point s;
    Sfera(Point s, double r): s(s), r(r){}
    bool intersect(Ray ray){
        Vec dir(s,ray.o);
        double cos=dot(ray,ray);
        double x= dir.norm*sqrt(1-cos*cos);
        if(x<r) return true;
        return false;
    }
    Point intersection (Ray ray){
        Vec dir(ray.o,s);
        double cos1=dot(ray,ray);
        Vec z(ray);
        Vec z2(ray);
        z*=dir.norm*cos1;
        Vec x(s,z.pointing());
        double cos2=x.norm/r;
        double y=r*sqrt(1-cos2*cos2);
        z2*=(dir.norm*cos1-y);
        return z2.pointing();
    }
};

class Scena{
public:
    vector<Sfera> obiekty;
    double dist;
};

int main(){
    Sfera sfera(Point(2,-0.5,0),1);
    Ray ray(Point(0,0,0),Point(1,0,0));
    Point a;
    a=sfera.intersection(ray);
    cout<<a.x<<" "<<a.y<<" "<<a.z;

}
