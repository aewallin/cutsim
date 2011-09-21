#include <string>
#include <g2m/g2m.hpp>
#include <g2m/canonLine.hpp>
#include <g2m/point.hpp>

int main( int argc, char **argv ) {
    
    g2m::g2m g;
    
    g.setFile("/home/anders/Desktop/cutsim/examples/g2m_test1/cds.ngc");
    g.setToolTable("/home/anders/Desktop/cutsim/examples/g2m_test1/tooltable.tbl");
    g.setInterp("/home/anders/emc2-dev/bin/rs274");
    g.interpret_file();
    
    std::vector< g2m::canonLine*> lines = g.getCanonLines();
    std::cout << " got " << lines.size() << " canon-lines \n";
    
    for (unsigned int m=0; m<lines.size(); ++m) {
        g2m::canonLine* l = lines[m];
        if ( l->isMotion() ) {
            std::vector< g2m::Point> pts = l->points();
            for (unsigned int i=0;i<pts.size();i++)
                std::cout << m << " : " << pts[i].str() << "\n";
        }
    }
    
    return 0;
}
