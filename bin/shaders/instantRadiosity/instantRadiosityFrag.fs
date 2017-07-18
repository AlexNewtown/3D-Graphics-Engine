#version 430

in vec4 gPosition;
in vec4 gLocalPosition;
in vec4 gColor;
in vec4 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;
vec3 globalPosition;
flat in vec3 basisX;
flat in vec3 basisZ;
in vec4 entityNormal;

flat in vec2 texVert1;
flat in vec2 texVert2;
flat in vec2 texVert3;

flat in vec3 vert1;
flat in vec3 vert2;
flat in vec3 vert3;

vec3 computeBumpMapNormal();

layout(location = 0) out vec4 outColor;
layout(location = 1) out float fragDepth;

const int MAX_NUM_IR_LIGHTS = 100;
uniform int numIrLightMaps;
uniform vec4 irLightPosition[MAX_NUM_IR_LIGHTS];
uniform mat4 irCameraMatrix[MAX_NUM_IR_LIGHTS];
uniform vec3 irLightColor[MAX_NUM_IR_LIGHTS];
uniform sampler2DArray irDepthMap;
//x,y,z,power

void main()
{
	vec3 color;
	vec3 s = vec3(0.0,0.0,0.0);
	color = pixelColor(gTextureCoord, gMaterialIndex);
	vec3 ambient = 0.0*vec3(1.0,1.0,1.0);
	float direct = 1.0/float(numIrLightMaps + numLightMaps);

	vec3 n = computeBumpMapNormal();
	vec3 entPos = vec3(matrixMult(entityMatrix, vec4(vec3(gLocalPosition),1.0)));
	vec3 viewingDirection = -normalize(vec3(gPosition));

	for(int i=0; i< numIrLightMaps; i++)
	{
		vec3 lightDirection = normalize(vec3(irLightPosition[i]) - entPos);
		
		float dp = dot(lightDirection,vec3(entityNormal));
		float sh = shadowed(gLocalPosition,irCameraMatrix[i], irDepthMap, i);
		vec3 lColor = 10.0*irLightColor[i];
		sh = min(sh,1.0);
		sh = sh*dp;
		s = s + lColor*direct*sh;
	}

	for(int i=0; i< numLightMaps; i++)
	{
		vec3 lightDirection = normalize(vec3(lightPosition[i]) - entPos);

		float sh = shadowed(gLocalPosition,i);
		float dp = dot(lightDirection,vec3(entityNormal));
		float f = dp;
		
		sh = min(sh,1.0);
		s = s + f*sh*vec3(1.0,1.0,1.0);
	}

	s = min(s + ambient,vec3(1.0,1.0,1.0));
	s = max(vec3(0.0,0.0,0.0),s);
	color = color*s;
	color = color*dot(viewingDirection,n);
	outColor = vec4(color,1.0);
	fragDepth = gl_FragDepth;
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
}