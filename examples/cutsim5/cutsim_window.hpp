#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QPluginLoader>

#include <cutsim/cutsim.hpp>
#include <cutsim/glwidget.hpp>

#include <g2m/g2m.hpp>

class QAction;
class QLabel;
class QMenu;

class CutsimWindow : public QMainWindow {
    Q_OBJECT
public:
    CutsimWindow(QStringList ags) : args(ags), myLastFolder(tr("")) {
        myGLWidget = new cutsim::GLWidget(); 
        unsigned int max_depth=8;
        double octree_cube_side=10.0;
        myCutsim  =  new cutsim::Cutsim(octree_cube_side , max_depth, myGLWidget);
        this->setCentralWidget(myGLWidget);
        
        // g-code interpreter
        myG2m = new g2m::g2m();
        
        QDockWidget *dockWidget1 = new QDockWidget(this);
        QDockWidget *dockWidget2 = new QDockWidget(this);
        addDockWidget(Qt::RightDockWidgetArea, dockWidget1);
        addDockWidget(Qt::BottomDockWidgetArea, dockWidget2);
        
        createActions();
        createMenus();
        setStatusBar( new QStatusBar(this) );
        myToolBar = new QToolBar(this);
        addToolBar( myToolBar );
        createToolBar();        

        setWindowTitle(tr(" cutsim - alpha"));
        //setMinimumSize(300, 300);
        showNormal();
        move(100,100); // position the main window
        resize(789,527);  // size window
        //showMaximized();
        //lnk mylink(this);
    }
    
    QAction* getHelpMenu() { return helpAction; };
    QString getArg(int n) {if(n<=args.count())return args[n];else return "n--";};
    QStringList* getArgs() {return &args;};
private slots:
    void newFile() {
        statusBar()->showMessage(tr("Invoked File|New"));
    }
    void open() {
        QString     fileName;
        QString     fileType;
        QFileInfo   fileInfo;

        statusBar()->showMessage(tr("Invoked File|Open"));
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
            }
            myLastFolder = fileInfo.absolutePath();
            //reader.importModel ( fileInfo.absoluteFilePath(), format, myOCC->getContext() );
        }
    }
    void save(){
        statusBar()->showMessage(tr("Invoked File|Save"));
    }
    void runProgram() {
        statusBar()->showMessage(tr("Running program..."));
    }
    void pauseProgram() {
        statusBar()->showMessage(tr("pause program."));
    }
    void stopProgram() {
        statusBar()->showMessage(tr("stop program."));
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
    void createToolBar() {
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
    
    QToolBar * myToolBar;
    cutsim::Cutsim* myCutsim;
    cutsim::GLWidget* myGLWidget;
    g2m::g2m* myG2m;
    QString myLastFolder;
};

#endif


