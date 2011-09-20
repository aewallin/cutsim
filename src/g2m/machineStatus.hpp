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
//#include <Bnd_Box.hxx>
//#include <TopoDS_Edge.hxx>
//#include <gp_Pnt.hxx>

//#include "tool.hh"
//#include "canon.hh"

#include "point.hpp"

enum CANON_PLANE {CANON_PLANE_XY, CANON_PLANE_YZ, CANON_PLANE_XZ};
struct coolantStruct {
    bool flood; 
    bool mist; 
    bool spindle;
};
enum SPINDLE_STATUS {OFF,CW,CCW,BRAKE};

enum MOTION_TYPE { NOT_DEFINED, MOTIONLESS, HELICAL, STRAIGHT_FEED, TRAVERSE };
struct pntPair {
    Point a,b;
};

typedef int toolNumber;

/**
\class machineStatus
\brief This class contains the machine's state for one canonical command.
The information stored includes the 
* coolant state, spindle speed and direction, feedrate, start and end pose, tool in use

Important: 'pose' refers to how the machine's axes are positioned, 
* while 'direction' refers to the direction of motion
*/
class machineStatus {
  protected:
    gp_Ax1 startPose, endPose;
    double F,S;  //feedrate, spindle speed
    SPINDLE_STATUS spindleStat;
    coolantStruct coolant;
    gp_Dir startDir, endDir, prevEndDir;
    bool first,lastMotionWasTraverse;
    //static Bnd_Box traverseBbox,feedBbox;
    MOTION_TYPE motionType;
    toolNumber myTool;
    CANON_PLANE plane;
  public:
    machineStatus(machineStatus const& oldStatus);
    machineStatus(gp_Ax1 initial);
    void setMotionType(MOTION_TYPE m);
    void setEndPose(gp_Ax1 newPose);
    void setEndPose(gp_Pnt p);
    void addToBounds();
    void setFeed(const double f) {F=f;};
    void setSpindleSpeed(const double s) {S=s;};
    void setSpindleStatus(const SPINDLE_STATUS s) {spindleStat=s;};
    void setCoolant(coolantStruct c) {coolant = c;};
    void setTool(toolNumber n); //n is the ID of the tool to be used.
    void setPlane(CANON_PLANE p) {plane = p;};
    double getFeed() const {return F;};
    double getSpindleSpeed() const {return S;};
    SPINDLE_STATUS getSpindleStatus() const {return spindleStat;};
    const coolantStruct getCoolant() {return coolant;};
    const gp_Ax1 getStartPose() {return startPose;};
    const gp_Ax1 getEndPose() {return endPose;};
    CANON_PLANE getPlane() const {return plane;};
    void setEndDir(gp_Dir d) {endDir = d;};
    void setStartDir(gp_Dir d) {startDir = d;};
    const gp_Dir getStartDir() const {return startDir;};
    const gp_Dir getEndDir() const {return endDir;};
    const gp_Dir getPrevEndDir() const {return prevEndDir;};
    void clearAll(void);
    bool isFirst() {return first;};
    toolNumber getToolNum() {return myTool;};
    //void addArcToBbox(TopoDS_Edge e); //LINEAR_* is added automagically
    //static pntPair getTraverseBounds();  //bnd_box for *_TRAVERSE
    //static pntPair getFeedBounds(); //bnd_box for *_FEED

  private:
    machineStatus();  //prevent use of this ctor by making it private 
};
#endif //MACHINESTATUS_HH
