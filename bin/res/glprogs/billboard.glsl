#ifdef VERTEX_SHADER

#include "common_vertex.h"

uniform vec4 u_Color;

#ifndef SELECTION
out vec2 oTexCoord;
#endif
out vec4 oVertexColor;

void main()
{
	oVertexColor = u_Color;
	
	#ifndef SELECTION
	oTexCoord  = uv.xy;
	#endif

	mat4 invMV = inverse(u_ModelViewMatrix);

	vec3 right = pullRightVector(invMV);
	vec3 up = pullUpVector(invMV);
					
	vec3 pos = xyz + (right * normal.x * u_Scale.x / 2.f) + (up * normal.y * u_Scale.y / 2.f);
	gl_Position = _transform(pos);	    
}

#endif

#ifdef FRAGMENT_SHADER

#include "common_fragment.h"

#ifndef SELECTION
uniform sampler2D u_Diffuse;
in vec2 oTexCoord;
#endif
in vec4 oVertexColor;

void main()
{
#ifndef SELECTION
	vec4 diffuseSample = texture2D(u_Diffuse,oTexCoord);	 
	oFragColor = diffuseSample * oVertexColor;	     
#else
	oFragColor = oVertexColor;	     
#endif


	oSelColor.x = u_ObjectSerialNumber;
	oSelColor.y = 0;

} 


#endif