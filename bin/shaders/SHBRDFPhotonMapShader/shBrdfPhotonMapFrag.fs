#version 430

in vec4 gPosition;
in vec4 gLocalPosition;
flat in vec4 gColor;
flat in vec4 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;
vec3 globalPosition;
in vec3 radiance;
flat in vec3 basisX;
flat in vec3 basisZ;
flat in int faceIndex;
in vec3 eyeCoords;

void main()
{
	vec3 radianceBounce = radiance;
	vec3 color = pixelColor(gTextureCoord, gMaterialIndex);
	float s = 0;
	float ambient = 0.00;
	float direct = 1;
	float q = 0.0;
	for(int i=0; i< numLightMaps; i++)
	{
		vec4 lightCameraTransform = matrixMult(lightCameraMatrix[i],gLocalPosition);
		vec3 lightDirection = -normalize(vec3(lightCameraTransform));
		vec3 viewingDirection = -normalize(vec3(gPosition));
		s = direct*shadowed(gLocalPosition);
	}
	s = min(s + ambient,1.0);
	color = 1000.0*color*radianceBounce + 0.0*color*s;

	gl_FragColor = vec4(color,1.0);
}