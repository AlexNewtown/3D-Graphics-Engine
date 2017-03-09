#version 430

in vec4 gPosition;
in vec4 gColor;
in vec4 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;

uniform isamplerBuffer materialTexInt;
uniform int materialTexIntSize;
uniform samplerBuffer materialTexFloat;
uniform int materialTexFloatSize;

uniform sampler2DArray kdTexture;

layout (std140) uniform lightData
{
	vec4 lightPosition;
	vec4 lightDirection;
	vec4 lightDiffuseColor;
	vec4 lightSpecularColor;
};

layout (std140) uniform boundingBox
{
	vec4 boundsMin;
	vec4 boundsMax;
};

vec3 getMaterialKa(int matIndex);
vec3 getMaterialKd(int matIndex);
vec3 getMaterialKs(int matIndex);
vec3 getMaterialTf(int matIndex);
float getMaterialNi(int matIndex);
float getMaterialNs(int matIndex);
float getMaterialD(int matIndex);
float getMaterialSharpness(int matIndex);
int getMaterialIllum(int matIndex);

vec3 halfwayVector();
float diffuse();

vec3 getMaterialKa(int matIndex)
{
	int texIndex = matIndex*materialTexFloatSize;
	vec4 fetch = texelFetch(materialTexFloat,texIndex);
	return vec3(fetch);
}
float getMaterialNi(int matIndex)
{
	int texIndex = matIndex*materialTexFloatSize;
	vec4 fetch = texelFetch(materialTexFloat,texIndex);
	return fetch.w;
}

vec3 getMaterialKd(int matIndex)
{
	int texIndex = matIndex*materialTexFloatSize;
	vec4 fetch = texelFetch(materialTexFloat,texIndex + 1);
	return vec3(fetch);
}
float getMaterialNs(int matIndex)
{
	int texIndex = matIndex*materialTexFloatSize;
	vec4 fetch = texelFetch(materialTexFloat,texIndex + 1);
	return fetch.w;
}

vec3 getMaterialKs(int matIndex)
{
	int texIndex = matIndex*materialTexFloatSize;
	vec4 fetch = texelFetch(materialTexFloat,texIndex + 2);
	return vec3(fetch);
}
float getMaterialD(int matIndex)
{
	int texIndex = matIndex*materialTexFloatSize;
	vec4 fetch = texelFetch(materialTexFloat,texIndex + 2);
	return fetch.w;
}

vec3 getMaterialTf(int matIndex)
{
	int texIndex = matIndex*materialTexFloatSize;
	vec4 fetch = texelFetch(materialTexFloat,texIndex + 3);
	return vec3(fetch);
}
float getMaterialSharpness(int matIndex)
{
	int texIndex = matIndex*materialTexFloatSize;
	vec4 fetch = texelFetch(materialTexFloat,texIndex + 3);
	return fetch.w;
}
int getMaterialIllum(int matIndex)
{
	int texIndex = matIndex*materialTexIntSize;
	ivec4 fetch = texelFetch(materialTexInt,texIndex);
	return fetch.y;
}

vec3 halfwayVector()
{
	vec3 num = -vec3(lightDirection) - vec3(gPosition);
	float magnitude = num.x*num.x + num.y*num.y + num.z*num.z;
	magnitude = sqrt(magnitude);
	vec3 h = num/magnitude;
	return h;
}

float diffuse()
{
	float d = dot(-vec3(lightDirection),vec3(gNormal));
	return d;
}

float specular()
{
	vec3 h = halfwayVector();
	float s = dot(h,vec3(gNormal));
	int illum = getMaterialIllum(gMaterialIndex);
	s = pow(s,illum);
	return s;
}

vec3 specularColor()
{
	float s = specular();
	vec3 sc = getMaterialKs(gMaterialIndex);
	sc = sc*s;
	return sc;
}

vec3 ambientColor()
{
	return getMaterialKa(gMaterialIndex);
}

vec3 diffuseColor()
{
	vec3 dc = getMaterialKd(gMaterialIndex);
	dc = dc*diffuse();
	return dc;
}

vec3 pixelColor()
{
	/*
	if there is no obstructon between a pixel's normal and the light source
	{
		color = ambientColor() + diffuseColor() + specularColor();
	}
	else
	{
		color = ambientColor();
	}
	*/
	
	vec3 color;
	vec3 lightColor = ambientColor() + diffuseColor() + specularColor();
	vec4 texLookup = texture(kdTexture,vec3(gTextureCoord.x,1 - gTextureCoord.y, float(gMaterialIndex)));
	
	return lightColor*vec3(texLookup);
}

void main()
{
	
	vec4 fetch = texelFetch(materialTexFloat,int(5));
	vec3 bb = getMaterialKa(1);

	vec4 texLookup = texture(kdTexture,vec3(gTextureCoord.x,1 - gTextureCoord.y, float(gMaterialIndex)));
	
	gl_FragColor = vec4(pixelColor(),1.0);

}