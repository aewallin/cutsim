/*  
 *  Copyright 2010-2011 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  
 *  This file is part of OpenCAMlib.
 *
 *  OpenCAMlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenCAMlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OpenCAMlib.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "cutsim_window.hpp"




CutsimWindow::CutsimWindow(QStringList ags) : args(ags), myLastFolder(tr("")), settings("github.aewallin.cutsim","cutsim") {
        myGLWidget = new cutsim::GLWidget(); 
        unsigned int max_depth=8;
        double octree_cube_side=10.0;
        myCutsim  =  new cutsim::Cutsim(octree_cube_side , max_depth, myGLWidget);
        this->setCentralWidget(myGLWidget);
        
        // hard-coded stock
        cutsim::SphereVolume* stock = new cutsim::SphereVolume();
        stock->setRadius(7);
        stock->setCenter( cutsim::GLVertex(0,0,0) );
        stock->setColor(0,1,1);
        myCutsim->sum_volume(stock);
        
        currentTool = 0;
        // hard-coded tool
        cutsim::SphereVolume* s1 = new cutsim::SphereVolume();
        s1->setRadius(2);
        s1->setCenter( cutsim::GLVertex(0,0,0) );
        s1->setColor(1,1,0);
        myTools.push_back(s1);
        
        cutsim::SphereVolume* s2 = new cutsim::SphereVolume();
        s2->setRadius(3);
        s2->setCenter( cutsim::GLVertex(0,0,0) );
        s2->setColor(1,0,0);
        myTools.push_back(s2);
        
        cutsim::SphereVolume* s3 = new cutsim::SphereVolume();
        s3->setRadius(4);
        s3->setCenter( cutsim::GLVertex(0,0,0) );
        s3->setColor(0,1,0);
        myTools.push_back(s3);
        
        cutsim::SphereVolume* s4 = new cutsim::SphereVolume();
        s4->setRadius(2);
        s4->setCenter( cutsim::GLVertex(0,0,0) );
        s4->setColor(0,0,1);
        myTools.push_back(s4);
        
        createDock();
        createActions();
        createMenus();
        setStatusBar( new QStatusBar(this) );
        myProgress = new QProgressBar();
        myProgress->setMaximum(100);
        myProgress->setMinimum(0);
        statusBar()->insertPermanentWidget( 0, myProgress , 0);        
        createToolBar();        
        
        myG2m = new g2m::g2m(); // g-code interpreter
        connect( this, SIGNAL(setGcodeFile(QString)), myG2m, SLOT(setFile(QString)) );
        
        connect( this, SIGNAL(setRS274(QString)), myG2m, SLOT(setInterp(QString)) );
        connect( this, SIGNAL(setToolTable(QString)), myG2m, SLOT(setToolTable(QString)) );
        connect( this, SIGNAL( interpret() ), myG2m, SLOT( interpret_file() ) );
        connect( myG2m, SIGNAL( debugMessage(QString) ), this, SLOT( debugMessage(QString) ) );
        
        connect( myG2m, SIGNAL( gcodeLineMessage(QString) ), this, SLOT( appendGcodeLine(QString) ) );
        connect( myG2m, SIGNAL( canonLineMessage(QString) ), this, SLOT( appendCanonLine(QString) ) );
        
        myPlayer = new g2m::GPlayer();
        connect( myPlayer, SIGNAL( debugMessage(QString) ), this, SLOT( debugMessage(QString) ) );
        connect( this, SIGNAL( play() ), myPlayer, SLOT( play() ) );
        connect( this, SIGNAL( pause() ), myPlayer, SLOT( pause() ) );
        connect( this, SIGNAL( stop() ), myPlayer, SLOT( stop() ) );
        connect( myPlayer, SIGNAL( signalProgress(int) ), this, SLOT( slotSetProgress(int) ) );

        connect( myG2m, SIGNAL( signalCanonLine(canonLine*) ), myPlayer, SLOT( appendCanonLine(canonLine*) ) );

        connect( myPlayer, SIGNAL( signalToolPosition(double,double,double) ), this, SLOT( slotSetToolPosition(double,double,double) ) );
        connect( myPlayer, SIGNAL( signalToolChange( int ) ), this, SLOT( slotToolChange(int) ) );
     

        findInterp();
        chooseToolTable();

        QString title = tr(" cutsim - ") + VERSION_STRING;
        setWindowTitle(title);
        //setMinimumSize(300, 300);
        showNormal();
        move(100,100); // position the main window
        resize(789,527);  // size window
        //showMaximized();
        
        myCutsim->updateGL();
}

void CutsimWindow::slotSetToolPosition(double x, double y, double z) {
    //debugMessage( tr("ui: setToolPosition( %1, )").arg(x) );
    myTools[currentTool]->setCenter( cutsim::GLVertex(x,y,z) );
    myCutsim->slot_diff_volume( myTools[currentTool] ); 
    myCutsim->updateGL();
}

void CutsimWindow::slotToolChange(int t) {
    debugMessage( tr("ui: Tool-change to  %1 ").arg(t) );
    currentTool = t-1;
}    

///find the interpreter. uses QSettings, so user is only asked once unless the file is deleted
void CutsimWindow::findInterp() {
    QString interp;
    interp = settings.value("rs274/binary","/usr/bin/rs274").toString();
    if (!QFileInfo(interp).isExecutable()) {
        QString m = "Tried to use ";
        m += interp + " as the interpreter, but it doesn't exist or isn't executable.";
        debugMessage(m);
        interp = QFileDialog::getOpenFileName ( this, "Locate rs274 interpreter", "~", "EMC2 stand-alone interpreter (rs274)" );
        if (!QFileInfo(interp).isExecutable()) {
            debugMessage("interpreter does not exist!");
        }
        settings.setValue("rs274/binary",interp);
    }
    emit setRS274( interp );
}

///find the tool table. uses QSettings, to preselect the last table used
void CutsimWindow::chooseToolTable() {
    QString path;
    bool ttconf = settings.contains("rs274/tool-table");
    if (ttconf) {
        //passing the file name as the path means that it is preselected
        path = settings.value("rs274/tool-table").toString();
    } else {
        path =  "/usr/share/doc/emc2/examples/sample-configs/sim"; //location when installed
    }
    path = QFileDialog::getOpenFileName ( this, "Locate tool table", path, "EMC2 new-style tool table (*.tbl)" );

    if (!QFileInfo(path).exists()){
        debugMessage("tool table does not exist!");
    }
    settings.setValue("rs274/tool-table",path);
    emit setToolTable(path);
}

void CutsimWindow::open() {
    QString     fileName;
    QString     fileType;
    QFileInfo   fileInfo;

    statusBar()->showMessage(tr("Open G-code file"));
    fileName = QFileDialog::getOpenFileName (this,
                        tr("Open G-code File"),
                        myLastFolder,
                        tr( "G-code (*.ngc);;"
                            ) );
    if (!fileName.isEmpty()) {
        fileInfo.setFile(fileName);
        fileType = fileInfo.suffix();
        if (fileType.toLower() == tr("G-code") || fileType.toLower() == tr("ngc"))  {
            statusBar()->showMessage( tr(" Opening g-code file %1").arg(fileName) );
        }
        myLastFolder = fileInfo.absolutePath();
        emit setGcodeFile( fileName );
        emit interpret();
        //reader.importModel ( fileInfo.absoluteFilePath(), format, myOCC->getContext() );
    }
}

void CutsimWindow::createDock() {
    QDockWidget* dockWidget1 = new QDockWidget(this);
    dockWidget1->setWindowTitle("Debug");
    debugText = new TextArea(); 
    dockWidget1->setWidget(debugText);
    debugText->setReadOnly(true);
    
    QDockWidget *dockWidget2 = new QDockWidget(this);
    dockWidget2->setWindowTitle("G-Code");
    gcodeText = new TextArea(); 
    dockWidget2->setWidget(gcodeText);
    gcodeText->setReadOnly(true);
    QDockWidget *dockWidget3 = new QDockWidget(this);
    dockWidget3->setWindowTitle("CANON-lines");
    canonText = new TextArea(); 
    dockWidget3->setWidget(canonText);
    canonText->setReadOnly(true);
    
    
    addDockWidget(Qt::RightDockWidgetArea, dockWidget2);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget3);
    addDockWidget(Qt::BottomDockWidgetArea, dockWidget1);
    
}

void CutsimWindow::createToolBar() {
    myToolBar = new QToolBar(this);
    addToolBar( myToolBar );
    myToolBar->addAction( newAction );
    myToolBar->addAction( openAction );
    myToolBar->addSeparator();
    myToolBar->addAction( playAction );
    myToolBar->addAction( pauseAction );
    myToolBar->addAction( stopAction );
}

void CutsimWindow::createActions() {        
    QIcon newIcon = QIcon::fromTheme("document-new");
    newAction = new QAction(newIcon,tr("&New"), this);
    newAction->setShortcut(tr("Ctrl+N"));
    newAction->setStatusTip(tr("Create a new file"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));
    
    QIcon openIcon = QIcon::fromTheme("document-open" );
    openAction = new QAction(openIcon, tr("&Open..."), this);
    openAction->setShortcut(tr("Ctrl+O"));
    openAction->setStatusTip(tr("Open an existing file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+X"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    
    QIcon playIcon = QIcon::fromTheme("media-playback-start");
    playAction = new QAction(playIcon,tr("&Play"), this);
    playAction->setShortcut(tr("Ctrl+P"));
    playAction->setStatusTip(tr("Run G-code program"));
    connect(playAction, SIGNAL(triggered()), this, SLOT( runProgram() ));
    
    QIcon pauseIcon = QIcon::fromTheme("media-playback-pause");
    pauseAction = new QAction(pauseIcon,tr("&Play"), this);
    pauseAction->setShortcut(tr("Ctrl+L"));
    pauseAction->setStatusTip(tr("Pause"));
    connect(pauseAction, SIGNAL(triggered()), this, SLOT( pauseProgram() ));
    
    QIcon stopIcon = QIcon::fromTheme("media-playback-stop");
    stopAction = new QAction(stopIcon,tr("&Stop"), this);
    stopAction->setShortcut(tr("Ctrl+C"));
    stopAction->setStatusTip(tr("Stop"));
    connect(stopAction, SIGNAL(triggered()), this, SLOT( stopProgram() ));

}

void CutsimWindow::createMenus() {
    fileMenu = menuBar()->addMenu( tr("&File") );
        fileMenu->addAction( newAction );
        fileMenu->addAction( openAction );
        fileMenu->addSeparator();
        fileMenu->addAction( exitAction );

    helpMenu = new QMenu(tr("&Help"));
        helpAction = menuBar()->addMenu(helpMenu);
        helpMenu->addAction(aboutAction);
}
