/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#ifndef PM_TRACE_H
#define PM_TRACE_H


#define PM_NORMAL		0x00000000
#define PM_STUDIO_IGNORE	0x00000001	// Skip studio models
#define PM_STUDIO_BOX	0x00000002	// Use boxes for non-complex studio models (even in traceline)
#define PM_GLASS_IGNORE	0x00000004	// Ignore entities with non-normal rendermode
#define PM_WORLD_ONLY	0x00000008	// Only trace against the world
#define PM_CUSTOM_IGNORE	0x00000010	// Ignore entities with SOLID_CUSTOM mode

// Values for flags parameter of PM_TraceLine
#define PM_TRACELINE_PHYSENTSONLY	0
#define PM_TRACELINE_ANYVISIBLE	1

typedef struct
{
	vec3_t	normal;
	float	dist;
} pmplane_t;

typedef struct pmtrace_s pmtrace_t;

struct pmtrace_s
{
	qboolean	allsolid;		// if true, plane is not valid
	qboolean	startsolid;	// if true, the initial point was in a solid area
	qboolean	inopen, inwater;  // End point is in empty space or in water
	float	fraction;		// time completed, 1.0 = didn't hit anything
	vec3_t	endpos;		// final position
	pmplane_t	plane;		// surface normal at impact
	int	ent;		// entity at impact
	vec3_t	deltavelocity;	// Change in player's velocity caused by impact.
				// Only run on server.
	int	hitgroup;
};

#endif//PM_TRACE_H