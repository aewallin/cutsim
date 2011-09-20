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
//for LINEAR_TRAVERSE, LINEAR_FEED, ARC_FEED
#include <string>
#include <limits.h>

/*
#include <Precision.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_PipeError.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <TopoDS.hxx>
#include <gp_Circ.hxx>
#include <Geom_Curve.hxx>
#include <Handle_Geom_Curve.hxx>
#include <ShapeFix_Solid.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <AIS_Shape.hxx>
*/

#include "canonMotion.hpp"
#include "canonLine.hpp"
#include "point.hpp"

//#include "uio.hh"

canonMotion::canonMotion(std::string canonL, machineStatus prevStatus): canonLine(canonL,prevStatus) {
  //if (uio::debuggingOn())
  //  infoMsg(myLine.substr(0,myLine.size()-1)); //remove newline
}

/// for STRAIGHT_* and ARC_FEED, first 3 are always xyz and last 3 always abc
gp_Ax1 canonMotion::getPoseFromCmd() {
  double x,y,z;

  //need 3,4,5,and -3,-2,-1
  x = tok2d(3);
  y = tok2d(4);
  z = tok2d(5);
  gp_Pnt p(x,y,z); // point class!

/* FIXME
  double a,b,c;
  uint s = canonTokens.size(); //a,b,c are last 3 numbers
  c = tok2d(s-1);
  b = tok2d(s-2);
  a = tok2d(s-3);
  assert (a+b+c < 3.0 * Precision::Confusion());
  //now how to convert those angles to a unit vector (i.e. gp_Dir)?
*/
  //for now we take the easy way out
  
  gp_Dir d(0,0,1); //vertical, NO ROTATION!
  return gp_Ax1(p,d);
}

/*
void canonMotion::computeSolid() {
  if (unsolidErrors) {
    myShape.Nullify();
    return;
  }
  if (!status.isFirst()) {
    switch (solidMode) {
      case SWEPT:
        sweepSolid();
        //addToolMaybe();
        break;
      case BRUTEFORCE:
        bruteForceSolid();
        break;
      case ASSEMBLED:
        assembleSolid();
        //addToolMaybe();
      default:
        break;
    }
  } else {  //this is the first line
    //FIXME: create tool at end pos, don't create any other solid
  }
}*/

///fuse tool at start of mySolid, if necessary
///faster to build a shell and attach in place of one face?
///faster to fuse a half-tool? (this won't work unless horizontal!)

/*
void canonMotion::addToolMaybe() {
  Standard_Real angTol = 0.0175;  //approx 1 degree
  if (!status.getPrevEndDir().IsParallel(status.getStartDir(),angTol)) {
    //translate the tool to the startpoint of the sweep, then fuse it with the sweep

    //if (vert) { //add in another tool shape
    //  tob.Perform(getTool(status.getToolNum())->get3d(),true);
    //  t = BRepAlgoAPI_Fuse(toa.Shape(),tob.Shape());
    //}
    
    try {
      myShape = BRepAlgoAPI_Fuse( myShape, toolAtStart() );
    } catch (...){
      infoMsg("Problematic Fuse operation: " + myLine);
      //uio::sleep(1);
    }
  }

}*/


///Returns 3d tool, shifted to startpoint
/*
TopoDS_Shape canonMotion::toolAtStart() {
  gp_Trsf tr;
  tr.SetTranslation(gp::Origin(),status.getStartPose().Location());
  BRepBuilderAPI_Transform btr(tr);
  btr.Perform(getTool(status.getToolNum())->get3d(),true);
  return btr.Shape();
}
*/


/**
Creates a transform that rotates from first to second, about center
NOTE: when rotating and translating, do rotation first!
*/
/*
gp_Trsf canonMotion::trsfRotDirDir(gp_Dir first, gp_Dir second, gp_Pnt center) {
  gp_Dir perpendicular = first^second; //perpendicular to both first and second
  gp_Ax1 ax(center,perpendicular);
  Standard_Real ang = ax.Direction().Angle(second);
  gp_Trsf t;
  t.SetRotation(ax,ang);
  return t;
}
*/

