
attribute float bones; // cannot be int (some ati-bug ???)
uniform mat4 matrices[20];

void main(void)
{
	vec4 v = vec4(gl_Vertex);

	int i=int(bones); // convert to int
	mat4 m=matrices[i];
	gl_Position = gl_ModelViewProjectionMatrix *m*v;
} 