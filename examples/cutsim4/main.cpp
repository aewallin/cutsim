#include <QApplication>

#include <functional>
#include <boost/bind.hpp>

#include <cutsim/gldata.hpp>
#include <cutsim/glwidget.hpp>
#include <cutsim/cutsim.hpp>
#include <cutsim/volume.hpp>


int main( int argc, char **argv ) {
    QApplication app( argc, argv );
    
    // create a new GLWidget, which inherits from QGLWidget. This is the OpenGL view.
    // it displays GLData objects.
    cutsim::GLWidget *w = new cutsim::GLWidget(); // this is the only widget of the window, so automagically fills the space?
    QObject::connect( w, SIGNAL(s_sig()), w, SLOT( slotWriteScreenshot()  ) );
    
    unsigned int max_depth=8;
    double octree_cube_side=10.0;
    cutsim::Cutsim* cs = new cutsim::Cutsim(octree_cube_side , max_depth, w);
    QObject::connect( w, SIGNAL(sig()), cs, SLOT( cut() ) );
    
    cutsim::SphereVolume* stock = new cutsim::SphereVolume();
    stock->radius = 15;
    stock->center = cutsim::GLVertex(10,10,10);
    stock->calcBB();
    stock->setColor(0,1,1);

    //cs->setColor(0,1,1);
    cs->sum_volume(stock);
    
    stock->center = cutsim::GLVertex(-10,10,10);
    stock->radius = 13;
    stock->calcBB();
    stock->setColor(1,1,0);
    //cs->setColor(1,1,0);
    cs->diff_volume(stock);
    //cs->sum_volume(stock);
    //cs->intersect_volume(stock);
    delete stock;
  
    
    // show the main window
    w->show();
    std::cout << " w->show() done.\n";
    return app.exec();
}
