#include "shading utils\ShaderUtils.h"
#include "GL\glew.h"
#include "data structures\MeshStructures.h"
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <cstdlib>
#include <windows.h>
#include <debugapi.h>

using namespace std;

static int maxTextureUnits;
static bool* activeTextures = NULL;

std::vector<BufferObject*> bufferObjects;

int Shader::loadShaderCpp(GLchar *fileName, GLchar ***shaderSource, unsigned int *len, unsigned int *lines)
{
	ifstream myFile;
	myFile.open(fileName);
	
	string current_line;
	vector<string> lines_string;
	*len = 0;
	while (getline(myFile, current_line))
	{
		lines_string.push_back(current_line);
		*len = *len + current_line.length();
	}
	
	int numLines = lines_string.size();
	*lines = numLines;

	convertVectorToArray(lines_string, shaderSource);
	return 1;
}

int Shader::loadShader(GLchar *fileName, GLchar ***shaderSource, unsigned int *len, unsigned int *lines)
{
	FILE *file;
	file = fopen(fileName, "r");
	
	GLchar c;
	int addedChars = 0;
	int numLines = 0;
	int i=0;
	int charLength;

	fseek(file, 0, SEEK_END);
	charLength = ftell(file);
	GLchar *shader = new GLchar[charLength]; // <GLchar>(charLength);

	fseek(file, 0, SEEK_SET);
	while ( ( c = fgetc(file)) != EOF)
	{
		shader[i] = c;
		if( c == '\n')
			numLines++;

		else if( c == '#')
			addedChars = addedChars + 3;
		i++;
	}
	numLines++;
	charLength = i;
	*len = (unsigned int)charLength;
	charLength = charLength + addedChars + 1;
	shader = (GLchar*)realloc( shader, charLength);

	*shaderSource = new GLchar*[numLines];// newStruct<GLchar*>(numLines);

	int j = 0;
	bool replaceLineFeed = true;
	bool isPreprocessor = false;
	for( i =0; i <numLines; i++)
	{
		if (shader[j] == '#' )
			replaceLineFeed = false;

		(*shaderSource)[i] = &shader[j];
		while(shader[j] != '\n' && j <= *len )
		{
			j++;
		}
		if(!replaceLineFeed)
		{
			for(int k = *len; k > j+1; k--)
			{
				shader[k] = shader[k-1];
			}
			shader[j+1] = '\0';
			j = j+2;
			replaceLineFeed = true;
		}
		else
		{
			shader[j] = '\0';
			j++;
		}

	}
	shader[j] = '\0';
	
	*len =(unsigned int)charLength;
	*lines = (unsigned int)numLines;

	fclose(file);

	return 0;
}

void Shader::convertVectorToArray(std::vector<std::string> sourceVector, GLchar*** sourceArray)
{
	int numLines = sourceVector.size();
	*sourceArray = new GLchar*[numLines];
	for(int i=0; i<numLines; i++)
	{
		int lineLength = sourceVector[i].length();
		bool precompiler = false; 
		if(sourceVector[i].c_str()[0] == '#')
		{
			precompiler = true;
			(*sourceArray)[i] = new GLchar[lineLength+2];
		}
		else
		{
			(*sourceArray)[i] = new GLchar[lineLength+2];
		}

		int j=0;
		for(j=0; j< lineLength; j++)
		{
			(*sourceArray)[i][j] = (GLchar)sourceVector[i].c_str()[j];
		}
		if(precompiler)
		{
			(*sourceArray)[i][j] = '\n';
			j++;
		}
		else
		{
			(*sourceArray)[i][j] = '\n';
			j++;
		}

		(*sourceArray)[i][j] = '\0';
	}
}

void Shader::initBufferObjects()
{
	__vao = -1;
	__numPrimitives = 1;
	__primitiveType = GL_POINTS;
}

