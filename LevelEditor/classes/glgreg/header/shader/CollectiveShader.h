#ifndef COLLECTIVE_SHADER_H
#define COLLECTIVE_SHADER_H

#include "../shading utils/ShaderUtils.h"
#include <stdio.h>
#include <vector>

#define SHADER_MATRIX_UTILS "../bin/shaders/utilities/matrixUtils.txt"
#define SHADER_MATERIAL_UTILS "../bin/shaders/utilities/materialUtils.txt"
#define SHADER_BRDF_UTILS "../bin/shaders/utilities/brdfUtils.txt"
#define SHADER_GEOMETRY_UTILS "../bin/shaders/utilities/geometryUtils.txt"
#define SHADER_RADIANCE_UTILS "../bin/shaders/utilities/radianceUtils.txt"
#define SHADER_RANDOM_UTILS "../bin/shaders/utilities/randomUtils.txt"
#define SHADER_BOUNDARY_UTILS "../bin/shaders/utilities/boundaryUtils.txt"
#define SHADER_KDTREE_UTILS "../bin/shaders/utilities/kdTreeUtils.txt"
#define SHADER_RADIANCE_CACHE_UTILS "../bin/shaders/utilities/radianceCacheUtils.txt"
#define SHADER_LIGHT_UTILS "../bin/shaders/utilities/lightUtils.txt"
#define SHADER_SHADOW_MAP_UTILS "../bin/shaders/utilities/shadowMapUtils.txt"
#define SHADER_SVD_UTILS "../bin/shaders/utilities/SVDBrdfUtils.txt"
#define SHADER_SH_UTILS "../bin/shaders/utilities/shUtils.txt"
#define SHADER_ENVIRONMENT_MAP_UTILS "../bin/shaders/utilities/environmentMapUtils.txt"
#define SHADER_FACE_HIERARCHY_UTILS "../bin/shaders/utilities/faceHierarchyUtils.txt"
#define SHADER_PHOTON_MAP_UTILS "../bin/shaders/utilities/photonMapUtils.txt"
#define SHADER_POINT_LIGHT_TEXTURE_UTILS "../bin/shaders/utilities/pointLightTextureUtils.txt"
#define SHADER_BVH_UTILS "../bin/shaders/utilities/bvhUtils.txt"

class CollectiveShader : public Shader
{
public:
	CollectiveShader();
	virtual ~CollectiveShader();
	virtual void render();

protected:
	void constructShader();
	void addDeclarations(GLchar* filePath, std::vector<std::string> &shaderLines);
	void addDefinitions(GLchar* filePath, std::vector<std::string> &shaderLines);
	void renderFlushPrimitive();

	void addMain(char* filePath, std::vector<std::string> &shaderLines);
	void include(char* filePath, std::vector<std::string> &shaderLines);

	std::vector<std::string> __shaderLinesVert;
	std::vector<std::string> __shaderLinesGeom;
	std::vector<std::string> __shaderLinesFrag;

private:
	void addSourceToBeginning(std::vector<std::string> &source, std::vector<std::string> &target);
	void appendSource(std::vector<std::string> &source, std::vector<std::string> &target);
	std::vector<std::string> __vertSourceMain;
	std::vector<std::string> __geomSourceMain;
	std::vector<std::string> __fragSourceMain;
};

#endif