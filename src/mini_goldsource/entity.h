
#include "../color.h"

typedef bool qboolean;
typedef color24_t color24;

typedef struct entity_state_s
{
// Fields which are filled in by routines outside of delta compression
	int			entityType;
	// Index into cl_entities array for this entity.
	int			number;      
	float		msg_time;

	// Message number last time the player/entity state was updated.
	int			messagenum;		

	// Fields which can be transitted and reconstructed over the network stream
	Vector		origin;
	Vector		angles;

	int			modelindex;
	int			sequence;
	float		frame;
	int			colormap;
	short		skin;
	short		solid;
	int			effects;
	float		scale;

	byte		eflags;
	
	// Render information
	int			rendermode;
	int			renderamt;
	color24		rendercolor;
	int			renderfx;

	int			movetype;
	float		animtime;
	float		framerate;
	int			body;
	byte		controller[4];
	byte		blending[4];
	Vector		velocity;

	// Send bbox down to client for use during prediction.
	Vector		mins;
	Vector		maxs;

	int			aiment;
	// If owned by a player, the index of that player ( for projectiles ).
	int			owner; 

	// Friction, for prediction.
	float		friction;       
	// Gravity multiplier
	float		gravity;		

// PLAYER SPECIFIC
	int			team;
	int			playerclass;
	int			health;
	qboolean	spectator;  
	int         weaponmodel;
	int			gaitsequence;
	// If standing on conveyor, e.g.
	Vector		basevelocity;
	// Use the crouched hull, or the regular player hull.
	int			usehull;		
	// Latched buttons last time state updated.
	int			oldbuttons;     
	// -1 = in air, else pmove entity number
	int			onground;		
	int			iStepLeft;
	// How fast we are falling
	float		flFallVelocity;  

	float		fov;
	int			weaponanim;

	// Parametric movement overrides
	Vector				startpos;
	Vector				endpos;
	float				impacttime;
	float				starttime;

	// For mods
	int			iuser1;
	int			iuser2;
	int			iuser3;
	int			iuser4;
	float		fuser1;
	float		fuser2;
	float		fuser3;
	float		fuser4;
	Vector		vuser1;
	Vector		vuser2;
	Vector		vuser3;
	Vector		vuser4;
}entity_state_t;

typedef struct cl_entity_s
{
    int index;
    entity_state_t curstate;

	Vector origin;

	model_t * model;
}cl_entity_t;