/* CrazyRussian: part of renderdefs */

/*
Trinity Rendering Engine - Copyright Andrew Lucas 2009-2012
Spirinity Rendering Engine - Copyright FranticDreamer 2020-2021

The Trinity Engine is free software, distributed in the hope th-
at it will be useful, but WITHOUT ANY WARRANTY; without even the 
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE. See the GNU Lesser General Public License for more det-
ails.

Renderer base definitions and functions
Written by Andrew Lucas, Richard Rohac, BUzer, Laurie, Botman and Id Software
*/

struct particle_system_t
{
	int id;
	int shapetype;
	int randomdir;

	Vector origin;
	Vector dir;

	float minvel;
	float maxvel;
	float maxofs;

	float skyheight;

	float spawntime;
	float fadeintime;
	float fadeoutdelay;
	float velocitydamp;
	float stuckdie;
	float tracerdist;

	float maxheight;

	float windx;
	float windy;
	float windvar;
	float windmult;
	float windmultvar;
	int windtype;

	float maxlife;
	float maxlifevar;
	float systemsize;

	Vector primarycolor;
	Vector secondarycolor;
	float transitiondelay;
	float transitiontime;
	float transitionvar;
	
	float rotationvar;
	float rotationvel;
	float rotationdamp;
	float rotationdampdelay;

	float rotxvar;
	float rotxvel;
	float rotxdamp;
	float rotxdampdelay;

	float rotyvar;
	float rotyvel;
	float rotydamp;
	float rotydampdelay;

	float scale;
	float scalevar;
	float scaledampdelay;
	float scaledampfactor;
	float veldampdelay;
	float gravity;
	float particlefreq;
	float impactdamp;
	float mainalpha;

	int startparticles;
	int maxparticles;
	int	maxparticlevar;

	int overbright;
	int lightcheck;
	int collision;
	int colwater;
	int displaytype;
	int rendermode;
	int numspawns;

	int fadedistfar;
	int fadedistnear;

	int numframes;
	int framesizex;
	int framesizey;
	int framerate;

	char create[64];
	char deathcreate[64];
	char watercreate[64];

	particle_system_t *createsystem;
	particle_system_t *watersystem;
	particle_system_t *parentsystem;

	cl_texture_t *texture;
	mleaf_t *leaf;

	particle_system_t	*next;
	particle_system_t	*prev;

	struct cl_particle_t *particleheader;

	byte pad[14];
};

struct cl_particle_t
{
	Vector velocity;
	Vector origin;
	Vector color;
	Vector scolor;
	Vector lastspawn;
	Vector normal;

	float spawntime;
	float life;
	float scale;
	float alpha;

	float fadeoutdelay;

	float scaledampdelay;
	float secondarydelay;
	float secondarytime;

	float rotationvel;
	float rotation;

	float rotx;
	float rotxvel;

	float roty;
	float rotyvel;

	float windxvel;
	float windyvel;
	float windmult;

	float texcoords[4][2];

	int frame;

	particle_system_t *pSystem;

	cl_particle_t	*next;
	cl_particle_t	*prev;

	byte pad[4];
};
