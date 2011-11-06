// minimal vertex shader
// www.lighthouse3d.com


uniform float time;

varying vec3 lightDir,normal;


void main(void)
{
	vec4 v = vec4(gl_Vertex);
	
	v.z = v.z + sin(3.0*v.x + 2.0*v.y + time*0.3)*0.05;
	gl_Position = gl_ModelViewProjectionMatrix * v;

	normal = normalize(gl_NormalMatrix * gl_Normal);
	lightDir = normalize(vec3(gl_LightSource[0].position));
					
	gl_TexCoord[0] = gl_MultiTexCoord0;
} 