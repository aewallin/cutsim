/***************************************************************************
*   Copyright (C) 2010 by Mark Pictor                                      *
*   mpictor@gmail.com                                                      *
*   modified by Anders Wallin 2011, anders.e.e.wallin@gmail.com            *
* 
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
*   This program is distributed in the hope that it will be useful,        *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
*                                                                          *
*   You should have received a copy of the GNU General Public License      *
*   along with this program; if not, write to the                          *
*   Free Software Foundation, Inc.,                                        *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
***************************************************************************/

#include <iostream>
#include <cmath>
#include <fstream>
#include <stdlib.h>

#include <QProcess>
#include <QStringList>
#include <QString>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>
#include <QFileDialog>
#include <QStatusBar>

// g2m files
#include "g2m.hpp"


//#include "uio.hh"
//#include "canonLine.hh"
//#include "canonMotion.hh"
//#include "canonMotionless.hh"

#include "nanotimer.hpp"
//#include "dispShape.hh"

// contrib/salome
//#include "ShHealOper_ShapeProcess.hxx"
//#include "ShHealOper_FillHoles.hxx"
//#include "ShHealOper_Sewing.hxx"

// OCC
/*
#include <ShapeFix_Solid.hxx>
#include <Handle_Geom_TrimmedCurve.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <TopoDS.hxx>
#include <gp_Pln.hxx>
#include <TopoDS_Edge.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
*/


//init static members
std::vector<canonLine*> g2m::lineVector;
//bool g2m::interpDone = false;

//g2m::g2m() {
//    std::cout << "g2m() ctor\n";
    //QMenu* myMenu = new QMenu("gcode");

    //QAction* myAction = new QAction ( "Create 3D Model...", this );
    //myAction->setShortcut(QString("Ctrl+M"));
    //myAction->setStatusTip ( "Load a .ngc file and create a 3d model" );
    //connect(myAction,SIGNAL(triggered()),this,SLOT(slotModelFromFile()));

    //solidAction = new QAction ( "Toggle solid creation", this );
    //solidAction->setShortcut(QString("Ctrl+T"));
    //solidAction->setStatusTip ( "Toggle solid" );
    //solidAction->setCheckable(true);
    //connect(solidAction,SIGNAL(triggered()),this,SLOT(slotToggleSolid()));

    //myMenu->addAction( myAction );
    //myMenu->addAction( solidAction );
    //uio::mb()->insertMenu(uio::hm(),myMenu); // ui ref..

    //debug = uio::debuggingOn();

/*
    if ( (debug) && (uio::getDump()!=0) )
        solidToggle = true;  //if we're dumping shapes then obviously they need to be created
    else
        solidToggle = false;
    solidAction->setChecked(solidToggle);

    fromCmdLine = false;
    if (uio::window()->getArgs()->count() > 1) {
        if (uio::window()->getArg(0).compare("g2m") == 0) {
            fromCmdLine = true;
            uio::sleep(1); //this gives the app time to draw the main window.
            //  otherwise we don't see jack unless/until it succeeds
            slotModelFromFile();
        }
    }
    * */
//}

/*
void g2m::slotToggleSolid() {
    solidToggle = !solidToggle;
    solidAction->setChecked(solidToggle);
}*/