Shader::Shader(GLchar *vSource, GLchar *fSource, GLchar *gSource, unsigned int shaderType)
{
	if( vSource == NULL || fSource == NULL || gSource == NULL)
	{
		if( vSource == NULL)
		{
			OutputDebugStringA("===============Vertex file source is empty=================\n");
		}
		if( fSource == NULL)
		{
			OutputDebugStringA("===============fragment file source is empty=================\n");
		}
		if( gSource == NULL)
		{
			OutputDebugStringA("===============geometry file source is empty=================\n");
		}
		return;
	}

	__vShader = glCreateShader(GL_VERTEX_SHADER);
	__fShader = glCreateShader(GL_FRAGMENT_SHADER);
	__gShader = glCreateShader(GL_GEOMETRY_SHADER);

	if( __vShader < 0 || __fShader < 0 || __gShader < 0)
	{
		if( __vShader < 0)
		{
			OutputDebugStringA("===============Could not allocate vertex Shader=================\n");
		}
		if( __fShader < 0)
		{
			OutputDebugStringA("===============Could not allocate fragment Shader=================\n");
		}
		if( __gShader < 0)
		{
			OutputDebugStringA("===============Could not allocate geometry Shader=================\n");
		}
		return;
	}

	//__vss = new GLchar*;
	//__fss = new GLchar*;
	//__gss = new GLchar*;

	loadShaderCpp(vSource, &__vss, &__vssLength, &__vssLines);
	loadShaderCpp(fSource, &__fss, &__fssLength, &__fssLines);
	loadShaderCpp(gSource, &__gss, &__gssLength, &__gssLines);

	glShaderSource(__vShader, __vssLines, (const GLchar**)__vss, NULL);
	glShaderSource(__fShader, __fssLines, (const GLchar**)__fss, NULL);
	glShaderSource(__gShader, __gssLines, (const GLchar**)__gss, NULL);

	glCompileShader( __vShader );
	glCompileShader( __fShader );
	glCompileShader( __gShader );

	GLint result;
	glGetShaderiv( __vShader, GL_COMPILE_STATUS, &result );
	if( result == GL_FALSE )
	{
		//error
		OutputDebugStringA("================error compiling Vertex shader============================\n");
	}
	else if( result == GL_TRUE)
	{
		OutputDebugStringA("vertex shader compiled successfully\n");
	}

	glGetShaderiv( __fShader, GL_COMPILE_STATUS, &result );
	if( result == GL_FALSE )
	{
		//error
		OutputDebugStringA("================error compiling fragment shader============================\n");
	}
	else if( result == GL_TRUE)
	{
		OutputDebugStringA("fragment shader compiled successfully\n");
	}

	glGetShaderiv( __gShader, GL_COMPILE_STATUS, &result );
	if( result == GL_FALSE )
	{
		//error
		OutputDebugStringA("================error compiling geometry shader============================\n");
	}
	else if( result == GL_TRUE)
	{
		OutputDebugStringA("geometry shader compiled successfully\n");
	}

	this->shaderType = shaderType;
	__transformFeedback = false;
	initBufferObjects();
}

Shader::Shader()
{
}

Shader::~Shader()
{
	for (int i = 0; i < __vssLines; i++)
	{
		delete __vss[i];
	}

	for (int i = 0; i < __fssLines; i++)
	{
		delete __fss[i];
	}

	for (int i = 0; i < __gssLines; i++)
	{
		delete __gss[i];
	}

	delete __attributes;

	for (int i = 0; i < __numFeebackNames; i++)
	{
		delete __feedbackNames[i];
	}
	
	GLuint* b;
	while (__vbo.size() > 0)
	{
		b = __vbo[__vbo.size() - 1];
		glDeleteBuffers(1, b);
		__vbo.pop_back();
		delete b;
	}

	UniformVBO* u;
	while (__uniformVBO.size() > 0)
	{
		u = __uniformVBO[__uniformVBO.size() - 1];
		__uniformVBO.pop_back();
		delete u;
		/*do not delete, shared pointer*/
	}

	while (__uniformVBOBblock.size() > 0)
	{
		b = __uniformVBOBblock[__uniformVBOBblock.size() - 1];
		glDeleteBuffers(1, b);
		__uniformVBOBblock.pop_back();
		delete b;
		/*do not delete, shared pointer*/
	}
	
	for (int i = 0; i < __textures.size(); i++)
	{
		GLuint l = __textures[__textures.size() - 1][0]->bufferIndex;
		glDeleteTextures(1, &l);
	}
	Texture_obj<GLubyte>* tex;
	while (__textures.size() > 0)
	{
		while (__textures[__textures.size() - 1].size() > 0)
		{
			size_t pTextures = __textures.size();
			size_t cTextures = __textures[pTextures - 1].size();
			tex = __textures[pTextures-1][cTextures-1];
			activeTextures[tex->activeTexture] = false;
			//delete[] tex;
			__textures[pTextures - 1].pop_back();
		}
		__textures.pop_back();
	}

	TextureBuffersVBO* tvb;
	while(__textureBuffers.size() > 0)
	{
		tvb = __textureBuffers[__textureBuffers.size()-1];
		activeTextures[tvb->activeTexture] = false;
		delete tvb;
		__textureBuffers.pop_back();
	}

	glDetachShader(__program, __vShader);
	glDetachShader(__program, __fShader);
	glDetachShader(__program, __gShader);

	glDeleteShader(__vShader);
	glDeleteShader(__fShader);
	glDeleteShader(__gShader);
	glDeleteProgram(__program);
}

