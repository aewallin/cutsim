
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPluginLoader>

#include <cutsim/cutsim.hpp>
#include <cutsim/glwidget.hpp>
//#include "qoccviewercontext.h"
//#include "qoccviewwidget.h"
//#include "qoccinputoutput.h"
//#include "lnk.hh"

class QAction;
class QLabel;
class QMenu;
// class cam;

class CutsimWindow : public QMainWindow {
    Q_OBJECT

public:
    //QStringList ags)
    
    CutsimWindow(QStringList ags) : args(ags), myLastFolder(tr("")) {
        myGLWidget = new cutsim::GLWidget(); //Note this has changed!
        //myCutsim  = new cutsim::Cutsim();
        unsigned int max_depth=8;
        double octree_cube_side=10.0;
        myCutsim  =  new cutsim::Cutsim(octree_cube_side , max_depth, myGLWidget);
        

        this->setCentralWidget(myGLWidget);
        
        createActions();
        createMenus();
        //statusBar()->showMessage(tr("A context menu is available by right-clicking"));
        setWindowTitle(tr(" cutsim - alpha"));
        setMinimumSize(300, 300);
        showNormal();
        resize(789,527);  //so that it will fit nicely on 1/4 of my screen
        //showMaximized();
        //lnk mylink(this);
    }
    
