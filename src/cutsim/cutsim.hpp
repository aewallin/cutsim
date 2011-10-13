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
#include <QRunnable>

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

class DiffTask : public QObject, public QRunnable  {
    Q_OBJECT
public:
    DiffTask(Octree* t, GLData* g, const OCTVolume* v) : tree(t), gld(g), vol(v) {
    }
    void run() {
        qDebug() << "DiffTask thread" << QThread::currentThread();
        std::clock_t start, stop;
        start = std::clock();
        tree->diff( vol );
        stop = std::clock();
        qDebug() << "   " << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) ;
        qDebug() << "DiffTask thread DONE " << QThread::currentThread();
        emit signalDone();
    }
signals:
    void signalDone();
private:
    Octree* tree;
    GLData* gld;
    const OCTVolume* vol;
};

class UpdateGLTask : public QObject, public QRunnable  {
    Q_OBJECT
public:
    UpdateGLTask(Octree* t, GLData* g, IsoSurfaceAlgorithm* ia) : 
        tree(t), gld(g), iso_algo(ia) {
    }
    void run() {
        qDebug() << "UpdateGLTask thread" << QThread::currentThread();
        std::clock_t start, stop;
        start = std::clock();
        iso_algo->updateGL();
        gld->swap();
        stop = std::clock();
        qDebug() << "   " << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) ;
        //std::cout << "cutsim.cpp updateGL() : " << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
        emit signalDone();
    }
signals:
    void signalDone();
private:
    Octree* tree;
    GLData* gld;
    IsoSurfaceAlgorithm* iso_algo;
}; 

/// a Cutsim stores an Octree stock model, uses an iso-surface extraction
/// algorithm to generate surface triangles, and communicates with
/// the corresponding GLData surface which is used by GLWidget for rendering
class Cutsim : public QObject {
    Q_OBJECT
public:
    Cutsim(double octree_size, unsigned int octree_max_depth, GLData* gld);
    virtual ~Cutsim();
    void diff_volume( const OCTVolume* vol );
    void sum_volume( const OCTVolume* vol );
    void intersect_volume( const OCTVolume* vol );
    void updateGL(); 
signals:
    void signalDiffDone();
    void signalGLDone();
public slots:
    void slotDiffDone() {
        qDebug() << " Cutsim::slotDiffDone() ";
        emit signalDiffDone();
    }
    void slotGLDone() {
        emit signalGLDone();
    }
    void slot_diff_volume( const OCTVolume* vol) { diff_volume(vol);}
    void slot_diff_volume_mt( const OCTVolume* vol) { 
        DiffTask* dt = new DiffTask(tree, g, vol);
        connect( dt, SIGNAL( signalDone() ), this, SLOT( slotDiffDone() ) );
        QThreadPool::globalInstance()->start(dt);
        
        //QThreadPool::globalInstance()->waitForDone();
    }
    void update_gl_mt( ) { 
        UpdateGLTask* ua = new UpdateGLTask(tree, g, iso_algo);
        connect( ua, SIGNAL( signalDone() ), this, SLOT( slotGLDone() ) );
        QThreadPool::globalInstance()->start(ua);
        
        //QThreadPool::globalInstance()->waitForDone();
    }
    void slot_sum_volume( const OCTVolume* vol)  { sum_volume(vol);} 
    void slot_int_volume( const OCTVolume* vol)  { intersect_volume(vol);}
private:
    IsoSurfaceAlgorithm* iso_algo; // the isosurface-extraction algorithm to use
    Octree* tree; // this is the stock model
    GLData* g; // this is the graphics object drawn on the screen, representing the stock
};

} // end namespace

#endif
