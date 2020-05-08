/*
 * reftypes.c
 *
 * Copyright (c) Chris Putnam 2003-2010
 *
 * Program and source code released under the GPL
 *
 */
#include <stdio.h>
#include <string.h>
#include "is_ws.h"
#include "fields.h"
#include "reftypes.h"

int
get_reftype( char *p, long refnum, char *progname, variants *all, int nall, char *tag )
{
	int i;
	p = skip_ws( p );
	for ( i=0; i<nall; ++i )
		if ( !strncasecmp( all[i].type, p, strlen(all[i].type) ) ) 
			return i;
	if ( progname ) fprintf( stderr, "%s: ", progname );
	fprintf( stderr, "Did not recognize type '%s' of refnum %ld (%s).\n"
		"\tDefaulting to article.\n", p, refnum, tag );
	return 0;
}

int
process_findoldtag( char *oldtag, int reftype, variants all[], int nall )
{
        variants *v;
        int i;
        v = &(all[reftype]);
/*      for ( i=0; i<(all[reftype]).ntags; ++i )*/
        for ( i=0; i<v->ntags; ++i )
/*              if ( !strcasecmp( ((all[reftype]).tags[i]).oldstr, oldtag ) )*/
                if ( !strcasecmp( (v->tags[i]).oldstr, oldtag ) )
                        return i;
        return -1;
}


