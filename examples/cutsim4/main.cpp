#include <QApplication>

#include <functional>
#include <boost/bind.hpp>

#include <cutsim/gldata.hpp>
#include <cutsim/glwidget.hpp>
#include <cutsim/cutsim.hpp>
#include <cutsim/volume.hpp>

#include <g2m/g2m.hpp>
#include <g2m/canonLine.hpp>
#include <g2m/point.hpp>

int main( int argc, char **argv ) {
    QApplication app( argc, argv );
    
    
    // read g-code file with g2m
    g2m::g2m gcode;
    
    gcode.setFile("/home/anders/Desktop/cutsim/ngc/simple.ngc");
    gcode.setToolTable("/home/anders/Desktop/cutsim/ngc/tooltable.tbl");
    gcode.setInterp("/home/anders/emc2-dev/bin/rs274");
    gcode.interpret_file();
    
    std::vector< g2m::canonLine*> lines = gcode.getCanonLines();
    std::cout << " got " << lines.size() << " canon-lines \n";
    
    // create a new GLWidget, which inherits from QGLWidget. This is the OpenGL view.
    // it displays GLData objects.
    cutsim::GLWidget *w = new cutsim::GLWidget(); // this is the only widget of the window, so automagically fills the space?
    
    unsigned int max_depth=8;
    double octree_cube_side=10.0;
    cutsim::Cutsim* cs = new cutsim::Cutsim(octree_cube_side , max_depth, w);
    QObject::connect( w, SIGNAL(sig()), cs, SLOT( cut() ) );
    
    cutsim::GLData* path = w->addObject();
    for (unsigned int m=0; m<lines.size(); ++m) {
        g2m::canonLine* l = lines[m];
        if ( l->isMotion() ) {
            std::vector< g2m::Point> pts = l->points();
            for (unsigned int i=0;i<pts.size();i++) {
                float r(0),g(1),b(0);
                unsigned int v_index = path->addVertex( pts[i].x, pts[i].y, pts[i].z , r , g , b); 
                std::vector<GLuint> poly;
                poly.push_back(v_index);
                path->addPolygon( poly );
            }
        }
    }
    path->setLineStrip();
   
    //cutsim::CubeVolume* stock = new cutsim::CubeVolume();
    //stock->center = cutsim::GLVertex(2,2,-2.1);
    //stock->side = 4;

    cutsim::SphereVolume* stock = new cutsim::SphereVolume();
    stock->radius = 15;
    stock->center = cutsim::GLVertex(10,10,10);
    stock->calcBB();
    cs->setColor(0,1,1);
    cs->sum_volume(stock);
    
    stock->center = cutsim::GLVertex(-10,-10,-10);
    stock->calcBB();
    
    //cs->sum_volume(stock);
    delete stock;
  
    
    //cs->updateGL(); // update GL-data
    
    // show the main window
    w->show();
    std::cout << " w->show() done.\n";
    return app.exec();
}
