#ifndef PATHS_H
#define PATHS_H

#include <string>

static const std::string DATA_DIR("data\\");
static const std::string MESH_DIR(DATA_DIR + "meshes\\");
static const std::string SHADER_DIR(DATA_DIR + "shaders\\");
static const std::string PIXEL_SHADER_DIR(SHADER_DIR + "pixel\\");
static const std::string VERTEX_SHADER_DIR(SHADER_DIR + "vertex\\");

static const std::wstring DATA_DIR_W(L"data\\");
static const std::wstring MESH_DIR_W(DATA_DIR_W + L"meshes\\");
static const std::wstring SHADER_DIR_W(DATA_DIR_W + L"shaders\\");
static const std::wstring PIXEL_SHADER_DIR_W(SHADER_DIR_W + L"pixel\\");
static const std::wstring VERTEX_SHADER_DIR_W(SHADER_DIR_W + L"vertex\\");

#endif