void Shader::addAttributes(GLchar** attrib, int attribSize)
{
	__attributes = attrib;
	__numAttributes = attribSize;

	__program = glCreateProgram();
	if (__program == 0)
	{
		printf("================could not create programHandle=======================\n");
	}

	glAttachShader(__program, __vShader);
	glAttachShader(__program, __fShader);
	glAttachShader(__program, __gShader);

	for(int i=0; i< attribSize; i++)
	{
		glBindAttribLocation(__program, i, attrib[i]);
	}

	glUseProgram(__program);
	glUseProgram(0);

}

void Shader::linkProgram()
{
	glLinkProgram(__program);
	GLint programStatus;
	glGetProgramiv(__program, GL_LINK_STATUS, &programStatus);
	char *log;
	if(programStatus == GL_FALSE)
	{
		OutputDebugStringA("================could not link program to shader=====================\n");
		int maxLength;
		int length;
		glGetProgramiv(__program,GL_INFO_LOG_LENGTH,&maxLength);
		log = new char[maxLength];
		glGetProgramInfoLog(__program, maxLength,&length,log);
		OutputDebugStringA(log);
		OutputDebugStringA("\n");
	}
	else if(programStatus == GL_TRUE)
	{
		OutputDebugStringA("program Linked successfuly\n");
	}
}

GLuint Shader::vertexShader()
{
	return __vShader;
}

GLuint Shader::fragmentShader()
{
	return __fShader;
}

GLuint Shader::geometryShader()
{
	return __gShader;
}

GLuint Shader::program()
{
	return __program;
}

void Shader::setPrimitiveType(int primType)
{
	__primitiveType = primType;
}

int Shader::primitiveType()
{
	return __primitiveType;
}

unsigned int Shader::getNumPrimitives()
{
	return __numPrimitives;
}

void Shader::render()
{
	glUseProgram(__program);

	bindUniforms();

	if( __vao > 0)
	{
		glBindVertexArray(__vao);
	}

	if (__transformFeedback)
	{
		bindTransformFeedback();
		glBeginTransformFeedback(__primitiveType);
	}
	glDrawArrays(__primitiveType,0,__numPrimitives);

	if (__transformFeedback)
	{
		glEndTransformFeedback();
	}
}


bool Shader::addAttributeBuffer( void *ptr, int numDataPoints, int structSize, int dataType, GLchar* attribName, GLint attribLocation, GLuint attribSize)
{
	if (ptr == NULL)
	{
		OutputDebugStringA("Cannot add an attribute, null data pointer\n");
		return false;
	}
	if (numDataPoints < 0)
	{
		OutputDebugStringA("Cannot add an attribute, no data points\n");
		return false;
	}
	if (attribName == NULL)
	{
		OutputDebugStringA("Cannot add an attribute, no attribute name\n");
		return false;
	}
	if (attribLocation < 0)
	{
		OutputDebugStringA("Cannot add an attribute, attribute location not specified\n");
		return false;
	}
	if (attribSize < 0)
	{
		OutputDebugStringA("Cannot add an attribute, attribute size not specified\n");
		return false;
	}
	if (__program < 0)
	{
		OutputDebugStringA("Cannot add an attribute, shader program does not exist\n");
		return false;
	}
	
	bool attribInShader = false;
	for(int i=0; i< __numAttributes; i++)
	{
		if( std::string(__attributes[i]).compare(attribName) == 0 && i == attribLocation)
		{
			attribInShader = true;
			break;
		}
	}
	if (!attribInShader)
	{
		OutputDebugStringA("Cannot add an attribute, attribute is not present in the shader\n");
		return false;
	}

	if(__vao < 0)
		__vao = createVAO();

	GLuint* vbo= new GLuint();
	
	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	int totalData = numDataPoints*structSize;
	glBufferData(GL_ARRAY_BUFFER, totalData, ptr, GL_STATIC_DRAW);

	glBindVertexArray(__vao);
	glEnableVertexAttribArray(attribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glVertexAttribPointer(attribLocation, attribSize, dataType, GL_FALSE, 0, (GLubyte*)NULL);

	__vbo.push_back(vbo);

	if(__numPrimitives < (int)numDataPoints/attribSize)
		__numPrimitives = ceil((float)numDataPoints/attribSize);


	return true;
}

GLuint Shader::createVAO()
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	return vao;
}

