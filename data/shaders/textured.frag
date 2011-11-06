// minimal fragment shader
// www.lighthouse3d.com

uniform sampler2D tex;
varying vec3 lightDir,normal;

void main()
{
	vec3 ct,cf;
	vec4 texel;
	float intensity,at,af;
	
	intensity = max(dot(lightDir,normalize(normal)),0.0);
	
	cf = intensity * (gl_FrontMaterial.diffuse).rgb + 
																		gl_FrontMaterial.ambient.rgb;
	af = gl_FrontMaterial.diffuse.a;
	
	texel = texture2D(tex,gl_TexCoord[0].st);
	ct = texel.rgb;
	at = texel.a*0.7;
	
	gl_FragColor = vec4(ct * cf, at * af);
}

