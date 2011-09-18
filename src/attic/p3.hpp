
#ifndef GLPOINT_H
#define GLPOINT_H

#include <QString>

namespace ocl {

/// simple 3D point-class
/// ocl normally uses double as the coordinate type
/// but for the cutting simulation single-precision float might be sufficient and faster
template <class Scalar>
class  P3 {
public:
    P3<Scalar>() {
        x=0;
        y=0;
        z=0;
    }
    P3<Scalar>(Scalar xi, Scalar yi, Scalar zi ) {
        x=xi;
        y=yi;
        z=zi;
    }
    Scalar x;
    Scalar y;
    Scalar z;
    
    P3<Scalar>& operator+=( const P3<Scalar>& p) {
        x+=p.x;
        y+=p.y;
        z+=p.z;
        return *this;
    }
    
    const P3<Scalar> operator+( const P3<Scalar> &p) const {
        return P3<Scalar>(*this) += p;
    }
    
    P3<Scalar>& operator-=( const P3<Scalar>& p) {
        x-=p.x;
        y-=p.y;
        z-=p.z;
        return *this;
    }
    
    const P3<Scalar> operator-( const P3<Scalar> &p) const {
        return P3<Scalar>(*this) -= p;
    }
    const P3<Scalar>  operator*(const Scalar &a) const {
        return P3<Scalar>(*this) *= a;
    }
    P3<Scalar>& operator*=(const Scalar &a) {
        x*=a;
        y*=a;
        z*=a;
        return *this;
    }
    Scalar norm() const {
        return sqrt( x*x + y*y + z*z );
    }
    void normalize() {
        if (this->norm() != 0.0)
            *this *=(1/this->norm());
    }
    P3<Scalar> cross(const P3<Scalar> &p) const {
        Scalar xc = y * p.z - z * p.y;
        Scalar yc = z * p.x - x * p.z;
        Scalar zc = x * p.y - y * p.x;
        return P3<Scalar>(xc, yc, zc);
    }
    
    Scalar dot(const P3<Scalar> &p) const {
        return x*p.x + y*p.y + z*p.z;
    }
    
    P3<Scalar> closestPoint(const P3<Scalar> &p1, const P3<Scalar> &p2) const {
        P3<Scalar> v = p2 - p1;
        assert( v.norm() > 0.0 );
        double u = (*this - p1).dot(v) / v.dot(v);  // u = (p3-p1) dot v / (v dot v)
        return p1 + v*u;
    }
    
    
    Scalar xyDistanceToLine(const P3<Scalar> &p1, const P3<Scalar> &p2) const {
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
            P3<Scalar> v( p2.y-p1.y, -(p2.x-p1.x), 0 );
            v.normalize();
            P3<Scalar> r(p1.x - x, p1.y - y, 0);
            return fabs( v.dot(r));
        }
    }

    
    void rotate(const P3<Scalar>& origin, const P3<Scalar>& v, Scalar alfa) {
        // rotate point p by alfa deg/rad around vector o->v
        // p = o + M*(p-o)
        Scalar M[3][3];
        Scalar c = cos(alfa);
        Scalar D = 1.0 - c;
        Scalar s = sin(alfa);
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
        Scalar vector[3];
        vector[0] = x - origin.x;
        vector[1] = y - origin.y;
        vector[2] = z - origin.z;
        Scalar result[3];
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
    
    QString str() { return QString("(%1, %2, %3 )").arg(x).arg(y).arg(z); }

};

} // end ocl namespace

#endif
