void main()
{
	vMaterialIndex = int(round(materialIndex));
	
	vec4 eyeCoords = transformEye( position);
	vec4 clipCoords = transformClip( eyeCoords );
	
	vPosition = eyeCoords;
	vColor = color;
	vNormal = matrixMult(normalMatrix, normal);
	vTextureCoord = textureCoord;
	
	gl_Position = clipCoords;
}