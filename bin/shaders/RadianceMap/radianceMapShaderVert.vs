#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in float illum;

out float vIllum;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

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
	vec4 eyeCoords = transformEye( position);
	vec4 clipCoords = transformClip( eyeCoords );
	vIllum = illum;
	gl_Position = clipCoords;
}