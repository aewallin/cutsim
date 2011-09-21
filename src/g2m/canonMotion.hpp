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

#include "machineStatus.hpp"
#include "canonLine.hpp"

namespace g2m {

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
    bool isMotion() {return true;};
    virtual std::vector<Point> points() = 0;
  protected:
    canonMotion(std::string canonL, machineStatus prevStatus);
    Pose getPoseFromCmd();
};


} // end namespace 

#endif //CANONMOTION_HH
