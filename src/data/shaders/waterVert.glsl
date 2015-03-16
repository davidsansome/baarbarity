uniform vec3 eyePos;

void main()
{
	// Unpack coords
	vec3 objectPos = vec3(gl_Vertex.x, -0.01, gl_Vertex.y);

	// Calculate vector from eye to point.  Used in fragment shader
	vec4 objectEyeSpace = gl_ModelViewMatrix * vec4(objectPos, 1.0);
	vec3 cubeMapVector = objectPos - eyePos;
	
	gl_TexCoord[0] = vec4(cubeMapVector, 0.0);

	// Transform the current vertex position from object space to clip space
	gl_Position = gl_ModelViewProjectionMatrix * vec4(objectPos, 1.0);

	// Calculate fog
	float fog = 1.0 - smoothstep(gl_Fog.start, gl_Fog.end, gl_Position.z);

	// Provide additional data to fragment shader
	gl_TexCoord[1] = vec4(gl_Vertex.xyz, fog);
	gl_TexCoord[2] = vec4(gl_Position.xy / gl_Position.w, gl_Position.z, 0.0);
}