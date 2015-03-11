/******************************************************************************/
/*!
\file		PathNode.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef SIMIAN_PATHNODE_H_
#define SIMIAN_PATHNODE_H_

#include "Simian/SimianTypes.h"
#include "Simian/Vector2.h"

namespace Descension
{
    class PathNode 
    {
    private:
        Simian::u32 x_, y_;
        Simian::u32 fScore_;
        Simian::u32 gScore_;
        const PathNode* parent_;
    public:
        Simian::u32 X () const; 
        void X (const Simian::u32& x);
        Simian::u32 Y () const; 
        void Y (const Simian::u32& y);
        Simian::u32 FScore () const; 
        void FScore (const Simian::u32& fScore);
        Simian::u32 GScore() const;
        void GScore(Simian::u32 val);
        const PathNode* Parent()const;
        void Parent(const PathNode& parent);
    public:
        PathNode (PathNode node, const PathNode& parent );
        PathNode (Simian::u32 x = 0, Simian::u32 y = 0, Simian::u32 gScore = 0, 
                  Simian::u32 fScore = 0, const PathNode* parent = 0);

        PathNode& operator= (const PathNode& node);
        PathNode& operator= (const Simian::Vector2 vec);
    };
}
#endif
