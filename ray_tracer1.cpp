#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <random>
#include "Vec.cpp"
#include "Body.cpp"
using namespace std;

#ifndef __Err__
#define __Err__
const double err=0.000001;
#endif

double frac(short col){
    return (col)/255;
}

class Scena{
    Ray make_ray(int x, int y);
    int samples=5;
    void ray_intersections(Point& intersection, bool& intersects, Body*& ptr, colour& pixel, Ray ray);
    static double reflectance(double ior, double cos);
public:
    int xres;
    int yres;
    int max_bounces=10;
    int bounces;
    colour background;
    vector<Body*> obiekty;
    double ratio;
    Point sun;
    Ray cam;

    Scena(): xres(360), yres(240), background(50,50,50){
        ratio=1;
        sun=Point(-5,20,40);
        cam=Ray(Point(0,0,0),1,0,0);
    }
    Scena(int xres, int yres, int r, int g, int b): xres(xres), yres(yres), background(r,g,b){
        ratio=3;
        sun=Point(0,0,40);
        cam=Ray(Point(0,0,0),1,0,0);
    }
    void add(Body &a){
        obiekty.push_back(&a);
    }
    void set_bg(colour b){
        background=b;
    }
    colour iter(Ray ray);
    void render();
};

double Scena::reflectance(double ior, double cos){
    double r0=(1-ior)/(1+ior);
    r0*=r0;
    return r0+(1-r0)*std::pow((1-cos),5);
}

Ray Scena::make_ray (int x, int y){
    static std::uniform_real_distribution<double> distribution(0, 1.0);
    static std::mt19937 generator;
    double a=distribution(generator);
    double b=distribution(generator);
    //cout<<x+a-0.5<<" "<<y+b-0.5<<"\n";
    return Ray(cam.o,1,ratio*(x+a-0.5)/xres,ratio*(y+b-0.5)/xres);
}

void Scena::ray_intersections(Point& intersection, bool& intersects, Body*& ptr, colour& pixel, Ray ray){
    intersection=ray.o;
    double mindist=999999;
    for(auto &a: obiekty){
        //cout<<"test ";
        //double dist=Vec(cam.o,a.s).norm;
        if(a->intersect(ray)){
            //cout<<"test";
            double dist=Vec(ray.o,a->intersection(ray)).norm;
            if(dist<mindist){
                mindist=dist;
                intersection = a->intersection(ray);
                intersects=true;
                pixel=a->col;
                ptr=a;
            }
        }
    }
}

colour Scena::iter(Ray ray){
    if(bounces==max_bounces){
        //cout<<"test\n";
        return colour(0,0,0);
    }
    bounces++;
    bool intersects=false;
    Point intersection;
    colour pixel;
    Body* ptr=0;
    ray_intersections(intersection, intersects, ptr, pixel, ray);
    //cout<<intersects;
    
    if(intersects){
        Ray N=ptr->normal(intersection);
        if(ptr->reflect==1){
            static std::uniform_real_distribution<double> distribution(0, 1.0);
            static std::mt19937 generator;
            double a=distribution(generator);
            double ior = ptr->inner(ray,intersection)? (1-ptr->ior) : ptr->ior;
            if(a<reflectance(ior,-dot(ray,N)) || ior*sqrt(1-dot(ray,N)*dot(ray,N)>1-err)){
                Ray refl=ray+(-2*dot(ray,ptr->normal(intersection)))*N;
                refl.o=intersection;
                return iter(refl);
            }
            else{
                //Ray ep=(-dot(N,ray))*N+ray;
                Vec p=ior*(ray+(-dot(ray,N)*N));
                Vec r=(-sqrt(1-dot(p,p)))*N;
                Ray out=p+r;
                return iter(out);
            }
        }
        bool lit=true;
        double distInter=Vec(sun,intersection).norm;
        Ray shadow(sun,intersection);
        int test;
        for(auto &b: obiekty){
            if(b->intersect(shadow) && Vec(sun,b->intersection(shadow)).norm<distInter-err && Vec(intersection,b->intersection(shadow)).norm<distInter){
                lit=false;
                //cout<<"test1";
                break;
            }
        }
        if( dot(Vec(intersection,sun),N) * dot(Vec(intersection, ray.o),N) < 0){
            lit=false;
        }
        //cout<<"test";
        if(lit){
            Vec v;
            do{
                v=random_vec();
            }while(v.norm>1 || v.norm==0);
            v.normalize();
            Vec v2=ptr->normal(intersection);
            v2+=v;
            colour out=iter(Ray(v2));
            Ray shadow2 (intersection,sun);
            double k=abs(dot(shadow2,ptr->normal(intersection)))-err;
            pixel*=k;
            return colour( frac(ptr->col.r)*out.r,frac(ptr->col.g)*out.g,frac(ptr->col.b)*out.b )+pixel;
            //return colour(255,0,0);          
        }
        else{
            //cout<<"test\n";
            Vec v;
            do{
                v=random_vec();
            }while(v.norm>1 || v.norm==0);
            v.normalize();
            Vec v2=ptr->normal(intersection);
            v2+=v;
            colour out=iter(Ray(v2));
            return colour( frac(ptr->col.r)*out.r,frac(ptr->col.g)*out.g,frac(ptr->col.b)*out.b );
            //return colour(0,0,255);
        }
    }
    else{
        return background;
    }
}

void Scena::render(){
    ofstream out("image.ppm",ios::out | ios::trunc);
    out<<"P3"<<'\n';
    out<<2*xres<<' '<<2*yres<<'\n'<<255<<'\n';
    //cout<<"test\n";
    bool test=0;
    for(int y=yres;y>=-yres;y--){
        for(int x=xres;x>=-xres;x--){
            if(x!=0 && y!=0){
            //Ray ray(cam.o,1,ratio*x/xres,ratio*y/xres);
            //cout<<ray.pointing().x<<" "<<ray.pointing().y<<" "<<ray.pointing().z<<"\n";
            colour pixel;
            for(int i=0;i<samples;i++){
                bounces=0;
                colour temp=this->iter(make_ray(x,y));
                temp=temp/samples;
                pixel=pixel+temp;
            }
            //pixel=pixel/samples;
            //if(pixel.r==0) cout<<"test5";
            out<<pixel.r<<' '<<pixel.g<<' '<<pixel.b<<'\n';
            }
        }
    }
}


int main(){
    Sfera s1(Point(3,-1,0),1);
    Sfera s2(Point(0,0,-55),50,colour(70,70,70),0,0);
    Sfera s3(Point(4,3,0),0.2);
    Sfera s4(Point(1,0,0),0.4,colour(100,100,100),1,1.4);
    Plane p1(Point(1.5,1,-1),Point(6,5,-1),Point(6,-2,-1));
    p1.reflect=1;
    Scena scena;
    scena.add(s1);
    scena.add(s2);
    scena.add(s3);
    scena.add(s4);
    scena.add(p1);
    //Ray ray(Point(0,0,0),1,0,-1);
    //cout<<p1.intersect(ray)<<"\n";
    //cout<<p1.intersection(ray).x<<" "<<p1.intersection(ray).y<<" "<<p1.intersection(ray).z;
    scena.set_bg(colour(120,120,230));
    scena.render();
}
