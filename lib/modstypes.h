/*
 * modstypes.h
 *
 * Copyright (c) Chris Putnam 2008-9
 *
 * Program and source code released under the GPL
 *
 */
#ifndef MODSTYPES_H
#define MODSTYPES_H
typedef struct convert {
        char *mods; /* old */
        char *internal; /* new */
	int code;
} convert;

extern convert identifier_types[];
extern int nidentifier_types;

extern char *mods_find_attrib( char *internal_name, convert *data, int ndata );
extern char *mods_find_internal( char *mods_name, convert *data, int ndata );

#endif
