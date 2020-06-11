#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for light properties
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;


// Output value to fragment shader
out vec3 color;

void main()
{
	
	color = object_color*vec3(v_texture_coord,1);
	//color = v_color;

	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
