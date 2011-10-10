/*  
 *  Copyright 2010-2011 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  
 *  This file is part of Cutsim/OpenCAMlib.
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

#ifndef GL_DATA_H
#define GL_DATA_H

#include <QObject>
#include <QGLBuffer>
#include <QVarLengthArray>
#include <QMutex>
#include <QMutexLocker>

#include <iostream>
#include <set>
#include <cmath>

#include <boost/foreach.hpp>

#include "glvertex.hpp"

namespace cutsim {

class Octnode;

/// additional vertex data not needed for OpenGL rendering
/// but required for the isosurface or cutting-simulation algorithm.
struct VertexData {
    void str() {
        BOOST_FOREACH( GLuint pIdx, polygons ) {
            std::cout << pIdx << " ";
        }
    }
    inline void addPolygon( unsigned int idx ) { polygons.insert( idx ); }
    inline void removePolygon(unsigned int idx ) { polygons.erase( idx ); }
    inline bool empty() { return polygons.empty(); }
// DATA
    /// The set of polygons. Each polygon has an uint index which is stored here.
    /// Note: we want to access polygons from highest index to lowest, thus compare with "greater"
    typedef std::set< unsigned int, std::greater<unsigned int> > PolygonSet;
    /// the polygons to which this vertex belongs. i.e. for each vertex we store in this set all the polygons to which it belongs.
    PolygonSet polygons;
    /// the Octnode that created this vertex. 
    /// when an Octnode is cut the corresponding vertex/vertices are deleted.
    /// when a vertex is deleted, the Octnode that generated it is notified
    Octnode* node;
    // (an alternative callback-mechanism would be to store a function-pointer or similar)
};


// the "secret sauce" paper suggests the following primitives
//   http://www.cs.berkeley.edu/~jrs/meshpapers/SchaeferWarren2.pdf
//   or
//   http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.13.2631
//
// - add vertex  
//   add vertex with empty polygon list and pointer to octree-node
//
// - remove vertex (also removes associated polygons)
//   process list of polygons, from highest to lowest. call remove_polygon on each poly.
//   overwrite with last vertex. shorten list. request each poly to re-number.
// 
// - add polygon DONE
//   append new polygon to end of list, request each vertex to add new polygon to list.
//
// - remove_polygon( polygonIndex ) 
//   i) for each vertex: request remove this polygons index from list
//   ii) then remove polygon from polygon-list: overwrite with last polygon, then shorten list.
//   iii) process each vertex in the moved polygon, request renumber on each vert for this poly
//
// data structure:
//  vertex-table: index, pos(x,y,z)  , polygons(id1,id2,...), Node-pointer to octree 
// polygon-table: index, vertex-list
//

// parameters for rendering held by a GLData
// vertex position/color/normal and polygon indices held in vertex- and index-arrays other data here
// this may include lighting/shading/material parameters in the future
struct GLParameters {
    // the type of this GLData, one of:
    //                GL_POINTS,
    //                GL_LINE_STRIP,
    //                GL_LINE_LOOP,
    //                GL_LINES,
    //                GL_TRIANGLE_STRIP,
    //                GL_TRIANGLE_FAN,
    //                GL_TRIANGLES,
    //                GL_QUAD_STRIP,
    //                GL_QUADS,
    //                GL_POLYGON 
    GLenum type;
    GLenum polygonMode_face; // face = GL_FRONT | GL_BACK  | GL_FRONT_AND_BACK
    GLenum polygonMode_mode; // mode = GL_POINT, GL_LINE, GL_FILL
    int polyVerts; // vertices per polygon
};

/// a GLData object holds data which is drawn by OpenGL using VBOs
class GLData {
public:
    GLData();
    
    unsigned int addVertex(float x, float y, float z, float r, float g, float b);
    unsigned int addVertex(GLVertex v, Octnode* n);
    unsigned int addVertex(float x, float y, float z, float r, float g, float b, Octnode* n);
    void setNormal(unsigned int vertexIdx, float nx, float ny, float nz);
    void modifyVertex( unsigned int id, float x, float y, float z, float r, float g, float b, float nx, float ny, float nz);
    void removeVertex( unsigned int vertexIdx );
    int addPolygon( std::vector<GLuint>& verts);
    void removePolygon( unsigned int polygonIdx);
    void print() ;

// type of GLData
    void setTriangles() {setType(GL_TRIANGLES); glp[workIndex].polyVerts=3;}
    void setQuads() {setType(GL_QUADS); glp[workIndex].polyVerts=4;}
    void setPoints() {setType(GL_POINTS); glp[workIndex].polyVerts=1;}
    void setLineStrip() {setType(GL_LINE_STRIP); glp[workIndex].polyVerts=1;}
    void setLines() {setType(GL_LINES); glp[workIndex].polyVerts=2;}
    void setQuadStrip() {setType(GL_QUAD_STRIP); glp[workIndex].polyVerts=1;}
    
// face = GL_FRONT | GL_BACK  | GL_FRONT_AND_BACK
    void setPolygonModeFront() { glp[workIndex].polygonMode_face = GL_FRONT; }
    void setPolygonModeBack() { glp[workIndex].polygonMode_face = GL_BACK; }
    void setPolygonModeFrontAndBack() { glp[workIndex].polygonMode_face = GL_FRONT_AND_BACK; }
    
// mode = GL_POINT, GL_LINE, GL_FILL
    void setPolygonModeFill() { glp[workIndex].polygonMode_mode = GL_FILL; }
    void setPolygonModePoint() { glp[workIndex].polygonMode_mode = GL_POINT; }
    void setPolygonModeLine() { glp[workIndex].polygonMode_mode = GL_LINE; }
        
// constants and typedefs
    typedef GLVertex vertex_type;
    static const GLenum index_type = GL_UNSIGNED_INT;
    static const GLenum coordinate_type = GL_FLOAT;
    static const GLenum color_type = GL_FLOAT;
    static const unsigned int vertex_offset = 0;
    static const unsigned int color_offset = 12;
    static const unsigned int normal_offset = 24;
    
    QMutex renderMutex; // renderer locks this while rendering, swapBuffer locks while swapping
    QMutex workMutex;
    
// these 'getters' used by OpenGL renderer to render this GLData
    const GLVertex* getVertexArray() const { return vertexArray[renderIndex].data(); }
    const GLuint* getIndexArray() const { return indexArray[renderIndex].data(); }
    inline const int polygonVertices() const { return glp[renderIndex].polyVerts; }
    inline const GLenum GLType() const { return glp[renderIndex].type; }
    inline const GLenum polygonFaceMode() const { return glp[renderIndex].polygonMode_face;}
    inline const GLenum polygonFillMode() const { return glp[renderIndex].polygonMode_mode;}
    inline const int indexCount() const { return indexArray[renderIndex].size(); }
    
    void swap() {
        swapBuffers();
        copyBuffers();
    }
    void swapBuffers() {  // neither rendering nor working is allowed during this operation!
        renderMutex.lock();
        workMutex.lock();
            renderIndex = (renderIndex==0) ? 1 : 0 ;
            workIndex = (workIndex==0) ? 1 : 0 ;
        workMutex.unlock();
        renderMutex.unlock();
    }
    
    void copyBuffers() { // rendering is allowed durint this call, since we only read from [renderIndex] here
        workMutex.lock();
            vertexArray[workIndex] = vertexArray[renderIndex];
            indexArray[workIndex] = indexArray[renderIndex];
            glp[workIndex] = glp[renderIndex];
        workMutex.unlock();
    }
    

protected:
    /// set type for GL-rendering, e.g. GL_TRIANGLES, GL_QUADS, etc.
    void setType(GLenum t) { glp[workIndex].type = t; }
    
// data. double buffered. rendering uses [renderIndex], worker-task uses [workIndex]
    QVarLengthArray<GLVertex>    vertexArray[2];
    QVarLengthArray<VertexData>  vertexDataArray; // only one, since not needed for OpenGL drawing!
    QVarLengthArray<GLuint>      indexArray[2];
    GLParameters glp[2];

    unsigned int renderIndex; // either 0 or 1
    unsigned int workIndex;   // either 1 or 0
    
};

} // end namespace

#endif
