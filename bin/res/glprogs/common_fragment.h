// Common routines for scene objects rendering and selection

uniform int u_ObjectSerialNumber;

#ifdef USING_PARTID
flat in int oPartId;
#endif

layout(location = 0) out vec4 oFragColor;
layout(location = 1) out ivec2 oSelColor;

