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
        NodeState prev_state;
        Color color;
        /// create suboctant idx of parent with scale nodescale and depth nodedepth
        Octnode(Octnode* parent, unsigned int idx, double nodescale, unsigned int nodedepth, GLData* g);
        virtual ~Octnode();
        /// create all eight children of this node
        void subdivide(); 
        void force_subdivide() { // this is only called from octree-init..
            setUndecided();
            subdivide();
        }
        void sum(const OCTVolume* vol);
        void diff(const OCTVolume* vol);
        void intersect(const OCTVolume* vol);
        bool is_inside()    { return (state==INSIDE); }
        bool is_outside()   { return (state==OUTSIDE); }
        bool is_undecided() { return (state==UNDECIDED); }
        
        bool all_child_state(NodeState s);
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
        std::string spaces() const;
        std::string type() const;
        
    protected: 
        void set_state();
        void setInside();
        void setOutside();
        void setUndecided();
        void setChildValid( unsigned int id );
        inline void setChildInvalid( unsigned int id );

        // the vertex indices that this node produces
        std::set<unsigned int> vertexSet;
        /// return center of child with index n
        GLVertex* childcenter(int n); // return position of child centerpoint
        GLData* g;
        /// flag for telling isosurface extraction is valid for this node
        /// if false, the node needs updating.
        bool isosurface_valid;
        char childStatus; // bit-field indicating if children have valid gldata
        
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
// end file octnode.hpp
