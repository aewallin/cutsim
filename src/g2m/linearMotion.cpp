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
//#include <BRepAlgo_Fuse.hxx>
//#include <BRep_Builder.hxx>
//#include <BRepTools.hxx>
//#include <BRepBuilderAPI_MakeEdge.hxx>
//#include <BRepBuilderAPI_MakeFace.hxx>
//#include <BRepBuilderAPI_Transform.hxx>
//#include <BRepPrimAPI_MakePrism.hxx>
//#include <TopoDS.hxx>
//#include <TopoDS_Wire.hxx>

#include "machineStatus.hpp"
#include "canonMotion.hpp"
#include "canonLine.hpp"
//#include "uio.hh"

linearMotion::linearMotion(std::string canonL, machineStatus prevStatus): canonMotion(canonL,prevStatus) {
  status.setMotionType(getMotionType());
  status.setEndPose(getPoseFromCmd());
  gp_Pnt a,b;
  a = status.getStartPose().loc;
  b = status.getEndPose().loc;
  std::cout << " linear feed: " << a.str() << " to " << b.str() << "\n";
  
  
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


/*
void linearMotion::assembleSolid() {
  gp_Pnt a,b;
  a = status.getStartPose().Location();
  b = status.getEndPose().Location();

  gp_Trsf ta;
  ta.SetTranslation(gp::Origin(),a);
  gp_Trsf tar = trsfRotDirDir(gp::DY(),status.getStartDir());

  TopoDS_Wire tool = TopoDS::Wire(getTool(status.getToolNum())->getProfile());
  TopoDS_Face toolf = BRepBuilderAPI_MakeFace(tool);
  BRepBuilderAPI_Transform(toolf,tar); //rotate
  BRepBuilderAPI_Transform(toolf,ta); //shift from origin to start

  gp_Vec d(a,b);

  //FIXME: probably need to rotate so it's normal to d

  try {
  TopoDS_Shape myShape = BRepPrimAPI_MakePrism(toolf,d);
  } catch (...) {
    infoMsg("make prism failed");
    solidErrors = true;
  }
}*/

//need to return RAPID for rapids...
MOTION_TYPE linearMotion::getMotionType() {
  bool traverse = cmdMatch("STRAIGHT_TRAVERSE");
  if (traverse) {
    return TRAVERSE;
  } else {
    return STRAIGHT_FEED;
  }
}

/*
TopTools_ListOfShape oneShell (TopoDS_Solid a, TopoDS_Solid b) {
  TopTools_ListOfShape lsh;
  TopoDS_Shell shp1_shell = BRepTools::OuterShell(a);
  TopoDS_Shell shp2_shell = BRepTools::OuterShell(b);
  TopoDS_Shape fused_shells = BRepAlgoAPI_Fuse(shp1_shell, shp2_shell).Shape();

  TopExp_Explorer Ex;
  for (Ex.Init(fused_shells, TopAbs_ShapeEnum.TopAbs_SHELL); Ex.More(); Ex.Next()) {
    TopoDS_Shell crt_shell = TopoDS::Shell(Ex.Current());
    ShapeFix_Shell FixTopShell(crt_shell);
    FixTopShell.Perform();
    if (FixTopShell.NbShells() > 1) {
      TopoDS_Compound shellComp = OCTopoDS.Compound(FixTopShell.Shape());
      TopExp_Explorer ExShls(shellComp, TopAbs_ShapeEnum.TopAbs_SHELL);
      for (; ExShls.More(); ExShls.Next()) {
        TopoDS_Shell shl1 = TopoDS::Shell(ExShls.Current());
        ShapeFix_Shell FixShl = new ShapeFix_Shell(shl1);
        FixShl.Perform();

        TopoDS_Solid sol_tmp1 = BRepBuilderAPI_MakeSolid(FixShl.Shell()).Solid();
        ShapeFix_Solid FixSld(sol_tmp1);
        FixSld.Perform();
        TopoDS_Solid sol_tmp = TopoDS.Solid(FixSld.Solid());
        // add it to collection....
        lsh.Add(sol_tmp);
        cout << "multiple solids!" << endl;
        }
        } else {
          TopoDS_Shell aShell = FixTopShell.Shell();
          TopoDS_Solid sol = new BRepBuilderAPI_MakeSolid(aShell).Solid();
          // add it to collection....
          lsh.Add(sol);
          cout << "one solid" << endl;
          }
          }
          return lsh;
          }
          */
