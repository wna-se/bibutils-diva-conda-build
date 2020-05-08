/*
 * xml.h
 *
 * Copyright (c) Chris Putnam 2004-2009
 *
 * Source code released under the GPL
 *
 */
#ifndef XML_H
#define XML_H

#include "list.h"
#include "newstr.h"

typedef struct xml_attrib {
	list attrib;
	list value;
} xml_attrib;

typedef struct xml {
	newstr *tag;
	newstr *value;
	xml_attrib *a;
	struct xml *down;
	struct xml *next;
} xml;

extern void xml_init( xml *x );
extern newstr * xml_getattrib( xml *node, char *attrib );
extern char * xml_findstart( char *buffer, char *tag );
extern char * xml_findend( char *buffer, char *tag );
extern int xml_tagexact( xml *node, char *s );
extern int xml_tag_attrib( xml *node, char *s, char *attrib, char *value );
extern void xml_free( xml *x );
extern char * xml_tree( char *p, xml *onode );

extern char *xml_pns; /* global Namespace */

#endif

