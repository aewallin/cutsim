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
#ifndef CANONLINE_HH
#define CANONLINE_HH

#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <sstream>
#include <limits.h>

#include "machineStatus.hpp"
#include "point.hpp"

/**
\class canonLine
\brief A canonLine object represents one canonical command.
Each gcode line produces one or more canonical commands. It can either be a 
* motion command (one of LINEAR_TRAVERSE LINEAR_FEED ARC_FEED), or a motionless 
* command (anything else)
You cannot create objects of this class - instead, create an object of a class 
* that inherits from this class via canonLineFactory()
*/
class canonLine {
  public:
    const std::string getLine() {return myLine;};
    const gp_Ax1 getStart() {return status.getStartPose(); };
    const gp_Ax1 getEnd() {return status.getEndPose(); };
    int getN(); //returns the number after N on the line, -1 if none
    int getLineNum() {return tok2i(0);} //returns the canon line number
    const std::string getCanonType();
    const machineStatus* getStatus() {return &status;} //returns the machine's status after execution of this canon line
    virtual MOTION_TYPE getMotionType() { return NOT_DEFINED;} //= 0;
    virtual bool isMotion() {return false;} // = 0;
    virtual bool isNCend() {return false;}
    
    // produce a canonLine based on string l, and previous machineStatus s
    static canonLine* canonLineFactory (std::string l, machineStatus s);
    
    inline std::string cantok(unsigned int n) {
      if (n < canonTokens.size()) {
        return canonTokens[n]; 
      } else {
        std::cout << "malformed input line " << myLine << std::endl;
        std::string s = ""; 
        return s;
      }
    }
    const std::string getLnum();
    
  protected:
    // protected ctor, create through factory
    canonLine(std::string canonL, machineStatus prevStatus); 
    double tok2d(unsigned int n);
    int tok2i(unsigned int n, unsigned int offset=0);
    void tokenize(std::string str, std::vector<std::string>& tokenV, const std::string& delimiters = "(), ");    
    const std::string getCanonicalCommand();
    
    ///return true if the canonical command for this line matches 'm'
    inline bool cmdMatch(std::string m) {
        if (canonTokens.size() < 3)
            return false;
        return (m.compare(canonTokens[2]) == 0); //compare returns zero for a match
    };

    std::string myLine; // the canon-line
    machineStatus status; //the machine's status *after* execution of this canon line
    std::vector<std::string> canonTokens; // the tokens are here, after tokenizing myLine
};

#endif //CANONLINE_HH
