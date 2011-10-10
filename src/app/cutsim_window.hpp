#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QPluginLoader>

#include <cutsim/cutsim.hpp>
#include <cutsim/glwidget.hpp>

#include <g2m/g2m.hpp>
#include <g2m/gplayer.hpp>

#include "version_string.hpp"
#include "text_area.hpp"

class QAction;
class QLabel;
class QMenu;

class CutsimWindow : public QMainWindow {
    Q_OBJECT
public:
    CutsimWindow(QStringList ags);
    void findInterp();
    void chooseToolTable();
    QAction* getHelpMenu() { return helpAction; };
    QString getArg(int n) {if(n<=args.count())return args[n];else return "n--";};
    QStringList* getArgs() {return &args;};
public slots:
    void slotSetProgress(int n) { myProgress->setValue(n); }
    void debugMessage(QString s) { debugText->appendLine(s); }
    void appendGcodeLine(QString s) { gcodeText->appendLine(s); }
    void appendCanonLine(QString s) { canonText->appendLine(s); }
    void slotSetToolPosition(double x, double y, double z);
    void slotToolChange(int t);
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


