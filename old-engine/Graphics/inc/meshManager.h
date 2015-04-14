#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include "configGraphicsDLL.h"
#include "singleton.h"
#include "meshTypes.h"

namespace GFX
{
    class Mesh;

    class ENGINE_GRAPHICS_EXPORT MeshManager
        :   public Util::Singleton<MeshManager>
    {
    public:
        MeshManager();
        ~MeshManager();
        void Load();
        void Unload();
        Mesh* GetMesh(unsigned int type);
    private:
        Mesh* meshList_[MT_TOTAL];
    };
}

#endif