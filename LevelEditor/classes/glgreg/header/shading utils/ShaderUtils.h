#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

/**********************************************************************
* FILENAME :        ShaderUtils.cpp
*
* DESCRIPTION :
*       contains the Shader class and buffer objects.
*
* AUTHOR :    gsmith       START DATE :    1 Jan 13
*
**********************************************************************/


#include "GL\glew.h"
#include <stdio.h>
#include <cmath>
#include "data structures\MeshStructures.h"
#include "entities\ObjLoader.h"
#include <vector>
#include <string>
#include "openGL\Renderbuffer.h"
#include <lodepng.h>

#define SHADER_TYPE_RASTERIZE 0
#define SHADER_TYPE_RAY_TRACE 1
#define SHADER_TYPE_RADIOSITY 2
#define SHADER_TYPE_TREE_BOUNDARY_OUTLINE 3
#define SHADER_TYPE_ADS 4
#define SHADER_TYPE_ADS_BRDF 5
#define SHADER_TYPE_GLOBAL_ILLUMINATION_JACOBI 6
#define SHADER_TYPE_RADIANCE_CACHE_UNIFORM_GRID 7
#define SHADER_TYPE_RAIDANCE_MAP 8
#define SHADER_TYPE_MONTE_CARLO_BRDF 9
#define SHADER_TYPE_DEPTH 10
#define SHADER_TYPE_SHADOW_MAP 11
#define SHADER_TYPE_PHOTON_MAP 12
#define SHADER_TYPE_PROGRESSIVE_PHOTON_MAP 13
#define SHADER_TYPE_SVD_BRDF 14
#define SHADER_TYPE_SH_BRDF 15
#define SHADER_TYPE_WAVELET_ENVIRONMENT_MAP 16
#define SHADER_TYPE_FILTER_BLUR 17
#define SHADER_TYPE_SH_BRDF_PHOTON_MAP 18
#define SHADER_TYPE_RADIUS_SHADER 19
#define SHADER_TYPE_SHADOW_MAP_PROJECTION_TEXTURE 20
#define SHADER_TYPE_SUBSURFACE_SCATTERING_DIFFUSE 21
#define SHADER_TYPE_PHOTON_MAP_PROJECTION_TEXTURE 22
#define SHADER_TYPE_NORMAL_DEPTH_PROJECTION_TEXTURE 23
#define SHADER_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE 24
#define SHADER_TYPE_POSITION_PROJECTION_TEXTURE 25
#define SHADER_TYPE_SUBSURFACE_SCATTERING_DIFFUSE_PROJECTION_TEXTURE 26
#define SHADER_TYPE_REFLECTIVE_SHADOW_MAP 27
#define SHADER_TYPE_NORMAL_PROJECTION_TEXTURE 28
#define SHADER_TYPE_RADIANCE_SHADOW_MAP 29
#define SHADER_TYPE_INSTANT_RADIOSITY 30

#define BRDF_TORRANCE_SPARROW 0
#define BRDF_BEARD_MAXWELL 1
#define BRDF_COOK_TORRANCE 2
#define BRDF_KAJIYA 3
#define BRDF_POULIN_FOURNIER 4
#define BRDF_HE_TORRANCE_SILLION_GREENBERG 5
#define BRDF_OREN_NAYAR 6
#define BRDF_COUPLED 7
#define BRDF_ASHIKHMIN_SHIRLEY 8
#define BRDF_GRANIER_HEIDRICH 9
#define BRDF_MINNAERT 10
#define BRDF_PHONG 11
#define BRDF_BLINN 12
#define BRDF_LEWIS 13
#define BRDF_NEUMANN_NEUMANN 14
#define BRDF_STRAUSS 15
#define BRDF_WARD 16
#define BRDF_SCHLICK 17
#define BRDF_LAFORTUNE 18
#define BRDF_LAMBERTIAN 19
#define BRDF_FRESNEL 20
#define BRDF_PHBECKMANN 21

#define UNIFORM_FLOAT_1 0
#define UNIFORM_FLOAT_2 1
#define UNIFORM_FLOAT_3 2
#define UNIFORM_FLOAT_4 3

#define UNIFORM_INT_1 4
#define UNIFORM_INT_2 5
#define UNIFORM_INT_3 6
#define UNIFORM_INT_4 7

#define UNIFORM_MAT_2 8
#define UNIFORM_MAT_3 9
#define UNIFORM_MAT_4 10

struct BufferObject
{
	BufferObject();
	~BufferObject();
	GLuint bufferIndex;
	std::string bufferName;
};

extern std::vector<BufferObject*> bufferObjects;
BufferObject* searchBuffer(char* bufferName);
void addBuffer(char* bufferName, unsigned int bufferIndex);

struct UniformVBO;
struct TextureVBO;

struct UniformVBO
{
	UniformVBO();
	~UniformVBO();
	GLuint location;
	bool update;
	void* data;
	void (*uniformFunction)(UniformVBO* uvbo);
};

