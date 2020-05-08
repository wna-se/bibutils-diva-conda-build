/*
 * endin.c
 *
 * Copyright (c) Chris Putnam 2003-2010
 *
 * Program and source code released under the GPL
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "is_ws.h"
#include "doi.h"
#include "newstr.h"
#include "newstr_conv.h"
#include "fields.h"
#include "name.h"
#include "title.h"
#include "serialno.h"
#include "reftypes.h"
#include "endin.h"

/* Endnote tag definition:
    character 1 = '%'
    character 2 = alphabetic character or digit (or other characters)
    character 3 = space (ansi 32)
*/
static int
endin_istag( char *buf )
{
	const char others[]="!@#$^&*()+=?[~>";
	if ( buf[0]!='%' ) return 0;
	if ( buf[2]!=' ' ) return 0;
	if ( isalpha( buf[1] ) ) return 1;
	if ( isdigit( buf[1] ) ) return 1;
	if ( strchr( others, buf[1] ) ) return 1;
	return 0;
}

static int
readmore( FILE *fp, char *buf, int bufsize, int *bufpos, newstr *line )
{
	if ( line->len ) return 1;
	else return newstr_fget( fp, buf, bufsize, bufpos, line );
}

int
endin_readf( FILE *fp, char *buf, int bufsize, int *bufpos, newstr *line, newstr *reference, int *fcharset )
{
	int haveref = 0, inref = 0;
	unsigned char *up;
	char *p;
	*fcharset = CHARSET_UNKNOWN;
	while ( !haveref && readmore( fp, buf, bufsize, bufpos, line ) ) {
		if ( !line->data ) continue;
		p = &(line->data[0]);

		/* Skip <feff> Unicode header information */
		/* <feff> = ef bb bf */
		up = (unsigned char* ) p;
		if ( line->len > 2 && up[0]==0xEF && up[1]==0xBB &&
				up[2]==0xBF ) {
			*fcharset = CHARSET_UNICODE;
			p += 3;
		}

		if ( !*p ) {
			if ( inref ) haveref = 1; /* blank line separates */
			else continue; /* blank line to ignore */
		}
		/* Each reference starts with a tag && ends with a blank line */
		if ( endin_istag( p ) ) {
			if ( reference->len ) newstr_addchar( reference, '\n' );
			newstr_strcat( reference, p );
			inref = 1;
		} else if ( inref && p ) {
			newstr_addchar( reference, '\n' );
		   	newstr_strcat( reference, p );
		}
		newstr_empty( line );
	}
	if ( reference->len ) haveref = 1;
	return haveref;
}

static char*
process_endline( newstr *tag, newstr *data, char *p )
{
	int  i;

	i = 0;
	while ( i<2 && *p ) {
		newstr_addchar( tag, *p++);
		i++;
	}
	while ( *p==' ' || *p=='\t' ) p++;

	while ( *p && *p!='\r' && *p!='\n' )
		newstr_addchar( data, *p++ );
	newstr_trimendingws( data );

	while ( *p=='\r' || *p=='\n' ) p++;

	return p;
}

static char *
process_endline2( newstr *tag, newstr *data, char *p )
{
	while ( *p==' ' || *p=='\t' ) p++;
	while ( *p && *p!='\r' && *p!='\n' )
		newstr_addchar( data, *p++ );
	newstr_trimendingws( data );
	while ( *p=='\r' || *p=='\n' ) p++;
	return p;
}

