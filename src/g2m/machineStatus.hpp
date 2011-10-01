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
#ifndef MACHINESTATUS_HH
#define MACHINESTATUS_HH

#include <map>
#include <limits.h> //to fix "error: INT_MIN was not declared in this scope"
#include "point.hpp"

namespace g2m {

enum CANON_PLANE {CANON_PLANE_XY, CANON_PLANE_YZ, CANON_PLANE_XZ};
enum SPINDLE_STATUS {OFF,CW,CCW,BRAKE};
enum MOTION_TYPE { NOT_DEFINED, MOTIONLESS, HELICAL, STRAIGHT_FEED, TRAVERSE };

struct coolantStruct {
    bool flood; 
    bool mist; 
    bool spindle;
};

/**
\class machineStatus
\brief This class contains the machine's state for one canonical command.
The information stored includes the 
* coolant state, spindle speed and direction, feedrate, start and end pose, tool in use

Important: 'pose' refers to how the machine's axes are positioned, 
* while 'direction' refers to the direction of motion
*/
class machineStatus {
  public:
    machineStatus(machineStatus const& oldStatus);
    machineStatus(Pose initial);
    void setMotionType(MOTION_TYPE m);
    void setEndPose(Pose newPose);
    void setEndPose(Point p);
    //void addToBounds();
    void setFeed(const double f) {F=f;};
    void setSpindleSpeed(const double s) {S=s;};
    void setSpindleStatus(const SPINDLE_STATUS s) {spindleStat=s;};
    void setCoolant(coolantStruct c) {coolant = c;};
    void setPlane(CANON_PLANE p) {plane = p;};
    double getFeed() const {return F;};
    double getSpindleSpeed() const {return S;};
    SPINDLE_STATUS getSpindleStatus() const {return spindleStat;};
    const coolantStruct getCoolant() {return coolant;};
    const Pose getStartPose() {return startPose;};
    const Pose getEndPose() {return endPose;};
    CANON_PLANE getPlane() const {return plane;};
    void setStartDir( Point d) {startDir = d;};
    const Point getStartDir() const {return startDir;};
    void setEndDir( Point d) {endDir = d;};
    const Point getEndDir() const {return endDir;};
    const Point getPrevEndDir() const {return prevEndDir;};
    void clearAll(void);
    bool isFirst() {return first;};
    
    void setTool(int n); //n is the ID of the tool to be used.
    int  getTool() const {return myTool;};
  protected:
    Pose startPose, endPose;
    double F,S;  //feedrate, spindle speed
    coolantStruct coolant;
    Point startDir, endDir, prevEndDir;
    bool first,lastMotionWasTraverse;
    int myTool;
    MOTION_TYPE motionType;
    SPINDLE_STATUS spindleStat;
    CANON_PLANE plane;
  private:
    machineStatus();  //prevent use of this ctor by making it private 
};

} // end namespace

#endif //MACHINESTATUS_HH
