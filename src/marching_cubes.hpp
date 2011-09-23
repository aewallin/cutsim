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

#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include <iostream>
#include <list>
#include <vector>

#include <boost/foreach.hpp>

#include "isosurface.hpp"
#include "bbox.hpp"
#include "octnode.hpp"
#include "gldata.hpp"

namespace cutsim {

/// Marching-cubes isosurface extraction from distance field stored in Octree
/// see http://en.wikipedia.org/wiki/Marching_cubes
///
class MarchingCubes : public IsoSurfaceAlgorithm {
    public:
        MarchingCubes() {}
        virtual ~MarchingCubes() { }
        /// run mc on one Octnode, return triangles
        void updateGL() { updateGL( tree->root );}
        
        std::vector< std::vector< GLVertex >  > polygonize_node(const Octnode* node);

    protected:
        void updateGL( Octnode* node) {}
        /// generate the interpolated vertices required for triangle construction
        std::vector<GLVertex> interpolated_vertices(const Octnode* node, unsigned int edges) ;
        /// use linear interpolation of the distance-field between vertices idx1 and idx2
        /// to generate a new iso-surface point on the idx1-idx2 edge
        GLVertex interpolate(const Octnode* node, int idx1, int idx2);
        /// get table-index based on the funcion values (positive or negative) at the corners
        unsigned int mc_edgeTableIndex(const Octnode* node);
        /// Marching-Cubes edge table
        static const unsigned int edgeTable[256];
        /// Marching-Cubes triangle table
        static const int triTable[256][16]; 

};


} // end namespace
#endif
// end file marching_cubes.h