int
endin_processf( fields *endin, char *p, char *filename, long nref )
{
	newstr tag, data;
	int n;
	newstrs_init( &tag, &data, NULL );
	while ( *p ) {
		if ( endin_istag( p ) ) {
			p = process_endline( &tag, &data, p );
			/* no empty fields allowed */
			if ( data.len ) {
				fields_add( endin, tag.data, data.data, 0 );
			}
		} else {
			p = process_endline2( &tag, &data, p );
			/* endnote puts %K only on 1st line of keywords */
			n = endin->nfields;
			if ( n>0 && data.len ) {
			if ( !strncmp( endin->tag[n-1].data, "%K", 2 ) ) {
				fields_add( endin, "%K", data.data, 0 );
			} else {
				newstr_addchar( &(endin->data[n-1]), ' ' );
				newstr_strcat( &(endin->data[n-1]), data.data );
			}
			}
		}
		newstrs_empty( &tag, &data, NULL );
	}
	newstrs_free( &tag, &data, NULL );
	return 1;
}

static void
addtype( fields *info, char *data, int level )
{
	lookups types[] = {
		{ "GENERIC", "ARTICLE" },
		{ "BOOK", "BOOK" }, 
		{ "MANUSCRIPT", "MANUSCRIPT" },
		{ "CONFERENCE PROCEEDINGS", "INPROCEEDINGS"},
		{ "REPORT", "REPORT" },
		{ "COMPUTER PROGRAM", "BOOK" },
		{ "AUDIOVISUAL MATERIAL", "AUDIOVISUAL" },
		{ "ARTWORK", "BOOK" },
		{ "PATENT", "BOOK" },
		{ "BILL", "BILL" },
		{ "CASE", "CASE" },
		{ "JOURNAL ARTICLE", "ARTICLE" }, 
		{ "MAGAZINE ARTICLE", "ARTICLE" }, 
		{ "BOOK SECTION", "INBOOK" },
		{ "EDITED BOOK", "BOOK" },
       		{ "NEWSPAPER ARTICLE",  "NEWSARTICLE" },
		{ "THESIS", "PHDTHESIS" },
		{ "PERSONAL COMMUNICATION", "COMMUNICATION" },
		{ "ELECTRONIC SOURCE", "TEXT" },
		{ "FILM OR BROADCAST", "AUDIOVISUAL" },
		{ "MAP", "MAP" },
		{ "HEARING", "HEARING" },
		{ "STATUTE", "STATUTE" },
		{ "CHART OR TABLE", "CHART" },
		{ "WEB PAGE", "WEBPAGE" },
	};
	int  ntypes = sizeof( types ) / sizeof( lookups );
	int  i, found=0;
	for ( i=0; i<ntypes; ++i ) {
		if ( !strcasecmp( types[i].oldstr, data ) ) {
			found = 1;
			fields_add( info, "INTERNAL_TYPE", types[i].newstr, level );
		}
	}
	if ( !found ) {
		fprintf( stderr, "Did not identify reference type '%s'\n",
			data );
		fprintf( stderr, "Defaulting to journal article type\n");
		fields_add( info, "INTERNAL_TYPE", types[0].newstr, level );
	}
}

static void
addpage( fields *info, char *p, int level )
{
	newstr page;
	newstr_init( &page );
	p = skip_ws( p );
	while ( *p && !is_ws(*p) && *p!='-' && *p!='\r' && *p!='\n' ) 
		newstr_addchar( &page, *p++ );
	if ( page.len>0 ) fields_add( info, "PAGESTART", page.data, level );
	newstr_empty( &page );
	while ( *p && (is_ws(*p) || *p=='-' ) ) p++;
	while ( *p && !is_ws(*p) && *p!='-' && *p!='\r' && *p!='\n' ) 
		newstr_addchar( &page, *p++ );
	if ( page.len>0 ) fields_add( info, "PAGEEND", page.data, level );
	newstr_free( &page );
}

