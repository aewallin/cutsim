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

#ifndef ISOSURFACE_H
#define ISOSURFACE_H

#include <cassert>
#include <vector>


#include "gldata.hpp"
#include "octree.hpp"
#include "octnode.hpp"

namespace cutsim {

/// abstract base class for isosurface extraction algorithms
/// 
///
class IsoSurfaceAlgorithm {
    public:
        IsoSurfaceAlgorithm(GLData* gl, Octree* tr) : g(gl), tree(tr) {}
        virtual ~IsoSurfaceAlgorithm() { }
        
        // return polygons corresponding to the octree node
        //virtual std::vector< std::vector< GLVertex >  > polygonize_node(const Octnode* node) = 0;
        virtual void updateGL(){ 
            update_calls=0;
            valid_count=0;
            debugValid();
            updateGL( tree->root );
            debugValid();
            
            g->updateVBO();
            std::cout << update_calls << " calls made\n";
            std::cout << valid_count << " valid_nodes\n";
        }
        void setColor(GLfloat r, GLfloat g, GLfloat b) {
            red=r;
            green=g;
            blue=b;
        }

    protected:
        int update_calls, valid_count;
        virtual void updateGL( Octnode* node) =0 ;
        GLfloat red,green,blue; // current color for vertices
        GLData* g;
        Octree* tree;
        
        void remove_node_vertices(Octnode* current ) {
            while( !current->vertexSetEmpty() ) {
                unsigned int delId = current->vertexSetTop();
                //std::cout << "removing " << delId << "\n";
                current->removeIndex( delId );
                g->removeVertex( delId );
            }
            assert( current->vertexSetEmpty() ); // when done, set should be empty
        }
        
        void debugValid() {
            std::vector<Octnode*> nodelist; // = new std::vector<Octnode*>();
            tree->get_all_nodes( tree->root,  nodelist);
            int val=0,inv=0;
            BOOST_FOREACH( Octnode* node , nodelist ) {
                if ( node->valid() )
                    val++;
                else
                    inv++;
            }
            std::cout << "debugValid() " << val << "valid nodes and " << inv << " invalid total=" << nodelist.size() <<" \n";
        }
};


class CubeSurf : public IsoSurfaceAlgorithm {
    public:
        CubeSurf(GLData* gl, Octree* tr ) : IsoSurfaceAlgorithm(gl,tr) {
            g->setLines(); // two indexes per line-segment
            g->setUsageDynamicDraw();

            inside_color.set(1,0,0);
            undecided_color.set(0,1,0);
            outside_color.set(0,0,1);
            
            draw_inside=true;
            draw_outside=false;
            draw_undecided=false;
            
        }
    


    protected:
        Color inside_color;
        Color outside_color;
        Color undecided_color;
        
        bool draw_inside, draw_outside,draw_undecided;
        // traverse tree and add/remove gl-elements to GLData
        void updateGL( Octnode* node) {
            if (node->valid()) {
                valid_count++;
                return;
            } else if ( !node->valid() ) {
                update_calls++;
                node->clearVertexSet();
                
                // add lines corresponding to the cube.
                const int segTable[12][2] = { // cube image: http://paulbourke.net/geometry/polygonise/
                    {0, 1},{1, 2},{2, 3},{3, 0},
                    {4, 5},{5, 6},{6, 7},{7, 4},
                    {0, 4},{1, 5},{2, 6},{3, 7}
                };
                if ( (node->is_inside() && draw_inside) ||
                     (node->is_outside() && draw_outside) ||
                     (node->is_undecided() && draw_undecided) 
                    ) {
                    for (unsigned int i=0; i <12 ; i++ ) {
                        std::vector< unsigned int > lineSeg;
                        GLVertex p1 = *(node->vertex)[ segTable[i][0 ] ];
                        GLVertex p2 = *(node->vertex)[ segTable[i][1 ] ];
                        Color line_color;
                        if (node->is_outside()) {
                            line_color = outside_color;
                        } 
                        if (node->is_inside()) {
                            line_color = inside_color;
                        } 
                        if ( node->is_undecided() ) {
                            line_color = undecided_color;
                        }
                        p1.setColor( line_color );
                        p2.setColor( line_color );
                            
                        lineSeg.push_back( g->addVertex( p1, node ) );
                        lineSeg.push_back( g->addVertex( p2, node ) );
                        node->addIndex( lineSeg[0] ); 
                        node->addIndex( lineSeg[1] ); 
                        g->addPolygon( lineSeg );
                    }
                }
                node->setValid();
                
                // current node done, now recurse into tree.
                if ( node->childcount == 8 ) {
                    for (unsigned int m=0;m<8;m++) {
                        //if ( !node->child[m]->valid() )
                            updateGL( node->child[m] );
                    }
                }
            }
        }
        
        /*
        std::vector< std::vector< GLVertex > > polygonize_node(const Octnode* node) {
            assert( node->childcount == 0 ); // don't call this on non-leafs!
            std::vector< std::vector< GLVertex > > triangles;
            // unsigned int edgeTableIndex = mc_edgeTableIndex(node);
            // the index into this table now tells us which edges have the vertices
            // for the new triangles
            // the lookup returns a 12-bit number, where each bit indicates wether 
            // the edge is cut by the isosurface
            //unsigned int edges = edgeTable[edgeTableIndex];
            // calculate intersection points by linear interpolation
            // there are now 12 different cases:
            //std::vector< GLVertex > vertices = interpolated_vertices(node, edges);
            // assert( vertices.size()==12 );
            // form triangles by lookup in triTable
            
            const int triTable[12][3] = {
                {0, 1, 2},
                {0, 2, 3},
                {0, 4, 7},
                {0, 3, 7},
                {0, 1, 5},
                {0, 4, 5},
                {4, 5, 6},
                {4, 6, 7},
                {1, 5, 6},
                {1, 2, 6},
                {2, 6, 7},
                {2, 3, 7}
                }; 
            if (node->inside) {
                for (unsigned int i=0; i <12 ; i++ ) {
                    std::vector< GLVertex > triangle;
                    triangle.push_back( *(node->vertex)[ triTable[i][0 ] ] );
                    triangle.push_back( *(node->vertex)[ triTable[i][1 ] ] );
                    triangle.push_back( *(node->vertex)[ triTable[i][2 ] ] );
                    // calculate normal
                    GLVertex n = (triangle[0]-triangle[1]).cross( triangle[0]-triangle[2] );
                    n.normalize();
                    triangle[0].setNormal(n.x,n.y,n.z);
                    triangle[1].setNormal(n.x,n.y,n.z);
                    triangle[2].setNormal(n.x,n.y,n.z);
                    // setColor
                    triangle[0].setColor(red,green,blue);
                    triangle[1].setColor(red,green,blue);
                    triangle[2].setColor(red,green,blue);
                    triangles.push_back( triangle );
                }
            }
            return triangles;
        }*/
};

} // end namespace
#endif
// end file isosurface.hpp
