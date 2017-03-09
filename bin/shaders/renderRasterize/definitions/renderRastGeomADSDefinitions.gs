void main()
{
	vectorReturn = vGlobalLightPosition[0];
	gPosition = vPosition[0];
	gColor = vColor[0];
	gNormal = vNormal[0];
	gMaterialIndex = vMaterialIndex[0];
	gTextureCoord = vTextureCoord[0];
	globalLightPosition = vGlobalLightPosition[0];
	globalPosition = vGlobalPosition[0];
	
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	
	
	vectorReturn = vGlobalLightPosition[1];
	gPosition = vPosition[1];
	gColor = vColor[1];
	gNormal = vNormal[1];
	gMaterialIndex = vMaterialIndex[1];
	gTextureCoord = vTextureCoord[1];
	globalLightPosition = vGlobalLightPosition[1];
	globalPosition = vGlobalPosition[1];
	
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	
	vectorReturn = vGlobalLightPosition[2];
	gPosition = vPosition[2];
	gColor = vColor[2];
	gNormal = vNormal[2];
	gMaterialIndex = vMaterialIndex[2];
	gTextureCoord = vTextureCoord[2];
	globalLightPosition = vGlobalLightPosition[2];
	globalPosition = vGlobalPosition[2];
	
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	
	EndPrimitive();
}