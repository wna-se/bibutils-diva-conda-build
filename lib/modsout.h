/*
 * modsout.h
 *
 * Copyright (c) Chris Putnam 2003-2009
 *
 * Source code released under the GPL
 *
 */
#ifndef MODSOUT_H
#define MODSOUT_H

/* format-specific options */
#define MODSOUT_DROPKEY (2)

#include <stdio.h>
#include <stdlib.h>
#include "bibl.h"
#include "bibutils.h"

extern void modsout_writeheader( FILE *outptr, param *p );
extern void modsout_writefooter( FILE *outptr );
extern void modsout_write( fields *info, FILE *outptr,
	param *p, unsigned long numrefs );

#endif