struct TextureVBO
{
	TextureVBO();
	~TextureVBO();
	GLuint location;
	GLint activeTexture;
	void* data;
	int width;
	int height;
	int internalFormat;
	int dataType;
};

struct TextureBuffersVBO
{
	TextureBuffersVBO();
	~TextureBuffersVBO();
	GLuint location;
	GLuint vbo;
	GLuint tex;
	GLuint activeTexture;
};
TextureBuffersVBO* textureBuffersVBO();

class Shader;

class IShader
{
public:

}; 

class Shader: public IShader
{
public:
	Shader(GLchar *vSource, GLchar *fSource, GLchar *gSource, unsigned int shaderType);
	Shader(Shader* s);
	Shader();
	virtual ~Shader();

	void addAttributes(GLchar** attrib, int attribSize);
	bool addTransformFeedbackAttributes(const GLchar** attribNames, int numAttribs);
	void linkProgram();

	virtual void render();
	void bindUniforms();
	
	bool addAttributeBuffer( void *ptr, int numDataPoints, int structSize, int dataType, GLchar* attribName, GLint attribLocation, GLuint attribSize);
	bool addUniform(void *ptr, GLchar* uniformName, int uniformType, bool updatable);
	bool addTexture(void *ptr, int width, int height, GLchar* texName, int dataType, int internalFormat, int activeTexture);
	bool addTexture(GLuint texIndex, GLchar* texName, int activeTexture);
	bool addTextureCopy(GLuint texIndex, GLchar* texName, int activeTexture);
	bool addBufferTexture(void *ptr, int numDataPoints, int structSize, GLchar* texName, int dataType, int activeTexture);
	bool addTextureArray(std::vector<Texture_obj<GLubyte>*>, int numTextures, GLchar* texName, int activeTexture);
	bool addTextureArray(std::vector<Texture_obj<GLfloat>*>, int numTextures, GLchar* texName, int activeTexture);
	bool addTextureArrayDepth(std::vector<Texture_obj<GLfloat>*> tex, int numTextures, GLchar* texName, int activeTexture);
	bool addTransfromFeedbackBuffers(int numBuffers);
	bool addUniformBlock( void *ptr, int numDataPoints, int structSize, GLchar* blockName, int blockBindingIndex);
	void bindTransformFeedback();
	void unbindTransformFeedback();
	void addUniformNoiseTexture(int width, int height, GLchar* textureName, int activeTexture);

	/*GETTERS AND SETTERS*/
	GLuint vertexShader();
	GLuint fragmentShader();
	GLuint geometryShader();

	GLuint program();
	
	void setTransformFeedback(bool val);

	void setPrimitiveType(int primType);
	int primitiveType();
	unsigned int getNumPrimitives();
	unsigned int getShaderType();
	void setShaderType(unsigned int _shaderType);
	GLuint currentActiveTexture();
	unsigned int shaderType;
protected:
	GLuint __vShader;
	GLuint __fShader;
	GLuint __gShader;
	
	GLuint __program;

	GLchar** __vss;
	GLchar** __fss;
	GLchar** __gss;

	unsigned int __vssLength;
	unsigned int __vssLines;
	unsigned int __fssLength;
	unsigned int __fssLines;
	unsigned int __gssLength;
	unsigned int __gssLines;

	GLchar** __attributes;
	GLint __numAttributes;

	int loadShader(GLchar *fileName, GLchar ***shaderSource, unsigned int *len, unsigned int *lines);
	int loadShaderCpp(GLchar *fileName, GLchar ***shaderSource, unsigned int *len, unsigned int *lines);
	void initBufferObjects();
	void convertVectorToArray(std::vector<std::string> sourceVector, GLchar*** sourceArray);

	GLint __vao;
	std::vector<GLuint*> __vbo;
	std::vector<UniformVBO*> __uniformVBO;
	std::vector<GLuint*> __uniformVBOBblock;
	std::vector<std::vector<Texture_obj<GLubyte>*> > __textures;
	std::vector<TextureBuffersVBO*> __textureBuffers;

	int __primitiveType;
	unsigned int __numPrimitives;

	GLuint createVAO();
	int getTextureColorFormat(int channelSize);

	GLchar** __feedbackNames;
	int __numFeebackNames;

	bool __transformFeedback;
	GLuint __transformFeedbackBuffer;
	GLuint __transformFeedbackVBO;
};

int getMaxTextureUnits();
bool* getActiveTextures();
void initTex();

void uniformFunction1f(UniformVBO* uvbo);
void uniformFunction2f(UniformVBO* uvbo);
void uniformFunction3f(UniformVBO* uvbo);
void uniformFunction4f(UniformVBO* uvbo);

void uniformFunction1i(UniformVBO* uvbo);
void uniformFunction2i(UniformVBO* uvbo);
void uniformFunction3i(UniformVBO* uvbo);
void uniformFunction4i(UniformVBO* uvbo);

void uniformFunction2mat(UniformVBO* uvbo);
void uniformFunction3mat(UniformVBO* uvbo);
void uniformFunction4mat(UniformVBO* uvbo);



#endif