///a *very* bad idea - *extremely* time consuming, potential for crashes, ...
/*
void canonMotion::bruteForceSolid() {
  double len = status.getStartPose().Location().Distance(status.getEndPose().Location()); //FIXME: very inaccurate for arcs
  double dia = getTool(status.getToolNum())->Dia();
  double reps = (len/dia)*10; //10 fuse ops per tool diameter
  double param1=0,param2 = 0;
  TopLoc_Location loc;    //transform used for edge
  Handle(Geom_Curve) C = BRep_Tool::Curve(TopoDS::Edge(myUnSolid),loc,param1,param2);
  C->Transform(loc.Transformation());  //transform C using loc
  if (param1 != param2) {
    double increment = (param1-param2)/reps;
    int i;
    TopoDS_Shape result;
    result.Nullify();
    TopoDS_Shape tool = getTool(status.getToolNum())->get3d();
    gp_Trsf trsf;
    for (i=0;i<reps;i++) {
      gp_Pnt pntOnLine =  C->Value(param1 + increment*i);
      trsf.SetTranslation(gp::Origin(),pntOnLine);
      BRepBuilderAPI_Transform btrsf(trsf);
      btrsf.Perform(tool,true);
      if (!result.IsNull()) {
        try {
          result = BRepAlgoAPI_Fuse(btrsf.Shape(),result);
        } catch (...) {
          cout << "Brute force: caught exception in fuse, " << i << " of " << reps << " on line " << getN() << endl;
        }
      } else {
        result = btrsf.Shape();
      }
    }
    myShape = result;
  } else cout << "brep_tool::curve failed, params have identical values: " << param1 << "," << param2 << endl;
}*/

/// Sweep tool outline along myUnSolid and put result in myShape
/*
void canonMotion::sweepSolid() {
  Standard_Real angTol = 0.035;  //approx 2 degrees
  double radius = getTool(status.getToolNum())->Dia()/2.0;
  TopoDS_Solid solid;
  gp_Pnt a,b;
  a = status.getStartPose().Location();
  b = status.getEndPose().Location();

  gp_Vec d(a,b);

  gp_Trsf oa,ob;
  oa.SetTranslation(gp::Origin(),a);
  BRepBuilderAPI_Transform toa(oa);
  ob.SetTranslation(gp::Origin(),b);
  BRepBuilderAPI_Transform tob(ob);

  if (a.Distance(b) < radius/10.0) {  //FIXME? skip, motion is very short
    tob.Perform(getTool(status.getToolNum())->get3d());
    myShape = tob.Shape();
    return;
  }

  TopoDS_Wire w;
  bool vert = false;

  //check if the sweep will be vertical. if so, we can't use the tool's profile
  if ( d.IsParallel( gp::DZ(), angTol )) {
    vert = true;
    if (uio::debuggingOn()) infoMsg("Line " +cantok(0)+"/"+cantok(1)+" is vertical.");
    gp_Circ c(gp::XOY(),radius);
    w = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(c));
    tob.Perform(w,true);  //toa will always be used, whether vertical or not

  } else {
    w = TopoDS::Wire(getTool(status.getToolNum())->getProfile());
  }
  toa.Perform(w,true);
  BRepOffsetAPI_MakePipeShell pipe(BRepBuilderAPI_MakeWire(TopoDS::Edge(myUnSolid)).Wire());
  if (!w.Closed()) {
    infoMsg("Wire not closed!");
  } else {
      pipe.Add(toa.Shape(),false,true);  //transform the sweep outline, so that the pipe will be located correctly
    if (vert) {
      pipe.Add(tob.Shape(),false,true);
    } else {
      
      //FIXME: setmode (or build, if setmode is commented out) causes a crash at
      //segf.ngc:N070 - crashes because the edge is too far from horizontal?
      
      try {
      pipe.SetMode(gp_Dir(0,0,1)); //binormal mode: can only rotate about Z
      //FIXME: above line causes segfault at nas.ngc:N0029 (helix)
      } catch (...) {
        solidErrors = true;
        infoMsg("pipe set mode failed for " + myLine);
      }
    }
    if ( pipe.IsReady() ) {
      try {
        pipe.Build();
      } catch (...) {
        solidErrors = true;
        infoMsg("pipe build failed for " + myLine);
      }
      BRepBuilderAPI_PipeError perror = pipe.GetStatus();
      switch (perror) {
        case BRepBuilderAPI_PipeNotDone:
          infoMsg("Pipe not done");
          solidErrors = true;
          break;
        case BRepBuilderAPI_PlaneNotIntersectGuide:
          infoMsg("Pipe not intersect guide");
          solidErrors = true;
          break;
        case BRepBuilderAPI_ImpossibleContact:
          infoMsg("Pipe impossible contact");
          solidErrors = true;
          break;
        case BRepBuilderAPI_PipeDone:
          //ready = true;
          break;
        default:
          infoMsg("Pipe switch default?!");
          solidErrors = true;
      }
    }
  }
  if (!solidErrors) {
    TopoDS_Shape t;
    try {
    t = pipe.Shape();
    pipe.MakeSolid();
    } catch (...) {
      infoMsg("can't make solid - a:" + uio::toString(a) + " b:" + uio::toString(b) +" on "+ cantok(0) + "/" + cantok(1));
      solidErrors = true;
      solid.Nullify();
    }
    if (!solidErrors) {
      TopoDS_Shape s = pipe.Shape();
      if (s.ShapeType()==TopAbs_SOLID) {
        ShapeFix_Solid fs(TopoDS::Solid(s));
        fs.Perform();
          try {
            solid = TopoDS::Solid(fs.Solid());
          } catch (...) {
            cout << "shapefix_solid exception at " << cantok(0) << cantok(1) << cantok(2) << endl;
            solidErrors = true;
          }
      } else {
        solidErrors = true;
        cout << "pipe isn't of type solid even though pipe.MakeSolid() didn't fail: " << cantok(0) << cantok(1) << cantok(2) << endl;
      }
      if (solidErrors) {
        solidErrors = true;
        myShape = s;
      }
    } else {
      myShape = t; //couldn't make the pipe solid
    }
  } else {
    solid.Nullify();
  }
  if ((!solid.IsNull()) && (!solidErrors)) {
    if (vert) {
      //raise the sweep up by 1 radius
      gp_Pnt v(0,0,getTool(status.getToolNum())->Dia()/2.0);
      gp_Trsf ov;
      ov.SetTranslation(gp::Origin(),v);
      solid = TopoDS::Solid(BRepBuilderAPI_Transform(solid, ov, true).Shape());
    }

//    BRepCheck_Analyzer bca(solid);
  //  if (!bca.IsValid()) BRepTools::Dump(solid,std::cout);
  //  cout << "line N" << getN() << ": solid's mass - " << uio::mass(solid) << endl;


      myShape = solid;
      if (uio::debuggingOn()) infoMsg("pipe: no errors on " + cantok(0) + "/" + cantok(1));
  } else {
    solidErrors = true;
    if (uio::debuggingOn()) infoMsg("pipe not ready on "+ cantok(0) + "/" + cantok(1));
  }

  //dump the data? used env var DUMP. dumps all if DUMP=-1
  if (uio::debuggingOn() && ((uio::getDump() == getLineNum()) || (uio::getDump() ==-1))) {
    //what to dump: endpoints and end Dirs, tool wire, sweep path, myShape
    std::string name,type;
    name="output/Dump_"+cantok(0)+"_outline.brep";
    BRepTools::Write(toa.Shape(),name.c_str());

    name="output/Dump_"+cantok(0)+"_spine_edge.brep";
    BRepTools::Write(myUnSolid,name.c_str());

    name="output/Dump_"+cantok(0)+"_pipe.brep";
    BRepTools::Write(myShape,name.c_str());

    MOTION_TYPE mt = getMotionType();
    if (mt == HELICAL) {
      type = "HELICAL";
    } else if (mt == STRAIGHT_FEED) {
      type = "STRAIGHT_FEED";
    } else if (mt == TRAVERSE) {
      type = "TRAVERSE";
    } else type = "?!";

    infoMsg("Dumping breps for " + cantok(0) + " " + cantok(1) + " " + cantok(2) + ", starting at " + uio::toString(a) + " and ending at " + uio::toString(b) +/ " with type " + type);

  }
}
*/

