#include <QApplication>

#include <cutsim/gldata.hpp>
#include <cutsim/glwidget.hpp>
#include <cutsim/cutsim.hpp>
#include <cutsim/volume.hpp>

/*
 * This example illustrates basic octree operations:
 * union(sum), difference, intersection
 * 
 * */
int main( int argc, char **argv ) {
    QApplication app( argc, argv );
    
    // create a new GLWidget, which inherits from QGLWidget. This is the OpenGL view.
    // it displays GLData objects.
    cutsim::GLWidget *w = new cutsim::GLWidget(); // this is the only widget of the window, so automagically fills the space?
    
    // example signal/slot, writes a screenshot to a file
    QObject::connect( w, SIGNAL(s_sig()), w, SLOT( slotWriteScreenshot()  ) );
    
    // create the octree that represents the material
    unsigned int max_depth=8;
    double octree_cube_side=10.0;
    cutsim::Cutsim* cs = new cutsim::Cutsim(octree_cube_side , max_depth, w->addGLData());

    // do some operations on the material    
    cutsim::SphereVolume* stock = new cutsim::SphereVolume();
    stock->setRadius(7);
    stock->setCenter( cutsim::GLVertex(0,0,0) );
    stock->setColor(0,1,1);

    cs->sum_volume(stock);
    
    stock->setCenter( cutsim::GLVertex(0,0,7) );
    stock->setRadius( 5 );
    stock->setColor(1,1,0);
    cs->diff_volume(stock);
    //cs->sum_volume(stock);
    //cs->intersect_volume(stock);
    delete stock;
  
    
    // show the main window
    w->show();
    cs->updateGL();
    std::cout << " w->show() done.\n";
    return app.exec();
}
