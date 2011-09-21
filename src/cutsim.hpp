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

#ifndef CUTSIM_H
#define CUTSIM_H

#include <QObject>

#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>

#include <boost/bind.hpp>
#include <boost/timer.hpp>

#include "octree.hpp"
#include "octnode.hpp"
#include "volume.hpp"
#include "marching_cubes.hpp"
#include "gldata.hpp"

namespace cutsim {
    
/// a Cutsim stores an Octree stock model, uses an iso-surface extraction
/// algorithm to generate surface triangles, and communicates with
/// the corresponding GLData surface which is used for rendering
class Cutsim : public QObject {
    Q_OBJECT
public:
    Cutsim () ;
    void setGLData(cutsim::GLData* gldata) ;
    void updateGL() ;
public slots:
    void cut() ;
private:
    cutsim::MarchingCubes* mc; // the isosurface-extraction algorithm to use
    cutsim::Octree* tree; // this is the stock model
    cutsim::GLData* g; // this is the graphics object drawn on the screen, representing the stock
};

} // end namespace

#endif