bool Shader::addUniform(void *ptr, GLchar* uniformName, int uniformType, bool updatable)
{
	UniformVBO* uvbo = new UniformVBO();
	glUseProgram(__program);
	GLuint uniformLocation = glGetUniformLocation(__program, uniformName);
	if(uniformLocation == -1) return false;
	
	uvbo->location = uniformLocation;
	uvbo->update = updatable;
	uvbo->data = ptr;

	switch(uniformType)
	{
	case UNIFORM_FLOAT_1:
		uvbo->uniformFunction = uniformFunction1f;
		break;
	case UNIFORM_FLOAT_2:
		uvbo->uniformFunction = uniformFunction2f;
		break;
	case UNIFORM_FLOAT_3:
		uvbo->uniformFunction = uniformFunction3f;
		break;
	case UNIFORM_FLOAT_4:
		uvbo->uniformFunction = uniformFunction4f;
		break;

	case UNIFORM_INT_1:
		uvbo->uniformFunction = uniformFunction1i;
		break;
	case UNIFORM_INT_2:
		uvbo->uniformFunction = uniformFunction2i;
		break;
	case UNIFORM_INT_3:
		uvbo->uniformFunction = uniformFunction3i;
		break;
	case UNIFORM_INT_4:
		uvbo->uniformFunction = uniformFunction4i;
		break;

	case UNIFORM_MAT_2:
		uvbo->uniformFunction = uniformFunction2mat;
		break;
	case UNIFORM_MAT_3:
		uvbo->uniformFunction = uniformFunction3mat;
		break;
	case UNIFORM_MAT_4:
		uvbo->uniformFunction = uniformFunction4mat;
		break;
	default:
		return false;
	}

	__uniformVBO.push_back(uvbo);
	uvbo->uniformFunction(uvbo);
	return true;
}

void Shader::bindUniforms()
{
	glUseProgram(__program);
	UniformVBO* currentUVBO;
	for(int i=0; i<__uniformVBO.size(); i++)
	{
		currentUVBO = __uniformVBO[i];
		if(currentUVBO->update)
		{
			currentUVBO->uniformFunction(currentUVBO);
		}
	}
}

bool Shader::addTexture(void *ptr, int width, int height, GLchar* texName, int dataType, int internalFormat, int activeTexture)
{
	if(ptr == NULL)
		return false;
	if( width < 0 || height < 0)
		return false;
	if(texName == NULL) 
		return false;

	glUseProgram(__program);
	GLuint texLocation = glGetUniformLocation(__program, texName);
	if (texLocation == -1)
		return false;

	if (activeTexture == -1)
		activeTexture = currentActiveTexture();
	/*
	TextureVBO* textureVBO = new TextureVBO();
	textureVBO->activeTexture = activeTexture;
	textureVBO->data = ptr;
	textureVBO->location = texLocation;
	textureVBO->width = width;
	textureVBO->height = height;
	textureVBO->internalFormat = internalFormat;
	textureVBO->dataType = dataType;
	*/
	Texture_obj<GLubyte>* to = new Texture_obj<GLubyte>();
	to->width = width;
	to->height = height;
	to->channelSize = dataType;
	to->buf = (GLubyte*)ptr;

	GLuint* tex = new GLuint();
	glGenTextures(1, tex);
	glUseProgram(__program);
	glActiveTexture(GL_TEXTURE0+activeTexture);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
	//glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, internalFormat, dataType, ptr);
	//glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, internalFormat, dataType, ptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	glUniform1i(texLocation, activeTexture);
	
	to->bufferIndex = *tex;
	to->activeTexture = activeTexture;
	std::vector<Texture_obj<GLubyte>*> t;
	t.push_back(to);
	__textures.push_back(t);

	return true;
}

