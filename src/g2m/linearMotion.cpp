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
#include "linearMotion.hpp"

#include <string>

#include "machineStatus.hpp"
#include "canonMotion.hpp"
#include "canonLine.hpp"


linearMotion::linearMotion(std::string canonL, machineStatus prevStatus): canonMotion(canonL,prevStatus) {
  status.setMotionType(getMotionType());
  status.setEndPose(getPoseFromCmd());
  //Point a,b;
  start = status.getStartPose().loc;
  end = status.getEndPose().loc;
  std::cout << " linear feed: " << start.str() << " to " << end.str() << "\n";
  
  
  //TODO: add support for 5 or 6 axis motion
  /*
  if (a.Distance(b) < Precision::Confusion()) { //is the edge long enough to bother making?
    unsolidErrors = true;
    cout << "skipped zero-length line at " << getLnum() << endl;
  } else {
    myUnSolid = BRepBuilderAPI_MakeEdge(a,b).Edge();
    //check if the swept shape will be accurate
    //use the ratio of rise or fall to distance
    double deltaZ = abs(a.Z() - b.Z());
    double dist = a.Distance(b);
    if ( (deltaZ/dist) > 0.001) {
      sweepIsSuspect = true;
    }
    gp_Dir endd(gp_Vec(a,b)); //end direction
    status.setEndDir(endd);
    status.setStartDir(endd); //for linearMotion, same as end.
  }*/
}
std::vector<Point> linearMotion::points() {
    std::vector<Point> output;
    output.push_back( start );
    output.push_back( end );
    return output;
}
//need to return RAPID for rapids...
MOTION_TYPE linearMotion::getMotionType() {
  bool traverse = cmdMatch("STRAIGHT_TRAVERSE");
  if (traverse) {
    return TRAVERSE;
  } else {
    return STRAIGHT_FEED;
  }
}

