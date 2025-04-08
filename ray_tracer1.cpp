#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <fstream>
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
    Ray() : Vec(){} 
    Ray(Point o, double x, double y, double z): Vec(o,x,y,z){
        (*this).normalize();
    }
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

class Body{
public:
    short red,green,blue;
    Body(): red(0), green(255), blue(0){}
    Body(short r,short g,short b): red(r),green(g),blue(b){}
    virtual bool intersect(Ray ray)=0;
    virtual Point intersection(Ray ray)=0;
    virtual Ray normal(Point x)=0;
};

class Sfera: public Body{
public:
    double r;
    Point s;
    Sfera(Point s, double r): s(s), r(r){
        red=0;
        green=255;
        blue=0;
    }
    bool intersect(Ray ray){
        Vec dir(ray.o,s);
        double norma=dir.norm;
        dir.normalize();
        double cos=dot(ray,dir);
        double x= norma*sqrt(1-cos*cos);
        if(x<r) return true;
        return false;
    }
    Point intersection (Ray ray){
        Vec dir(ray.o,s);
        double norma=dir.norm;
        dir.normalize();
        double cos1=dot(ray,dir);
        Vec z(ray);
        Vec z2(ray);
        z*=norma*cos1;
        Vec x(s,z.pointing());
        double cos2=x.norm/r;
        double y=r*sqrt(1-cos2*cos2);
        z2*=(norma*cos1-y);
        return z2.pointing();
    }
    Ray normal(Point x){
        return Ray(s,x);
    }
};
/*

class Plane: public Body{
public:
    Ray plane;
    double 
    Plane(Ray p, double a, double b): plane(p),a(a),b(b){}
    //Plane (short r, short g, short b){}
    Ray normal(Point x){
        return plane;
    }
    bool intersect(Ray ray){
        double d;
        Point inter;
        Vec temp(ray);
        d=dot(Ray(plane.o,ray.o),plane)/dot(ray,plane);
        temp*=d;
        inter=temp.pointing();
        if(inter.x-plane.o.x<)
    }
};*/

class Scena{
public:
    int xres;
    int yres;
    short rback,gback,bback;
    vector<Body*> obiekty;
    double ratio;
    Point sun;
    Ray cam;

    Scena(): xres(360), yres(240), rback(50), gback(50), bback(50){
        ratio=1;
        sun=Point(-5,20,40);
        cam=Ray(Point(0,0,0),1,0,0);
    }
    Scena(int xres, int yres, int r, int g, int b): xres(xres), yres(yres), rback(r), gback(g), bback(b){
        ratio=3;
        sun=Point(0,0,40);
        cam=Ray(Point(0,0,0),1,0,0);
    }
    void add(Sfera a){
        obiekty.push_back(&a);
    }
    void render(){
        ofstream out("image.ppm",ios::out | ios::trunc);
        out<<"P3"<<'\n';
        out<<2*xres<<' '<<2*yres<<'\n'<<255<<'\n';
        //cout<<"test\n";
        bool test=0;
        double err=0.000001;
        for(int y=yres;y>=-yres;y--){
            for(int x=xres;x>=-xres;x--){
                if(x!=0 && y!=0){
                Ray ray(cam.o,1,ratio*x/xres,ratio*y/xres);
                //cout<<ray.pointing().x<<" "<<ray.pointing().y<<" "<<ray.pointing().z<<"\n";
                bool intersects=false;
                Point intersection=cam.o;
                double mindist=999999;
                short r,g,b;
                Body* ptr=0;
                for(auto &a: obiekty){
                    //double dist=Vec(cam.o,a.s).norm;
                    if(a->intersect(ray)){
                        double dist=Vec(cam.o,a->intersection(ray)).norm;
                        if(dist<mindist){
                            mindist=dist;
                            intersection = a->intersection(ray);
                            intersects=true;
                            r=a->red;
                            g=a->green;
                            b=a->blue;
                            ptr=a;
                        }
                    }
                }
                
                if(intersects){
                    bool lit=true;
                    double distInter=Vec(sun,intersection).norm;
                    Ray shadow(sun,intersection);
                    for(auto &b: obiekty){
                        /*
                        if(test==0){
                            cout<<ptr<<" "<<&b<<"\n";
                            //test=1;
                        }*/

                        if(b->intersect(shadow) && Vec(sun,b->intersection(shadow)).norm<distInter-err){
                            lit=false;
                            break;
                        }
                    }

                    if(lit){
                        /*
                        if(distInter>0.1){
                            r*=5/log(distInter);
                            g*=10/(distInter*distInter);
                            b*=10/(distInter*distInter);
                        }*/

                        Ray shadow2 (intersection,sun);
                        double k=dot(shadow2,ptr->normal(intersection));
                        out<<k*r<<' '<<k*g<<' '<<k*b<<'\n';
                    }
                    else{
                        out<<0<<' '<<0<<' '<<0<<'\n';
                    }
                }
                else{
                    out<<rback<<' '<<gback<<' '<<bback<<'\n';
                }
                }
            }
        }
    }
    
};

int main(){
    Sfera s1(Point(3,-1,0),1);
    Sfera s2(Point(4,0,0),2);
    Sfera s3(Point(2,1,0),0.2);
    Scena scena;
    scena.add(s1);
    scena.add(s2);
    scena.add(s3);
    scena.render();
}