bool Shader::addTextureCopy(GLuint texIndex, GLchar* texName, int activeTexture)
{
	int tWidth, tHeight;
	float* tempByte;
	glBindTexture(GL_TEXTURE_2D, texIndex);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);
	tempByte = new float[tWidth*tHeight * 4];
	GLubyte* texByte = new GLubyte[tWidth*tHeight * 4];

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, tempByte);
	Texture_obj<GLubyte>* t = new Texture_obj<GLubyte>();
	t->channelSize = 4;
	t->height = tHeight;
	t->width = tWidth;
	std::vector<Texture_obj<GLubyte>*> tVec;
	
	for (int i = 0; i < tWidth*tHeight * 4; i++)
	{
		texByte[i] = (GLubyte)round(255.0*tempByte[i]);
	}
	
	t->buf = texByte;
	tVec.push_back(t);

	if (!addTextureArray(tVec, 1, texName, activeTexture))
	{
		tVec.clear();
		return false;
	}
	else
	{
		return true;
	}
}

bool Shader::addTexture(GLuint texIndex, GLchar* texName, int activeTexture)
{
	if(texIndex == -1)
		return false;

	glUseProgram(__program);
	GLuint texLocation = glGetUniformLocation(__program, texName);
	if (texLocation == -1)
		return false;

	if (activeTexture == -1)
		activeTexture = currentActiveTexture();

	glActiveTexture(GL_TEXTURE0 + activeTexture);
	glBindTexture(GL_TEXTURE_2D, texIndex);

	glUniform1i(texLocation, activeTexture);

	Texture_obj<GLubyte>* to = new Texture_obj<GLubyte>();
	to->width = -1;
	to->height = -1;
	to->channelSize = -1;
	to->buf = NULL;
	to->activeTexture = activeTexture;
	to->bufferIndex = texIndex;

	std::vector<Texture_obj<GLubyte>*> t;
	t.push_back(to);
	__textures.push_back(t);

	return true;
}

bool Shader::addBufferTexture(void *ptr, int numDataPoints, int structSize, GLchar* texName, int dataType, int activeTexture)
{
	if(ptr == NULL)
		return false;
	if( numDataPoints < 0)
		return false;
	if(structSize < 0 )
		return false;
	if( texName == NULL)
		return false;

	glUseProgram(__program);

	GLint textureLocation = glGetUniformLocation(__program, texName);
	if(textureLocation == -1)
		return false;

	if (activeTexture == -1)
		activeTexture = currentActiveTexture();

	GLuint vbo;
	int totalBytes = numDataPoints*structSize;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_TEXTURE_BUFFER, vbo);
	glBufferData(GL_TEXTURE_BUFFER, totalBytes, ptr, GL_STATIC_DRAW);

	GLuint tex;
	glGenTextures(1, &tex);

	glActiveTexture(GL_TEXTURE0+activeTexture);

	glBindTexture(GL_TEXTURE_BUFFER, tex);

	glTexBuffer(GL_TEXTURE_BUFFER, dataType, vbo);

	glUniform1i(textureLocation, activeTexture);

	TextureBuffersVBO* tbvbo = new TextureBuffersVBO();
	tbvbo->location = textureLocation;
	tbvbo->vbo = vbo;
	tbvbo->tex = tex;
	tbvbo->activeTexture = activeTexture;
	__textureBuffers.push_back(tbvbo);
	return true;

}

bool Shader::addTextureArray(std::vector<Texture_obj<GLfloat>*> tex, int numTextures, GLchar* texName, int activeTexture)
{
	if (numTextures <= 0)
		return false;
	if (texName == NULL)
		return false;

	GLint textureLocation = glGetUniformLocation(__program, texName);
	if (textureLocation == -1)
		return false;

	if (activeTexture == -1)
		activeTexture = currentActiveTexture();

	glActiveTexture(GL_TEXTURE0 + activeTexture);

	GLuint texVBO;
	glUseProgram(__program);
	glGenTextures(1, &texVBO);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texVBO);

	GLuint maxWidth = 0;
	GLuint maxHeight = 0;
	for (int i = 0; i<numTextures; i++)
	{
		tex[i]->bufferIndex = texVBO;
		if (tex[i]->width > maxWidth)
			maxWidth = tex[i]->width;
		if (tex[i]->height > maxHeight)
			maxHeight = tex[i]->height;
	}

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F, maxWidth, maxHeight, numTextures);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i<numTextures; i++)
	{
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tex[i]->width, tex[i]->height, 1, GL_RGBA, GL_FLOAT, tex[i]->buf);
		tex[i]->activeTexture = activeTexture;
	}

	glUniform1i(textureLocation, activeTexture);

	std::vector<Texture_obj<GLfloat>*> t;
	for (int i = 0; i < numTextures; i++)
		t.push_back(tex[i]);


	return true;
	//__textures.push_back(t);
}

