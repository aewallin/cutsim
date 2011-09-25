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


#include <list>
#include <cassert>
#include <iostream>
#include <sstream>

#include <boost/foreach.hpp>

#include "octnode.hpp"

namespace cutsim {

//**************** Octnode ********************/

// this defines the position of each octree-vertex with relation to the center of the node
// this also determines in which direction the center of a child node is
const GLVertex Octnode::direction[8] = {
                     GLVertex( 1, 1,-1),   // 0
                     GLVertex(-1, 1,-1),   // 1
                     GLVertex(-1,-1,-1),   // 2
                     GLVertex( 1,-1,-1),   // 3
                     GLVertex( 1, 1, 1),   // 4
                     GLVertex(-1, 1, 1),   // 5
                     GLVertex(-1,-1, 1),   // 6
                     GLVertex( 1,-1, 1)    // 7
                    };
// surface enumeration
// surf     vertices  vertices
// 0:       2,3,7     2,6,7
// 1:       0,4,7     0,3,7
// 2:       0,1,4     1,4,5
// 3:       1,5,6     1,2,6
// 4:       0,2,3     0,1,2
// 5:       4,6,7     4,5,6

const char Octnode::octant[8] = {
                    1,
                    2,
                    4,
                    8,
                    16,
                    32,
                    64,
                    128
                };

Octnode::Octnode(Octnode* nodeparent, unsigned int index, double nodescale, unsigned int nodedepth, GLData* gl) {
    g=gl;
    parent = nodeparent;
    idx = index;
    scale = nodescale;
    depth = nodedepth;
    
    child.resize(8);
    vertex.resize(8);
    f.resize(8);
    if (parent) {
        center = parent->childcenter(idx);
        state = parent->prev_state;
        prev_state = state;
        color = parent->color;
    } else { // root node has no parent
        center = new GLVertex(0,0,0); // default center for root is (0,0,0)
        state = UNDECIDED;
        prev_state = OUTSIDE;
    }

    
    for ( int n=0;n<8;++n) {
        vertex[n] = new GLVertex(*center + direction[n] * scale ) ;
        if (parent) {
            assert( parent->state == UNDECIDED );
            assert( parent->prev_state != UNDECIDED );
            //std::cout << parent->prev_state << "\n";
            if (parent->prev_state == INSIDE) {
                f[n]=1;
                state = INSIDE;
            }
            else if (parent->prev_state == OUTSIDE) {
                f[n]=-1;
                state = OUTSIDE;
            }
            else
                assert(0);
                
             //f[n]= parent->f[n];  // why does this make a big diggerence in the speed of sum() and dif() ??
             // sum() sum(): 0.15s + 0.27s   compared to 1.18 + 0.47
             // sum() diff(): 0.15 + 0.2     compared to 1.2 + 0.46
        } else {
            f[n] = -1; 
        }
    }
    bb.clear();
    bb.addPoint( *vertex[2] ); // vertex[2] has the minimum x,y,z coordinates
    bb.addPoint( *vertex[4] ); // vertex[4] has the max x,y,z
    
    isosurface_valid = false;
    evaluated = false;
    
    childcount = 0;
    childStatus = 0;
    //set_flags();
}

// call delete on children, vertices, and center
Octnode::~Octnode() {
    if (childcount == 8 ) {
        for(int n=0;n<8;++n) {
            delete child[n];
            child[n] = 0;
            delete vertex[n];
            vertex[n] = 0;
        }
    }
    delete center;
    center = 0;
}

// return centerpoint of child with index n
GLVertex* Octnode::childcenter(int n) {
    return  new GLVertex(*center + ( direction[n] * 0.5*scale ));
}


// create the 8 children of this node
void Octnode::subdivide() {
    if (this->childcount==0) {
        if( state != UNDECIDED )
            std::cout << " subdivide() error: state==" << state << "\n";
            
        assert( state == UNDECIDED );
        for( int n=0;n<8;++n ) {
            Octnode* newnode = new Octnode( this, n , scale/2.0 , depth+1 , g); // parent,  idx, scale,   depth, GLdata
            this->child[n] = newnode;
            ++childcount;
        }
    } else {
        std::cout << " DON'T subdivide a non-leaf node \n";
        assert(0); 
    }
}


void Octnode::delete_children() {
    if (childcount==8) {
        NodeState s0 = child[0]->state;
        //std::cout << spaces() << depth << ":" << idx << " delete_children\n";
        //std::cout << "before: s0= " << s0 << " \n";
        //std::cout << " delete_children() states: ";
        //for ( int m=0;m<8;m++ ) {
        //    std::cout << child[m]->state << " ";
        //}
                
        for (int n=0;n<8;n++) {
            //std::cout << depth << " deleting " << n << "\n";
            if ( s0 != child[n]->state ) {
                std::cout << " delete_children() error: ";
                //for ( int m=0;m<8;m++ ) {
                //    std::cout << child[m]->state << " ";
                //}
                std::cout << "\n";
                std::cout << " s0= " << s0 << " \n";
            }
            assert( s0 == child[n]->state );
            child[n]->clearVertexSet(  );
            delete child[n];
            child[n]=0;
            childcount--;
        }
        assert( childcount == 0);
    }
    
}
                    






void Octnode::setValid() {
    isosurface_valid = true;
    //std::cout << spaces() << depth << ":" << idx << " setValid()\n";
    if (parent) 
        parent->setChildValid( idx ); // try to propagate valid up the tree:
}
void Octnode::setChildValid( unsigned int id ) {
    childStatus |= octant[id];
    if (childStatus == 255) { // all children valid...
        setValid(); // ...so *this valid
    }
}

void Octnode::setChildInvalid( unsigned int id ) {
    setInvalid();
    childStatus &= ~octant[id];
}

void Octnode::setInvalid() { 
    isosurface_valid = false;
    if ( parent && parent->valid() )  {// update parent status also
        //parent->setInvalid();
        parent->setChildInvalid(idx);
    }
}
bool Octnode::valid() const {
    return isosurface_valid;
}


void Octnode::addIndex(unsigned int id) { 
    std::set<unsigned int>::iterator found = vertexSet.find( id );
    assert( found == vertexSet.end() ); // we should not have id
    vertexSet.insert(id); 
}
void Octnode::swapIndex(unsigned int oldId, unsigned int newId) {
    std::set<unsigned int>::iterator found = vertexSet.find(oldId);
    assert( found != vertexSet.end() ); // we must have oldId
    vertexSet.erase(oldId);
    vertexSet.insert(newId);
}

void Octnode::removeIndex(unsigned int id) {
    std::set<unsigned int>::iterator found = vertexSet.find( id );
    assert( found != vertexSet.end() ); // we must have id
    vertexSet.erase(id);
}

void Octnode::clearVertexSet( ) {
    while( !vertexSetEmpty() ) {
        unsigned int delId = vertexSetTop();
        removeIndex( delId );
        g->removeVertex( delId );
    }
    assert( vertexSetEmpty() ); // when done, set should be empty
}

// string repr
std::ostream& operator<<(std::ostream &stream, const Octnode &n) {
    stream << " node "; //c=" << *(n.center) << " depth=" << n.depth ;     
    return stream;
}

std::string Octnode::printF() {
    std::ostringstream o;
    for (int n=0;n<8;n++) {
        o << "f[" << n <<"] = " << f[n] << "\n";
    }
    return o.str();
}

// string repr
std::string Octnode::str() const {
    std::ostringstream o;
    o << *this;
    return o.str();
}

} // end namespace
// end of file octnode.cpp
