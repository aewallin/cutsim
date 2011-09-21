#include <QApplication>

#include <functional>
#include <boost/bind.hpp>

#include <cutsim/gldata.hpp>
#include <cutsim/glwidget.hpp>
#include <cutsim/cutsim.hpp>

#include <g2m/g2m.hpp>
#include <g2m/canonLine.hpp>
#include <g2m/point.hpp>

int main( int argc, char **argv ) {
    QApplication app( argc, argv );
    
    
    // read g-code file with g2m
    g2m::g2m gcode;
    
    gcode.setFile("/home/anders/Desktop/cutsim/examples/g2m_test1/cds.ngc");
    gcode.setToolTable("/home/anders/Desktop/cutsim/examples/g2m_test1/tooltable.tbl");
    gcode.setInterp("/home/anders/emc2-dev/bin/rs274");
    gcode.interpret_file();
    
    std::vector< g2m::canonLine*> lines = gcode.getCanonLines();
    std::cout << " got " << lines.size() << " canon-lines \n";
    

    
    // create a new GLWidget, which inherits from QGLWidget. This is the OpenGL view.
    // it displays GLData objects.
    cutsim::GLWidget *w = new cutsim::GLWidget(); // this is the only widget of the window, so automagically fills the space?
    
    // create and return a new GLData object for us
    /*
    cutsim::GLData* g = w->addObject();
    cutsim::Cutsim cs;
    QObject::connect( w, SIGNAL(sig()), &cs, SLOT(cut()) );
    cs.setGLData(g); // link GLData with cutting-simulation
    */
    
    cutsim::GLData* path = w->addObject();
    
    for (unsigned int m=0; m<lines.size(); ++m) {
        g2m::canonLine* l = lines[m];
        if ( l->isMotion() ) {
            std::vector< g2m::Point> pts = l->points();
            for (unsigned int i=0;i<pts.size();i++) {
                float r(1),g(0),b(0);
                unsigned int v_index = path->addVertex( pts[i].x, pts[i].y, pts[i].z , r , g , b); // std::cout << m << " : " << pts[i].str() << "\n";
                std::vector<GLuint> poly;
                poly.push_back(v_index);
                path->addPolygon( poly );
            }
        }
    }
    path->setLineStrip();
    
    //cs.updateGL(); // update GL-data
    // show the main window
    w->show();
    return app.exec();
}
