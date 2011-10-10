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

#include <iostream>

#include <GL/glut.h>

#include <QObject>
#include <QTimer>

#include "glwidget.hpp"

namespace cutsim {

#define PI 3.1415926535897932

GLWidget::GLWidget( QWidget *parent, char *name ) {
    setSceneRadius(20);
    showEntireScene();
    file_number=0;
}

/// add new GLData object and return pointer to it.
GLData* GLWidget::addObject() {
    GLData* g = new GLData();
    glObjects.push_back(g);
    return g;
}






/// loop through glObjects and for each GLData draw it using VBO
void GLWidget::draw()  {

    
    BOOST_FOREACH( GLData* g, glObjects ) { // draw each object
        //glLoadIdentity();
        //glTranslatef( g->pos.x, g->pos.y , g->pos.z ); 
        // apply a transformation-matrix here !?
        QMutexLocker locker( &(g->renderMutex) );
        glPolygonMode( g->polygonFaceMode(), g->polygonFillMode()  ); 

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        
        // http://www.opengl.org/sdk/docs/man/xhtml/glNormalPointer.xml
        //                 type,                          stride,                 offset/pointer
        glNormalPointer( GLData::coordinate_type, sizeof( GLData::vertex_type ),    ((GLbyte*)g->getVertexArray()  + GLData::normal_offset ) );
        // http://www.opengl.org/sdk/docs/man/xhtml/glColorPointer.xml
        //             coords/vert,                    type,                        stride,  offset/pointer
        glColorPointer(  3, GLData::color_type     , sizeof( GLData::vertex_type ), ((GLbyte*)g->getVertexArray()  + GLData::color_offset  ) ); 
        glVertexPointer( 3, GLData::coordinate_type, sizeof( GLData::vertex_type ), ((GLbyte*)g->getVertexArray()  + GLData::vertex_offset  ) ); 
        // http://www.opengl.org/sdk/docs/man/xhtml/glDrawElements.xml
        //              mode       idx-count             type               offset/pointer
        glDrawElements( g->GLType() , g->indexCount() , GLData::index_type, g->getIndexArray());
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    
    

    lastFrameTime = QTime::currentTime();
    
    
}


void GLWidget::slotWriteScreenshot() {
    QImage img = grabFrameBuffer(); 
    QString file_name = "frame_" + QString::number(file_number) + ".png";
    img.save( file_name );
    file_number++;
}

void GLWidget::drawCornerAxis() {
    int viewport[4];
    int scissor[4];
    // The viewport and the scissor are changed to fit the lower left
    // corner. Original values are saved.
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetIntegerv(GL_SCISSOR_BOX, scissor);
    // Axis viewport size, in pixels
    const int size = 150;
    glViewport(0,0,size,size);
    glScissor(0,0,size,size);
    // The Z-buffer is cleared to make the axis appear over the
    // original image.
    glClear(GL_DEPTH_BUFFER_BIT);
    // Tune for best line rendering
    glDisable(GL_LIGHTING);
    glLineWidth(3.0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMultMatrixd( camera()->orientation().inverse().matrix() );
    glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0); // red X-axis
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(1.0, 0.0, 0.0);
        glColor3f(0.0, 1.0, 0.0); // green Y-axis
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 1.0, 0.0);
        glColor3f(0.0, 0.0, 1.0); // blue Z-axis
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 1.0);
    glEnd();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_LIGHTING);
    // The viewport and the scissor are restored.
    glScissor(scissor[0],scissor[1],scissor[2],scissor[3]);
    glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
}


} // end ocl namespace


// mouse movement
// dx in [-1,1]
// dx = (xnew - xold) / width
// dy = (ynew - yold) / height

// function that rotates point around axis:
// rotatePoint( point, origin, direction, angle)

// rotation around axis: 
// dir_x = up
// dir_y = up cross (center-eye) / norm( up cross (center-eye) )
// now rotations are:
// eye = rotatePoint(eye, center, dir_x, -dx * pi )
// eye = rotatePoint(eye, center, dir_y, dy*pi )
// up = rotatePoint( center+up, center, dir_y, dy*pi) - center
// (normalize up after operations)
//
// zoom
// eye = center + (eye-center)*(dy+1)
//
// pan
// height of window in object space: length = 2* norm(eye-center)*tan(fovy/2)
// new position of center is:
// center = center + dir_y *dx*length *width/height
//                 + dir_x * dy * length



