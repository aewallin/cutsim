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

#ifndef OCTNODE_H
#define OCTNODE_H

#include <iostream>
#include <sstream>

#include <list>
#include <set>
#include <vector>




#include "volume.hpp"
#include "bbox.hpp"
#include "glvertex.hpp"
#include "gldata.hpp"

namespace cutsim {



/// Octnode represents a node in the octree.
///
/// each node in the octree is a cube with side length scale
/// the distance field at each corner vertex is stored.
class Octnode {
    public:
        enum NodeState  {INSIDE, OUTSIDE, UNDECIDED };
        NodeState state;
        NodeState childState[8];
        
        /// create suboctant idx of parent with scale nodescale and depth nodedepth
        Octnode(Octnode* parent, unsigned int idx, double nodescale, unsigned int nodedepth, GLData* g);
        virtual ~Octnode();
        /// create all eight children of this node
        void subdivide(); 

        void sum(const OCTVolume* vol) {
            for ( int n=0;n<8;++n) 
                f[n] = std::max( f[n], vol->dist( *(vertex[n]) ) );
            set_flags();
        }
        void diff(const OCTVolume* vol) {
            for ( int n=0;n<8;++n) 
                f[n] = std::min( f[n], -vol->dist( *(vertex[n]) ) );
            set_flags();
        }
        void intersect(const OCTVolume* vol) {
            for ( int n=0;n<8;++n) {
                f[n] = std::min( f[n], vol->dist( *(vertex[n]) ) );
            }
            set_flags();
        }
        
        
        
        void set_flags() {
            NodeState old_state = state;
            outside = true;
            inside = true;
            for ( int n=0;n<8;n++) {
                if ( f[n] >= 0.0 ) {// if one vertex is inside
                    outside = false; // then it's not an outside-node
                } else { // if one vertex is outside
                    assert( f[n] < 0.0 );
                    inside = false; // then it's not an inside node anymore
                }
            }
            assert( !( outside && inside) ); // sanity check
            
            if ( (inside) && (!outside) )
                setInside();
            else if ( (outside) && (!inside) )
                setOutside();
            else if ( (!inside) && (!outside) )
                setUndecided();
            else
                assert(0);
                
            assert( (is_inside() && !is_outside() && !is_undecided() ) ||
                    (!is_inside() && is_outside() && !is_undecided() ) ||
                    (!is_inside() && !is_outside() && is_undecided() ) );
            
            if ( ((old_state == INSIDE) && (state== INSIDE)) ||
                ((old_state == OUTSIDE) && (state== OUTSIDE))
             ) {
                } else {
                    setInvalid();
                }
        }
        
        bool is_inside()    { return (state==INSIDE); }
        bool is_outside()   { return (state==OUTSIDE); }
        bool is_undecided() { return (state==UNDECIDED); }
        
        void setInside() {
            //std::cout << spaces() << depth << ":" << idx << "setInside()";
            state = INSIDE;
            if (parent)
                parent->setInside( this->idx );
        }
        void setInside(unsigned int id) {
            childState[id] = INSIDE;
            if ( all_child_state(INSIDE) && (state!=INSIDE) ) {
                setInside();
            }
        }
        void setUndecided() {
            //std::cout << spaces() << depth << ":" << idx << " setUndecided()\n";
            state = UNDECIDED;
            if (parent)
                parent->setUndecided( this->idx );
        }
        
        void setUndecided( unsigned int id ) {
            childState[id] = UNDECIDED;
            setUndecided(); // no checks, if one child UNDECIDED, then this UNDECIDED
        }
        
        void setOutside() {
            //std::cout << spaces() << depth << ":" << idx << " setOutside()\n";
            state = OUTSIDE;
            if (parent)
                parent->setOutside( this->idx );
        }
        void setOutside(unsigned int id) {
            childState[id] = OUTSIDE;
            if ( all_child_state(OUTSIDE)  && (state!=OUTSIDE) )  { // 
                setOutside(); // can remove children?

            }
        }
        
        bool all_child_state(NodeState s) {
            int n=0;
            for (unsigned int m=0;m<8;m++) {
                if ( childState[m] == s ) {
                    n++;
                }
            }
            return (n==8);
        }
        
        void delete_children();

    // manipulate the valid-flag
        void setValid();
        void setInvalid();
        bool valid() const;
        
        // surface nodes are neither inside nor outside
        inline bool hasChild(int n) { return (this->child[n] != NULL); }
        inline bool isLeaf() {return (childcount==0);}
    // DATA
        /// pointers to child nodes
        std::vector<Octnode*> child;
        /// pointer to parent node
        Octnode* parent;
        /// number of children
        unsigned int childcount;
        /// The eight corners of this node
        std::vector<GLVertex*> vertex; 
        /// value of implicit function at vertex
        std::vector<double> f; 
        /// flag set true if this node is outside
        bool outside;
        /// flag for inside node
        bool inside;

        /// the center point of this node
        GLVertex* center; // the centerpoint of this node
        /// the tree-dept of this node
        unsigned int depth; // depth of node
        /// the index of this node [0,7]
        unsigned int idx; // index of node
        /// the scale of this node, i.e. distance from center out to corner vertices
        double scale; // distance from center to vertices

        /// bounding-box corresponding to this node
        Bbox bb;
    

    
    // for manipulating vertexSet
        void addIndex(unsigned int id);
        void swapIndex(unsigned int oldId, unsigned int newId);
        void removeIndex(unsigned int id);
        bool vertexSetEmpty() {return vertexSet.empty(); }
        unsigned int vertexSetTop() { return *(vertexSet.begin()); }
        void clearVertexSet();
        
        
    // string output
        friend std::ostream& operator<<(std::ostream &stream, const Octnode &o);
        std::string str() const;
        std::string printF();
        std::string spaces() const {
            std::ostringstream stream;
            for (unsigned int m=0;m<this->depth;m++)
                stream << " ";  
            return stream.str();
        }
        std::string type() const {
            std::ostringstream stream;
            if (inside)
                stream << "inside";  
            else if (outside)
                stream << "outside";  
            else if (!outside && !inside)
                stream << "undecided";  
            else
                assert(0);
            return stream.str();
        }
    protected:  
        void setChildValid( unsigned int id );
        inline void setChildInvalid( unsigned int id );
        
        void inherit_f();
        
        // the vertex indices that this node produces
        std::set<unsigned int> vertexSet;
        /// flag for checking if evaluate() has run
        bool evaluated;
        /// return center of child with index n
        GLVertex* childcenter(int n); // return position of child centerpoint
        GLData* g;
        /// flag for telling isosurface extraction is valid for this node
        /// if false, the node needs updating.
        bool isosurface_valid;
        char childStatus;
        
// STATIC
        /// the direction to the vertices, from the center 
        static const GLVertex direction[8];
        /// bit masks for the status
        static const char octant[8];
    private:
        Octnode(){}
};

} // end namespace
#endif
// end file octnode.h
