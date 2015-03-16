uniform sampler2D texture;

void main()
{
	vec4 colorMod = vec4(gl_Color.rgb * 2.0, gl_Color.a);
	
	gl_FragColor = texture2D(texture, gl_TexCoord[0].xy) * colorMod;
}
