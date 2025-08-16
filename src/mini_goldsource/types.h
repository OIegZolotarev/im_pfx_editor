#include <common.h>
#include <gl_texture.h>
#include <mathlib.h>

typedef glm::vec3 Vector;
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

typedef struct cl_entity_s
{
    int index;
}cl_entity_t;

class TriAPIWrapper;

typedef struct cl_enginefunc_s
{
    byte		*(*COM_LoadFile)( const char *path, int usehunk, int *pLength );
    void		(*Con_Printf)( const char *fmt, ... );
    char*	(*COM_ParseFile)( char *data, char *token );
    void	(*COM_FreeFile)( void *buffer );
    struct cl_entity_s *(*GetLocalPlayer)( void );

    int	(*PM_PointContents)( const float *point, int *truecontents );
    float	(*GetClientTime)( void );

    TriAPIWrapper* pTriAPI;
}cl_enginefunc_s;

extern cl_enginefunc_s gEngfuncs;

