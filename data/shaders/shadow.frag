// minimal fragment shader
// www.lighthouse3d.com

uniform sampler2D tex;
uniform sampler2DShadow shadowMap;

varying vec4 projCoord; 
const vec4 ambient = vec4(0.13), boost = vec4(1.06);

void main()
{
/*
	vec4 v=vec4(0,0,0,1);
	vec4 shadowValue = shadow2DProj(shadowMap, projCoord);
	
	//vec4 tex=texture2DProj(
	
	gl_FragColor=shadowValue+ ambient;
	*/
	
	float tex1Color = shadow2DProj(shadowMap, projCoord).z;
 gl_FragColor.r = tex1Color;
 gl_FragColor.g = tex1Color;
 gl_FragColor.b = tex1Color;
 gl_FragColor.a = 1.0;
	
}

