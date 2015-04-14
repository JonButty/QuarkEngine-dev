/******************************************************************************/
/*!
\file		PathNode.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "PathNode.h"

namespace Descension
{
    PathNode::PathNode (Simian::u32 x, Simian::u32 y, Simian::u32 gScore, 
                        Simian::u32 fScore,const PathNode* parent)
        :   x_(x),
            y_(y),
            fScore_(fScore),
            gScore_(gScore),
            parent_(parent)
    {
    }

    PathNode::PathNode( PathNode node, const PathNode& parent )
        :   x_(node.x_),
            y_(node.y_),
            fScore_(node.fScore_),
            gScore_(parent.gScore_),
            parent_(&parent)
    {
    }

    //-Public-------------------------------------------------------------------

    Simian::u32 PathNode::X () const
    {
        return x_;
    }

    void PathNode::X (const Simian::u32& x)
    {
        x_ = x;
    }

    Simian::u32 PathNode::Y () const
    {
        return y_;
    }

    void PathNode::Y (const Simian::u32& y)
    {
        y_ = y;
    }
    Simian::u32 PathNode::FScore () const
    {
        return fScore_;
    }

    void PathNode::FScore (const Simian::u32& fScore)
    {
        fScore_ = fScore;
    }

    Simian::u32 PathNode::GScore() const
    {
        return gScore_;
    }

    void PathNode::GScore( Simian::u32 val )
    {
        gScore_ = val;
    }

    const PathNode* PathNode::Parent()const
    {
        return parent_;
    }

    void PathNode::Parent( const PathNode& parent )
    {
        parent_ = &parent;
    }

    PathNode& PathNode::operator= (const PathNode& node)
    {
        x_ = node.X();
        y_ = node.Y();
        fScore_ = node.FScore();
        parent_ = node.Parent();
        return *this;
    }

    PathNode& PathNode::operator= (const Simian::Vector2 vec)
    {
        x_ = static_cast<Simian::s32>(vec.X() + 0.5f);
        y_ = static_cast<Simian::s32>(vec.Y()+ 0.5f);
        return *this;
    }
}
