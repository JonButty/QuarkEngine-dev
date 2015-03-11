#include "cameraManager.h"
#include "camera.h"
#include "cameraFactory.h"

#define MAX_CAMERA_COUNT 128

namespace GFX
{
    CameraManager::CameraManager()
    {
        for(unsigned int i = 0; i < C_TOTAL; ++i)
            cameraList_[i] = 0;
    }

    CameraManager::~CameraManager()
    {
    }

    void CameraManager::Load()
    {
        // Initialize handle pool
        for(unsigned int i = 0; i < MAX_CAMERA_COUNT; ++i)
            handlePool_.Push(i);

        // Must have at least one camera
        SetCurrentCamera(AddCamera(C_PIVOT));
    }

    void CameraManager::Unload()
    {
        for(unsigned int i = 0; i < C_TOTAL; ++i)
        {
            if(cameraList_[i])
            {
                CameraList::iterator it = cameraList_[i]->begin();
                while(it != cameraList_[i]->end())
                {
                    handlePool_.Return(it->first);
                    delete it->second;

                    CameraList::iterator temp = it;
                    ++it;
                    cameraList_[i]->erase(temp);
                }
                delete cameraList_[i];
                cameraList_[i] = 0;
            }
        }
    }

    GFX::CameraHandle CameraManager::AddCamera( Camera* camera )
    {
        if(!cameraList_[camera->Type()])
            cameraList_[camera->Type()] = new CameraList();

        CameraHandle handle = handlePool_.Pop();
        cameraList_[camera->Type()]->insert(std::pair<CameraHandle,Camera*>(handle,camera));
        return handle;
    }

    GFX::CameraHandle CameraManager::AddCamera( unsigned int type )
    {
        if(!cameraList_[type])
            cameraList_[type] = new CameraList();

        Camera* camera = 0;
        CameraFactory::Instance().CreateInstance(type,camera);
        CameraHandle handle = handlePool_.Pop();
        cameraList_[type]->insert(std::pair<CameraHandle,Camera*>(handle,camera));
        return handle;
    }

    void CameraManager::RemoveCamera( CameraHandle handle )
    {
        for(unsigned int i = 0; i < C_TOTAL; ++i)
        {
            if(!cameraList_[i])
                continue;

            CameraList::iterator it = cameraList_[i]->find(handle);
            if(it != cameraList_[i]->end())
            {
                cameraList_[i]->erase(it);
                handlePool_.Return(handle);
            }
        }
    }

    void CameraManager::SetCurrentCamera( CameraHandle handle )
    {
        // Check if this handle exists in the camera list
        for(unsigned int i = 0; i < C_TOTAL; ++i)
        {
            if(!cameraList_[i])
                continue;

            CameraList::iterator it = cameraList_[i]->find(handle);
            if(it != cameraList_[i]->end())
                currCam_ = handle;
        }
    }

    GFX::CameraHandle CameraManager::CycleNextCamera( CameraHandle handle )
    {
        for(unsigned int i = 0; i < C_TOTAL; ++i)
        {
            if(!cameraList_[i])
                continue;

            CameraList::iterator it = cameraList_[i]->find(handle);
            if(it != cameraList_[i]->end())
            {
                // If handle references the last camera, return the first camera
                if((++it) == cameraList_[i]->end())
                    return cameraList_[i]->begin()->first;
                return it->first;
            }
        }
        return handle;
    }

    GFX::CameraHandle CameraManager::CyclePreviousCamera( CameraHandle handle )
    {
        for(unsigned int i = 0; i < C_TOTAL; ++i)
        {
            if(!cameraList_[i])
                continue;

            CameraList::iterator it = cameraList_[i]->find(handle);
            if(it != cameraList_[i]->end())
            {
                // If handle references the first camera, return the last camera
                if(it == cameraList_[i]->begin())
                    return (--cameraList_[i]->end())->first;
                return it->first;
            }
        }
        return handle;
    }

    Camera* CameraManager::GetCamera( CameraHandle handle ) const
    {
        for(unsigned int i = 0; i < C_TOTAL; ++i)
        {
            if(!cameraList_[i])
                continue;

            CameraList::const_iterator it = cameraList_[i]->find(handle);
            if(it != cameraList_[i]->end())
                return it->second;
        }
        return NULL;
    }

    CameraManager::CameraList* CameraManager::GetCameras( unsigned int type ) const
    {
        return cameraList_[type];
    }

    //--------------------------------------------------------------------------

    GFX::CameraHandle CameraManager::GetCurrentCamera() const
    {
        return currCam_;
    }
}