static void
adddate( fields *info, char *tag, char *newtag, char *p, int level )
{
	char *months[12]={ "January", "February", "March", "April",
		"May", "June", "July", "August", "September",
		"October", "November", "December" };
	char month[10];
	int found,i,part;
	newstr date;
	newstr_init( &date );
	part = (!strncasecmp(newtag,"PART",4));
	if ( !strcasecmp( tag, "%D" ) ) {
		while ( *p ) newstr_addchar( &date, *p++ );
		if ( date.len>0 ) {
			if ( part ) 
				fields_add(info, "PARTYEAR", date.data, level);
			else
				fields_add( info, "YEAR", date.data, level );
		}
	} else if ( !strcasecmp( tag, "%8" ) ) {
		while ( *p && *p!=' ' && *p!=',' ) newstr_addchar( &date, *p++ );
		if ( date.len>0 ) {
			found = -1;
			for ( i=0; i<12 && found==-1; ++i )
				if ( !strncasecmp( date.data, months[i], 3 ) )
					found = i;
			if ( found!=-1 ) {
				if (found>8) sprintf( month, "%d", found+1 );
				else sprintf( month, "0%d", found+1 );
				if ( part ) 
					fields_add( info, "PARTMONTH", month, level );
				else    fields_add( info, "MONTH", month, level );
			} else {
				if ( part )
					fields_add( info, "PARTMONTH", date.data, level );
				else
					fields_add( info, "MONTH", date.data, level );
			}
		}
		newstr_empty( &date );
		p = skip_ws( p );
		while ( *p && *p!='\n' && *p!=',' ) newstr_addchar( &date, *p++ );
		if ( date.len>0 && date.len<3 ) {
			if ( part )
				fields_add( info, "PARTDAY", date.data, level );
			else
				fields_add( info, "DAY", date.data, level );
		}
	}
	newstr_free( &date );
}

/* Endnote defaults if no %0
 *
 * if %J & %V - journal article
 * if %B - book section
 * if %R & !%T - report
 * if %I & !%B & !%J & !%R - book
 * if !%B & !%J & !%R & !%I - journal article
 */
int
endin_typef( fields *endin, char *filename, int nrefs, param *p, 
	variants *all, int nall )
{
	char *refnum = "";
	int n, reftype, nrefnum, nj, nv, nb, nr, nt, ni;
	n = fields_find( endin, "%0", 0 );
	nrefnum = fields_find( endin, "%F", 0 );
	if ( nrefnum!=-1 ) refnum = endin->data[nrefnum].data;
	if ( n!=-1 ) {
		reftype = get_reftype( endin->data[n].data, nrefs, 
			p->progname, all, nall, refnum );
	} else {
		nj = fields_find( endin, "%J", 0 );
		nv = fields_find( endin, "%V", 0 );
		nb = fields_find( endin, "%B", 0 );
		nr = fields_find( endin, "%R", 0 );
		nt = fields_find( endin, "%T", 0 );
		ni = fields_find( endin, "%I", 0 );
		if ( nj!=-1 && nv!=-1 ) {
			reftype = get_reftype( "Journal Article", nrefs,
					p->progname, all, nall, refnum );
		} else if ( nb!=-1 ) {
			reftype = get_reftype( "Book Section", nrefs,
					p->progname, all, nall, refnum );
		} else if ( nr!=-1 && nt==-1 ) {
			reftype = get_reftype( "Report", nrefs,
					p->progname, all, nall, refnum );
		} else if ( ni!=-1 && nb==-1 && nj==-1 && nr==-1 ) {
			reftype = get_reftype( "Book", nrefs,
					p->progname, all, nall, refnum );
		} else if ( nb==-1 && nj==-1 && nr==-1 && ni==-1 ) {
			reftype = get_reftype( "Journal Article", nrefs,
					p->progname, all, nall, refnum );
		} else {
			reftype = get_reftype( "", nrefs, p->progname, 
					all, nall, refnum ); /* default */
		}
	}
	return reftype;
}

/* Wiley puts multiple authors separated by commas on the %A lines.
 * We can detect this by finding the terminal comma in the data.
 *
 * "%A" "Author A. X. Last, Author N. B. Next,"
 */
static int
is_wiley_author( fields *endin, int n )
{
	newstr *t, *d;
	t = &(endin->tag[n]);
	if ( !t->data || strcmp( t->data, "%A" ) ) return 0;
	d = &( endin->data[n] );
	if ( !(d->data) || d->len==0 ) return 0;
	if ( d->data[d->len-1]!=',' ) return 0;
	return 1;
}

