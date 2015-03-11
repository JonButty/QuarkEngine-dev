/*************************************************************************/
/*!
\file		terraformhooks.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifdef _DESCENDED

#include "Simian/SimianPlatform.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"
#include "Simian/LogManager.h"

#include "GCEditor.h"
#include "StateEditor.h"

#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <string>
#include <functional>

#define DESCENDED_EXPORT __declspec(dllexport) __stdcall

template <typename T>
static void splitString_(const char* str, std::vector<T>& vec)
{
    std::stringstream ss;
    ss << str;

    std::copy(std::istream_iterator<T>(ss), std::istream_iterator<T>(),
        std::back_inserter(vec));
}

static Simian::s32 resolveTile_(const std::string& tileName)
{
    if (!Simian::GameFactory::Instance().EntityTable().HasValue(tileName))
    {
        SWarn("No such tile: " << tileName);
        return -1;
    }
    return Simian::GameFactory::Instance().EntityTable().Value(tileName);
}

static void createTiles_(const char* tiles, std::vector<Simian::s32>& vec)
{
    std::vector<std::string> tiles_;
    splitString_(tiles, tiles_);

    // resolve the strings
    std::transform(tiles_.begin(), tiles_.end(),
        std::back_inserter(vec), resolveTile_);

    // remove any invalid tiles
    std::vector<Simian::s32>::iterator i = 
        std::remove_if(vec.begin(), vec.end(), 
        std::bind2nd(std::equal_to<Simian::s32>(), -1));
    vec.resize(i - vec.begin());
}

extern "C"
{
    void DESCENDED_EXPORT TerraformFill(const char* tiles, Simian::Entity* parent)
    {
        // resolve the strings
        std::vector<Simian::s32> tilesId;
        createTiles_(tiles, tilesId);

        if (tilesId.size())
            Descension::GCEditor::Instance().TerraformFill(tilesId, parent);
    }

    void DESCENDED_EXPORT TerraformFringe(const char* sides,
                                          const char* corners,
                                          const char* fills,
                                          const char* knobs,
                                          float knobOffset,
                                          Simian::Entity* parent)
    {
        std::vector<Simian::s32> side;
        createTiles_(sides, side);

        std::vector<Simian::s32> corner;
        createTiles_(corners, corner);

        std::vector<Simian::s32> fill;
        createTiles_(fills, fill);

        std::vector<Simian::s32> knob;
        createTiles_(knobs, knob);

        Descension::GCEditor::Instance().TerraformFringe(side, corner, fill, knob, knobOffset, parent);
    }
};

#endif

