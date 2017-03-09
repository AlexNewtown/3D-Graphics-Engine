float lightDecayRate = 600000.0;
void main()
{
	gPointLight = globalLightPosition;
	vec3 color = ambientColor() + diffuseColor() + specularColor();
	vec3 texLookup = vec3(texture(kdTexture,vec3(gTextureCoord.x,1 - gTextureCoord.y, float(gMaterialIndex))));
	float lightDistance = length(globalPosition - vec3(lPosition));
	float lightDecay = exp(-lightDistance*lightDistance/lightDecayRate);
	gl_FragColor = vec4(diffuseColor(),1.0);
}