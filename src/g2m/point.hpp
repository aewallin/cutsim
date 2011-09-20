

#ifndef POINT_HPP
#define POINT_HPP

struct Point {
    Point():x(0),y(0),z(0) {}
    Point(double a, double b, double c):x(a),y(b),z(c) {}
    double x,y,z;
};
typedef Point gp_Dir;
typedef Point gp_Pnt;
struct gp_Ax1 {
    gp_Ax1() {}
    gp_Ax1( gp_Pnt a, gp_Dir b ) {
        loc = a;
        vdir = b;
    }
    Point loc;
    Point vdir;
};


#endif 
