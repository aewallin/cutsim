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
#include <string>
#include <climits>

#include "helicalMotion.hpp"
#include "machineStatus.hpp"

// example from cds.ngc:
//     231 N2250  ARC_FEED(3.5884, 1.9116, 3.5000, 2.0000, -1, 1.8437, 0.0000, 0.0000, 0.0000)
//tok: 0   1      2        3       4       5       6        7  8       9       10      11 
helicalMotion::helicalMotion(std::string canonL, machineStatus prevStatus): canonMotion(canonL,prevStatus) {
    
    double a1,a2,e1,e2,e3,ea,eb,ec,hdist;
    //  x=y=z=a1=a2=e1=e2=e3=ea=eb=ec=0;
    // e = end pose position

    // ( comments relate to XY-plane )
    // see the rs274 spec, www.isd.mel.nist.gov/documents/kramer/RS274NGC_22.pdf or similar
    e1 = tok2d(3); // first_end
    e2 = tok2d(4); // second_end
    
    a1 = tok2d(5); // first_axis   (x-coord of centerpoint)
    a2 = tok2d(6); // second_axis  (y-coord of centerpoint)
    
    rotation = tok2d(7); //rotation (ccw if rotation==1,cw if rotation==-1)
    // If rotation is positive, the arc is traversed counterclockwise as viewed from the positive end of
    // the coordinate axis perpendicular to the currently selected plane. If rotation is negative, the
    // arc is traversed clockwise.


    e3 = tok2d(8); // z-coord of endpoint
    
    ea = tok2d(9); //a
    eb = tok2d(10); //b
    ec = tok2d(11); //c
  
    /// Shuffle variables around based on the active plane.
    switch (status.getPlane()) {
        /*
        ** the order for these vars is copied from saicannon.cc, line 509+
        ** a,b,c remain in order
        */
        case CANON_PLANE_XZ:
            status.setEndPose(Point(e2,e3,e1));
            axis =   Point(0,1,0); // rotate around Y-axis
            center = Point(a2,start.y,a1);
            hdist = e3 - start.y;
            break;
        case CANON_PLANE_YZ:
            status.setEndPose(Point(e3,e1,e2));
            axis = Point(1,0,0); // rotate around X-axis
            center = Point(start.x,a1,a2);
            hdist = e3 - start.x;
            break;
        case CANON_PLANE_XY:
            default:
            status.setEndPose(Point(e1,e2,e3));
            axis = Point(0,0,1); // rotate around Z-axis
            center = Point(a1,a2,start.z);
            hdist = e3 - start.z; // change in z-coordinate, i.e. helix instead of arc.
    }
    status.setMotionType(HELICAL);
    start = status.getStartPose().loc;
    end = status.getEndPose().loc; 

/* //skip arc if zero length; caught this bug thanks to tort.ngc
    cout << "Skipped zero-length arc at N" << getN() << endl;
    status.setEndDir(status.getPrevEndDir());
    unsolidErrors = true;
    //myUnSolid.Nullify()
  //} else {
*/
    //cout << myLine; //endl supplied by helix() or arc()
    if (fabs(hdist) > 0.000001) {
        /// Create a helix if the third-axis delta is > .000001.
        planar = false;
        helix();
    } else {
        /// Otherwise, create an arc.
        planar = true;
        arc();
    }


    // FIXME - check for gaps

    /// Find vector direction at start and end of the edge, and save them in status
    double f,l,fd,ld;
    Point fp,lp; // first point, last point
    Point fv,lv; // first vector, last vector

    //BRepAdaptor_Curve bac(TopoDS::Edge(myUnSolid));

    //f = bac.FirstParameter();
    //l = bac.LastParameter();
    //bac.D1(f,fp,fv);
    //bac.D1(l,lp,lv);

/*
    fd = fp.SquareDistance(start);
    ld = lp.SquareDistance(start);
    if ( fd > ld ) { //compare the distances to decide which one is at the start
      //"first" end (fd) is farther from the starting point, so
      status.setEndDir(fv);          //use first vector at end
      status.setStartDir(lv);        //and use last vector at start
    } else {                         //other way around
      status.setEndDir(lv);
      status.setStartDir(fv);
    }*/
    // why do we need EndDir and StartDir ?
}


