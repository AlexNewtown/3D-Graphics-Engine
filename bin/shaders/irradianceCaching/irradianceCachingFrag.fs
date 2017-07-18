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

flat in vec2 texVert1;
flat in vec2 texVert2;
flat in vec2 texVert3;

flat in vec3 vert1;
flat in vec3 vert2;
flat in vec3 vert3;

in vec3 irradiance;

const int inputSize = 12;
const int nnLayerSize1 = 8;
const int nnLayerSize2 = 8;
const int nnOutputSize = 3;
uniform samplerBuffer nnWeights;
uniform samplerBuffer nnBiases;

float getWeightLayer1(int inputIndex, int layer1Index);
float getWeightLayer2(int layer1Index, int layer2Index);
float getWeightLayerOutput(int layer2Index, int layerOutputIndex);

float getBiasLayer1(int layer1Index);
float getBiasLayer2(int layer2Index);
float getBiasLayerOutput(int layerOutputIndex);

vec3 computeBumpMapNormal();
vec3 nnShading(vec3 globalPosition, vec3 outDirection, vec3 globalNormal, vec3 lightPosition);

layout(location = 0) out vec4 outColor;
layout(location = 1) out float fragDepth;

void main()
{
	vec3 color;
	vec3 s = vec3(0.0,0.0,0.0);
	color = pixelColor(gTextureCoord, gMaterialIndex);
	vec3 ambient = 0.0*vec3(1.0,1.0,1.0);
	float direct = 1.0/float(numLightMaps);
	float ss = 0.0;
	float q = 0.0;
	vec3 n = computeBumpMapNormal();

	for(int i=0; i< numLightMaps; i++)
	{
		vec4 lightCameraTransform = matrixMult(lightCameraMatrix[i],gLocalPosition);	
		vec3 lightDirection = -normalize(vec3(lightCameraTransform));
		vec3 viewingDirection = -normalize(vec3(gPosition));
		
		float f = sampleBRDF(lightDirection, viewingDirection, n, gMaterialIndex, gTextureCoord);
		s = s + f*direct*shadowed(gLocalPosition);
	}
	s = min(s + ambient,vec3(1.0,1.0,1.0));
	s = max(vec3(0.0,0.0,0.0),s);
	color = color*s;

	vec3 outDirection = vec3(gPosition);
	outDirection = normalize(outDirection);
	vec3 indirect = nnShading(vec3(gPosition), outDirection, vec3(gNormal), vec3(0.0, 27.0, 30.0));

	outColor = vec4(indirect,1.0);
	fragDepth = gl_FragDepth;
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
}

vec3 nnShading(vec3 globalPosition, vec3 outDirection, vec3 globalNormal, vec3 lightPosition)
{
	float inputLayer[inputSize];
	float layer1[nnLayerSize1];
	float layer2[nnLayerSize2];
	float layerOutput[nnOutputSize];

	inputLayer[0] = globalPosition.x;
	inputLayer[1] = globalPosition.y;
	inputLayer[2] = globalPosition.z;
	inputLayer[3] = outDirection.x;
	inputLayer[4] = outDirection.y;
	inputLayer[5] = outDirection.z;
	inputLayer[6] = globalNormal.x;
	inputLayer[7] = globalNormal.y;
	inputLayer[8] = globalNormal.z;
	inputLayer[9] = lightPosition.x;
	inputLayer[10] = lightPosition.y;
	inputLayer[11] = lightPosition.z;

	for(int layer1Index = 0; layer1Index < nnLayerSize1; layer1Index++)
	{
		layer1[layer1Index] = 0.0;
		for(int inputIndex = 0; inputIndex < inputSize; inputIndex++)
		{
			layer1[layer1Index] = layer1[layer1Index] + inputLayer[inputIndex]*getWeightLayer1(inputIndex,layer1Index);
		}
		layer1[layer1Index] + layer1[layer1Index] + getBiasLayer1(layer1Index);
	}

	for(int layer2Index = 0; layer2Index < nnLayerSize2; layer2Index++)
	{
		layer2[layer2Index] = 0.0;
		for(int layer1Index = 0; layer1Index < nnLayerSize1; layer1Index++)
		{
			layer2[layer2Index] = layer2[layer2Index] + layer1[layer1Index]*getWeightLayer2(layer1Index,layer2Index);
		}
		layer2[layer2Index] = layer2[layer2Index] + getBiasLayer2(layer2Index);
	}


	for(int layerOutputIndex = 0; layerOutputIndex < nnOutputSize; layerOutputIndex++)
	{
		layerOutput[layerOutputIndex] = 0.0;
		for(int layer2Index = 0; layer2Index < nnLayerSize2; layer2Index++)
		{
			layerOutput[layerOutputIndex] = layerOutput[layerOutputIndex] + layer2[layer2Index]*getWeightLayerOutput(layer2Index,layerOutputIndex);
		}
		layerOutput[layerOutputIndex] = layerOutput[layerOutputIndex] + getBiasLayerOutput(layerOutputIndex);
	}

	return vec3(layerOutput[0], layerOutput[1], layerOutput[2]);
}


float getWeightLayer1(int inputIndex, int layer1Index)
{
	int i = inputIndex*nnLayerSize1 + layer1Index;
	vec4 fetch = texelFetch(nnWeights,i);
	return fetch.x;
}

float getBiasLayer1(int layer1Index)
{
	vec4 fetch = texelFetch(nnBiases, layer1Index);
	return fetch.x;
}

float getWeightLayer2(int layer1Index, int layer2Index)
{
	int i = layer1Index*nnLayerSize2 + layer2Index;
	i = i + nnLayerSize1*inputSize;
	vec4 fetch = texelFetch(nnWeights,i);
	return fetch.x;
}

float getBiasLayer2(int layer2Index)
{
	vec4 fetch = texelFetch(nnBiases, layer2Index + inputSize + nnLayerSize1);
	return fetch.x;
}

float getWeightLayerOutput(int layer2Index, int layerOutputIndex)
{
	int i = layer2Index*nnOutputSize + layerOutputIndex;
	i = i + nnLayerSize1*inputSize + nnLayerSize2*nnLayerSize1;
	vec4 fetch = texelFetch(nnWeights,i);
	return fetch.x;
}

float getBiasLayerOutput(int layerOutputIndex)
{
	vec4 fetch = texelFetch(nnBiases, layerOutputIndex + inputSize + nnLayerSize1 + nnLayerSize2);
	return fetch.x;
}