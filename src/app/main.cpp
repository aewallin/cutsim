//#include <QApplication>

/*
#include <functional>
#include <boost/bind.hpp>

#include <cutsim/gldata.hpp>
#include <cutsim/glwidget.hpp>
#include <cutsim/cutsim.hpp>
#include <cutsim/volume.hpp>

#include <g2m/g2m.hpp>
#include <g2m/canonLine.hpp>
#include <g2m/point.hpp>
*/

#include "cutsim_app.hpp"
#include "cutsim_window.hpp"

int main( int argc, char **argv ) {
    //QApplication app( argc, argv );
    
    CutsimApplication app( argc, argv );

    QStringList qsl;
    for(int i = 1; i < argc; i++) {
        qsl.append(argv[i]);
    }

    CutsimWindow *window = new CutsimWindow(qsl);

    window->show();

    int retval = app.exec();
  
    
    return retval;
}
