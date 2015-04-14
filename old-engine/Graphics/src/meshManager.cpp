#include "meshManager.h"
#include "meshBuilderOBJ.h"
#include "mesh.h"
#include "paths.h"
#include "gfxLogger.h"
#include "gfxAPI.h"

#include <string>

namespace GFX
{
    // TODO
    // Create a pre build process that parses an xml file that associates mesh files
    // with mesh types to generate the meshtypes.h file
    // Create a translation table (dictionary) that can map 1 - 1 between 2 types
    static const std::string MESH_FILES[MT_TOTAL] =
    {
        //"cube.obj"
        //"triangle.obj"//,
        //"diamond.obj"//,
        //"dragon_lowres_cleaned.obj"//,
        "bunny.obj"//,
        //"horse_lowres_cleaned.obj",
        //"happy_lowres_cleaned.obj"
    };

    MeshManager::MeshManager()
    {
        for(unsigned int i = 0; i < MT_TOTAL; ++i)
            meshList_[i] = 0;
    }

    MeshManager::~MeshManager()
    {
    }

    void MeshManager::Load()
    {
        unsigned int meshLoadCount = 0;
        for(unsigned int meshType = 0; meshType < MT_TOTAL; ++meshType)
        {
            GFX_LOG("Parsing mesh: " << MESH_FILES[meshType]);
            MeshBuilderOBJ::Instance().Parse(MESH_DIR + MESH_FILES[meshType],
                                             meshList_[meshType],
                                             meshType);
            if(!&meshList_[meshType])
            {
                GFX_LOG(MESH_FILES[meshType] << " failed to load.");
                continue;
            }

            meshList_[meshType]->Load(true);
            ++meshLoadCount;
        }

        GFX_LOG("========== Mesh Manager: " << meshLoadCount <<" succeeded, " <<
            MT_TOTAL - meshLoadCount << " failed ==========");
    }

    void MeshManager::Unload()
    {
        for(unsigned int i = 0; i < MT_TOTAL; ++i)
        {
            if(meshList_[i])
            {
                meshList_[i]->Unload();
                delete meshList_[i];
                meshList_[i] = 0;
            }
        }
    }

    Mesh* MeshManager::GetMesh( unsigned int type )
    {
        return meshList_[type];
    }
}