/*
void canonMotion::display() {
  if (dispMode == NO_DISP) {
    return;
  } else if ((dispMode == THIN_MOTION) || (dispMode == THIN)) {
    aisShape = new AIS_Shape(myUnSolid);
    if ( unsolidErrors || solidErrors ) {
      uio::context()->SetColor(aisShape, Quantity_NOC_BLUE1 );
    } else if (getMotionType() != TRAVERSE) {
      uio::context()->SetColor(aisShape, Quantity_NOC_GREEN );
    } else {
      uio::context()->SetColor(aisShape, Quantity_NOC_RED );
    }
  } else if ((dispMode == ONLY_MOTION) || (dispMode == BEST)) {
    if (myShape.IsNull()) {
      aisShape = new AIS_Shape(myUnSolid);
    } else {
      aisShape = new AIS_Shape(myShape);
    }
  }
  Graphic3d_NameOfMaterial nom;
  if (unsolidErrors) {
    nom = Graphic3d_NOM_NEON_PHC;//green
  } else if (solidErrors) {
    nom = Graphic3d_NOM_NEON_GNC;
  } else {
    nom = Graphic3d_NOM_DEFAULT;
  }
  //  Graphic3d_NameOfMaterial nom = Graphic3d_NOM_PLASTIC;

  //can also use ->SetColor(aisShape, Quantity_NameOfColor)
  uio::context()->SetMaterial ( aisShape, nom, Standard_True );
  uio::context()->SetDisplayMode ( aisShape,Standard_Integer(AIS_Shaded),Standard_False );
  uio::context()->Display ( aisShape );
}*/


