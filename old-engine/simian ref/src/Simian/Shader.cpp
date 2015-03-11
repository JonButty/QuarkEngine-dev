/************************************************************************/
/*!
\file		Shader.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Shader.h"
#include "Simian/ShaderProgram.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/FileObjectSubNode.h"
#include "Simian/DataFileIO.h"
#include "Simian/LogManager.h"

#include <iterator>
#include "Simian/ShaderFloatParameter.h"
#include "Simian/ShaderBoolParameter.h"
#include "Simian/ShaderMatrixParameter.h"
#include "Simian/Texture.h"

namespace Simian
{
    static std::map<std::string, void(Shader::*)(ShaderParameter&)> GenerateBindingTable()
    {
        std::map<std::string, void(Shader::*)(ShaderParameter&)> ret;

        // world view proj etc
        ret["World"] = &Shader::updateWorld_;
        ret["View"] = &Shader::updateView_;
        ret["Proj"] = &Shader::updateProj_;
        ret["WorldViewProj"] = &Shader::updateWorldViewProj_;
        ret["WorldViewProj"] = &Shader::updateWorldViewProj_;

        // texture matrix access
        ret["TextureMatrix0"] = &Shader::updateTextureMatrix0_;
        ret["TextureMatrix1"] = &Shader::updateTextureMatrix1_;
        ret["TextureMatrix2"] = &Shader::updateTextureMatrix2_;
        ret["TextureMatrix3"] = &Shader::updateTextureMatrix3_;
        ret["TextureMatrix4"] = &Shader::updateTextureMatrix4_;
        ret["TextureMatrix5"] = &Shader::updateTextureMatrix5_;
        ret["TextureMatrix6"] = &Shader::updateTextureMatrix6_;
        ret["TextureMatrix7"] = &Shader::updateTextureMatrix7_;

        // accessing the texture state constant color
        ret["TextureStateColorConstant"] = &Shader::updateTextureStateColorConstant_;
        ret["TextureStateColorAdd"] = &Shader::updateTextureStateColorAdd_;

        // time access
        ret["HSample"] = &Shader::updateHSample_;
        ret["VSample"] = &Shader::updateVSample_;
        ret["Time"] = &Shader::updateTime_;

        return ret;
    }
    static const std::map<std::string, void(Shader::*)(ShaderParameter&)> CONSTANT_BINDING_TABLE(GenerateBindingTable());

    //--------------------------------------------------------------------------

    Shader::Shader(GraphicsDevice* device)
        : vertexProgram_(0),
          fragmentProgram_(0),
          device_(device)
    {
    }

    //--------------------------------------------------------------------------

    Simian::ShaderParameter Shader::VertexProgramConstant( const std::string& name )
    {
        return vertexProgram_ ? vertexProgram_->Parameter(name) : ShaderParameter();
    }

    Simian::ShaderParameter Shader::FragmentProgramConstant( const std::string& name )
    {
        return fragmentProgram_ ? fragmentProgram_->Parameter(name) : ShaderParameter();
    }

    //--------------------------------------------------------------------------

    template <typename T>
    bool Shader::loadShader_(T*& shader, bool(GraphicsDevice::*method)(T*&),
                             ProgramParameterSet& parameterSet_,
                             const FileObjectSubNode* data,
                             const std::string& defaultProfile)
    {
        std::string profile;
        data->Data("Profile", profile, defaultProfile);

        std::string entryPoint;
        data->Data("EntryPoint", entryPoint, "main");

        std::string path;
        data->Data("File", path, "");

        if (path == "")
            return false;

        // create the shader (if it fails then return false)
        if (!(device_->*method)(shader))
            return false;

        // try to load the shader
        if (!shader->Load(path, entryPoint, profile))
            return false;

        // parse constants and set them up!
        const FileArraySubNode* constants = data->Array("Constants");
        loadConstants_(shader, parameterSet_, constants);

        // everything okay!
        return true;
    }

    template <typename T>
    void Shader::loadConstants_( T*& shader, ProgramParameterSet& parameterSet_, 
                                 const FileArraySubNode* constants )
    {
        for (u32 i = 0; i < constants->Size(); ++i)
        {
            const FileObjectSubNode* constant = constants->Object(i);

            std::string name;
            constant->Data("Name", name, "");
            if (!shader->HasParameter(name))
            {
                SWarn("Shader does not have parameter: " << name);
                continue;
            }

            std::string type;
            constant->Data("Type", type, "");

            const FileDataSubNode* value = constant->Data("Value");
            ShaderParameter parameter = shader->Parameter(name);

            ShaderSetParameter* setParameter = 0;
            // see if we can find the parameter
            ProgramParameterSet::iterator iter = parameterSet_.find(name);
            if (iter != parameterSet_.end())
                setParameter = iter->second;

            if (type == "float")
                parseFloat_(setParameter, parameter, value);
            else if (type == "float[]" || type == "float2" || type == "float3" || type == "float4")
                parseFloatArray_(setParameter, parameter, value);
            else if (type == "bool")
                parseBool_(setParameter, parameter, value);
            else if (type == "bool[]")
                parseBoolArray_(setParameter, parameter, value);
            else if (type == "float4x4")
                parseMatrix_(setParameter, parameter, value);
            else if (type == "float4x4[]")
                parseMatrixArray_(setParameter, parameter, value);
            else
            {
                SWarn("Unrecognized shader constant type: " << type);
                continue;
            }

            // add set parameter to set
            if (iter == parameterSet_.end() && setParameter)
                parameterSet_.insert(std::make_pair(name, setParameter));
        }
    }

    void Shader::parseFloat_( ShaderSetParameter*& setparam, ShaderParameter& param, const FileDataSubNode* data )
    {
        if (addBoundConstant(param, data->AsString()))
            return;

        f32 floatData = data->AsF32();

        if (!setparam)
            setparam = new ShaderFloatParameter(param, &floatData, 1);
        else
            reinterpret_cast<ShaderFloatParameter*>(setparam)->SetFloat(&floatData, 1);
    }

    void Shader::parseFloatArray_( ShaderSetParameter*& setparam, ShaderParameter& param, const FileDataSubNode* data )
    {
        if (addBoundConstant(param, data->AsString()))
            return;

        std::stringstream ss;
        ss << data->AsString();

        // extract all elements of the float array
        std::vector<f32> floatArray;
        std::copy(std::istream_iterator<f32>(ss), std::istream_iterator<f32>(),
            std::back_inserter(floatArray));

        if (!setparam)
            setparam = new ShaderFloatParameter(param, &floatArray[0], floatArray.size());
        else
            reinterpret_cast<ShaderFloatParameter*>(setparam)->SetFloat(&floatArray[0], floatArray.size());
    }

    void Shader::parseBool_( ShaderSetParameter*& setparam, ShaderParameter& param, const FileDataSubNode* data )
    {
        if (addBoundConstant(param, data->AsString()))
            return;

        bool boolData = data->AsBool();
        if (!setparam)
            setparam = new ShaderBoolParameter(param, &boolData, 1);
        else
            reinterpret_cast<ShaderBoolParameter*>(setparam)->SetBool(&boolData, 1);
    }

    void Shader::parseBoolArray_( ShaderSetParameter*& setparam, ShaderParameter& param, const FileDataSubNode* data )
    {
        if (addBoundConstant(param, data->AsString()))
            return;

        std::stringstream ss;
        ss << data->AsString();

        std::vector<s32> boolArray;
        std::copy(std::istream_iterator<bool>(ss), std::istream_iterator<bool>(),
            std::back_inserter(boolArray));

        if (!setparam)
            setparam = new ShaderBoolParameter(param, reinterpret_cast<bool*>(&boolArray[0]), boolArray.size());
        else
            reinterpret_cast<ShaderBoolParameter*>(setparam)->SetBool(reinterpret_cast<bool*>(&boolArray[0]), boolArray.size());
    }

    void Shader::parseMatrix_( ShaderSetParameter*& setparam, ShaderParameter& param, const FileDataSubNode* data )
    {
        // if a bound constant is found don't parse
        if (addBoundConstant(param, data->AsString()))
            return;

        std::stringstream ss;
        ss << data->AsString();

        std::vector<f32> floatArray;
        std::copy(std::istream_iterator<f32>(ss), std::istream_iterator<f32>(),
            std::back_inserter(floatArray));
        floatArray.resize(16, 0);

        Matrix matrixData(&floatArray[0]);
        if (!setparam)
            setparam = new ShaderMatrixParameter(param, &matrixData, 1);
        else
            reinterpret_cast<ShaderMatrixParameter*>(setparam)->SetMatrix(&matrixData, 1);
    }

    void Shader::parseMatrixArray_( ShaderSetParameter*& setparam, ShaderParameter& param, const FileDataSubNode* data )
    {
        if (addBoundConstant(param, data->AsString()))
            return;

        std::stringstream ss;
        ss << data->AsString();

        std::vector<f32> floatArray;
        std::copy(std::istream_iterator<f32>(ss), std::istream_iterator<f32>(),
            std::back_inserter(floatArray));
        // resize to next closest multiple of 16
        floatArray.resize(floatArray.size() + floatArray.size()%16, 0);

        if (!setparam)
            setparam = new ShaderMatrixParameter(param, reinterpret_cast<Matrix*>(&floatArray[0]), floatArray.size()/16);
        else
            reinterpret_cast<ShaderMatrixParameter*>(setparam)->SetMatrix(reinterpret_cast<Matrix*>(&floatArray[0]), floatArray.size()/16);
    }

    bool Shader::addBoundConstant(const ShaderParameter& param, const std::string& value )
    {
        std::map<std::string, void(Shader::*)(ShaderParameter&)>::const_iterator i = CONSTANT_BINDING_TABLE.find(value);
        
        if (i != CONSTANT_BINDING_TABLE.end())
        {
            boundConstants_.push_back(std::make_pair(param, i->second));
            return true;
        }
        
        return false;
    }

    void Shader::updateBoundConstants_()
    {
        for (u32 i = 0; i < boundConstants_.size(); ++i)
            (this->*boundConstants_[i].second)(boundConstants_[i].first);
    }

    void Shader::updateWorld_( ShaderParameter& param )
    {
        param.Set(device_->World());
    }

    void Shader::updateView_( ShaderParameter& param )
    {
        param.Set(device_->View());
    }

    void Shader::updateProj_( ShaderParameter& param )
    {
        param.Set(device_->Projection());
    }

    void Shader::updateWorldViewProj_( ShaderParameter& param )
    {
        Simian::Matrix wvp = device_->Projection() * device_->View() * device_->World();
        param.Set(wvp);
    }

#define UPDATE_TEXMATRIX_FUNC(id) \
    void Shader::updateTextureMatrix##id##_( ShaderParameter& param ) \
    { \
        updateTextureMatrix_(id, param); \
    }

    UPDATE_TEXMATRIX_FUNC(0)
    UPDATE_TEXMATRIX_FUNC(1)
    UPDATE_TEXMATRIX_FUNC(2)
    UPDATE_TEXMATRIX_FUNC(3)
    UPDATE_TEXMATRIX_FUNC(4)
    UPDATE_TEXMATRIX_FUNC(5)
    UPDATE_TEXMATRIX_FUNC(6)
    UPDATE_TEXMATRIX_FUNC(7)

#undef UPDATE_TEXMATRIX_FUNC

    void Shader::updateTextureMatrix_( u32 id, ShaderParameter& param )
    {
        param.Set(device_->TextureMatrix(id));
    }

    void Shader::updateTextureStateColorConstant_( ShaderParameter& param )
    {
        DWORD color = device_->TextureState(0, Texture::TSF_CONSTANT);
        u8 a = (color >> 24) & 0xFF;
        u8 r = (color >> 16) & 0xFF;
        u8 g = (color >> 8) & 0xFF;
        u8 b = color & 0xFF;
        param.Set(Simian::Vector4(r/255.0f, g/255.0f, b/255.0f, a/255.0f));
    }

    void Shader::updateTextureStateColorAdd_( ShaderParameter& param )
    {
        param.Set(device_->TextureState(0, Texture::TSF_COLOROP) == Texture::TSO_ADD);
    }

    void Shader::updateHSample_( ShaderParameter& param )
    {
        float hsample = 1.0f/device_->Size().X();
        param.Set(hsample);
    }

    void Shader::updateVSample_( ShaderParameter& param )
    {
        float vsample = 1.0f/device_->Size().Y();
        param.Set(vsample);
    }

    void Shader::updateTime_( ShaderParameter& param )
    {
        float time = OS::Instance().GetOSTicks();
        param.Set(time);
    }

    //--------------------------------------------------------------------------

    bool Shader::Load( const DataFileIO& document )
    {
        // get the root node
        FileObjectSubNode* root = document.Root();

        // get the vertex program
        FileObjectSubNode* vertexProgram = root->Object("VertexProgram");
        if (vertexProgram && !loadShader_(vertexProgram_,
                                          &GraphicsDevice::CreateVertexProgram, 
                                          vertexProgramParameters_,
                                          vertexProgram, "vs_1_1"))
        {
            vertexProgram_ = 0;
            SWarn("Vertex program failed to load.");
        }

        FileObjectSubNode* fragmentProgram = root->Object("FragmentProgram");
        if (fragmentProgram && !loadShader_(fragmentProgram_,
                                            &GraphicsDevice::CreateFragmentProgram,
                                            fragmentProgramParameters_,
                                            fragmentProgram, "ps_1_1"))
        {
            fragmentProgram_ = 0;
            SWarn("Fragment program failed to load.");
        }

        // succeeds when either vertex or fragment program loaded
        return vertexProgram_ || fragmentProgram_;
    }

    void Shader::Unload()
    {
        boundConstants_.clear();

        if (fragmentProgram_)
        {
            // unload shader parameters
            for (ProgramParameterSet::iterator i = fragmentProgramParameters_.begin();
                 i != fragmentProgramParameters_.end(); ++i)
                 delete i->second;
            fragmentProgramParameters_.clear();

            fragmentProgram_->Unload();
            device_->DestroyFragmentProgram(fragmentProgram_);
        }

        if (vertexProgram_)
        {
            // unload shader parameters
            for (ProgramParameterSet::iterator i = vertexProgramParameters_.begin();
                i != vertexProgramParameters_.end(); ++i)
                delete i->second;
            vertexProgramParameters_.clear();

            vertexProgram_->Unload();
            device_->DestroyVertexProgram(vertexProgram_);
        }
    }

    void Shader::LoadVertexProgramConstants( const FileArraySubNode& constants )
    {
        if (vertexProgram_)
            loadConstants_(vertexProgram_, vertexProgramParameters_, &constants);
    }

    void Shader::LoadFragmentProgramConstants( const FileArraySubNode& constants )
    {
        if (fragmentProgram_)
            loadConstants_(fragmentProgram_, vertexProgramParameters_, &constants);
    }

    void Shader::Set()
    {
        if (vertexProgram_)
        {
            // set vertex program constants
            for (ProgramParameterSet::iterator i = vertexProgramParameters_.begin();
                i != vertexProgramParameters_.end(); ++i)
                i->second->Set();
            vertexProgram_->Set();
        }
        if (fragmentProgram_)
        {
            // set fragment program constants
            for (ProgramParameterSet::iterator i = fragmentProgramParameters_.begin();
                i != fragmentProgramParameters_.end(); ++i)
                i->second->Set();
            fragmentProgram_->Set();
        }
    }

    void Shader::Update()
    {
        updateBoundConstants_();
    }

    void Shader::Unset()
    {
        if (fragmentProgram_)
            fragmentProgram_->Unset();
        if (vertexProgram_)
            vertexProgram_->Unset();
    }
}
