void main()
{
	vMaterialIndex = int(round(materialIndex));
	
	vec4 eyeCoords = transformEye( position);
	vec4 clipCoords = transformClip( eyeCoords );
	
	vPosition = position;
	vColor = color;
	vNormal = matrixMult(normalMatrix, normal);
	vTextureCoord = textureCoord;

/*
	vGlobalLightPosition = vec3(transformEye(vec4(vec3(lPosition),1.0)));

*/
	vGlobalLightPosition = vec3(lPosition);
	vGlobalPosition = vec3(position);
	gl_Position = clipCoords;
}