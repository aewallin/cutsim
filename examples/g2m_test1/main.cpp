#include <string>
#include <g2m/g2m.hpp>

int main( int argc, char **argv ) {
    
    g2m::g2m g;
    
    g.setFile("/home/anders/Desktop/cutsim/ngc/cds.ngc");
    g.setToolTable("/home/anders/Desktop/cutsim/ngc/tooltable.tbl");
    g.setInterp("/home/anders/emc2-dev/bin/rs274");
    g.interpret_file();
    
    return 0;
}
