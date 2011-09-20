//#include <QApplication>

//#include <functional>
//#include <boost/bind.hpp>

//#include <cutsim/gldata.hpp>
//#include <cutsim/glwidget.hpp>

#include <g2m/g2m.hpp>


int main( int argc, char **argv ) {
    
    g2m g;
    
    g.interpret_file();
    
    return 0;
}
