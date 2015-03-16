void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	gl_TexCoord[0] = vec4(gl_Position.z, 0.0, 0.0, 0.0);

	gl_FrontColor = gl_Color;
}