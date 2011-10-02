/*  
 *  Copyright 2010 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  
 *  This file is part of OpenCAMlib.
 *
 *  OpenCAMlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenCAMlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OpenCAMlib.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BBOX_H
#define BBOX_H

#include "glvertex.hpp"

namespace cutsim {
    


/// axis-aligned bounding-box
class Bbox {
    public:
        //typedef P3<double> Point;
        Bbox();
        Bbox(double b1, double b2, double b3, double b4, double b5, double b6);
        virtual ~Bbox() {};
        /// index into maxpt and minpt returning a vector
        /// [minx maxx miny maxy minz maxz]
        double operator[](const unsigned int idx) const;        
        /// return true if Point p is inside this Bbox
        bool isInside(GLVertex& p) const;
        /// return true if *this overlaps Bbox b
        bool overlaps(const Bbox& other) const;
        /// reset the Bbox (sets initialized=false)
        void clear();
        /// Add a Point to the Bbox.
        /// This enlarges the Bbox so that p is contained within it.
        void addPoint(const GLVertex& p);
        /// Add each vertex of a Triangle to the Bbox.
        /// This enlarges the Bbox so that the Triangle is contained within it.
        /// Calls addPoint() for each vertex of the Triangle.
        //void addTriangle(const Triangle& t);
        friend std::ostream &operator<<(std::ostream& stream, const Bbox b);
//DATA
        /// the maximum point
        GLVertex maxpt; 
        /// the minimum point
        GLVertex minpt; 
    private:
        /// false until one Point or one Triangle has been added
        bool initialized;
};

} // end namespace
#endif
// end file bbox.h