bool Shader::addTextureArrayDepth(std::vector<Texture_obj<GLfloat>*> tex, int numTextures, GLchar* texName, int activeTexture)
{
	if (numTextures <= 0)
		return false;
	if (texName == NULL)
		return false;

	GLint textureLocation = glGetUniformLocation(__program, texName);
	if (textureLocation == -1)
		return false;

	if (activeTexture == -1)
		activeTexture = currentActiveTexture();

	glActiveTexture(GL_TEXTURE0 + activeTexture);

	GLuint texVBO;
	glUseProgram(__program);
	glGenTextures(1, &texVBO);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texVBO);

	GLuint maxWidth = 0;
	GLuint maxHeight = 0;
	for (int i = 0; i<numTextures; i++)
	{
		tex[i]->bufferIndex = texVBO;
		if (tex[i]->width > maxWidth)
			maxWidth = tex[i]->width;
		if (tex[i]->height > maxHeight)
			maxHeight = tex[i]->height;
	}

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_R32F, maxWidth, maxHeight, numTextures);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i<numTextures; i++)
	{
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tex[i]->width, tex[i]->height, 1, GL_RED, GL_FLOAT, tex[i]->buf);
		tex[i]->activeTexture = activeTexture;
	}

	glUniform1i(textureLocation, activeTexture);

	std::vector<Texture_obj<GLfloat>*> t;
	for (int i = 0; i < numTextures; i++)
		t.push_back(tex[i]);


	return true;
	//__textures.push_back(t);
}

bool Shader::addTextureArray(std::vector<Texture_obj<GLubyte>*> tex, int numTextures, GLchar* texName, int activeTexture)
{
	if(numTextures <= 0)
		return false;
	if(texName == NULL)
		return false;

	GLint textureLocation = glGetUniformLocation(__program, texName);
	if(textureLocation == -1)
		return false;

	if (activeTexture == -1)
		activeTexture = currentActiveTexture();

	glActiveTexture(GL_TEXTURE0+activeTexture);
	
	GLuint texVBO;
	glUseProgram(__program);
	glGenTextures( 1, &texVBO);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texVBO);

	GLuint maxWidth = 0;
	GLuint maxHeight = 0;
	for(int i=0; i<numTextures; i++)
	{
		tex[i]->bufferIndex = texVBO;
		if(tex[i]->width > maxWidth)
			maxWidth = tex[i]->width;
		if(tex[i]->height > maxHeight)
			maxHeight = tex[i]->height;
	}

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, maxWidth, maxHeight, numTextures);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for(int i=0; i<numTextures; i++)
	{
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,0,0,i, tex[i]->width, tex[i]->height, 1, getTextureColorFormat(tex[i]->channelSize), GL_UNSIGNED_BYTE, tex[i]->buf);
		tex[i]->activeTexture = activeTexture;
	}

	glUniform1i(textureLocation, activeTexture);

	std::vector<Texture_obj<GLubyte>*> t;
	for (int i = 0; i < numTextures; i++)
		t.push_back(tex[i]);

	__textures.push_back(t);
}

