#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
//#include <QPluginLoader>
//#include <QMutex>

#include <cutsim/cutsim.hpp>
#include <cutsim/glwidget.hpp>

#include <g2m/g2m.hpp>
#include <g2m/gplayer.hpp>

#include "version_string.hpp"
#include "text_area.hpp"

class QAction;
class QLabel;
class QMenu;

/// the main application window for the cutting-simulation
/// this includes menus, toolbars, text-areas for g-code and canon-lines and debug
/// the 3D view of tool/stock.
class CutsimWindow : public QMainWindow {
    Q_OBJECT
public:
    /// create window
    CutsimWindow(QStringList ags);
    void findInterp();
    void chooseToolTable();
    /// return helpAction
    QAction* getHelpMenu() { return helpAction; };
    /// return the n'th argument given to the constructor
    QString getArg(int n) {if(n<=args.count())return args[n];else return "n--";}
    /// return all arguments
    QStringList* getArgs() {return &args;}
public slots:
    /// set progress value (0..100)
    void slotSetProgress(int n) { myProgress->setValue(n); }
    /// show a debug message
    void debugMessage(QString s) { debugText->appendLine(s); }
    /// add a Qstring line to the g-code window
    void appendGcodeLine(QString s) { gcodeText->appendLine(s); }
    /// add a Qstring line to the canon-line window
    void appendCanonLine(QString s) { canonText->appendLine(s); }
    /// position tool
    void slotSetToolPosition(double x, double y, double z);
    /// change the tool
    void slotToolChange(int t);
    /// slot called by worker tasks when a diff-operation is done
    void slotDiffDone();
    /// slot called by GL-thread when GL is updated
    void slotGLDone();
signals:
    /// signal other objects (g2m) with the path to the g-code file
    void setGcodeFile(QString f);
    /// signal to g2m with the path to the rs274 binary
    void setRS274(QString s);
    /// signal to g2m the path to the tootable
    void setToolTable(QString s);
    /// signal to g2m to start interpreting
    void interpret();
    /// play signal to Gplayer
    void play();
    /// pause signal to Gplayer
    void pause();
    /// stop signal to Gplayer
    void stop();
    /// emitted when the current move is done and we can request a enw one
    void signalMoveDone();
private slots:
    void newFile() { statusBar()->showMessage(tr("Invoked File|New")); }
    void open();
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
    
    void createDock();
    void createToolBar();
    void createActions();
    void createMenus();

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
    QStringList args;
    QString myLastFolder;
    QSettings settings;
};

#endif


