void main()
{
	// Calculate lighting
	float ambient = 0.5;
	float diffuse = gl_TexCoord[0].x;
	float lighting = clamp(ambient + diffuse, 0.0, 1.0);

	// Take fog into account
	float fog = gl_TexCoord[0].z;
	
	gl_FragColor = vec4(gl_Color.rgb * lighting, fog);
}
