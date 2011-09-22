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

#include "octree.hpp"
#include "octnode.hpp"
#include "volume.hpp"

namespace cutsim {

//**************** Octree ********************/

Octree::Octree(double scale, unsigned int  depth, GLVertex& centerp) {
    root_scale = scale;
    max_depth = depth;
                    // parent, idx, scale, depth
    root = new Octnode( NULL , 0, root_scale, 0 );
    root->center = new GLVertex(centerp);
    for ( int n=0;n<8;++n) {
        root->child[n] = NULL;
    }
}

Octree::~Octree() {
    delete root;
    root = 0;
}

unsigned int Octree::get_max_depth() const {
    return max_depth;
}

double Octree::get_root_scale() const {
    return root_scale;
}

double Octree::leaf_scale() const {
    return (2.0*root_scale) / pow(2.0, (int)max_depth );
}
        
/// subdivide the Octree n times
void Octree::init(const unsigned int n) {
    for (unsigned int m=0;m<n;++m) {
        std::vector<Octnode*> nodelist;
        get_leaf_nodes(root, nodelist);
        BOOST_FOREACH( Octnode* node, nodelist) {
            node->subdivide();
        }
    }
}

void Octree::get_invalid_leaf_nodes( std::vector<Octnode*>& nodelist) const {
    get_invalid_leaf_nodes( root, nodelist );
}

void Octree::get_invalid_leaf_nodes(Octnode* current, std::vector<Octnode*>& nodelist) const {
    if ( current->childcount == 0 ) {
        if ( !current->valid() ) {
            nodelist.push_back( current );
        }
    } else {//surface()surface()
        for ( int n=0;n<8;++n) {
            if ( current->hasChild(n) ) {
                if ( !current->valid() ) {
                    get_leaf_nodes( current->child[n], nodelist );
                }
            }
        }
    }
}   


/// put leaf nodes into nodelist
void Octree::get_leaf_nodes(Octnode* current, std::vector<Octnode*>& nodelist) const {
    if ( current->isLeaf() ) {
        nodelist.push_back( current );
    } else {
        for ( int n=0;n<8;++n) {
            if ( current->child[n] != 0 )
                get_leaf_nodes( current->child[n], nodelist );
        }
    }
}

/// put all nodes into nodelist
void Octree::get_all_nodes(Octnode* current, std::vector<Octnode*>& nodelist) const {
    if ( current ) {
        nodelist.push_back( current );
        for ( int n=0;n<8;++n) {
            if ( current->child[n] != 0 )
                get_all_nodes( current->child[n], nodelist );
        }
    }
}

// starting at current, traverse tree and apply sum to leaf nodes
//  if overlapping non-leafs found, subdivide.
void Octree::sum(Octnode* current, const OCTVolume* vol) {
    if ( ( current->depth == max_depth ) && vol->bb.overlaps( current->bb ) ) { // overlapping leaf-node
        if (debug) std::cout << " depth=" << current->depth << " leaf sum()\n";
        current->sum(vol);
        current->setInValid();
    } else if ( vol->bb.overlaps( current->bb ) )  { // boulding-box of volume overlaps with this node
                                                     // so dive further into tree.
        //current->sum(vol);
        if ( current->childcount == 8 ) { // has-child nodes
                for(int m=0;m<8;++m) {
                    if (debug) std::cout << " depth=" << current->depth << " CHILD sum()\n";
                    sum( current->child[m], vol); // call sum on children
                }
            
        } else if ( (current->depth < (this->max_depth)) ) { //&& !current->outside && !current->inside) {
                
                current->subdivide(); // smash into 8 sub-pieces
                for(int m=0;m<8;++m) {
                    if (debug) std::cout << " depth=" << current->depth << " SUBDIVIDE sum()\n";
                    sum( current->child[m], vol); // call sum on children
                }

        }
    }
}

void Octree::diff(Octnode* current, const OCTVolume* vol) {
    if ( current->depth == (max_depth) && vol->bb.overlaps( current->bb ) ) { // a leaf-node
        if (debug) std::cout << " depth=" << current->depth << " leaf diff()\n";
        current->diff(vol);
        current->setInValid();
    } else if ( vol->bb.overlaps( current->bb ) )  { // boulding-box of volume overlaps with this node
                                                     // so dive further into tree.

        if ( current->childcount == 8 ) {
            for(int m=0;m<8;++m) {
                if (debug) std::cout << " depth=" << current->depth << " CHILD diff()\n";
                diff( current->child[m], vol); // call sum on children
            }
        } else if ( current->depth < (this->max_depth) ) { 
            current->subdivide(); // smash into 8 sub-pieces
            for(int m=0;m<8;++m) {
                if (debug) std::cout << " depth=" << current->depth << " SUBDIVIDE diff()\n";
                diff( current->child[m], vol); // call sum on children
            }
        }
    }
}

// subtract vol from the Octnode current
#ifdef OLD_DIFF
void Octree::diff(Octnode* current, const OCTVolume* vol) {
    if (debug)
        std::cout << " diff() depth=" << current->depth << "\n";
        
    if ( current->isLeaf() ) { // process only leaf-nodes
        current->evaluate( vol ); // this evaluates the distance field and sets the inside/outside flags
        if ( current->inside  ) { 
            // inside nodes should be deleted
            if (debug) {
                std::cout << " inside node, remove!: " << current->str() << " \n";
            }
            remove_node_vertices(current);
            Octnode* parent = current->parent;                          assert( parent );
            unsigned int delete_index = current->idx;                   assert( delete_index >=0 && delete_index <=7 ); 
            delete parent->child[ delete_index ]; // call destructor!
            parent->child[ delete_index ]=0;
            --parent->childcount;
            assert( parent->childcount >=0 && parent->childcount <=8);
            if (parent->childcount == 0)  { // if the parent has become a leaf node
                //diff_negative(parent, vol); // this causes segfault... WHY?
                parent->evaluate( vol ); // back up the tree
                if (! parent->inside ) {
                    std::cout << " !parent->inside \n" << parent->printF() << "\n";
                }
                assert( parent->inside  ); // then it is itself inside
            }
            
        } else if (current->outside) {
            if (debug)
                std::cout << " " << current->depth << " outside, do nothing.\n";
            // do nothing to outside  leaf nodes.
            // this terminates recursion.
        } else {// these are intermediate (netiher inside nor outside) leaf nodes
            if ( current->depth < (this->max_depth) ) { 
                if (debug)
                    std::cout << " " << current->depth << " not in/out. subdivide.\n";
                // subdivide, if possible
                current->subdivide();                                   assert( current->childcount == 8 );
                for(int m=0;m<8;++m) {
                    assert(current->child[m]); // when we subdivide() there must be a child.
                    if ( vol->bb.overlaps( current->child[m]->bb ) ) {
                        if (debug)
                            std::cout << " " << current->depth << " calling diff on NEW child " << m << "\n";
                        diff( current->child[m], vol); // call diff on child
                    }
                }
            } else { 
                // max depth reached, intermediate node, but can't subdivide anymore
                assert( current->depth == (this->max_depth) );
                if (debug)
                    std::cout << " " << current->depth << " at max depth, can't subdivide.\n";
            }
        }
    } else { // not a leaf, so go deeper into tree
        for(int m=0;m<8;++m) { 
            if ( current->child[m] ) {
                if ( vol->bb.overlaps( current->child[m]->bb ) )
                    if (debug)
                        std::cout << " " << current->depth << " not leaf. calling diff on child " << m << "\n";
                    
                    diff( current->child[m], vol); // call diff on child
            }
        }
    }

}
#endif


// starting at current, update the isosurface
void Octree::updateGL(Octnode* current) {
    if (current->valid() ) {
        return; // since valid(), do nothing. terminate recursion here as early as possible.
    } else if ( current->isLeaf() && current->surface()  && !current->valid() ) {  // 
        // this is a leaf and a surface-node
        BOOST_FOREACH( std::vector< GLVertex > poly, mc->mc_node(current) ) {
            std::vector<unsigned int> polyIndexes;
            for (unsigned int m=0;m< poly.size() ;++m) { // Three for triangles, FOUR for quads
                unsigned int vertexId =  g->addVertex( poly[m].x, poly[m].y, poly[m].z, poly[m].r, poly[m].g, poly[m].b, current ); // add vertex to GL
                current->addIndex( vertexId ); // associate node with vertex
                g->setNormal( vertexId, poly[m].nx, poly[m].ny, poly[m].nz );
                polyIndexes.push_back( vertexId );
            }
            g->addPolygon(polyIndexes); // add poly to GL
            current->setValid(); // isosurface is now valid for this node!
        }
    } else if ( current->isLeaf() && !current->surface() && !current->valid() ) { 
        //leaf, but no surface
        current->setValid(); // ??
    } else {
        for (int m=0;m<8;++m) { // go deeper into tree, if !valid
            if ( current->hasChild(m)  && !current->valid() ) { // 
                updateGL(current->child[m]);
            }
        }
    }
}


// we store the gl-vertices associated with this node in the vertexSet
// when a node is deleted, we here remove each vertex by calling g->removeVertex()
// this also removes any associated polygons
void Octree::remove_node_vertices(Octnode* current ) {
    if (debug)
        std::cout << " remove_node_vertices ....";
    while( !current->vertexSetEmpty() ) {
        unsigned int delId = current->vertexSetTop();
        current->removeIndex( delId );
        g->removeVertex( delId );
    }
    if (debug)
        std::cout << " done.\n";
    assert( current->vertexSetEmpty() ); // when done, set should be empty
}

// string repr
std::string Octree::str() const {
    std::ostringstream o;
    o << " Octree: ";
    std::vector<Octnode*> nodelist;
    Octree::get_all_nodes(root, nodelist);
    std::vector<int> nodelevel(this->max_depth);
    std::vector<int> invalidsAtLevel(this->max_depth);
    std::vector<int> surfaceAtLevel(this->max_depth);
    BOOST_FOREACH( Octnode* n, nodelist) {
        ++nodelevel[n->depth];
        if ( !n->valid() ) 
            ++invalidsAtLevel[n->depth];
        if (n->surface() ) 
            ++surfaceAtLevel[n->depth];
    }
    o << "  " << nodelist.size() << " leaf-nodes:\n";
    int m=0;
    BOOST_FOREACH( int count, nodelevel) {
        o << "depth="<<m <<"  " << count << " nodes, " << invalidsAtLevel[m] << " invalid, surface=" << surfaceAtLevel[m] << " \n";
        ++m;
    }
    return o.str();
}

} // end namespace
// end of file octree.cpp
