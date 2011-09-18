
#ifndef GLVERTEX_H
#define GLVERTEX_H

#include <cassert>
#include <cmath>
#include <QGLBuffer>
#include <QString>

namespace ocl
{

/// a vertex/point in 3D, with (x,y,z) coordinates of type GLfloat
/// normal is (nx,ny,nz)
/// color is (r,g,b)
struct GLVertex {
    GLVertex() : x(0), y(0), z(0), r(0), g(0), b(0) {}
    GLVertex(GLfloat x, GLfloat y, GLfloat z) 
         : x(x), y(y), z(z), r(0), g(0), b(0) {}
    GLVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b) 
         : x(x), y(y), z(z), r(r), g(g), b(b) {}
    void setNormal(GLfloat x, GLfloat y, GLfloat z) {
        nx=x;
        ny=y;
        nz=z;
        // normalize:
        GLfloat norm = sqrt( x*x+y*y+z*z );
        nx /= norm;
        ny /= norm;
        nz /= norm;
    }
    //void str() {
    //    std::cout << "(" << x << ", " << y << ", " << z << ")"; 
    //}
    QString str() { return QString("(%1, %2, %3 )").arg(x).arg(y).arg(z); }
    
// DATA
    GLfloat x,y,z; // position
    GLfloat r,g,b; // color, 12-bytes offset from position data.
    GLfloat nx,ny,nz; // normal, 24-bytes offset
    
// Operators etc
    GLfloat norm() const {
        return sqrt( x*x + y*y + z*z );
    }
    void normalize() {
        if (this->norm() != 0.0)
            *this *=(1/this->norm());
    }
    GLVertex& operator*=(const double &a) {
        x*=a;
        y*=a;
        z*=a;
        return *this;
    }
    GLVertex  operator*(const double &a) const {
        return GLVertex(*this) *= a;
    }
    GLVertex& operator+=( const GLVertex& p) {
        x+=p.x;
        y+=p.y;
        z+=p.z;
        return *this;
    }
    
    const GLVertex operator+( const GLVertex &p) const {
        return GLVertex(*this) += p;
    }
    
    GLVertex& operator-=( const GLVertex& p) {
        x-=p.x;
        y-=p.y;
        z-=p.z;
        return *this;
    }
    
    const GLVertex operator-( const GLVertex &p) const {
        return GLVertex(*this) -= p;
    }
    GLVertex cross(const GLVertex &p) const {
        GLfloat xc = y * p.z - z * p.y;
        GLfloat yc = z * p.x - x * p.z;
        GLfloat zc = x * p.y - y * p.x;
        return GLVertex(xc, yc, zc);
    }
    
    GLfloat dot(const GLVertex &p) const {
        return x*p.x + y*p.y + z*p.z;
    }
    
    GLVertex closestPoint(const GLVertex &p1, const GLVertex &p2) const {
        GLVertex v = p2 - p1;
        assert( v.norm() > 0.0 );
        double u = (*this - p1).dot(v) / v.dot(v);  // u = (p3-p1) dot v / (v dot v)
        return p1 + v*u;
    }
    GLfloat xyDistanceToLine(const GLVertex &p1, const GLVertex &p2) const {
        // see for example
        // http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
        if ((p1.x == p2.x) && (p1.y == p2.y)) {// no line in xy plane
            std::cout << "point.cpp: xyDistanceToLine ERROR!: can't calculate distance from \n";
            //std::cout << "point.cpp: xyDistanceToLine ERROR!: *this ="<<*this <<" to line through\n";
            //std::cout << "point.cpp: xyDistanceToLine ERROR!: p1="<<p1<<" and \n";
            //std::cout << "point.cpp: xyDistanceToLine ERROR!: p2="<<p2<< "\n";
            //std::cout << "point.cpp: xyDistanceToLine ERROR!: in the xy-plane\n";
            return -1;
        }
        else {
            GLVertex v( p2.y-p1.y, -(p2.x-p1.x), 0 );
            v.normalize();
            GLVertex r(p1.x - x, p1.y - y, 0);
            return fabs( v.dot(r));
        }
    }
    
    
    void rotate(const GLVertex& origin, const GLVertex& v, GLfloat alfa) {
        // rotate point p by alfa deg/rad around vector o->v
        // p = o + M*(p-o)
        GLfloat M[3][3];
        GLfloat c = cos(alfa);
        GLfloat D = 1.0 - c;
        GLfloat s = sin(alfa);
        M[0][0] = v.x*v.x*D+c; 
        M[0][1] = v.y*v.x*D+v.z*s; 
        M[0][2] = v.z*v.x*D-v.y*s;
        M[1][0] = v.x*v.y*D-v.z*s;
        M[1][1] = v.y*v.y*D+c;
        M[1][2] = v.z*v.y*D+v.x*s;
        M[2][0] = v.x*v.z*D+v.y*s;
        M[2][1] = v.y*v.z*D-v.x*s;
        M[2][2] = v.z*v.z*D+c;
        // matrix multiply
        GLfloat vector[3];
        vector[0] = x - origin.x;
        vector[1] = y - origin.y;
        vector[2] = z - origin.z;
        GLfloat result[3];
        for (int i=0; i < 3; i++) {
            result[i]=0;
            for (int j=0; j < 3; j++) {
                result[i]+=vector[j]*M[i][j];
            }
        }
        x = origin.x + result[0];
        y = origin.y + result[1];
        z = origin.z + result[2];
    }
    
};

} // end namespace

#endif
