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
#include "cube_wireframe.hpp"
#include "gldata.hpp"
#include "glwidget.hpp"

namespace cutsim {
    
/// a Cutsim stores an Octree stock model, uses an iso-surface extraction
/// algorithm to generate surface triangles, and communicates with
/// the corresponding GLData surface which is used for rendering
class Cutsim : public QObject {
    Q_OBJECT
public:
    Cutsim(double octree_size, unsigned int octree_max_depth, GLData* gld);
    virtual ~Cutsim();
    void diff_volume( const OCTVolume* vol );
    void sum_volume( const OCTVolume* vol );
    void intersect_volume( const OCTVolume* vol );
    void updateGL(); 
public slots:
    void slot_diff_volume( const OCTVolume* vol) { diff_volume(vol);}
    void slot_sum_volume( const OCTVolume* vol)  { sum_volume(vol);} 
    void slot_int_volume( const OCTVolume* vol)  { intersect_volume(vol);}
private:
    IsoSurfaceAlgorithm* iso_algo; // the isosurface-extraction algorithm to use
    Octree* tree; // this is the stock model
    GLData* g; // this is the graphics object drawn on the screen, representing the stock
    //GLWidget* widget; // the widget where stock should be displayed
    GLfloat red,green,blue;
};

} // end namespace

#endif
