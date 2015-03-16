void main()
{
	// Calculate fog
	float fog = 1.0 - smoothstep(gl_Fog.start, gl_Fog.end, gl_TexCoord[0].x);
	
	gl_FragColor = vec4(gl_Color.rgb, fog);
}
