#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include <common.h>
#include <gl_texture.h>
#include <mathlib.h>

typedef float vec3_t[3];

class Vector: public glm::vec3
{
public:
	Vector(float x, float y, float z) : glm::vec3(x,y,z)
	{

	}

	Vector(glm::vec3 a) : glm::vec3(a.x,a.y,a.z)
	{

	}

	Vector(const Vector & other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

	Vector() : glm::vec3(0,0,0)
	{
		
	}

	
	Vector &operator=(const glm::vec3 & a) 
	{
		this->x = a.x;
		this->y = a.y;
		this->z = a.z;

		return *this;
	}

	operator float *() { return &x; }

	inline float Length()
	{
		return length();
	}

	inline Vector Normalize()
	{
		return glm::normalize(*this);		
	}
};

typedef GLTexture* HSPRITE;

// Rendering constants
enum
{
	kRenderNormal,		// src
	kRenderTransColor,		// c*a+dest*(1-a)
	kRenderTransTexture,	// src*a+dest*(1-a)
	kRenderGlow,		// src*a+dest -- No Z buffer checks
	kRenderTransAlpha,		// src*srca+dest*(1-srca)
	kRenderTransAdd,		// src*a+dest
};



#ifndef WINDOWS
#define stricmp strcasecmp
#endif

#ifndef NULL
#define NULL nullptr
#endif

typedef void mleaf_t;

// contents of a spot in the world
#define	CONTENTS_EMPTY		-1
#define	CONTENTS_SOLID		-2
#define	CONTENTS_WATER		-3
#define	CONTENTS_SLIME		-4
#define	CONTENTS_LAVA		-5
#define	CONTENTS_SKY		-6
/* These additional contents constants are defined in bspfile.h
#define	CONTENTS_ORIGIN		-7		// removed at csg time
#define	CONTENTS_CLIP		-8		// changed to contents_solid
#define	CONTENTS_CURRENT_0		-9
#define	CONTENTS_CURRENT_90		-10
#define	CONTENTS_CURRENT_180	-11
#define	CONTENTS_CURRENT_270	-12
#define	CONTENTS_CURRENT_UP		-13
#define	CONTENTS_CURRENT_DOWN	-14

#define CONTENTS_TRANSLUCENT	-15
*/

#define	CONTENTS_CONVEYOR	-15

#define	CONTENTS_LADDER				-16

#define	CONTENT_FLYFIELD			-17
#define	CONTENT_GRAVITY_FLYFIELD	-18
//#define	CONTENT_FOG					-19
#define	CONTENT_FRICTIONMOD			-19
#define CONTENT_SPECIAL1			-20 //LRC - used by the particle systems
#define CONTENT_SPECIAL2			-21
#define CONTENT_SPECIAL3			-22
#define	CONTENT_FOG					-23

#define CONTENT_EMPTY	-1
#define CONTENT_SOLID	-2
#define	CONTENT_WATER	-3
#define CONTENT_SLIME	-4
#define CONTENT_LAVA	-5
#define CONTENT_SKY		-6

// euler angle order
#define PITCH 0
#define YAW   1
#define ROLL  2

typedef struct model_s
{
	char		name[64];		// model name
	int		numframes;	// sprite's framecount

	vec3_t mins,maxs;

} model_t;

#include "entity.h"
#include "enginefuncs.h"
#include "math.h"
#include "hud.h"