bool Shader::addUniformBlock( void *ptr, int numDataPoints, int structSize, GLchar* blockName, int blockBindingIndex)
{
	if( ptr == NULL)
		return false;
	if( numDataPoints <= 0)
		return false;
	if( structSize <= 0)
		return false;
	if(blockName == "" || blockName == NULL)
		return false;
	if(blockBindingIndex < 0)
		return false;

	BufferObject* bo = searchBuffer(blockName);
	if (bo == NULL)
	{

		GLuint blockIndex = glGetUniformBlockIndex(__program, blockName);
		if (blockIndex < 0)
			return false;
		glUniformBlockBinding(__program, blockIndex, blockBindingIndex);

		GLuint* bufferVBO = new GLuint;
		glGenBuffers(1, bufferVBO);
		glBindBuffer(GL_UNIFORM_BUFFER, *bufferVBO);

		int totalNumBytes = numDataPoints*structSize;
		glBufferData(GL_UNIFORM_BUFFER, totalNumBytes, ptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, blockBindingIndex, *bufferVBO);

		__uniformVBOBblock.push_back(bufferVBO);

		addBuffer(blockName, *bufferVBO);
	}
	else
	{
		GLuint blockIndex = glGetUniformBlockIndex(__program, blockName);
		if (blockIndex < 0)
			return false;
		glUniformBlockBinding(__program, blockIndex, blockBindingIndex);

		GLuint vbo = bo->bufferIndex;
		glBindBuffer(GL_UNIFORM_BUFFER, vbo);
		glBindBufferBase(GL_UNIFORM_BUFFER, blockBindingIndex, vbo);
	}

	return true;
}

int Shader::getTextureColorFormat(int channelSize)
{
	int format;
	switch( channelSize )
	{
	case 1:
		format = GL_R;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		format = GL_RGBA;
		break;
	}
	return format;
}

bool Shader::addTransformFeedbackAttributes(const GLchar** attribNames, int numAttribs)
{
	if (attribNames == NULL)
		return false;
	if (numAttribs <= 0)
		return false;
	glTransformFeedbackVaryings(__program, numAttribs, attribNames, GL_SEPARATE_ATTRIBS);
	return true;
}

void Shader::setTransformFeedback(bool val)
{
	__transformFeedback = val;
}

bool Shader::addTransfromFeedbackBuffers(int numBuffers)
{
	glGenBuffers(1, &__transformFeedbackVBO);
	glBindBuffer(GL_ARRAY_BUFFER, __transformFeedbackVBO);
	glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_DYNAMIC_COPY);
	return true;
}

float area(float deltaPhi, float deltaTheta, float phiAvg)
{
	float anglePhiSin = abs(float(sin(phiAvg)));
	float area = anglePhiSin*deltaTheta*deltaPhi;
	return area;
}

void testMonteCarlo()
{
	// DELETE THIS LATER

	// rendering equation is
	// integral over surface area of a sphere of: Incident light*brdf*dot(incident,normal)*dincident

	srand(time(NULL));
	float randomNumber = float(rand())/(RAND_MAX+1);

	const int num_phi = 20;
	const int num_theta = 40;

	const float pi = 3.141592654;

	float last_phi = 0.0;
	float last_theta = 0.0;

	float phi;
	float theta;
	float sum = 0.0;
	float monteCarloSum = 0.0;
	float totalArea = 2*pi;
	for(int i=1; i<= num_phi; i++)
	{
		phi =  (float(i)/float(num_phi))*pi*0.5;
		last_theta = 0.0;
		for(int j=1; j<= num_theta; j++)
		{
			theta =  (float(j)/float(num_theta))*pi*2;
			float phiAvg = (phi + last_phi)*0.5;
			float dPhi = phi - last_phi;
			float dTheta = theta - last_theta;
			float a = area(dPhi, dTheta, phiAvg);
			last_theta = theta;
			sum = sum + a;

		}

		last_phi = phi;
	}

	const int num_mc = 5000;
	float uniform_p = 1.0/(pi*pi);
	for(int i=0; i< num_mc; i++)
	{
		float randomTheta = float(rand())/(RAND_MAX+1)*2*pi;
		float randomPhi = float(rand())/(RAND_MAX+1)*pi*0.5;
		monteCarloSum = monteCarloSum + sin(float(randomPhi))/uniform_p;
	}
	monteCarloSum = monteCarloSum/num_mc;
	float bb = 22.2;
}

void Shader::addUniformNoiseTexture(int width, int height, GLchar* textureName, int activeTexture)
{
	testMonteCarlo();
	GLfloat* tex = new GLfloat[width*height];
	int currentIndex = 0.0;
	srand(time(NULL));
	for(int i=0; i<width; i++)
	{
		for(int j=0; j<height; j++)
		{
			float randomNumber = float(rand())/(RAND_MAX+1);
			tex[currentIndex] = randomNumber;
			currentIndex = currentIndex + 1;
		}
	}
	addTexture((void*)tex, width, height, textureName, GL_FLOAT, GL_RED, -1);
	delete tex;
}

