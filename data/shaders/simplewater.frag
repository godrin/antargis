// minimal fragment shader
// www.lighthouse3d.com

uniform sampler2D tex;
varying vec3 lightDir,normal;

const vec3 diffuse=vec3(1.0,1.0,1.0);
const vec3 ambient=vec3(1.0,1.0,1.0);

void main()
{
	vec3 ct,cf;
	vec4 texel;
	float intensity,at,af;
	
	intensity = max(dot(lightDir,normalize(normal)),0.0);
	
	cf = intensity * diffuse + ambient;
	af = gl_FrontMaterial.diffuse.a;
	
	texel = texture2D(tex,gl_TexCoord[0].st);
	ct = texel.rgb;
	at = texel.a*0.7;
	
	gl_FragColor = vec4(ct * cf, at); // * af);
}