void g2m::interpret_file() {
  lineVector.clear();
  //uio::hideGrid();
  //interpDone = false;

/*
  if (fromCmdLine) {
    file = uio::window()->getArg(1);
    if (!uio::fileExists(file))  {
      fromCmdLine = false;  //the file doesn't exist, we'll ask for a file
    }
  }
  if (!fromCmdLine) {
    file = QFileDialog::getOpenFileName ( uio::window(), "Choose input file", "./ngc-in", "All types (*.ngc *.canon);;G-Code (*.ngc);;Canon (*.canon)" );
  }
  fromCmdLine = false;  //so that if a second file is processed, this time within gui, the program won't use the one from the command line instead
*/
  nanotimer timer;
  timer.start();
   file = "test.ngc";
   
  if ( file.endsWith(".ngc") ) {
    interpret(); // reads from file
    //if (!success) 
    //    return;
  } else if (file.endsWith(".canon")) { //just process each line
    if (!chooseToolTable()) {
      infoMsg("Can't find tool table. Aborting.");
      return;
    }
    std::ifstream inFile(file.toAscii());
    std::string sLine;
    while(std::getline(inFile, sLine)) {
      if (sLine.length() > 1) {  //helps to prevent segfault in canonLine::cmdMatch()
        processCanonLine(sLine); // requires no interpret()
      }
    }
  } else {
    infoMsg("File name must end with .ngc or .canon!");
    return;
  }

  //interpDone = true;  
  //for g2m_threaded. tells threads they can quit when they reach the end of the vector.

  //must be called before canonLine solids are created, because it calculates the minimum tool length
  //createBlankWorkpiece();
  
  // what does this do?
  //canon::buildTools(tooltable.toStdString(),minToolLength);

  //createThreads();  //does nothing in g2m. overridden in g2m_threaded.

  //in g2m, this creates the solids
  //overridden in g2m_threaded - waits for the threads to finish
  //finishAllSolids(timer);

  //now display the workpiece
  //if ( solidToggle ) {
  //  dispShape wp(workpiece);
  //  wp.display();
  //}

  double e = timer.getElapsedS();
  std::cout << "Total time to process that file: " << timer.humanreadable(e).toStdString() << std::endl;

  //uio::fitAll();
}

/// call makeSolid on each obj in lineVector
/// this is overridden by g2m_threaded
/*
void g2m::finishAllSolids(nanotimer &timer) {
  for (uint i=0;i<lineVector.size();i++) {
    makeSolid(i);
    lineVector[i]->display();
    //if (i%20 == 0) { //every 20
      uio::fitAll();
      QString s = "Processing ";
      s+= lineVector[i]->getLnum().c_str();
      s+= " : " + i;
      s+= " of " + lineVector.size();
      statusBarUp(s,timer.getElapsedS()/double(i));
    //}
  }
}*/

///ask for a tool table, even if one is configured - user may wish to change it
bool g2m::chooseToolTable() {
    /*
  QString loc;
  bool ttconf = uio::conf().contains("rs274/tool-table");
  if (ttconf) {
    //passing the file name as the path means that it is preselected
    loc = uio::conf().value("rs274/tool-table").toString();
  } else {
    loc =  "/usr/share/doc/emc2/examples/sample-configs/sim";
  }
  tooltable = QFileDialog::getOpenFileName ( uio::window(), "Locate tool table", loc, "EMC2 new-style tool table (*.tbl)" );
    */
  tooltable = "table.tbl";
  if (!QFileInfo(tooltable).exists()){
    infoMsg(" cannot find tooltable! ");
    return false;
  }
  // uio::conf().setValue("rs274/tool-table",tooltable);
  return true;
}

bool g2m::startInterp(QProcess &tc) {
    //bool success = true;
    QString interp;

    //  if the user has specified a location for the interpreter, use it. if not, guess.
    //  if that fails, ask. save the result.
    //  QSettings settings("camocc.googlecode.com","cam-occ");
    
    /*
    interp = uio::conf().value("rs274/binary","/usr/bin/rs274").toString();
    if (!QFileInfo(interp).isExecutable()) {
        uio::infoMsg("Tried to use " + interp.toStdString() + " as the interpreter, but it doesn't exist or isn't executable.");
        interp = QFileDialog::getOpenFileName ( uio::window(), "Locate rs274 interpreter", "~", "EMC2 stand-alone interpreter (rs274)" );
        if (!QFileInfo(interp).isExecutable()) {
          return false;
        }
        uio::conf().setValue("rs274/binary",interp);
    }*/
    
    
    interp = "/home/anders/emc2-dev/bin/rs274";
    
    if (!chooseToolTable())
        return false;
    
    //Qprocess !!
    tc.start( interp , QStringList(file) );

  /**************************************************
  Apparently, QProcess::setReadChannel screws with waitForReadyRead() and canReadLine()
  So we just fly blind and assume that
  - there are no errors when we navigate the interp's "menu", and
  - it requires no delays.
  **************************************************/

    tc.write("3\n"); // "read tool file" command to rs274
    tc.write(tooltable.toAscii());
    tc.write("\n"); // "enter"
    tc.write("1\n"); // "start interpreting" command to rs274
    return true;
}

