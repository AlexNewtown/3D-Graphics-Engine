#include <fstream>
#include <iostream>
#include <stdio.h>
#include "shader\CollectiveShader.h"

using namespace std;

CollectiveShader::CollectiveShader():Shader()
{

}

void CollectiveShader::constructShader()
{
	appendSource(__vertSourceMain, __shaderLinesVert);
	appendSource(__geomSourceMain, __shaderLinesGeom);
	appendSource(__fragSourceMain, __shaderLinesFrag);

	if( __shaderLinesVert.size() <= 1  || __shaderLinesFrag.size() <= 1 || __shaderLinesGeom.size() <= 1)
	{
		if( __shaderLinesVert.size() <= 1)
		{
			OutputDebugStringA("===============Vertex file source is empty=================\n");
		}
		if( __shaderLinesFrag.size() <= 1)
		{
			OutputDebugStringA("===============fragment file source is empty=================\n");
		}
		if( __shaderLinesGeom.size() <= 1)
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

	convertVectorToArray(__shaderLinesVert, &__vss);
	convertVectorToArray(__shaderLinesGeom, &__gss);
	convertVectorToArray(__shaderLinesFrag, &__fss);

	__vssLines = __shaderLinesVert.size();
	__fssLines = __shaderLinesFrag.size();
	__gssLines = __shaderLinesGeom.size();

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

	GLint logLength;
	glGetShaderiv(__vShader,GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		GLchar* buildLog = new GLchar[logLength];
		glGetShaderInfoLog(__vShader, logLength, &logLength, buildLog);
		OutputDebugStringA(buildLog);
		delete[] buildLog;
	}

	glGetShaderiv(__gShader,GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		GLchar* buildLog = new GLchar[logLength];
		glGetShaderInfoLog(__gShader, logLength, &logLength, buildLog);
		OutputDebugStringA(buildLog);
		delete[] buildLog;
	}

	glGetShaderiv(__fShader,GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		GLchar* buildLog = new GLchar[logLength];
		glGetShaderInfoLog(__fShader, logLength, &logLength, buildLog);
		OutputDebugStringA(buildLog);
		delete[] buildLog;
	}

	__transformFeedback = false;
	initBufferObjects();
}
CollectiveShader::~CollectiveShader()
{
	while (__shaderLinesVert.size() > 0)
	{
		__shaderLinesVert.pop_back();
	}

	while (__shaderLinesGeom.size() > 0)
	{
		__shaderLinesGeom.pop_back();
	}

	while (__shaderLinesFrag.size() > 0)
	{
		__shaderLinesFrag.pop_back();
	}
}


void CollectiveShader::addDeclarations(GLchar* filePath, std::vector<std::string> &shaderLines)
{
	vector<string> lines_string;
	ifstream myFile;
	myFile.open(filePath);
	string current_line;

	while (getline(myFile, current_line))
	{
		shaderLines.push_back(current_line);
	}

	myFile.close();
}

void CollectiveShader::addDefinitions(GLchar* filePath, std::vector<std::string> &shaderLines)
{
	vector<string> lines_string;
	ifstream myFile;
	myFile.open(filePath);
	string current_line;

	while (getline(myFile, current_line))
	{
		shaderLines.push_back(current_line);
	}

	myFile.close();
}

void CollectiveShader::addMain(char* filePath, std::vector<std::string> &shaderLines)
{
	vector<string> lines_string;
	ifstream myFile;
	myFile.open(filePath);
	string current_line;

	std::vector<std::string> headerLines;
	
	while (getline(myFile, current_line))
	{
		if (current_line.size() == 0)
		{
			headerLines.push_back(current_line);
		}
		else
		{
			size_t start = current_line.find_first_not_of(" ", 0);
			size_t end = current_line.find_last_not_of(" ", 0);
			std::string line = current_line.substr(start, current_line.size() - end);
			if (line.compare("void main()") != 0)
			{
				headerLines.push_back(current_line);
			}
			else
			{
				break;
			}
		}

	}
	
	do
	{
		if (&shaderLines == &__shaderLinesVert)
		{
			__vertSourceMain.push_back(current_line);
		}
		else if (&shaderLines == &__shaderLinesGeom)
		{
			__geomSourceMain.push_back(current_line);
		}
		else if (&shaderLines == &__shaderLinesFrag)
		{
			__fragSourceMain.push_back(current_line);
		}
	} while ((getline(myFile, current_line)));
	

	addSourceToBeginning(headerLines, shaderLines);

	myFile.close();
}
void CollectiveShader::include(char* filePath, std::vector<std::string> &shaderLines)
{
	addDefinitions(filePath, shaderLines);
}

void CollectiveShader::addSourceToBeginning(std::vector<std::string> &source, std::vector<std::string> &target)
{
	for (int i = 0; i < target.size(); i++)
	{
		source.push_back(target[i]);
	}
	target = source;
}

void CollectiveShader::appendSource(std::vector<std::string> &source, std::vector<std::string> &target)
{
	for (int i = 0; i < source.size(); i++)
	{
		target.push_back(source[i]);
	}
}

void CollectiveShader::render()
{
	Shader::render();
}

void CollectiveShader::renderFlushPrimitive()
{
	glUseProgram(__program);

	bindUniforms();

	if (__vao > 0)
	{
		glBindVertexArray(__vao);
	}

	if (__transformFeedback)
	{
		bindTransformFeedback();
		glBeginTransformFeedback(__primitiveType);
	}
	for (int i = 0; i < __numPrimitives; i += 3)
	{
		glDrawArrays(__primitiveType, i, 3);
		glFinish();
	}

	if (__transformFeedback)
	{
		glEndTransformFeedback();
	}
}