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

#ifndef GLWIDGET_H
#define GLWIDGET_H

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

#define BUFFER_OFFSET(i) ((GLbyte *)NULL + (i))

class GLWidget : public QGLWidget {
    Q_OBJECT
    public:
        GLWidget( QWidget *parent=0, char *name=0 ) ;
        ~GLWidget() {}
        void initializeGL();
        GLData* addObject();
    signals:
        void sig(); // a test/dummy signal, emitted when the user presses "c"
        void s_sig(); // press "s" test/dummy
    public slots:
        void slotWriteScreenshot();
    protected:
        void genVBO(); // generate a VBO for each GLData object
        void resizeGL( int width, int height );
        void paintGL();
        void updateDir();
        void zoomView( int delta );
        void zoomView( const QPoint& newPos );
        void panView(const QPoint& newPos);
        void rotateView(const QPoint& newPos);
        void keyPressEvent( QKeyEvent *e );
        void mouseMoveEvent( QMouseEvent *e );
        void wheelEvent( QWheelEvent *e ) { zoomView( e->delta() ); }
        void mousePressEvent( QMouseEvent *e );
        void mouseReleaseEvent( QMouseEvent *e );
    private:
        /// camera position
        GLVertex _eye;
        /// camera up-vector 
        GLVertex _up; 
        /// camera look-at point
        GLVertex _center;
        /// rotation/pan y-axis 
        GLVertex _diry; 
        /// rotation/pan y-axis
        GLVertex _dirx;
        /// field of vision in y-dir 
        float _fovy;
        /// position of near clipping plane 
        float z_near;
        /// position of far clipping plane
        float z_far;
        
        /// these are the GLData objects which will be drawn in the OpenGL scene
        std::vector<GLData*> glObjects;

        QPoint _oldMousePos;
        int _width;
        int _height;
        bool _rightButtonPressed;
        bool _leftButtonPressed;
        bool _middleButtonPressed;
        QTime _lastFrameTime;
        int file_number;
};

} // end ocl namespace

#endif