// called after "file" set in constructor
void g2m::interpret() {
    //success = false;
    QProcess toCanon;
    bool foundEOF; // checked at the end
    
    if (!startInterp(toCanon)) // finds rs274, reads tooltable, start interpreter
        return;

    //uio::window()->statusBar()->clearMessage();
    //uio::window()->statusBar()->showMessage("Starting interpreter...");
    
    if (!toCanon.waitForReadyRead(1000) ) {
        if ( toCanon.state() == QProcess::NotRunning ){
            infoMsg("Interpreter died.  Bad tool table?");
        } else  
            infoMsg("Interpreter timed out for an unknown reason.");
        std::cout << "stderr: " << (const char*)toCanon.readAllStandardError() << std::endl;
        std::cout << "stdout: " << (const char*)toCanon.readAllStandardOutput() << std::endl;
        toCanon.close();
        return;
    }
    // rs274  has now run correctly?
    qint64 lineLength;
    char line[260];
    int fails = 0;
  
    do {
        if (toCanon.canReadLine()) {
          lineLength = toCanon.readLine(line, sizeof(line));
          if (lineLength != -1 ) {
            foundEOF = processCanonLine(line); // line is a canon-line
          } else {  //shouldn't get here!
          
            std::cout << " ERROR: lineLength= " << lineLength << "  fails="<< fails << "\n";
            fails++;
            //sleepSecond();
          }
        } else {
            std::cout << " ERROR: toCanon.canReadLine() fails="<< fails << "\n";
            fails++;
            sleepSecond();
        }
    } while ( (fails < 100) &&
           ( (toCanon.canReadLine()) ||
            ( toCanon.state() != QProcess::NotRunning ) )  );
  
  if (fails > 1) {
    if (fails < 100) {
        infoMsg("Waited for interpreter over 100  times.");
    } else {
      infoMsg("Waited 100 seconds for interpreter. Giving up.");
      toCanon.close();
      return;
    }
  }
  
  std::string s = (const char *)toCanon.readAllStandardError();
  s.erase(0,s.find("executing"));
  if (s.size() > 10) {
    infoMsg("Interpreter exited with error:\n"+s.substr(10));
    return;
  }
  if (!foundEOF) {
    infoMsg("Warning: file data not terminated correctly. If the file is terminated correctly, this indicates a problem interpreting the file.");
  }
  //success = true;
  return;
}

// input string is a canon-string from rs274
bool g2m::processCanonLine (std::string l) {
    nanotimer nt;
    nt.start();
    canonLine* cl;
    if (lineVector.size()==0) { 
        // no status exists, so make one up.
        cl = canonLine::canonLineFactory(l); //,  machineStatus( gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1)) )   ); 
    } else {
        // use the last element status
        cl = canonLine::canonLineFactory(l); //,  *(lineVector.back())->getStatus()  ); 
    }
    
    lineVector.push_back(cl); // cl stored here!
    
/* need to highlight the first *solid* rather than the first obj
  if (lineVector.size()==1) {
    lineVector[1].setFirst(); //different color, etc
    ///need to do something similar for the last vector...
}
*/

    /*
    double t = nt.getElapsedS();
    if ((debug) && (t>.00005)) { //don't print if fast or not debugging
        cout << "Line " << cl->getLineNum() << "/N" << cl->getN() << " - time " << nt.humanreadable(t).toStdString() << endl;
    }
    */
    //cl->checkErrors();
    
    
    // return true when we reach end-of-program
    /*
    if (!cl->isMotion())
        return ( (canonMotionless*)cl )->isNCend();
    */
        
    return false;
}

/*
void g2m::statusBarUp(QString s, double avgtime) {
    QString m;
    //uio::window()->statusBar()->clearMessage();
    //  std::string s = "Last processed:";
    m = s;
    m += "   |   Avg time: ";
    m += nanotimer::humanreadable(avgtime);
    uio::window()->statusBar()->showMessage(m);
}*/

///Sleep 1s and process events

void g2m::sleepSecond() {
    //uio::window()->statusBar()->clearMessage();
    std::cout << "Waiting for interpreter... \n";
    QTime dieTime = QTime::currentTime().addSecs(1);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);
    return;
}

void g2m::infoMsg(std::string s) {
    std::cout << s << std::endl;
}






