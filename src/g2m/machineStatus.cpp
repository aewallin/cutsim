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

//#include <BRepBndLib.hxx>
//#include <Bnd_Box.hxx>

#include <iostream>
#include <sstream>

#include "machineStatus.hpp"
//#include "uio.hh"

//millTool* machineStatus::theTool = 0;
//Bnd_Box machineStatus::traverseBbox;
//Bnd_Box machineStatus::feedBbox;

machineStatus::machineStatus(const machineStatus& oldStatus) {
    //FIXME: segfault on next line when modelling a second file?!
    spindleStat = oldStatus.spindleStat;
    F = oldStatus.F;
    S = oldStatus.S;
    coolant = oldStatus.coolant;
    plane = oldStatus.plane;
    endPose = startPose = oldStatus.endPose;
    myTool = oldStatus.myTool;
    endDir = gp_Dir(0,0,-1);
    prevEndDir = oldStatus.endDir;
    first = oldStatus.first;
    motionType = NOT_DEFINED;
    /// lastMotionWasTraverse gets copied from the previous machine status, 
    /// and only gets changed if the prev status was motion at feedrate 
    /// (this way, motionless cmds don't mess things up)
    lastMotionWasTraverse = oldStatus.lastMotionWasTraverse;
    if ( oldStatus.motionType == STRAIGHT_FEED || oldStatus.motionType == HELICAL) {
      lastMotionWasTraverse = false;
    }
}

/**
This constructor is only to be used when initializing the simulation; it would not be useful elsewhere.
\param initial is the initial pose of the machine, as determined by the interp from the variable file.
\sa machineStatus(machineStatus const& oldStatus)
*/
machineStatus::machineStatus(gp_Ax1 initial) {
  clearAll();
  //theTool = new ballnoseTool(0.0625,0.3125); //1/16" tool. TODO: use EMC's tool table for tool sizes
  startPose = endPose = initial;
  first = true;
  setTool(1);
}

void machineStatus::clearAll() {
  F=S=0.0;
  plane = CANON_PLANE_XY;
  coolant.flood = false;
  coolant.mist = false;
  coolant.spindle = false;
  endPose = startPose = gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1));
  endDir = prevEndDir = gp_Dir(0,0,-1);
  spindleStat = OFF;
  myTool = -1;
  motionType = NOT_DEFINED;
  lastMotionWasTraverse = false;
}

///sets motion type, and checks whether this is the second (or later) motion command.
void machineStatus::setMotionType(MOTION_TYPE m) {
    motionType = m;
    if (motionType == NOT_DEFINED) {
        std::cout << "motion type undef!! \n";
    } else if (motionType == TRAVERSE) {
        lastMotionWasTraverse = true;
    }
    static int count = 0;
    if ((first) && ((m == STRAIGHT_FEED) || (m == TRAVERSE) || (m == HELICAL)) ) {
        if (count == 0)
          count++;
        else
          first = false;
    }
}


/** \fn setEndPose
Set end points, and call addToBounds to add points to bndbox. For an arc or helix, the edge must be added from its ctor with addArcToBbox.
\sa addArcToBbox(TopoDS_Edge e)
*/
void machineStatus::setEndPose(gp_Pnt p) {
  endPose = gp_Ax1( p, gp_Dir(0,0,1) );
  //addToBounds();
}
void machineStatus::setEndPose(gp_Ax1 newPose) {
  endPose = newPose;
  //addToBounds();
}


/*
void machineStatus::addToBounds() {
  if (first) {
    if (uio::debuggingOn()) infoMsg("not adding to bndbox");
    return;
  }
  if (motionType == NOT_DEFINED) {
    if (uio::debuggingOn()) infoMsg("error, mtype not defined");
  } else if (motionType == STRAIGHT_FEED) {
    if (uio::debuggingOn()) infoMsg("adding to feed bndbox");
    gp_Pnt a,b;
    a=startPose.Location();
    b=endPose.Location();

    ///don't add the start of a vertical STRAIGHT_FEED if the previous motion was TRAVERSE
    //what about the end of a vert S_F when next motion is TRAVERSE? pita to implement...
    double angTol = .0175;
    if (a.SquareDistance(b)>Precision::Confusion() && !( gp_Vec(a,b).IsParallel(gp::DZ(),angTol) && lastMotionWasTraverse )) {
      feedBbox.Add(a);
    }
    feedBbox.Add(b);
  } else if (motionType == TRAVERSE) {
    if (uio::debuggingOn()) infoMsg("adding to traverse bndbox");
    traverseBbox.Add(startPose.Location());
    traverseBbox.Add(endPose.Location());
  } else {
    //if (uio::debuggingOn()) infoMsg("warning, motion type = "+uio::toString(motionType));
  }
}*/

void machineStatus::setTool(toolNumber n) {
    //std::cout << "adding tool " << n << ".\n";
    myTool = n;
    //canon::addTool(n);
}

///if not first, add an arc or helix to feedBbox. STRAIGHT_* is added in setEndPose()
/*
void machineStatus::addArcToBbox(TopoDS_Edge e) {
  if (!first) {
    infoMsg("adding to bndbox");
    BRepBndLib::Add(e,feedBbox);
  }
}*/


/*
pntPair machineStatus::getTraverseBounds() {
  pntPair b;
  if (!traverseBbox.IsVoid()) {
    double aXmin,aYmin,aZmin, aXmax,aYmax,aZmax;
    traverseBbox.Get (aXmin,aYmin,aZmin, aXmax,aYmax,aZmax );
    b.a=gp_Pnt(aXmin,aYmin,aZmin);
    b.b=gp_Pnt(aXmax,aYmax,aZmax);
  } else {
    b.a=gp_Pnt(0,0,0);
    b.b=gp_Pnt(0,0,0);
  }
  return b;
}*/

/*
pntPair machineStatus::getFeedBounds() {
  double aXmin,aYmin,aZmin, aXmax,aYmax,aZmax;
  pntPair b;
  int fail = 0;
  if (!feedBbox.IsVoid()) {
    try {
      feedBbox.Get ( aXmin,aYmin,aZmin, aXmax,aYmax,aZmax );
      b.a=gp_Pnt(aXmin,aYmin,aZmin);
      b.b=gp_Pnt(aXmax,aYmax,aZmax);
    } catch (Standard_ConstructionError) {
      fail = 1;
    }
  } else {
    fail = 2;
  }
  if (fail) {
    uio::infoMsg("feedBbox problem - " + uio::toString(fail));
    abort();
  }
  return b;
}*/

