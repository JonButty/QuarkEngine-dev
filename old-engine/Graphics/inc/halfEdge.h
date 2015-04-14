#ifndef HALFEDGE_H
#define HALFEDGE_H

namespace GFX
{
    struct Face;
    struct Vertex;

    struct HalfEdge
    {
        Face* face_;
        Vertex* vert_;
        HalfEdge* next_;
        HalfEdge* twin_;
        
        inline HalfEdge* prev()
        {
            return next_->next_;
        }
        
        inline bool isBoundaryEdge() 
        {
            return !twin_->face_ || !face_;
        }
        
        HalfEdge()
            :   face_(0),
                vert_(0),
                next_(0),
                twin_(0)
        {
        }
    };
}

#endif