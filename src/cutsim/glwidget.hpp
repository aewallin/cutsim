/*  
 *  Copyright 2010-2011 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  
 *  This file is part of Cutsim / OpenCAMlib.
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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLViewer/qglviewer.h>

#include <QObject>
#include <QGLBuffer>
#include <QVarLengthArray>
#include <QtGui>
#include <QTimer>
#include <QCursor>
#include <QDataStream>
#include <QDebug>
 
#include <boost/foreach.hpp> 
 
#include <iostream>
#include <cassert>
#include <set>
#include <vector>

#include "gldata.hpp"

namespace cutsim {

class GLWidget : public QGLViewer  {
    Q_OBJECT
    public:
        GLWidget( QWidget *parent=0, char *name=0 ) ;
        ~GLWidget() {}
        GLData* addGLData();
    signals:

    public slots:
        void slotWriteScreenshot();
        void slotNewDataWaiting() { 
            if ( lastFrameTime.elapsed() > 17)  // 60fps = 1 frame per 17 milliseconds)
                updateGL(); // only call if sufficient time elapsed since last draw()
        }
    protected:
        virtual void draw();
        //virtual void init() {}
        virtual void postDraw();
        virtual void keyPressEvent(QKeyEvent *e);
    private:
        void drawCornerAxis();
        /// these are the GLData objects which will be drawn in the OpenGL scene
        std::vector<GLData*> glObjects;
        /// time at which last frame was drawn
        QTime lastFrameTime;
        /// used for number screenshots
        int file_number; 
        bool corner_axis;
};

} // end ocl namespace

#endif
