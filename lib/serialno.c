/*
 * serialno.c
 *
 * Copyright (c) Chris Putnam 2005-2010
 *
 * Source code released under the GPL
 *
 */
#include <string.h>
#include "serialno.h"

void
addsn( fields *info, char *buf, int level )
{
	int ndigits=0, issn=0, isbn=0;
	char *p = buf;

	if ( !strncasecmp( p, "ISSN", 4 ) ) issn=1;
	else if ( !strncasecmp( p, "ISBN", 4 ) ) isbn=1;

	if ( !issn && !isbn ) {
		/* a lot have semicolons between multiple ISBN's for
		   paperbacks and hardbacks with different numbers */
		while ( *p && !(ndigits && (*p==';'||*p==':')) ) {
			if ( ( *p>='0' && *p<='9' ) || *p=='x' || *p=='X' ) 
				ndigits++;
			p++;
		}
		if ( ndigits==8 ) issn = 1;
		else if ( ndigits==10 || ndigits==13 ) isbn = 1;
	}
	
	if ( issn ) fields_add( info, "ISSN", buf, level );
	else if ( isbn ) fields_add( info, "ISBN", buf, level );
	else fields_add( info, "SERIALNUMBER", buf, level );
}
