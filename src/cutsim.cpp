/*  
 *  Copyright 2010-2011 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  
 *  This file is part of OpenCAMlib.
 *
 *  OpenCAMlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenCAMlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OpenCAMlib.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "cutsim.hpp"

namespace cutsim {
    

Cutsim::Cutsim (GLWidget* w): widget(w) {
    GLVertex octree_center(0,0,0);
    unsigned int max_depth = 7;
    tree = new cutsim::Octree(10.0, max_depth, octree_center );
    std::cout << "Cutsim() ctor: tree before init: " << tree->str() << "\n";
    tree->init(2u);
    tree->debug=false;
    std::cout << "Cutsim() ctor: tree after init: " << tree->str() << "\n";
    
    
    SphereOCTVolume stock_sphere;
    stock_sphere.radius = 7;
    stock_sphere.center = cutsim::GLVertex(0,0,0);
    stock_sphere.calcBB();
    stock_sphere.invert = true;
    tree->diff_negative( &stock_sphere );
    std::cout << " tree after stock-cut: " << tree->str() << "\n";
    
    
    
    /*
    ocl::SphereOCTVolume s;
    s.radius = 2;
    s.center = ocl::Point(4,4,4);
    s.calcBB();
    std::cout << " before diff: " << tree->str() << "\n";
    tree->diff_negative( &s );
    std::cout << " AFTER diff: " << tree->str() << "\n";
    */
    
    mc = new cutsim::MarchingCubes();
    tree->setIsoSurf(mc);
    //tree->debug=true;
    tree->debug=false;
    setGLData();
} 

/*
void Cutsim::setVolume( OCTVolume* vol ) {
    volume = vol;
}*/

void Cutsim::setGLData() {
    GLData* gldata = widget->addObject();
    // this is the GLData that corresponds to the tree
    g = gldata;
    g->setTriangles(); // mc: triangles, dual_contour: quads
    g->setPosition(0,0,0); // position offset (?used)
    g->setUsageDynamicDraw();
    tree->setGLData(g);
}

void Cutsim::updateGL() {
    // traverse the octree and update the GLData correspondingly
    tree->updateGL();
}

void Cutsim::diff_volume( OCTVolume* volume ) {
    //volume->invert = inv;
    std::clock_t start, stop;
    std::cout << " before diff: " << tree->str() << "\n";
    std::cout << " diff_negative() ..\n";
    start = std::clock();
    tree->diff_negative( volume );
    stop = std::clock();
    std::cout << " diff()  :" << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
    std::cout << " AFTER diff: " << tree->str() << "\n";
    
    start = std::clock();
    updateGL();
    stop = std::clock();
    std::cout << " updateGL() : " << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
    
    start = std::clock();
    g->updateVBO();
    stop = std::clock();
    std::cout << " updateVBO():   " << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
}

void Cutsim::cut() { // demo slot of doing a cutting operation on the tree with a volume.
    std::cout << " cut! called \n";
    cutsim::SphereOCTVolume s;
    s.radius = 2;
    s.center = cutsim::GLVertex(4,4,4);
    s.calcBB();
    s.invert = false;
    //std::cout << " before diff: " << tree->str() << "\n";
    std::clock_t start, stop;
    boost::timer tim;
    
    std::cout << " diff_negative() ..\n";
    tim.restart();
    start = std::clock();
    tree->diff_negative( &s );
    stop = std::clock();
    
    std::cout << " diff(): std::clock:" << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
    std::cout << " diff(): boost::timer:" << tim.elapsed() <<'\n';
    
    tim.restart();
    start = std::clock();
    //g->updateVBO();
    updateGL();
    stop = std::clock();
    
    std::cout << " updateGL() std::clock: " << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
    std::cout << " updateGL() boost::timer: " << tim.elapsed() <<'\n';
    //std::cout << " AFTER diff: " << tree->str() << "\n";

    tim.restart();
    start = std::clock();
    g->updateVBO();
    stop = std::clock();
    std::cout << " updateVBO(): std::clock: " << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
    std::cout << " updateVBO(): boost::timer: " << tim.elapsed() <<'\n';
}
    


} // end namespace
