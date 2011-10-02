#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QPluginLoader>

#include <cutsim/cutsim.hpp>
#include <cutsim/glwidget.hpp>

#include <g2m/g2m.hpp>
#include <g2m/gplayer.hpp>

#include "text_area.hpp"

class QAction;
class QLabel;
class QMenu;

class CutsimWindow : public QMainWindow {
    Q_OBJECT
public:
    CutsimWindow(QStringList ags) : args(ags), myLastFolder(tr("")), settings("github.aewallin.cutsim","cutsim") {
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
        statusBar()->insertPermanentWidget( 1, myProgress , 0);        
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

        setWindowTitle(tr(" cutsim - alpha"));
        //setMinimumSize(300, 300);
        showNormal();
        move(100,100); // position the main window
        resize(789,527);  // size window
        //showMaximized();
    }

    ///find the interpreter. uses QSettings, so user is only asked once unless the file is deleted
    void findInterp() {
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
    void chooseToolTable() {
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

    QAction* getHelpMenu() { return helpAction; };
    QString getArg(int n) {if(n<=args.count())return args[n];else return "n--";};
    QStringList* getArgs() {return &args;};
public slots:
    void slotSetProgress(int n) { myProgress->setValue(n); }
    void debugMessage(QString s) {  
        debugText->appendLine(s);
    }
    void appendGcodeLine(QString s) {  
        gcodeText->appendLine(s);
    }
    void appendCanonLine(QString s) {  
        canonText->appendLine(s);
    }
    void slotSetToolPosition(double x, double y, double z) {
        //debugMessage( tr("ui: setToolPosition( %1, )").arg(x) );
        myTools[currentTool]->setCenter( cutsim::GLVertex(x,y,z) );
        myCutsim->slot_diff_volume( myTools[currentTool] ); 
    }
    void slotToolChange(int t) {
        debugMessage( tr("ui: Tool-change to  %1 ").arg(t) );
        currentTool = t-1;
    }
signals:
    void setGcodeFile(QString f);
    void setRS274(QString s);
    void setToolTable(QString s);
    void interpret();
    void play();
    void pause();
    void stop();
private slots:
    void newFile() { statusBar()->showMessage(tr("Invoked File|New")); }
    void open() {
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
    void save(){
        statusBar()->showMessage(tr("Invoked File|Save"));
    }
    void runProgram() {
        statusBar()->showMessage(tr("Running program..."));
        emit play();
    }
    void pauseProgram() {
        statusBar()->showMessage(tr("pause program."));
        emit pause();
    }
    void stopProgram() {
        statusBar()->showMessage(tr("stop program."));
        emit stop();
    }

    void about() {
        statusBar()->showMessage(tr("Invoked Help|About"));
        QMessageBox::about(this, tr("About Menu"),
            tr("<b>cutsim</b> is an open-source cutting simulation. <p>(C) 2011 Anders Wallin."));
    }
    void statusMessage (const QString aMessage) {
        statusBar()->showMessage(aMessage);
    }

private:
    QStringList args;
    void createDock() {
        QDockWidget* dockWidget1 = new QDockWidget(this);
        dockWidget1->setWindowTitle("Debug");
        debugText = new TextArea(); 
        dockWidget1->setWidget(debugText);
        //lab->setText("Test label text");
        //lab->setReadOnly(true);
        
        
        QDockWidget *dockWidget2 = new QDockWidget(this);
        dockWidget2->setWindowTitle("G-Code");
        gcodeText = new TextArea(); 
        dockWidget2->setWidget(gcodeText);
        gcodeText->setReadOnly(true);
        //gcode->appendLine("test g-code text");
        //gcode->appendLine("line2");
        QDockWidget *dockWidget3 = new QDockWidget(this);
        dockWidget3->setWindowTitle("CANON-lines");
        canonText = new TextArea(); 
        dockWidget3->setWidget(canonText);
        canonText->setReadOnly(true);
        
        
        addDockWidget(Qt::RightDockWidgetArea, dockWidget2);
        addDockWidget(Qt::RightDockWidgetArea, dockWidget3);
        addDockWidget(Qt::BottomDockWidgetArea, dockWidget1);
        
    }
    void createToolBar() {
        myToolBar = new QToolBar(this);
        addToolBar( myToolBar );
        myToolBar->addAction( newAction );
        myToolBar->addAction( openAction );
        myToolBar->addSeparator();
        myToolBar->addAction( playAction );
        myToolBar->addAction( pauseAction );
        myToolBar->addAction( stopAction );
    }
    void createActions() {        
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
    
    void createMenus() {
        fileMenu = menuBar()->addMenu( tr("&File") );
            fileMenu->addAction( newAction );
            fileMenu->addAction( openAction );
            fileMenu->addSeparator();
            fileMenu->addAction( exitAction );

        helpMenu = new QMenu(tr("&Help"));
            helpAction = menuBar()->addMenu(helpMenu);
            helpMenu->addAction(aboutAction);
    }

    QMenu *fileMenu;
    QMenu *helpMenu;

    QAction *helpAction;  

    QAction *newAction;
    QAction *openAction;
    QAction *exitAction;
    QAction *aboutAction;

    QAction *playAction;
    QAction *pauseAction;
    QAction *stopAction;
    
    QProgressBar* myProgress;
    QToolBar* myToolBar;
    cutsim::Cutsim* myCutsim;
    cutsim::GLWidget* myGLWidget;
    std::vector<cutsim::SphereVolume*> myTools;
    unsigned int currentTool;
    
    g2m::g2m* myG2m;
    g2m::GPlayer* myPlayer;
    TextArea* debugText;
    TextArea* gcodeText;
    TextArea* canonText;

    QString myLastFolder;
    QSettings settings;
};

#endif


