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
#ifndef CANONMOTION_HH
#define CANONMOTION_HH

#include <string>
#include <vector>
#include <cmath>
#include <limits.h>

//#include <TopoDS_Edge.hxx>
//#include <TopoDS_Shape.hxx>

#include "machineStatus.hpp"
#include "canonLine.hpp"

//for STRAIGHT_TRAVERSE, STRAIGHT_FEED, ARC_FEED

//enum SOLID_MODE { SWEPT,BRUTEFORCE,ASSEMBLED };
/**
\class canonMotion
\brief This class is for the canonical commands STRAIGHT_TRAVERSE, STRAIGHT_FEED, and ARC_FEED.
canonMotion is an ABC. Its children should only be instantiated via 
* canonLine::canonLineFactory(), which creates 
* linearMotion objects for STRAIGHT_TRAVERSE and STRAIGHT_FEED commands, 
* and helicalMotion objects for ARC_FEED commands.

Note, you may find variations in the terminology I use - I misremembered some of 
* the canonical commands issued by the interpreter. For example, I thought it 
* issued LINEAR_FEED but it's actually STRAIGHT_FEED. So the class 
* linearMotion could have been named straightMotion.

Also, rapid and traverse are used interchangeably, at least in my comments...
*/
class canonMotion: protected canonLine {
  public:
    virtual MOTION_TYPE getMotionType() {return NOT_DEFINED;}
    //virtual const TopoDS_Shape& getShape() = 0;
    //TopoDS_Shape toolAtStart();
    bool isMotion() {return true;};
    //bool solErrors() {return solidErrors;};
    //bool isVolumeSuspect() {return sweepIsSuspect;};
    //void setSolidMode(SOLID_MODE s) {solidMode = s;};
    //void computeSolid();
    //void display();
  protected:
    canonMotion(std::string canonL, machineStatus prevStatus);
    //TopoDS_Shape myShape;
    gp_Ax1 getPoseFromCmd();
    //bool sweepIsSuspect;
    //void sweepSolid();
    //void dumpSweep();
    //void dumpSubtract();
    //void bruteForceSolid();
    //virtual void assembleSolid()=0;
    //gp_Trsf trsfRotDirDir(gp_Dir first, gp_Dir second,gp_Pnt center = gp::Origin());
    //void addToolMaybe();
    //SOLID_MODE solidMode;
};

#endif //CANONMOTION_HH
