#ifdef VERTEX_SHADER

#include "common_vertex.h"

uniform ivec4 u_Viewport;

#ifdef DIFFUSEMAP
out vec2 oTexCoord;
#endif

#ifdef TINT_COLOR
out vec4 oTintColor;
#endif

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{

#ifdef DIFFUSEMAP
	oTexCoord  = uv.xy;
#endif
	
#ifdef TINT_COLOR	
	oTintColor  = color;
#endif

	vec2 inp = xyz.xy;
	
	inp.x = map(inp.x, 0, u_Viewport[2], -1, 1);
	inp.y = map(u_Viewport[3] - inp.y, 0, u_Viewport[3], -1, 1);
	
	gl_Position = vec4(inp,0,1);	
}

#endif


#ifdef FRAGMENT_SHADER

out vec4 FragColor;
  
#ifdef DIFFUSEMAP
uniform sampler2D diffuse;
in vec2 oTexCoord;
#endif 

#ifdef TINT_COLOR	
in vec4 oTintColor;
#endif

void main()
{
	FragColor = vec4(1,1,1,1);

#ifdef TINT_COLOR	
	FragColor = oTintColor;
#endif
			
	#ifdef DIFFUSEMAP
		vec4 diffuseSample = texture2D(diffuse,oTexCoord);
		FragColor *= diffuseSample;
	#endif
	
} 

#endif