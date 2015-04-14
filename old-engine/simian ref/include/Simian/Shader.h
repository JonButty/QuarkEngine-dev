/************************************************************************/
/*!
\file		Shader.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SHADER_H_
#define SIMIAN_SHADER_H_

#include "SimianPlatform.h"
#include "ShaderParameter.h"

#include <string>
#include <vector>
#include <map>
#include <utility>

namespace Simian
{
    class VertexProgram;
    class FragmentProgram;
    class DataFileIO;
    class DataLocation;
    class FileObjectSubNode;
    class FileArraySubNode;
    class FileDataSubNode;
    class ShaderSetParameter;
    
    class SIMIAN_EXPORT Shader
    {
    private:
        typedef std::map<std::string, ShaderSetParameter*> ProgramParameterSet;
    private:
        VertexProgram* vertexProgram_;
        FragmentProgram* fragmentProgram_;

        ProgramParameterSet vertexProgramParameters_;
        ProgramParameterSet fragmentProgramParameters_;
        std::vector<std::pair<ShaderParameter, void(Shader::*)(ShaderParameter&)> > boundConstants_;

        GraphicsDevice* device_;
    public:
        ShaderParameter VertexProgramConstant(const std::string& name);
        ShaderParameter FragmentProgramConstant(const std::string& name);

        template <typename T>
        void VertexProgramConstant(const std::string& name, T*& param)
        {
            shaderSetConstant_(vertexProgramParameters_, name, param);
        }

        template <typename T>
        void FragmentProgramConstant(const std::string& name, T*& param)
        {
            shaderSetConstant_(fragmentProgramParameters_, name, param);
        }
    private:
        template <typename T>
        bool loadShader_(T*& shaderProgram, bool(GraphicsDevice::*method)(T*&), 
                         ProgramParameterSet& parameterSet_,
                         const FileObjectSubNode* data, 
                         const std::string& defaultProfile);
        template <typename T>
        void loadConstants_(T*& shaderProgram, ProgramParameterSet& parameterSet_,
                            const FileArraySubNode* constants);

        // constant parsing functions
        void parseFloat_(ShaderSetParameter*& setParam, ShaderParameter& param, const FileDataSubNode* data);
        void parseFloatArray_(ShaderSetParameter*& setParam, ShaderParameter& param, const FileDataSubNode* data);
        void parseBool_(ShaderSetParameter*& setParam, ShaderParameter& param, const FileDataSubNode* data);
        void parseBoolArray_(ShaderSetParameter*& setParam, ShaderParameter& param, const FileDataSubNode* data);
        void parseMatrix_(ShaderSetParameter*& setParam, ShaderParameter& param, const FileDataSubNode* data);
        void parseMatrixArray_(ShaderSetParameter*& setParam, ShaderParameter& param, const FileDataSubNode* data);

        // parse for bound constants
        bool addBoundConstant(const ShaderParameter& param, const std::string& value);

        // bound constants
        void updateBoundConstants_();

        // bound constant update functions
        void updateWorld_(ShaderParameter& param);
        void updateView_(ShaderParameter& param);
        void updateProj_(ShaderParameter& param);
        void updateWorldViewProj_(ShaderParameter& param);
        void updateTextureMatrix0_(ShaderParameter& param);
        void updateTextureMatrix1_(ShaderParameter& param);
        void updateTextureMatrix2_(ShaderParameter& param);
        void updateTextureMatrix3_(ShaderParameter& param);
        void updateTextureMatrix4_(ShaderParameter& param);
        void updateTextureMatrix5_(ShaderParameter& param);
        void updateTextureMatrix6_(ShaderParameter& param);
        void updateTextureMatrix7_(ShaderParameter& param);
        void updateTextureMatrix_(u32 id, ShaderParameter& param);
        void updateTextureStateColorConstant_(ShaderParameter& param);
        void updateTextureStateColorAdd_(ShaderParameter& param);
        void updateHSample_(ShaderParameter& param);
        void updateVSample_(ShaderParameter& param);
        void updateTime_(ShaderParameter& param);

        template <typename T>
        void shaderSetConstant_(ProgramParameterSet& set, const std::string& name, T*& param)
        {
            ProgramParameterSet::iterator i = set.find(name);
            param = i != set.end() ? reinterpret_cast<T*>(i->second) : 0;
        }
    public:
        Shader(GraphicsDevice* device);

        template <typename T>
        bool Load(const DataLocation& data)
        {
            T reader;
            if (!reader.Read(data))
                return false;
            return Load(reader);
        }

        bool Load(const DataFileIO& document);
        void Unload();

        void LoadVertexProgramConstants(const FileArraySubNode& constants);
        void LoadFragmentProgramConstants(const FileArraySubNode& constants);

        void Set();
        void Update();
        void Unset();

        friend static std::map<std::string, void(Shader::*)(ShaderParameter&)> GenerateBindingTable();
    };
}

#endif
