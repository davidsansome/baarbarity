void main()
{
	// TODO: Fix this
	vec4 lightVector = vec4(1.0, 0.5, 0.0, 0.0);
	
	// Transform the vertex position from object space to clip space
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	vec4 posEye = gl_ModelViewMatrix * gl_Vertex;
	
	// Transform things into view space
	vec3 normalViewSpace = normalize(gl_ModelViewMatrix * vec4(gl_Normal, 0.0)).xyz;
	vec3 lightViewSpace = normalize(gl_ModelViewMatrix * lightVector).xyz;
	
	// Calculate diffuse lighting
	float diffuse = 0.7 * max(dot(lightViewSpace, normalViewSpace), 0.0);
	float specular = 0.0;

	// Calculate fog
	float fog = 1.0 - smoothstep(gl_Fog.start, gl_Fog.end, gl_Position.z);

	// Output lighting values to the fragment shader
	gl_TexCoord[0] = vec4(diffuse, specular, fog, 0.0);
	
	gl_FrontColor = gl_Color;
	
	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
}