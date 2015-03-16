void main()
{
	vec4 offset = gl_ProjectionMatrix * vec4(gl_MultiTexCoord0.zw, 0.0, 1.0);
	offset.z = 0.0;
	offset.w = 0.0;
	
	// Transform the vertex position from object space to clip space
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex + offset;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	gl_FrontColor = gl_Color;
}