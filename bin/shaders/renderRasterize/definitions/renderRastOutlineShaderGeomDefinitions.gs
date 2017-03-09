void main()
{
	gPosition = vPosition[0];
	gColor = vColor[0];
	
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	
	gPosition = vPosition[1];
	gColor = vColor[1];
	
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	
	EndPrimitive();
}