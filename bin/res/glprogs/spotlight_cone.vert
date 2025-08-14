#version 330

layout (location = 0) in vec3 xyz; 
layout (location = 1) in vec3 normal; 
layout (location = 2) in vec3 tangent; 
layout (location = 3) in vec4 color; 
layout (location = 4) in vec3 uv; 
  
out vec4 vertexColor; // specify a color output to the fragment shader

uniform	vec4 colorOverride;
uniform vec3 scale;
uniform vec3 origin;
uniform mat4 view;
uniform mat4 transform;
uniform mat4 projection;

uniform float coneHeight;
uniform float coneAngle;

void main()
{
	vec3 vertTemp = xyz;
	
	vertTemp.z *= coneHeight;
	
	float coef = uv.x;
	
	float radius = 2 * tan(coneAngle) * coneHeight;  
	
	vertTemp.x *= coef * radius;
	vertTemp.y *= coef * radius;
	
    gl_Position = projection * (view * (transform * vec4(vertTemp * scale, 1.0)));	
    vertexColor = colorOverride;
}