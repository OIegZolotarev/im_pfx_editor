#ifdef VERTEX_SHADER

#include "common_vertex.h"

out vec4 oColor;

void main()
{
	gl_Position = _transform();	
	oColor = color;
}

#endif


#ifdef FRAGMENT_SHADER

#include "common_fragment.h"

in vec4 oColor;

void main()
{
	oFragColor = oColor;
	oSelColor = u_ObjectSerialNumber;
} 

#endif