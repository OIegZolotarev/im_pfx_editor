#ifdef VERTEX_SHADER

#include "common_vertex.h"


#ifdef DIFFUSE
out vec2 oTexCoord;
#endif 

#ifdef LIGHTMAP
out vec2 oLMCoord;
#endif 

#ifdef FLATSHADED
out vec3 oNormal;
#endif

#ifdef USING_PARTID
flat out int oPartId;
#endif

#ifdef USING_BONES
uniform mat4 u_BonesTransform[128];
#endif

void main()
{

#ifdef DIFFUSE
	oTexCoord  = uv.xy;
#endif

#ifdef LIGHTMAP
	oLMCoord  = color.xy;
#endif

#ifdef FLATSHADED
	oNormal = normal;
#endif

#ifdef USING_BONES 
	vec4 pos = vec4(xyz,1) * u_BonesTransform[bones[0]];
	gl_Position = _transform(pos.xyz);	
#endif

#ifdef USING_PARTID
	oPartId = partId;
#endif

#ifdef SPRITE

	mat4 invMV = inverse(u_ModelViewMatrix);

	vec3 right = pullRightVector(invMV);
	vec3 up = pullUpVector(invMV);
					
	vec3 pos = xyz + (right * normal.x * u_Scale.x / 2.f) + (up * normal.y * u_Scale.y / 2.f);
	gl_Position = _transform(pos);	 
#endif 

#ifdef STATIC_GEOMETRY
	gl_Position = _transform();	
#endif 


}

#endif


#ifdef FRAGMENT_SHADER

#include "common_fragment.h"


// Color 2 used to distinct selected objects
uniform vec4 u_Color2;

#ifdef SOLID_COLOR
// Solid color (for point entities)
uniform vec4 u_Color;
#endif

#ifdef DIFFUSE
// Diffuse
uniform sampler2D u_Diffuse;
in vec2 oTexCoord;
#endif

#ifdef LIGHTMAP
// Lightmap
uniform sampler2D u_Lightmap;
in vec2 oLMCoord;
#endif

#ifdef FLATSHADED
// Normal color (for flatshaded)
in vec3 oNormal;
#endif

void main()
{		
	vec4 outColor = vec4(1.f,1.f,1.f,1.f);	

#ifdef DIFFUSE
	vec4 diffuseSample = texture2D(u_Diffuse,oTexCoord);	
	outColor = diffuseSample;				
#endif

#ifdef LIGHTMAP
	vec4 lmSample = texture2D(u_Lightmap,oLMCoord);
	outColor *= vec4(lmSample.rgb,1.f);	
#endif

#ifdef FLATSHADED
	outColor = (vec4(oNormal,1.f) + vec4(1.f,1.f,1.f,1.f)) * 0.5f;
#endif

#ifdef SOLID_COLOR
	outColor *= u_Color;
#endif

	oFragColor = outColor * u_Color2;
	
	oSelColor.x = u_ObjectSerialNumber;  
	
#ifdef USING_PARTID
	oSelColor.y = oPartId;  
#else
	oSelColor.y = 0;  
#endif
	//oSelColor = 0;  
	
	
} 



#endif