	//Handle_AIS_InteractiveContext& getContext() { return myVC->getContext(); };
	//Handle_V3d_View getView() { return myOCC->getView(); };
	QAction* getHelpMenu() { return helpAction; };
    QString getArg(int n) {if(n<=args.count())return args[n];else return "n--";};
    QStringList* getArgs() {return &args;};
private slots:
    void newFile() {
        statusBar()->showMessage(tr("Invoked File|New"));
        //myOCC->getView()->ColorScaleErase();
        //myVC->deleteAllObjects();
    }
    void open() {
        QString		fileName;
        QString		fileType;
        QFileInfo	fileInfo;
        //QoccInputOutput::FileFormat format;
        //QoccInputOutput reader;
        statusBar()->showMessage(tr("Invoked File|Open"));
        //myVC->getContext()->CloseAllContexts();		//mark 1-29-09 - prevent wireframe display (should be shaded) if a local context is open
        fileName = QFileDialog::getOpenFileName (this,
                            tr("Open File"),
                            myLastFolder,
                            tr( "G-code (*.ngc);;"
                                ) );
        if (!fileName.isEmpty()) {
            fileInfo.setFile(fileName);
            fileType = fileInfo.suffix();
            if (fileType.toLower() == tr("G-code") || fileType.toLower() == tr("ngc"))  {
                statusBar()->showMessage(tr(" Opening g-code file"));
//                format = QoccInputOutput::FormatBREP;
            }
            myLastFolder = fileInfo.absolutePath();
            //reader.importModel ( fileInfo.absoluteFilePath(), format, myOCC->getContext() );
        }

        //myOCC->fitAll();

    }
    void save(){
        statusBar()->showMessage(tr("Invoked File|Save"));
    }
    //void print(){}
    //void undo(){}
    //void redo(){}
    //void cut(){}
    //void copy(){}
    //void paste(){}
    void about() {
        statusBar()->showMessage(tr("Invoked Help|About"));
        QMessageBox::about(this, tr("About Menu"),
            tr("<b>cutsim</b> is an open-source cutting simulation. (C) 2011 Anders Wallin."));
    }
	//void bottle(){}
    void aboutQt(){}
//	void xyzPosition (V3d_Coordinate X,
//					  V3d_Coordinate Y,
//					  V3d_Coordinate Z);
//	void addPoint (V3d_Coordinate X,
//				   V3d_Coordinate Y,
//				   V3d_Coordinate Z);
	void statusMessage (const QString aMessage) {
        statusBar()->showMessage(aMessage);
    }

private:
    QStringList args;
    void createActions() {
        newAction = new QAction(tr("&New"), this);
        newAction->setShortcut(tr("Ctrl+N"));
        newAction->setStatusTip(tr("Create a new file"));
        connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

        openAction = new QAction(tr("&Open..."), this);
        openAction->setShortcut(tr("Ctrl+O"));
        openAction->setStatusTip(tr("Open an existing file"));
        connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

/*
        saveAction = new QAction(tr("&Save"), this);
        saveAction->setShortcut(tr("Ctrl+S"));
        saveAction->setStatusTip(tr("Save the document to disk"));
        connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

        printAction = new QAction(tr("&Print..."), this);
        printAction->setShortcut(tr("Ctrl+P"));
        printAction->setStatusTip(tr("Print the document"));
        connect(printAction, SIGNAL(triggered()), this, SLOT(print()));
*/
        exitAction = new QAction(tr("E&xit"), this);
        exitAction->setShortcut(tr("Ctrl+X"));
        exitAction->setStatusTip(tr("Exit the application"));
        connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

/*
        undoAction = new QAction(tr("&Undo"), this);
        undoAction->setShortcut(tr("Ctrl+Z"));
        undoAction->setStatusTip(tr("Undo the last operation"));
        connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));

        redoAction = new QAction(tr("&Redo"), this);
        redoAction->setShortcut(tr("Ctrl+Y"));
        redoAction->setStatusTip(tr("Redo the last operation"));
        connect(redoAction, SIGNAL(triggered()), this, SLOT(redo()));

        cutAction = new QAction(tr("Cu&t"), this);
        cutAction->setShortcut(tr("Ctrl+X"));
        cutAction->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
        connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));

        copyAction = new QAction(tr("&Copy"), this);
        copyAction->setShortcut(tr("Ctrl+C"));
        copyAction->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
        connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));

        pasteAction = new QAction(tr("&Paste"), this);
        pasteAction->setShortcut(tr("Ctrl+V"));
        pasteAction->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
        connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
*/
        aboutAction = new QAction(tr("&About"), this);
        aboutAction->setStatusTip(tr("Show the application's About box"));
        connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
/*
        aboutQtAction = new QAction(tr("About &Qt"), this);
        aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
        connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
        connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));
*/

        // Now for the QtOCCViewWidget slots.
    /*
        fitAction = new QAction(tr("&Fit Window"), this);
        fitAction->setShortcut(tr("Ctrl+F"));
        fitAction->setStatusTip(tr("Fit to window"));
        connect(fitAction, SIGNAL(triggered()), myOCC, SLOT(fitExtents()));
    */
    /*
        fitAllAction = new QAction(tr("&Fit All"), this);
        fitAllAction->setShortcut(tr("Ctrl+F"));
        fitAllAction->setStatusTip(tr("Fit contents to viewport"));
        connect(fitAllAction, SIGNAL(triggered()), myOCC, SLOT(fitAll()));

        zoomAction = new QAction(tr("&Zoom"), this);
        zoomAction->setStatusTip(tr("Zoom in window"));
        connect(zoomAction, SIGNAL(triggered()), myOCC, SLOT(fitArea()));

        panAction = new QAction(tr("&Pan"), this);
        panAction->setStatusTip(tr("Window panning"));
        connect(panAction, SIGNAL(triggered()), myOCC, SLOT(pan()));

        rotAction = new QAction(tr("&Rotate"), this);
        rotAction->setShortcut(tr("Ctrl+R"));
        rotAction->setStatusTip(tr("Window rotation"));
        connect(rotAction, SIGNAL(triggered()), myOCC, SLOT(rotation()));

        gridToggleAction = new QAction(tr("Toggle &Grid"), this);
        gridToggleAction->setShortcut(tr("Ctrl+G"));
        gridToggleAction->setStatusTip(tr("Turn the grid on or off"));
        connect(gridToggleAction, SIGNAL(triggered()), myVC, SLOT(gridToggle()));
*/
    /*	gridOffAction = new QAction(tr("Gri&d Off"), this);
        gridOffAction->setShortcut(tr("Ctrl+D"));
        gridOffAction->setStatusTip(tr("Turn the grid on"));
        connect(gridOffAction, SIGNAL(triggered()), myVC, SLOT(gridOff()));
    */
    /*
        gridXYAction = new QAction(tr("XY Grid"), this);
        gridXYAction->setStatusTip(tr("Grid on XY Plane"));
        //gridOffAction->setShortcut(tr("Ctrl+Z"));
        connect(gridXYAction, SIGNAL(triggered()), myVC, SLOT(gridXY()));

        gridXZAction = new QAction(tr("XZ Grid"), this);
        gridXZAction->setStatusTip(tr("Grid on XZ Plane"));
        //gridXZAction->setShortcut(tr("Ctrl+Y"));
        connect(gridXZAction, SIGNAL(triggered()), myVC, SLOT(gridXZ()));

        gridYZAction = new QAction(tr("YZ Grid"), this);
        gridYZAction->setStatusTip(tr("Grid on YZ Plane"));
        //gridOffAction->setShortcut(tr("Ctrl+Z"));
        connect(gridYZAction, SIGNAL(triggered()), myVC, SLOT(gridYZ()));

        gridRectAction = new QAction(tr("Rectangular"), this);
        gridRectAction->setStatusTip(tr("Retangular grid"));
        //gridOffAction->setShortcut(tr("Ctrl+Z"));
        connect(gridRectAction, SIGNAL(triggered()), myVC, SLOT(gridRect()));

        gridCircAction = new QAction(tr("Circular"), this);
        gridCircAction->setStatusTip(tr("Circular grid"));
        //gridOffAction->setShortcut(tr("Ctrl+Z"));
        connect(gridCircAction, SIGNAL(triggered()), myVC, SLOT(gridCirc()));

        // Standard View

        viewFrontAction = new QAction(tr("Front"), this);
        viewFrontAction->setStatusTip(tr("View From Front"));
        connect(viewFrontAction, SIGNAL(triggered()), myOCC, SLOT(viewFront()));

        viewBackAction = new QAction(tr("Back"), this);
        viewBackAction->setStatusTip(tr("View From Back"));
        connect(viewBackAction, SIGNAL(triggered()), myOCC, SLOT(viewBack()));

        viewTopAction = new QAction(tr("Top"), this);
        viewTopAction->setStatusTip(tr("View From Top"));
        connect(viewTopAction, SIGNAL(triggered()), myOCC, SLOT(viewTop()));

        viewBottomAction = new QAction(tr("Bottom"), this);
        viewBottomAction->setStatusTip(tr("View From Bottom"));
        connect(viewBottomAction, SIGNAL(triggered()), myOCC, SLOT(viewBottom()));

        viewLeftAction = new QAction(tr("Left"), this);
        viewLeftAction->setStatusTip(tr("View From Left"));
        connect(viewLeftAction, SIGNAL(triggered()), myOCC, SLOT(viewLeft()));

        viewRightAction = new QAction(tr("Right"), this);
        viewRightAction->setStatusTip(tr("View From Right"));
        connect(viewRightAction, SIGNAL(triggered()), myOCC, SLOT(viewRight()));

        viewAxoAction = new QAction(tr("&Axonometric Fit"), this);
        viewAxoAction->setStatusTip(tr("Axonometric view and fit all"));
        viewAxoAction->setShortcut(tr("Ctrl+A"));
        connect(viewAxoAction, SIGNAL(triggered()), myOCC, SLOT(viewAxo()));

        viewGridAction = new QAction(tr("Grid"), this);
        viewGridAction->setStatusTip(tr("View from grid"));
        connect(viewGridAction, SIGNAL(triggered()), myOCC, SLOT(viewGrid()));

        viewResetAction = new QAction(tr("Reset"), this);
        viewResetAction->setStatusTip(tr("Reset the view"));
        connect(viewResetAction, SIGNAL(triggered()), myOCC, SLOT(viewReset()));
*/
/*
        backgroundAction = new QAction( tr("&Background"), this );
        backgroundAction->setStatusTip(tr("Change the background colour"));
        connect(backgroundAction, SIGNAL(triggered()), myOCC, SLOT(background()));
*/

        // The co-ordinates from the view
        /*
        connect( myOCC, SIGNAL(mouseMoved(V3d_Coordinate,V3d_Coordinate,V3d_Coordinate)),
                 this,   SLOT(xyzPosition(V3d_Coordinate,V3d_Coordinate,V3d_Coordinate)) );

        // Add a point from the view
        connect( myOCC, SIGNAL(pointClicked(V3d_Coordinate,V3d_Coordinate,V3d_Coordinate)),
                 this,   SLOT (addPoint    (V3d_Coordinate,V3d_Coordinate,V3d_Coordinate)) );

        connect( myOCC, SIGNAL(sendStatus(const QString)),
                 this,  SLOT  (statusMessage(const QString)) );

        // And the bottle example

        bottleAction = new QAction(tr("Load &Bottle"), this);
        bottleAction->setShortcut(tr("Ctrl+B"));
        bottleAction->setStatusTip(tr("Bottle sample."));
        connect(bottleAction, SIGNAL(triggered()), this, SLOT(bottle()));
*/
    }
    void createMenus() {
        fileMenu = menuBar()->addMenu( tr("&File") );
            fileMenu->addAction( newAction );
            fileMenu->addAction( openAction );
            //fileMenu->addAction( saveAction );
            //fileMenu->addAction( printAction );
    /*
            Comment out the 2 lines below to hide the
            Load Bottle menu option - still left in for
            now as a demo feature.
    */
            //fileMenu->addSeparator();
            //fileMenu->addAction( bottleAction );

            fileMenu->addSeparator();
            fileMenu->addAction( exitAction );
/*
        editMenu = menuBar()->addMenu( tr("&Edit") );
            editMenu->addAction( undoAction );
            editMenu->addAction( redoAction );
            editMenu->addSeparator();
            editMenu->addAction( cutAction );
            editMenu->addAction( copyAction );
            editMenu->addAction( pasteAction );
*/
/*
        viewMenu = menuBar()->addMenu( tr("&View") );
            viewDisplayMenu = viewMenu->addMenu( tr("&Display") );
                viewDisplayMenu->addAction( viewFrontAction );
                viewDisplayMenu->addAction( viewBackAction );
                viewDisplayMenu->addAction( viewTopAction );
                viewDisplayMenu->addAction( viewBottomAction );
                viewDisplayMenu->addAction( viewLeftAction );
                viewDisplayMenu->addAction( viewRightAction );
                viewDisplayMenu->addSeparator();
                viewDisplayMenu->addAction( viewAxoAction );
                viewDisplayMenu->addAction( viewGridAction );
                viewDisplayMenu->addSeparator();
                viewDisplayMenu->addAction( viewResetAction );
                viewDisplayMenu->addSeparator();
                viewDisplayMenu->addAction( backgroundAction );
*/

/*
            viewActionsMenu = viewMenu->addMenu( tr("&Actions") );
                //viewActionsMenu->addAction( fitAction );
                viewActionsMenu->addAction( fitAllAction );
                viewActionsMenu->addAction( zoomAction );
                viewActionsMenu->addAction( panAction );
                viewActionsMenu->addAction( rotAction );
*/

/*
            gridMenu = viewMenu->addMenu( tr("&Grid") );
                gridMenu->addAction( gridToggleAction );
                //gridMenu->addAction( gridOffAction );
                gridMenu->addSeparator();
                gridMenu->addAction( gridXYAction );
                gridMenu->addAction( gridXZAction );
                gridMenu->addAction( gridYZAction );
                gridMenu->addSeparator();
                gridMenu->addAction( gridRectAction );
                gridMenu->addAction( gridCircAction );
                */
        helpMenu = new QMenu(tr("&Help"));
            helpAction = menuBar()->addMenu(helpMenu);
            helpMenu->addAction(aboutAction);
            //helpMenu->addAction(aboutQtAction);
    }

