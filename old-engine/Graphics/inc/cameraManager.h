#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include "singleton.h"
#include "configGraphicsDLL.h"
#include "pool.h"
#include "cameraTypes.h"

#include <map>

namespace GFX
{
    class Camera;

    typedef unsigned int CameraHandle;

    class ENGINE_GRAPHICS_EXPORT CameraManager
        :   public Util::Singleton<CameraManager>
    {
    public:
        typedef std::map<CameraHandle,Camera*> CameraList;
    public:
        CameraManager();
        ~CameraManager();
        void Load();
        void Unload();
        CameraHandle AddCamera(Camera* camera);
        CameraHandle AddCamera(unsigned int type);
        void RemoveCamera(CameraHandle handle);
        void SetCurrentCamera(CameraHandle handle);
        CameraHandle CycleNextCamera(CameraHandle handle);
        CameraHandle CyclePreviousCamera(CameraHandle handle);
        Camera* GetCamera(CameraHandle handle) const;
        CameraList* GetCameras(unsigned int type) const;
    public:
        CameraHandle GetCurrentCamera() const;
    private:
        CameraHandle currCam_;
        CameraList* cameraList_[C_TOTAL];
        Util::Pool<CameraHandle> handlePool_;
    };
}

#endif