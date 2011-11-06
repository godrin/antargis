// minimal vertex shader
// www.lighthouse3d.com


uniform float time;
uniform mat4 lightComplete;

varying vec4 projCoord;

void main(void)
{
	vec4 v = vec4(gl_Vertex);
	
	v.z = v.z + sin(3.0*v.x + 2.0*v.y + time*1.0)*0.05;
	
//	vec4 realPos = gl_ModelViewMatrix * v;
	gl_Position = gl_ModelViewProjectionMatrix * v;
					
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	
	//vec4 p=vec4(v);
	//p.x=0.0;//y*=100.0;
	//p.x*=10.0;
	//projCoord=p;
	
	//projCoord = lightComplete * p;
	projCoord = gl_TextureMatrix[1] * v;//gl_Vertex;
} 