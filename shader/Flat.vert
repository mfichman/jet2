/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

attribute vec3 position_in;
 
/* Very fast simple solid-color shader for rendering to depth */
void main() {
	mat4 transform = projection_matrix * view_matrix * model_matrix;

	// Transform the vertex to get the clip-space position of the vertex
	gl_Position = transform * vec4(position_in, 1);
}
