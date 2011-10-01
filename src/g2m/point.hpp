
#include <cmath>
#include <sstream>

#ifndef POINT_HPP
#define POINT_HPP

namespace g2m {

struct Point {
    Point():x(0),y(0),z(0) {}
    Point(double a, double b, double c):x(a),y(b),z(c) {}
    Point(const Point& other): x(other.x),y(other.y),z(other.z) {}
    double Distance( Point other ) {
        return sqrt( (other.x-x)*(other.x-x) + (other.y-y)*(other.y-y) + (other.z-z)*(other.z-z) );
    }
    
    std::string str() const {
        std::ostringstream o;
        o << "(" << x << ", " << y << ", " << z << ")";
        return o.str();
    }
    Point& operator*=(const double &a) {
        x*=a;
        y*=a;
        z*=a;
        return *this;
    }
    Point  operator*(const double &a) const {
        return Point(*this) *= a;
    }
    Point& operator+=( const Point& p) {
        x+=p.x;
        y+=p.y;
        z+=p.z;
        return *this;
    }
    
    const Point operator+( const Point &p) const {
        return Point(*this) += p;
    }
    
    Point& operator-=( const Point& p) {
        x-=p.x;
        y-=p.y;
        z-=p.z;
        return *this;
    }
    
    const Point operator-( const Point &p) const {
        return Point(*this) -= p;
    }
    
// DATA
    double x,y,z;
};

struct Pose {
    Pose() {}
    Pose( Point a, Point b ) {
        loc = a;
        dir = b;
    }
    Point loc;
    Point dir;
};

} // end namespace
#endif 
