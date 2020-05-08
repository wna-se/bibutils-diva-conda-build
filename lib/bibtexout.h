/*
 * bibtexout.h
 *
 * Copyright (c) Chris Putnam 2005-2009
 *
 */
#ifndef BIBTEXOUT_H
#define BIBTEXOUT_H

#include <stdio.h>
#include "fields.h"
#include "bibutils.h"

/* bibtexout output options */
#define BIBOUT_FINALCOMMA (2)
#define BIBOUT_SINGLEDASH (4)
#define BIBOUT_WHITESPACE (8)
#define BIBOUT_BRACKETS  (16)
#define BIBOUT_UPPERCASE (32)
#define BIBOUT_STRICTKEY (64)

extern void bibtexout_write( fields *info, FILE *fp, param *p, 
		unsigned long refnum );
extern void bibtexout_writeheader( FILE *outptr, param *p );


#endif
