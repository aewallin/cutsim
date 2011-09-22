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

Octnode::Octnode(Octnode* nodeparent, unsigned int index, double nodescale, unsigned int nodedepth) {
    parent = nodeparent;
    idx = index;
    child.resize(8);
    vertex.resize(8);
    f.resize(8);
    if (parent) {
        center = parent->childcenter(idx);
        inside = parent->inside;
        outside = parent->outside;
    } else { 
        outside = true;
        inside = false;
        center = new GLVertex(0,0,0); // default center for root is (0,0,0)
    }
    scale = nodescale;
    depth = nodedepth;
    
    for ( int n=0;n<8;++n) {
        vertex[n] = new GLVertex(*center + direction[n] * scale ) ;
        f[n] = -1; // everything is "outside" by default.
    }
    bb.clear();
    bb.addPoint( *vertex[2] ); // vertex[2] has the minimum x,y,z coordinates
    bb.addPoint( *vertex[4] ); // vertex[4] has the max x,y,z
    
    
    isosurface_valid = false;
    evaluated = false;
    childcount = 0;
    childStatus = 0;
}

// call delete on children, vertices, and center
Octnode::~Octnode() {
    for(int n=0;n<8;++n) {
            delete child[n];
            child[n] = 0;
            delete vertex[n];
            vertex[n] = 0;
    }
    delete center;
    center = 0;
}


// create the 8 children of this node
void Octnode::subdivide() {
    if (this->childcount==0) {
        for( int n=0;n<8;++n ) {
            Octnode* newnode = new Octnode( this, n , scale/2.0 , depth+1 ); // parent,  idx, scale,   depth
            this->child[n] = newnode;
            newnode->inherit_f();
            ++childcount;
            
            // optimization: inherit one f[n] from the corner?
        }
    } else {
        std::cout << " DON'T subdivide a non-leaf node \n";
        assert(0); 
    }
}

                    
// inherit as well as possible the f-values from parent. (?a good idea? needed?)

void Octnode::inherit_f() {
    assert( this->parent );
    
    for (unsigned int i =0 ; i<8 ;i++) {
        if (i == idx) 
            f[i] = parent->f[i];
        else {
            f[i] = 0.5*(parent->f[idx] + parent->f[i]);  // some kind of linear interpolation
        }
    }
    
    //evaluated = true; //(?)
}

// evaluate vol->dist() at all the vertices and store in f[]
// set the insinde/outside flags based on the sings of dist()
void Octnode::evaluate(const OCTVolume* vol) {
    //assert( childcount==0 );
    outside = true;
    inside = true;
    for ( int n=0;n<8;++n) { // go through the 8 corners of the cube
        double newf = vol->dist( *(vertex[n]) );
        if ( !evaluated ) {
            f[n] = newf;
            setInValid();
        } else {
        // there is an existing stock. stock=+, void=-
            if ( f[n] < 0 ) {
                // do nothing, since we are "cutting air"
            } else if ( f[n] > 0 ) {
                if( (newf < f[n] )   ) { 
                    f[n] = newf;
                    setInValid();
                } 
            }
        }
        
        // set the flags
        if ( f[n] <= 0.0 ) {// if one vertex is inside
            outside = false; // then it's not an outside-node
        } else { // if one vertex is outside
            assert( f[n] > 0.0 );
            inside = false; // then it's not an inside node anymore
        }
    }
    evaluated = true;
}

void Octnode::setValid() {
    isosurface_valid = true;
    if (parent) 
        parent->setChildValid( idx ); // try to propagate valid up the tree:
}
void Octnode::setChildValid( unsigned int id ) {
    childStatus |= octant[id];
    if (childStatus == 255) { // all children valid...
        setValid(); // ...so *this valid
    }
}

void Octnode::setChildInValid( unsigned int id ) {
    childStatus &= ~octant[id];
}

void Octnode::setInValid() { 
    isosurface_valid = false;
    if ( parent && !parent->valid() )  {// update parent status also
        parent->setInValid();
        parent->setChildInValid(idx);
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

// return centerpoint of child with index n
GLVertex* Octnode::childcenter(int n) {
    return  new GLVertex(*center + ( direction[n] * 0.5*scale ));
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
