attribute float bones; // cannot be int (some ati-bug ???)
uniform mat4 matrices[20];

varying vec3 lightDir,normal;


void main(void)
{
	vec4 v = vec4(gl_Vertex);

	int i=int(bones); // convert to int
	mat4 m=matrices[i];
	gl_Position = gl_ModelViewProjectionMatrix *m*v;
	//gl_Position = gl_ModelViewProjectionMatrix *v;

	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	lightDir = normalize(vec3(gl_LightSource[0].position));
					
	gl_TexCoord[0] = gl_MultiTexCoord0;
} 