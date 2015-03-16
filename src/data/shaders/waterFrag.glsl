uniform float t;

uniform sampler2D reflection;
uniform sampler2D refraction;
uniform sampler2D displacementMap;

uniform vec3 eyePos;

void main()
{
	// Constants
	float pi = 3.14159265;
	float pi_2 = 1.57079633;
	
	// Unpack input data
	vec3 cubeMapVector = gl_TexCoord[0].xyz;
	vec2 position = gl_TexCoord[1].xy;
	float height = gl_TexCoord[1].z;
	float fog = gl_TexCoord[1].w;
	float tSlow = t / 5.0;
	float depth = gl_TexCoord[2].z / 20.0;

	// Sample the displacement texture at various points
	vec2 basePos = position / 20.0;
	vec2 texPos1 = basePos + tSlow;
	vec2 texPos2 = basePos + 0.5 - tSlow;
	vec2 texPos3 = basePos + 0.2 + tSlow/2.0;
	vec3 wobble1 = texture2D(displacementMap, texPos1).rgb;
	vec3 wobble2 = texture2D(displacementMap, texPos2).rgb;
	vec3 wobble3 = texture2D(displacementMap, texPos3).rgb;
	vec3 wobble = (wobble1 + wobble2 + wobble3 - 1.5) / 70.0;
	wobble /= max(1.0, depth);

	// Calculate the alpha value of the water based on viewing angle and terrain height
	float alphaHeightMod = clamp(cos(clamp(height*pi*0.25 + pi_2, 0.0, pi_2)), 0.0, 1.0);
	float alphaAngleMod = clamp((normalize(cubeMapVector)+1.0).y*0.4 + 0.2, 0.0, 1.0);
	float wobbleMod = pow(alphaHeightMod, 0.4);
	
	wobble *= wobbleMod;

	// Sample the cube map and output the final colour
	vec2 reflectionCoord = gl_TexCoord[2].xy / 2.0 + 0.5 + wobble.xy;
	vec2 refractionCoord = gl_TexCoord[2].xy / 2.0 + 0.5 - wobble.xy;
	
	vec3 reflectionSample = texture2D(reflection, reflectionCoord).rgb;
	vec3 refractionSample = (1.0 - alphaHeightMod) * texture2D(refraction, refractionCoord).rgb + alphaHeightMod * vec3(0.22, 0.4, 0.6);
	
	vec3 color = alphaAngleMod * reflectionSample + (1.0 - alphaAngleMod) * refractionSample;
	gl_FragColor = vec4(color, 1.0);
}