static void
cleanup_wiley_author( fields *endin, int n )
{	
	newstr tmp, tmppart;
	int i, nauthor = 0;
	newstrs_init( &tmp, &tmppart, NULL );
	newstr_newstrcpy( &tmp, &( endin->data[n] ) );
	i = 0;
	while ( i<tmp.len ) {
		if ( tmp.data[i]==',' ) {
			if ( nauthor==0 )
				newstr_newstrcpy( &(endin->data[n]), &tmppart );
			else
				fields_add( endin, endin->tag[n].data,
					tmppart.data, endin->level[n] );
			newstr_empty( &tmppart );
			nauthor++;
			while ( i<tmp.len && is_ws( tmp.data[i] ) ) i++;
		} else {
			newstr_addchar( &tmppart, tmp.data[i] );
		}
		i++;
	}
	newstrs_free( &tmp, &tmppart, NULL );
}

static void
endin_cleanref( fields *endin )
{
	int i;
	for ( i=0; i<endin->nfields; ++i ) {
		if ( is_wiley_author( endin, i ) )
			cleanup_wiley_author( endin, i );
	}
}

void
endin_cleanf( bibl *bin, param *p )
{
        long i;
        for ( i=0; i<bin->nrefs; ++i )
                endin_cleanref( bin->ref[i] );
}

static void
endin_notag( param *p, char *tag, char *data )
{
	if ( p->verbose ) {
		if ( p->progname ) fprintf( stderr, "%s: ", p->progname );
		fprintf( stderr, "Cannot find tag '%s'='%s'\n", tag, data );
	}
}

/* Wiley EndNote download has DOI's in "%1" tag */
static void
addnotes( fields *info, char *tag, char *data, int level )
{
	int doi = is_doi( data );
	if ( doi!=-1 )
		fields_add( info, "DOI", &(data[doi]), level );
	else
		fields_add( info, tag, data, level );
}

void
endin_convertf( fields *endin, fields *info, int reftype, param *p, variants *all, int nall )
{
	newstr *d;
	int  i, level, n, process;
	char *newtag, *t;
	for ( i=0; i<endin->nfields; ++i ) {
		/* Ensure that data exists */
		d = &( endin->data[i] );
		if ( !(d->data) || d->len==0 ) continue;
		/*
		 * All refer format tags start with '%'.  If we have one
		 * that doesn't, assume that it comes from endx2xml
		 * and just copy and paste to output
		 */
		t = endin->tag[i].data;
		if ( t[0]!='%' ) {
			fields_add( info, t, d->data, endin->level[i] );
			continue;
		}
		n = process_findoldtag( t, reftype, all, nall );
		if ( n==-1 ) {
			endin_notag( p, t, d->data );
			continue;
		}
		process = ((all[reftype]).tags[n]).processingtype;
		if ( process == ALWAYS ) continue; /* add these later */
		level = ((all[reftype]).tags[n]).level;
		newtag = ((all[reftype]).tags[n]).newstr;
		if ( process==SIMPLE )
			fields_add( info, newtag, d->data, level );
		else if ( process==TYPE )
			addtype( info, d->data, level );
		else if ( process==TITLE )
			title_process( info, newtag, d->data, level, 
					p->nosplittitle );
		else if ( process==PERSON )
			name_add( info, newtag, d->data, level, 
					&(p->asis), &(p->corps) );
		else if ( process==DATE )
			adddate( info, t, newtag,d->data,level);
		else if ( process==PAGES )
			addpage( info, d->data, level );
		else if ( process==SERIALNO )
			addsn( info, d->data, level );
		else if ( process==NOTES )
			addnotes( info, newtag, d->data, level );
		else {
/*				fprintf(stderr,"%s: internal error -- illegal process %d\n", r->progname, process );
*/
		}
	}
}