/// Create a helix, place it in myUnSolid
void helicalMotion::helix(  ) {
  //if (uio::debuggingOn()) cout << "helix" << endl;
  double pU,pV;
  double radius = start.Distance(center);
  
  
    std::cout << "helix radius=" << radius << " rot=" << rotation << std::endl;
  /*
  gp_Pnt2d p1,p2;
  Handle(Geom_CylindricalSurface) cyl = new Geom_CylindricalSurface(gp_Ax2(center,axis) , radius);
  GeomAPI_ProjectPointOnSurf proj;
  int success = 0;

  //myUnSolid.Nullify();
  //cout << "Radius " << radius << "   Rot has the value " << rotation << endl;
  proj.Init(start,cyl);
  if(proj.NbPoints() > 0) {
    proj.LowerDistanceParameters(pU, pV);
    if(proj.LowerDistance() > 1.0e-6 ) {
      if (uio::debuggingOn())
        cout << "Point fitting distance " << double(proj.LowerDistance()) << endl;
    }
    success++;
    p1 = gp_Pnt2d(pU,pV);
  }

  proj.Init(end,cyl);
  if(proj.NbPoints() > 0) {
    proj.LowerDistanceParameters(pU, pV);
    if(proj.LowerDistance() > 1.0e-6 ) {
      if (uio::debuggingOn())
        cout << "Point fitting distance " << double(proj.LowerDistance()) << endl;
    }
    success++;
    p2 = gp_Pnt2d(pU,pV);
  }

  if (success != 2) {
    if (uio::debuggingOn())
      cout << "Couldn't create a helix from " << uio::toString(start) << " to " << uio::toString(end)  << ". Replacing with a line." <<endl;
    unsolidErrors=true;
    myUnSolid = BRepBuilderAPI_MakeEdge( start, end );
    return;
  }*/
  
  

  //for the 2d points, x axis is about the circumference.  Units are radians.
  //change direction if rotation = 1, not if rotation = -1
  //if (rotation==1) p2.SetX((p1.X()-p2.X())-2*M_PI); << this is wrong!
  //cout << "p1x " << p1.X() << ", p2x " << p2.X() << endl;

  //switch direction if necessary, only works for simple cases
  //should always work for G02/G03 because they are not multi-revolution
  /*
  if ( (rotation==1) && (p2.X() <= p1.X()) ) {
    p2.SetX(p2.X()+2*M_PI);
  } else if ( (rotation==-1) && (p2.X() >= p1.X()) ) {
    p2.SetX(p2.X()-2*M_PI);
  }
  */
  
  
    //cout << "p2x now " << p2.X() << endl;

  //Handle(Geom2d_TrimmedCurve) segment = GCE2d_MakeSegment(p1 , p2);
  //myUnSolid = BRepBuilderAPI_MakeEdge(segment , cyl);

  return;
}

/// Create an arc, place it in myUnSolid

void helicalMotion::arc() {
  //gp_Vec Va = gp_Vec(axis);     //vector along arc's axis
  //if the endpoints are the same, assume it's a complete circle
  //previous behaviour was to assume no motion
  /*
  if (start.SquareDistance(end) < Precision::Confusion()) {
    //make circle
    double radius = fabs(center.Distance(start));
    if (uio::debuggingOn())
      cout << "Circle with radius " << radius << endl;
    Handle(Geom_Curve) C;
    C = GC_MakeCircle ( center, axis, radius);
    myUnSolid = BRepBuilderAPI_MakeEdge ( C );
  } else {
    gp_Vec Vr = gp_Vec(center,start);   //vector from center to start
    gp_Vec startVec = Vr^Va;      //find perpendicular vector using cross product
    if (rotation==1)
      startVec *= -1;
    if (uio::debuggingOn())
      cout << "Arc with vector at start: " << uio::toString(startVec) << endl;
    Handle(Geom_TrimmedCurve) Tc;
    Tc = GC_MakeArcOfCircle ( start, startVec, end );
    myUnSolid = BRepBuilderAPI_MakeEdge ( Tc );
  }*/
  
}
