/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
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
********************************************************************************
*	Generic Gauss firing routines
*	Written by David Flor (dflor@mach3.com), (c) 1999 Mach III Enterprises
*		http://www.planethalflife.com/mach3/
*
*	Virtually IDENTICAL to "CGauss::Fire" from "gauss.cpp"...
********************************************************************************/

#pragma once

typedef struct
{
	short	red, green, blue, brightness;
	short	beam_width;
} gaussfire_s;

extern void GaussFireExt(entvars_t *pSrc, edict_t *pIgnore, Vector vecOrigSrc, 
						 Vector vecDir, float flDamage, gaussfire_s *gf_data );

extern void GaussFire(entvars_t *pSrc, edict_t *pIgnore, Vector vecOrigSrc, 
					  Vector vecDir, float flDamage );
