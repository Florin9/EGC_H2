#version 330

// TODO: get values from fragment shader
in vec3 f_color;
in vec3 f_normal;


layout(location = 0) out vec4 out_color;

void main()
{
	// TODO: write pixel out color
	out_color = vec4(f_normal,1);
}