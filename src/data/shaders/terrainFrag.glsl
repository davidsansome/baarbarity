uniform sampler2D rockTex;
uniform sampler2D grassTex;

void main()
{
	// Get values from textures
	vec4 rockSample = texture2D(rockTex, gl_TexCoord[0].xy);
	vec4 grassSample = texture2D(grassTex, gl_TexCoord[0].xy);
	float grassAlpha = gl_TexCoord[0].z;
	vec3 rockColor = rockSample.rgb;
	vec3 grassColor = grassSample.rgb;
	float rockSpec = rockSample.a;
	float grassSpec = grassSample.a;

	// Merge the textures to create an overall texture color
	vec3 textureColor = rockColor*(1.0-grassAlpha) + grassColor*grassAlpha;
	float textureSpec = rockSpec*(1.0-grassAlpha) + grassSpec*grassAlpha;

	// Calculate lighting
	float ambient = 0.2;
	float diffuse = gl_TexCoord[1].x;
	float specular = gl_TexCoord[1].y * textureSpec;
	float lighting = clamp(ambient + diffuse + specular, 0.0, 1.0);

	// Take fog into account
	float fog = gl_TexCoord[1].z;
	
	gl_FragColor = vec4(textureColor * lighting, fog);
}
