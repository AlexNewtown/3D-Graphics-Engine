#version 430

in float gIllum;

void main()
{

	gl_FragColor = vec4(vec3(1.0,1.0,1.0)*gIllum,1.0);

}