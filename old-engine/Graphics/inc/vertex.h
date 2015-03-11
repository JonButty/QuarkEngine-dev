#ifndef VERTEX_H
#define VERTEX_H

#include "vector3.h"
#include "color.h"
#include "configGFX.h"
#include "halfEdge.h"
#include "delegate.h"

#include <set>

namespace GFX
{
    struct Vertex
    {
        Math::Vec3F pos_;
        Math::Vec3F norm_;
        Color color_;
        HalfEdge* edge_;

        Vertex()
            :   edge_(0),
                pos_(Math::Vec3F::Zero),
                norm_(Math::Vec3F::Zero),
                color_(Color::Black)
        {
        }
    };

    struct EdgeParam
        :   Util::DelegateParameter
    {
        HalfEdge* edge_;
    };

    static void OneRingTraversal(Vertex* v,
                                 Util::Delegate& func)
    {
        std::set<HalfEdge*> closedList;
        
        if(!v->edge_)
            return;

        HalfEdge* currEdge = v->edge_;
        HalfEdge* stopEdge = currEdge;
        closedList.insert(currEdge);

        do 
        {
            EdgeParam param;
            param.edge_ = currEdge;
            func.Call(param);
            
            // Reached a border
            // Try the other direction
            // This applies only if vertex layout is not a fan
            if(!currEdge->next_)
            {
                currEdge = v->edge_->twin_;
                while(currEdge->next_)
                {
                    // Current edge has been visited
                    if(closedList.find(currEdge) != closedList.end())
                        break;

                    closedList.insert(currEdge);
                    param.edge_ = currEdge->next_;
                    func.Call(param);
                    currEdge = currEdge->next_->twin_;
                }
                break;
            }
            
            currEdge = currEdge->prev()->twin_;
            closedList.insert(currEdge);
        } while (currEdge != stopEdge);
    }
}

#endif