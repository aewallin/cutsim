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

Cutsim::Cutsim (double octree_size, unsigned int octree_max_depth, GLData* gld): g(gld) {
    GLVertex octree_center(0,0,0);
    tree = new Octree(octree_size, octree_max_depth, octree_center, g );
    std::cout << "Cutsim() ctor: tree before init: " << tree->str() << "\n";
    tree->init(2u);
    tree->debug=false;
    std::cout << "Cutsim() ctor: tree after init: " << tree->str() << "\n";
    
    iso_algo = new MarchingCubes(g, tree);
    //iso_algo = new CubeWireFrame(g, tree);
    
} 

Cutsim::~Cutsim() {
    delete iso_algo;
    delete tree;
    delete g;
}

void Cutsim::updateGL() {
    std::clock_t start, stop;
    start = std::clock();
    iso_algo->updateGL();
    g->swap();
    stop = std::clock();
    std::cout << "cutsim.cpp updateGL() : " << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
}

void Cutsim::sum_volume( const OCTVolume* volume ) {
    //iso_algo->setColor(red,green,blue);
    std::clock_t start, stop;
    start = std::clock();
    tree->sum( volume );
    stop = std::clock();
    std::cout << "cutsim.cpp sum_volume()  :" << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
    
    //start = std::clock();
    //iso_algo->updateGL();
    //stop = std::clock();
    //std::cout << "cutsim.cpp updateGL() : " << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
}

void Cutsim::diff_volume( const OCTVolume* volume ) {
    //iso_algo->setColor(red,green,blue);
    std::clock_t start, stop;
    start = std::clock();
    tree->diff( volume );
    stop = std::clock();
    std::cout << "cutsim.cpp diff_volume()  :" << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';

    //start = std::clock();
    //iso_algo->updateGL();
    //stop = std::clock();
    //std::cout << "cutsim.cpp updateGL() : " << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
}

void Cutsim::intersect_volume( const OCTVolume* volume ) {
    //iso_algo->setColor(red,green,blue);
    std::clock_t start, stop;
    start = std::clock();
    tree->intersect( volume );
    stop = std::clock();
    std::cout << "cutsim.cpp intersect_volume()  :" << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';

    //start = std::clock();
    //iso_algo->updateGL();
    //stop = std::clock();
    //std::cout << "cutsim.cpp updateGL() : " << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
}

} // end namespace
