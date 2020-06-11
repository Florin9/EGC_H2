#version 330

// TODO: get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture;
layout(location = 3) in vec3 v_color;


// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time;

// TODO: output values to fragment shader
out vec3 f_color;
out vec3 f_normal;


void main()
{
	// TODO: send output to fragment shader
	f_color = v_color;
	f_normal = v_normal * sin(Time);
	// TODO: compute gl_Position
	gl_Position = Projection * View * (Model *	mat4(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, sin(Time), 0, 1))* vec4(v_position, 1.0);
}
