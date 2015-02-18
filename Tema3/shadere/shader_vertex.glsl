#version 330

//3 variante: layout location pentru varianta 1 (Cea mai buna), layout location sau doar in/attribute pentru varianta 2, doar in pentru varianta 3.
//attribute e sintaxa veche (OpenGL 2) e recomandat sa folositi in

layout(location = 0) in vec3 in_position;		
layout(location = 1) in vec3 in_color;		

uniform mat4 model_matrix, view_matrix, projection_matrix;

uniform float timer;
uniform int dynamic;
out vec3 vertex_to_fragment_color;


void main(){
	if (dynamic == 1) {
		vertex_to_fragment_color = (1 - timer)*vec3(0,1,0) + timer*vec3(1,0,0);
		//gl_Position = projection_matrix*view_matrix*(3*timer*model_matrix)*vec4(in_position,1);
	}
	else {
		vertex_to_fragment_color = in_color; 
	}
	
		gl_Position = projection_matrix*view_matrix*model_matrix*vec4(in_position,1);
}
