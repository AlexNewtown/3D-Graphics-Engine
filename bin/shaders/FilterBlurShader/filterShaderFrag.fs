#version 430

uniform int texWidth;
uniform int texHeight;
uniform sampler2D inputTex;
uniform samplerBuffer kernel;
uniform int kernelW;
uniform int kernelH;

vec3 filterImage(vec2 center);
vec3 sampleKernel(int index);

layout(location = 0) out vec4 outColor;
layout(location = 1) out float fragDepth;

void main()
{
	vec2 texPos = vec2(gl_FragCoord.x/float(texWidth),gl_FragCoord.y/float(texHeight));
	outColor = vec4(filterImage(texPos),1.0);
	fragDepth = gl_FragDepth;
}

vec3 sampleKernel(int index)
{
	vec4 fetch = texelFetch(kernel,index);
	return vec3(fetch);
}

vec3 filterImage(vec2 center)
{
	float sampleDeltaX = 1.0/float(texWidth);
	float sampleDeltaY = 1.0/float(texHeight);
	vec3 color = vec3(0.0,0.0,0.0);
	int filterIndex = 0;
	
	int filterHalfX = int(floor(kernelW/2));
	int filterHalfY = int(floor(kernelH/2));

	for(int j=-filterHalfX; j<=filterHalfX; j++)
	{
		for(int i=-filterHalfY; i<=filterHalfY; i++)
		{
			color = color + sampleKernel(filterIndex)*vec3(texture(inputTex,center + vec2(i*sampleDeltaX,j*sampleDeltaY)));
			filterIndex++;
		}
	}

	return color;
}