unsigned int Shader::getShaderType()
{
	return shaderType;
}

void Shader::setShaderType(unsigned int _shaderType)
{
	shaderType = _shaderType;
} 

void Shader::bindTransformFeedback()
{
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,__transformFeedbackVBO);
}
void Shader::unbindTransformFeedback()
{
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, __transformFeedbackBuffer);
}

GLuint Shader::currentActiveTexture()
{
	for (int i = 0; i < getMaxTextureUnits(); i++)
	{
		if (!activeTextures[i])
		{
			getActiveTextures()[i] = true;
			return i;
		}
	}
	return -1;
}

/* 
	new attribute buffer ( int numdatapoints, int dataPoints)
	- create the attribBuffer, set the dataSize, numPoints,
	- outside set the attribNumber
	- outside set the dataPointer,

	- createVAO, if not created,
	addAttribBuffer( vao, attribBuffer)
	- genBuffers()
	- bind bu 
*/

UniformVBO::UniformVBO()
{
	location = -1;
	update = false;
	uniformFunction = NULL;
	data = NULL;
}

UniformVBO::~UniformVBO()
{
	location = 0;
	update = false;
	data = NULL;
}

TextureVBO::~TextureVBO()
{
	location = 0;
	activeTexture = 0;
	width = 0;
	height = 0;
	internalFormat = 0;
	dataType = 0;
	data = NULL;
}

TextureVBO::TextureVBO()
{
	location = -1;
	activeTexture = -1;
	data = NULL;
}

BufferObject::BufferObject()
{
	bufferIndex = -1;
}

BufferObject::~BufferObject()
{
	bufferIndex = -1;
}

BufferObject* searchBuffer(char* bufferName)
{
	for (int i = 0; i < bufferObjects.size(); i++)
	{
		if (bufferObjects[i]->bufferName.compare(bufferName) == 0)
			return bufferObjects[i];
	}
	return NULL;
}
void addBuffer(char* bufferName, unsigned int bufferIndex)
{
	if (searchBuffer(bufferName) == NULL)
	{
		BufferObject* bo = new BufferObject();
		bo->bufferIndex = bufferIndex;
		bo->bufferName = bufferName;
		bufferObjects.push_back(bo);
	}
}

TextureBuffersVBO::TextureBuffersVBO()
{
	location = -1;
	vbo = -1;
	tex = -1;
	activeTexture = -1;
}
TextureBuffersVBO::~TextureBuffersVBO()
{

}

int getMaxTextureUnits()
{
	return maxTextureUnits;
}
bool* getActiveTextures()
{
	return activeTextures;
}
void initTex()
{
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	activeTextures = new bool[maxTextureUnits];
	for (int i = 0; i < maxTextureUnits; i++)
		activeTextures[i] = false;
}

void uniformFunction1f(UniformVBO* uvbo){glUniform1fv(uvbo->location,1,(const GLfloat*)uvbo->data);}
void uniformFunction2f(UniformVBO* uvbo){glUniform2fv(uvbo->location,1,(const GLfloat*)uvbo->data);}
void uniformFunction3f(UniformVBO* uvbo){glUniform3fv(uvbo->location,1,(const GLfloat*)uvbo->data);}
void uniformFunction4f(UniformVBO* uvbo){glUniform4fv(uvbo->location,1,(const GLfloat*)uvbo->data);}

void uniformFunction1i(UniformVBO* uvbo){glUniform1iv(uvbo->location,1,(const GLint*)uvbo->data);}
void uniformFunction2i(UniformVBO* uvbo){glUniform2iv(uvbo->location,1,(const GLint*)uvbo->data);}
void uniformFunction3i(UniformVBO* uvbo){glUniform3iv(uvbo->location,1,(const GLint*)uvbo->data);}
void uniformFunction4i(UniformVBO* uvbo){glUniform4iv(uvbo->location,1,(const GLint*)uvbo->data);}

void uniformFunction2mat(UniformVBO* uvbo){glUniformMatrix2fv(uvbo->location,1,false,(const GLfloat*)uvbo->data);}
void uniformFunction3mat(UniformVBO* uvbo){glUniformMatrix3fv(uvbo->location,1,false,(const GLfloat*)uvbo->data);}
void uniformFunction4mat(UniformVBO* uvbo){glUniformMatrix4fv(uvbo->location,1,false,(const GLfloat*)uvbo->data);}