#ifdef VERTEX_SHADER

#include "common_vertex.h"

uniform mat4 u_BonesTransform[128];

out vec2 oTexCoord;


void main()
{
	vec4 pos = vec4(xyz,1) * u_BonesTransform[bones[0]];
	
	oTexCoord  = uv.xy;
	gl_Position = _transform(pos.xyz);	
}

#endif


#ifdef FRAGMENT_SHADER

#include "common_fragment.h"

in vec2 oTexCoord;
uniform sampler2D u_Diffuse;

void main()
{
	oFragColor =  texture2D(u_Diffuse,oTexCoord);	
	oSelColor.x = u_ObjectSerialNumber;
	oSelColor.y = 0;
} 

#endif