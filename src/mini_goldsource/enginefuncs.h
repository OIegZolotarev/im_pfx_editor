#include "cvardef.h"
#include "pm_trace.h"

#include "event_api.h"


typedef enum
{
	TRI_FRONT = 0,
	TRI_NONE = 1,
} TRICULLSTYLE;

typedef struct triangleapi_s
{
	int	version;

	void	(*RenderMode)( int mode );
	void	(*Begin)( int primitiveCode );
	void	(*End)( void );

	void	(*Color4f)( float r, float g, float b, float a );
	void	(*Color4ub)( unsigned char r, unsigned char g, unsigned char b, unsigned char a );
	void	(*TexCoord2f)( float u, float v );
	void	(*Vertex3fv)( const float *worldPnt );
	void	(*Vertex3f)( float x, float y, float z );
	void	(*Brightness)( float brightness );
	void	(*CullFace)( TRICULLSTYLE style );
	int		(*SpriteTexture)( struct model_s *pSpriteModel, int frame );
	int		(*WorldToScreen)( const float *world, float *screen );  // Returns 1 if it's z clipped
	void	(*Fog)( float flFogColor[3], float flStart, float flEnd, int bOn ); //Works just like GL_FOG, flFogColor is r/g/b.
	void	(*ScreenToWorld)( const float *screen, float *world  );
	void	(*GetMatrix)( const int pname, float *matrix );
	int		(*BoxInPVS)( float *mins, float *maxs );
	void	(*LightAtPoint)( float *pos, float *value );
	void	(*Color4fRendermode)( float r, float g, float b, float a, int rendermode );
	void	(*FogParams)( float flDensity, int iFogSkybox );
} triangleapi_t;

typedef struct cl_enginefunc_s
{
    byte		       *(*COM_LoadFile)( const char *path, int usehunk, int *pLength );
    void		        (*Con_Printf)( const char *fmt, ... );
    char*	            (*COM_ParseFile)( char *data, char *token );
    void	            (*COM_FreeFile)( void *buffer );
    struct cl_entity_s *(*GetLocalPlayer)( void );
    struct cl_entity_s *(*GetEntityByIndex)( int );
    void            	(*GetViewAngles)( float * );
    
	
	int					(*pfnRandomLong)( int lLow, int lHigh );

    int	                (*PM_PointContents)( vec3_t point, int *truecontents );
    float	            (*GetClientTime)( void );

    float	(*pfnRandomFloat)( float flLow, float flHigh );
	cvar_t*	(*pfnRegisterVariable)( const char *szName, const char *szValue, int flags );
	cvar_t* (*pfnGetCvarPointer)( const char *szName );

	const struct model_s* (*GetSpritePointer)( HSPRITE hSprite );
	struct pmtrace_s *(*PM_TraceLine)( float *start, float *end, int flags, int usehull, int ignore_pe );

    triangleapi_t* pTriAPI;
	event_api_t* pEventAPI;
}cl_enginefunc_s;

extern cl_enginefunc_s gEngfuncs;

inline struct cvar_s* CVAR_CREATE(const char* cv, const char* val, const int flags) { return gEngfuncs.pfnRegisterVariable((char*)cv, (char*)val, flags); }


inline HSPRITE SPR_Load(const char* name)
{
	return nullptr;
}


