/*
render_api.h - Xash3D extension for client interface
Copyright (C) 2011 Uncle Mike

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef RENDER_API_H
#define RENDER_API_H

#include "lightstyle.h"
#include "dlight.h"

// changes for version 28
// replace decal_t from software declaration to hardware (matched to normal HL)
// mextrasurf_t->increased limit of reserved fields (up from 7 to 32)
// replace R_StoreEfrags with him extended version
// formed group for BSP decal manipulating
// move misc functions at end of the interface
// added new export for clearing studio decals

#define CL_RENDER_INTERFACE_VERSION	35
#define MAX_STUDIO_DECALS		4096	// + unused space of BSP decals

#define SURF_INFO( surf, mod )	((mextrasurf_t *)mod->cache.data + (surf - mod->surfaces)) 
#define INFO_SURF( surf, mod )	(mod->surfaces + (surf - (mextrasurf_t *)mod->cache.data)) 

// render info parms
#define PARM_TEX_WIDTH	1	// all parms with prefix 'TEX_' receive arg as texnum
#define PARM_TEX_HEIGHT	2	// otherwise it's not used
#define PARM_TEX_SRC_WIDTH	3
#define PARM_TEX_SRC_HEIGHT	4
#define PARM_TEX_SKYBOX	5	// second arg as skybox ordering num
#define PARM_TEX_SKYTEXNUM	6	// skytexturenum for quake sky
#define PARM_TEX_LIGHTMAP	7	// second arg as number 0 - 128
#define PARM_TEX_TARGET	8
#define PARM_TEX_TEXNUM	9
#define PARM_TEX_FLAGS	10
#define PARM_TEX_TYPE	11
#define PARM_TEX_CACHEFRAME	12	// compare with worldmodel->needload
#define PARM_TEX_GLFORMAT	13	// get a texture GL-format
// reserved
#define PARM_WORLD_VERSION	16	// return the version of bsp
#define PARM_SKY_SPHERE	17	// sky is quake sphere ?
#define PARM_MAP_HAS_MIRRORS	18	// current map has mirorrs
#define PARM_MAP_HAS_DELUXE	19	// map has deluxedata
#define PARM_MAX_ENTITIES	20
#define PARM_WIDESCREEN	21
#define PARM_FULLSCREEN	22
#define PARM_SCREEN_WIDTH	23
#define PARM_SCREEN_HEIGHT	24
#define PARM_CLIENT_INGAME	25
#define PARM_FEATURES	26	// same as movevars->features
#define PARM_ACTIVE_TMU	27	// for debug
#define PARM_CACHEFRAME	28
#define PARM_MAX_IMAGE_UNITS	29
#define PARM_CLIENT_ACTIVE	30

enum
{
	// skybox ordering
	SKYBOX_RIGHT	= 0,
	SKYBOX_BACK,
	SKYBOX_LEFT,
	SKYBOX_FORWARD,
	SKYBOX_UP,
	SKYBOX_DOWN,
};

typedef enum
{
	TEX_INVALID = 0,	// free slot
	TEX_SYSTEM,	// generated by engine
	TEX_NOMIP,	// hud pics, menu etc
	TEX_BRUSH,	// a map texture
	TEX_SPRITE,	// sprite frames
	TEX_STUDIO,	// studio skins
	TEX_LIGHTMAP,	// lightmap textures
	TEX_DECAL,	// decals
	TEX_VGUI,		// vgui fonts or images
	TEX_CUBEMAP,	// cubemap textures (sky)
	TEX_DETAIL,	// detail textures
	TEX_REMAP,	// local copy of remap texture
	TEX_SCREENCOPY,	// keep screen copy e.g. for mirror
	TEX_CUSTOM,	// user created texture
	TEX_DEPTHMAP	// shadowmap texture
} texType_t;

typedef enum
{
	TF_NEAREST	= (1<<0),		// disable texfilter
	TF_KEEP_RGBDATA	= (1<<1),		// some images keep source
	TF_NOFLIP_TGA	= (1<<2),		// Steam background completely ignore tga attribute 0x20
	TF_KEEP_8BIT	= (1<<3),		// keep original 8-bit image (if present)
	TF_NOPICMIP	= (1<<4),		// ignore r_picmip resample rules
	TF_UNCOMPRESSED	= (1<<5),		// don't compress texture in video memory
	TF_CUBEMAP	= (1<<6),		// it's cubemap texture
	TF_DEPTHMAP	= (1<<7),		// custom texture filter used
	TF_INTENSITY	= (1<<8),		// monochrome intensity image
	TF_LUMINANCE	= (1<<9),		// force image to grayscale
	TF_SKYSIDE	= (1<<10),	// this is a part of skybox
	TF_CLAMP		= (1<<11),	// clamp texcoords to [0..1] range
	TF_NOMIPMAP	= (1<<12),	// don't build mips for this image
	TF_HAS_LUMA	= (1<<13),	// sets by GL_UploadTexture
	TF_MAKELUMA	= (1<<14),	// create luma from quake texture (only q1 textures contain luma-pixels)
	TF_NORMALMAP	= (1<<15),	// is a normalmap
	TF_HAS_ALPHA	= (1<<16),	// image has alpha (used only for GL_CreateTexture)
	TF_FORCE_COLOR	= (1<<17),	// force upload monochrome textures as RGB (detail textures)
	TF_TEXTURE_1D	= (1<<18),	// this is GL_TEXTURE_1D
	TF_BORDER		= (1<<19),	// zero clamp for projected textures
	TF_TEXTURE_3D	= (1<<20),	// this is GL_TEXTURE_3D
	TF_STATIC		= (1<<21),	// a marker for purge mechanism (not used by engine)
	TF_TEXTURE_RECTANGLE= (1<<22),	// this is GL_TEXTURE_RECTANGLE
	TF_ALPHA_BORDER	= (1<<23),	// clamp to (0,0,0,255) (probably no difference)
	TF_IMAGE_PROGRAM	= (1<<24),	// enable image program support like in Doom3
	TF_ALPHACONTRAST	= (1<<25),	// special texture flags for internal usage
	TF_FLOAT		= (1<<26),	// float textures
	TF_NOCOMPARE	= (1<<27),	// disable comparing for depth textures
	TF_FLOATDATA	= (1<<28),
} texFlags_t;

typedef struct beam_s BEAM;
typedef struct particle_s particle_t;

// 12 bytes here
typedef struct modelstate_s
{
	short		sequence;
	short		frame;		// 10 bits multiple by 4, should be enough
	byte		blending[2];
	byte		controller[4];
	byte		body;
	byte		skin;
} modelstate_t;

typedef struct decallist_s
{
	vec3_t		position;
	char		name[64];
	short		entityIndex;
	byte		depth;
	byte		flags;
	float		scale;

	// this is the surface plane that we hit so that
	// we can move certain decals across
	// transitions if they hit similar geometry
	vec3_t		impactPlaneNormal;

	modelstate_t	studio_state;	// studio decals only
} decallist_t;

typedef struct render_api_s
{
	// Get renderer info (doesn't changes engine state at all)
	int		(*RenderGetParm)( int parm, int arg );	// generic
	void		(*GetDetailScaleForTexture)( int texture, float *xScale, float *yScale );
	void		(*GetExtraParmsForTexture)( int texture, byte *red, byte *green, byte *blue, byte *alpha );
	lightstyle_t*	(*GetLightStyle)( int number ); 
	dlight_t*		(*GetDynamicLight)( int number );
	dlight_t*		(*GetEntityLight)( int number );
	byte		(*TextureToTexGamma)( byte color );	// software gamma support
	void		(*GetBeamChains)( BEAM ***active_beams, BEAM ***free_beams, particle_t ***free_trails );

	// Set renderer info (tell engine about changes)
	void		(*R_SetCurrentEntity)( struct cl_entity_s *ent ); // tell engine about both currententity and currentmodel
	void		(*R_SetCurrentModel)( struct model_s *mod );	// change currentmodel but leave currententity unchanged
	void		(*GL_SetWorldviewProjectionMatrix)( const float *glmatrix ); // update viewprojection matrix (tracers uses it)
	void		(*R_StoreEfrags)( struct efrag_s **ppefrag, int framecount );// store efrags for static entities

	// Texture tools
	int		(*GL_FindTexture)( const char *name );
	const char*	(*GL_TextureName)( unsigned int texnum );
	const byte*	(*GL_TextureData)( unsigned int texnum ); // may be NULL
	int		(*GL_LoadTexture)( const char *name, const byte *buf, size_t size, int flags );
	int		(*GL_CreateTexture)( const char *name, int width, int height, const void *buffer, int flags ); 
	void		(*GL_SetTextureType)( unsigned int texnum, unsigned int type );
	void		(*GL_TextureCacheFrame)( unsigned int texnum );
	void		(*GL_FreeTexture)( unsigned int texnum );

	// Decals manipulating (draw & remove)
	void		(*DrawSingleDecal)( struct decal_s *pDecal, struct msurface_s *fa );
	float		*(*R_DecalSetupVerts)( struct decal_s *pDecal, struct msurface_s *surf, int texture, int *outCount );
	void		(*R_EntityRemoveDecals)( struct model_s *mod ); // remove all the decals from specified entity (BSP only)

	// AVIkit support
	void		*(*AVI_LoadVideo)( const char *filename, int ignore_hwgamma );
	int		(*AVI_GetVideoInfo)( void *Avi, long *xres, long *yres, float *duration );
	long		(*AVI_GetVideoFrameNumber)( void *Avi, float time );
	byte		*(*AVI_GetVideoFrame)( void *Avi, long frame );
	void		(*AVI_UploadRawFrame)( int texture, int cols, int rows, int width, int height, const byte *data );
	void		(*AVI_FreeVideo)( void *Avi );
	int		(*AVI_IsActive)( void *Avi );

	// glState related calls (must use this instead of normal gl-calls to prevent de-synchornize local states between engine and the client)
	void		(*GL_Bind)( int tmu, unsigned int texnum );
	void		(*GL_SelectTexture)( int tmu );
	void		(*GL_LoadTextureMatrix)( const float *glmatrix );
	void		(*GL_TexMatrixIdentity)( void );
	void		(*GL_CleanUpTextureUnits)( int last );	// pass 0 for clear all the texture units
	void		(*GL_TexGen)( unsigned int coord, unsigned int mode );
	void		(*GL_TextureTarget)( unsigned int target ); // change texture unit mode without bind texture
	void		(*GL_TexCoordArrayMode)( unsigned int texmode );
	void		(*GL_Reserved0)( void );	// for potential interface expansion without broken compatibility
	void		(*GL_Reserved1)( void );
	void		(*GL_Reserved2)( void );
	void		(*GL_Reserved3)( void );
		
	// Misc renderer functions
	void		(*GL_DrawParticles)( const float *vieworg, const float *fwd, const float *rt, const float *up, unsigned int clipFlags );
	void		(*EnvShot)( const float *vieworg, const char *name, qboolean skyshot, int shotsize ); // creates a cubemap or skybox into gfx\env folder
	int		(*COM_CompareFileTime)( const char *filename1, const char *filename2, int *iCompare );
	void		(*Host_Error)( const char *error, ... ); // cause Host Error
	int		(*SPR_LoadExt)( const char *szPicName, unsigned int texFlags ); // extended version of SPR_Load
	void		(*TessPolygon)( struct msurface_s *surf, struct model_s *mod, float tessSize );
	struct mstudiotex_s *( *StudioGetTexture )( struct cl_entity_s *e );
	const struct ref_overview_s *( *GetOverviewParms )( void );
	void		(*S_FadeMusicVolume)( float fadePercent );	// fade background track (0-100 percents)
	void		(*SetRandomSeed)( long lSeed );		// set custom seed for RANDOM_FLOAT\RANDOM_LONG for predictable random
	// static allocations
	void		*(*pfnMemAlloc)( size_t cb, const char *filename, const int fileline );
	void		(*pfnMemFree)( void *mem, const char *filename, const int fileline );
 	// find in files
	char		**(*pfnGetFilesList)( const char *pattern, int *numFiles, int gamedironly );
	// ONLY ADD NEW FUNCTIONS TO THE END OF THIS STRUCT.  INTERFACE VERSION IS FROZEN AT 35
} render_api_t;

// render callbacks
typedef struct render_interface_s
{
	int		version;
	// passed through R_RenderFrame (0 - use engine renderer, 1 - use custom client renderer)
	int		(*GL_RenderFrame)( const struct ref_params_s *pparams, qboolean drawWorld );
	// build all the lightmaps on new level or when gamma is changed
	void		(*GL_BuildLightmaps)( void );
	// setup map bounds for ortho-projection when we in dev_overview mode
	void		(*GL_OrthoBounds)( const float *mins, const float *maxs );
	// handle decals which hit mod_studio or mod_sprite
	void		(*R_StudioDecalShoot)( int decalTexture, struct cl_entity_s *ent, const float *start, const float *pos, int flags, modelstate_t *state );
	// prepare studio decals for save
	int		(*R_CreateStudioDecalList)( decallist_t *pList, int count, qboolean changelevel );
	// clear decals by engine request (e.g. for demo recording or vid_restart)
	void		(*R_ClearStudioDecals)( void );
	// grab r_speeds message
	qboolean		(*R_SpeedsMessage)( char *out, size_t size );
	// replace with built-in R_DrawCubemapView for make skyshots or envshots
	qboolean		(*R_DrawCubemapView)( const float *origin, const float *angles, int size );
	// alloc or destroy studiomodel custom data
	void		(*Mod_ProcessUserData)( struct model_s *mod, qboolean create, const byte *buffer );
} render_interface_t;

#endif//RENDER_API_H