/***************************************************************************
 *   Copyright (C) 2010 by Mark Pictor                                     *
 *   mpictor@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef HELICALMOTION_HH
#define HELICALMOTION_HH

#include <string>
#include <vector>
#include <cmath>
#include <limits.h>

#include "point.hpp"
#include "canonMotion.hpp"
#include "canonLine.hpp"
#include "machineStatus.hpp"

namespace g2m {

/**
\class helicalMotion
\brief For the canonical command ARC_FEED.
This class handles both planar arcs and helical arcs. Inherits from canonMotion.
*/

class helicalMotion: protected canonMotion {
  friend canonLine* canonLine::canonLineFactory(std::string l, machineStatus s);
  public:
    helicalMotion(std::string canonL, machineStatus prevStatus);
    MOTION_TYPE getMotionType() {return HELICAL;};
    std::vector<Point> points();
  private:

    
    // rotate by cos/sin. from emc2 gcodemodule.cc
    static void rotate(double &x, double &y, double c, double s) {
        double tx = x * c - y * s;
        y = x * s + y * c;
        x = tx;
    }

    //void arc();
    //void arc(gp_Pnt start, gp_Vec startVec, gp_Pnt end);
    //bool planar;
    //Point center;
    //Point axis;
    Point start, end;
    //double radius;
    //double hdist;
    //int rotation;
    
    double x1,y1,z1; // endpoint for this move
    double a,b,c; // abc axis endpoints
    double rot; // rotation for this move
    double cx,cy; // center-point for this move

};

} // end namespace 

#endif //HELICALMOTION_HH
