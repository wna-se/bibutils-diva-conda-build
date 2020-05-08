/*
 * bibutils.h
 *
 * Copyright (c) Chris Putnam 2005-2009
 *
 */
#ifndef BIBUTILS_H
#define BIBUTILS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include "bibl.h"
#include "list.h"

#define BIBL_OK           (0)
#define BIBL_ERR_BADINPUT (-1)
#define BIBL_ERR_MEMERR   (-2)
#define BIBL_ERR_CANTOPEN (-3)

#define BIBL_FIRSTIN      (100)
#define BIBL_MODSIN       (BIBL_FIRSTIN)
#define BIBL_BIBTEXIN     (BIBL_FIRSTIN+1)
#define BIBL_RISIN        (BIBL_FIRSTIN+2)
#define BIBL_ENDNOTEIN    (BIBL_FIRSTIN+3)
#define BIBL_COPACIN      (BIBL_FIRSTIN+4)
#define BIBL_ISIIN        (BIBL_FIRSTIN+5)
#define BIBL_MEDLINEIN    (BIBL_FIRSTIN+6)
#define BIBL_ENDNOTEXMLIN (BIBL_FIRSTIN+7)
#define BIBL_BIBLATEXIN   (BIBL_FIRSTIN+8)
#define BIBL_EBIIN        (BIBL_FIRSTIN+9)
#define BIBL_WORDIN       (BIBL_FIRSTIN+10)
#define BIBL_LASTIN       (BIBL_FIRSTIN+10)

#define BIBL_FIRSTOUT     (200)
#define BIBL_MODSOUT      (BIBL_FIRSTOUT)
#define BIBL_BIBTEXOUT    (BIBL_FIRSTOUT+1)
#define BIBL_RISOUT       (BIBL_FIRSTOUT+2)
#define BIBL_ENDNOTEOUT   (BIBL_FIRSTOUT+3)
#define BIBL_ISIOUT       (BIBL_FIRSTOUT+4)
#define BIBL_WORD2007OUT  (BIBL_FIRSTOUT+5)
#define BIBL_ADSABSOUT    (BIBL_FIRSTOUT+6)
#define BIBL_LASTOUT      (BIBL_FIRSTOUT+6)

#define BIBL_FORMAT_VERBOSE (1)

#define BIBL_RAW_WITHCHARCONVERT (4)
#define BIBL_RAW_WITHMAKEREFID   (8)

#define BIBL_CHARSET_UNKNOWN (-1)
#define BIBL_CHARSET_UNICODE (-2)
#define BIBL_CHARSET_GB18030 (-3)
#define BIBL_CHARSET_DEFAULT (66)  /* Latin-1/ISO8859-1 */

#define BIBL_SRC_DEFAULT (0)  /* value from program default */
#define BIBL_SRC_FILE    (1)  /* value from file, priority over default */
#define BIBL_SRC_USER    (2)  /* value from user, priority over file, default */

typedef unsigned char uchar;

typedef struct param {

	int readformat;
	int writeformat;

	int charsetin;
	uchar charsetin_src; /*BIBL_SRC_DEFAULT, BIBL_SRC_FILE, BIBL_SRC_USER*/
	uchar latexin;
	uchar utf8in;
	uchar xmlin;
	uchar nosplittitle;

	int charsetout;
	uchar charsetout_src; /* BIBL_SRC_PROG, BIBL_SRC_USER */
	uchar latexout;       /* If true, write Latex codes */
	uchar utf8out;        /* If true, write characters encoded by utf8 */
	uchar utf8bom;        /* If true, write utf8 byte-order-mark */
	uchar xmlout;         /* If true, write characters in XML entities */

	int format_opts; /* options for specific formats */
	int addcount;  /* add reference count to reference id */
	uchar output_raw;
	uchar verbose;
	uchar singlerefperfile;

	list asis;  /* Names that shouldn't be mangled */
	list corps; /* Names that shouldn't be mangled-MODS corporation type */

	char *progname;

} param;

extern void bibl_initparams( param *p, int readmode, int writemode,
	char *progname );
extern void bibl_freeparams( param *p );
extern void bibl_readasis( param *p, char *filename );
extern void bibl_addtoasis( param *p, char *entry );
extern void bibl_readcorps( param *p, char *filename );
extern void bibl_addtocorps( param *p, char *entry );
extern int bibl_read( bibl *b, FILE *fp, char *filename, param *p );
extern int bibl_write( bibl *b, FILE *fp, param *p );
extern void bibl_reporterr( int err );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

