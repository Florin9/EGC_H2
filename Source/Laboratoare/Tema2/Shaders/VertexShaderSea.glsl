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
uniform float time;

uniform vec3 object_color;


// Output value to fragment shader
out vec3 color;
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
void main()
{
	
	//color = object_color*vec3(v_texture_coord,1);
	color = object_color * v_color*vec3(1,v_position[1],1);
	vec3 new_pos = v_position;
	//new_pos[0] = sin(time + v_position[0]);
	//new_pos[1] = sin(time + v_position[1]);
	//if(int(v_position[2]) % 2 == 0){

	new_pos[2] = v_position[2] + 0.002* sin(time + v_position[1])+ 0.1*rand(vec2(v_position[0],v_position[1]));
	//new_pos[1] = v_position[1] + 0.2* sin(time + v_position[1]);

	//new_pos[1] = sin(time + v_position[1]);
	//}

	gl_Position = Projection * View * Model * vec4(new_pos, 1.0);
}
