#ifdef VERTEX_SHADER

#include "common_vertex.h"

out vec4 pixelCoord;

void main()
{
    gl_Position = _transform();	
    pixelCoord = vec4(xyz,1);
}

#endif

#ifdef FRAGMENT_SHADER

out vec4 FragColor;
in vec4 pixelCoord; 

uniform	int u_GridStep;
uniform	int u_GridHiglightCustom;

uniform	vec4    u_GridAxisColor;
uniform	vec4    u_GridMainColor;
uniform	vec4    u_Grid64thLineColor;
uniform	vec4    u_Grid1024thLineColor;
uniform	vec4    u_GridCustomColor;

bool isNthLine1Dim(int step, int width, float coord)
{
    int intCoord = int(coord);

    float cs = coord / float(step);

    int adjCoord1 = int(cs) * step;
    int adjCoord2 = int(cs + 1)  * step;

    int d1 = abs(intCoord - adjCoord1);
    int d2 = abs(intCoord - adjCoord2);
    int dist = min(d1,d2);

    return dist <= (width / 2);
}

bool isNthLine(int step)
{
    int w = 2;
    return isNthLine1Dim(step, w, pixelCoord.x)  || isNthLine1Dim(step, w, pixelCoord.y);    
}



vec4 calcLineColor()
{
    if (isNthLine(0))
        return vec4(u_GridAxisColor.rgb,1);

        
    if (isNthLine(1024))                    return u_Grid1024thLineColor;
    if (isNthLine(64))                      return u_Grid64thLineColor;
    if (isNthLine(u_GridHiglightCustom) && u_GridHiglightCustom != 0)      return u_GridCustomColor;
    
    return vec4(u_GridMainColor.rgb, 0);
}

layout(location = 0) out vec4 oFragColor;
layout(location = 1) out int oSelColor;

void main()
{    
    
    oFragColor = calcLineColor();
    oFragColor.a = 0.f;


    if (isNthLine(u_GridStep))
        oFragColor.a = 1.f;
								
								oSelColor = 0;
      
} 

#endif