#ifdef VERTEX_SHADER

#include "common_vertex.h"

void main()
{
	gl_Position = _transform();	
}

#endif


#ifdef FRAGMENT_SHADER

#include "common_fragment.h"

uniform vec4 u_Color;

void main()
{
	oFragColor = u_Color;
	oSelColor.x = u_ObjectSerialNumber;
	oSelColor.y = 0;
} 

#endif