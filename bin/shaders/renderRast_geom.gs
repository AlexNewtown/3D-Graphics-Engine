#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 vPosition[];
in vec4 vColor[];
in vec4 vNormal[];
in int vMaterialIndex[];
in vec2 vTextureCoord[];

out vec4 gPosition;
out vec4 gColor;
out vec4 gNormal;
flat out int gMaterialIndex;
out vec2 gTextureCoord;
out vec3 vectorReturn;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

vec4 matrixMult(mat4 m, vec4 v);
vec4 transformEye( vec4 coord);
vec4 transformClip( vec4 coord);

vec4 matrixMult(mat4 m, vec4 v)
{
	vec4 trans;
	trans.x = m[0].x*v.x + m[0].y*v.y + m[0].z*v.z + m[0].w*v.w;
	trans.y = m[1].x*v.x + m[1].y*v.y + m[1].z*v.z + m[1].w*v.w;
	trans.z = m[2].x*v.x + m[2].y*v.y + m[2].z*v.z + m[2].w*v.w;
	trans.w = m[3].x*v.x + m[3].y*v.y + m[3].z*v.z + m[3].w*v.w;
	return trans;
}

vec4 transformEye( vec4 coord)
{
	vec4 ec;
	ec = matrixMult(modelViewMatrix,coord);
	return ec;
}

vec4 transformClip( vec4 coord)
{
	vec4 cc;
	cc = matrixMult(projectionMatrix, coord);
	cc.x = cc.x/cc.w;
	cc.y = cc.y/cc.w;
	cc.z = cc.z/cc.w;
	cc.w = 1.0;
	return cc;
}

void main()
{
	vectorReturn = vec3(gl_in[0].gl_Position);
	gPosition = vPosition[0];
	gColor = vColor[0];
	gNormal = vNormal[0];
	gMaterialIndex = vMaterialIndex[0];
	gTextureCoord = vTextureCoord[0];
	
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	
	
	vectorReturn = vec3(gl_in[1].gl_Position);
	gPosition = vPosition[1];
	gColor = vColor[1];
	gNormal = vNormal[1];
	gMaterialIndex = vMaterialIndex[1];
	gTextureCoord = vTextureCoord[1];
	
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	
	vectorReturn = vec3(gl_in[2].gl_Position);
	gPosition = vPosition[2];
	gColor = vColor[2];
	gNormal = vNormal[2];
	gMaterialIndex = vMaterialIndex[2];
	gTextureCoord = vTextureCoord[2];
	
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	
	EndPrimitive();
}