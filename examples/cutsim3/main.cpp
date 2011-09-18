#include <QApplication>

#include <functional>
#include <boost/bind.hpp>

#include <cutsim/gldata.hpp>
#include <cutsim/glwidget.hpp>

#include "cutsim.hpp"


int main( int argc, char **argv ) {
    QApplication app( argc, argv );
    
    // create a new GLWidget, which inherits from QGLWidget. This is the OpenGL view.
    // it displays GLData objects.
    cutsim::GLWidget *w = new cutsim::GLWidget();
    
    // create and return a new GLData object for us
    cutsim::GLData* g = w->addObject();
    Cutsim cs;
    QObject::connect( w, SIGNAL(sig()), &cs, SLOT(cut()) );
    
    // link GLData with cutting-simulation
    cs.setGLData(g);
    
    // update GL-data
    cs.updateGL();
    
    // show the main window
    w->show();
    return app.exec();
}