    QMenu *fileMenu;
    //QMenu *editMenu;
    QMenu *viewMenu;
    //QMenu *viewActionsMenu;
    //QMenu *viewDisplayMenu;
    //QMenu *formatMenu;
    QMenu *helpMenu;
    QAction *helpAction;  //this is so we can insert menus before the help menu...
    //QMenu *gridMenu;

    QAction *newAction;
    QAction *openAction;
    //QAction *saveAction;
    //QAction *printAction;
    QAction *exitAction;
    //QAction *undoAction;
    //QAction *redoAction;
    //QAction *cutAction;
    //QAction *copyAction;
    //QAction *pasteAction;
    QAction *aboutAction;

	//QAction *fitAction;
    /*
	QAction *fitAllAction;
	QAction *zoomAction;
	QAction *panAction;
	QAction *rotAction;

	QAction *gridXYAction;
	QAction *gridXZAction;
	QAction *gridYZAction;
	QAction *gridToggleAction;
	//QAction *gridOffAction;
	QAction *gridRectAction;
	QAction *gridCircAction;

    //QAction *viewDefaultAction;

	QAction *viewFrontAction;
	QAction *viewBackAction;
	QAction *viewTopAction;
	QAction *viewBottomAction;
	QAction *viewLeftAction;
	QAction *viewRightAction;
	QAction *viewAxoAction;
	QAction *viewResetAction;
	QAction *viewGridAction; 
*/
	QAction *backgroundAction;

    QAction *aboutQtAction;

    cutsim::Cutsim* myCutsim;
    cutsim::GLWidget* myGLWidget;
    
    QString myLastFolder;

  public:
    //QoccViewWidget*     getOCC() { return myOCC; };
    //QoccViewerContext*  getVC() { return myVC; };

};

#endif


