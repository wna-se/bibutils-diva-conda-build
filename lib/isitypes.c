/*
 * isitypes.c
 *
 * Copyright (c) Chris Putnam 2004-2010
 *
 * Program and source code released under the GPL
 *
 */

#include <stdio.h>
#include <string.h>
#include "is_ws.h"
#include "fields.h"
#include "reftypes.h"

static lookups article[] = {
	{ "AU",     "AUTHOR",    PERSON, LEVEL_MAIN },
	{ "AF",     "AUTHOR",    PERSON, LEVEL_MAIN },
	{ "BE",     "EDITOR",    PERSON, LEVEL_MAIN },
	{ "TI",     "TITLE",     TITLE,  LEVEL_MAIN },
	{ "TI",     "TITLE",     TITLE,  LEVEL_MAIN },
	{ "SO",     "TITLE",     TITLE,  LEVEL_HOST }, /* full source(journal) name */
	{ "SE",     "TITLE",     TITLE,  LEVEL_SERIES }, /* book series title, for example conference proceedings */
	{ "JI",     "SHORTTITLE",TITLE,  LEVEL_HOST }, /* abbr journal name */
	{ "J9",     "SHORTTITLE",TITLE,  LEVEL_HOST }, /* 29char journal name */
	{ "CT",     "EVENTTITLE:CONF",   SIMPLE,  LEVEL_MAIN },
	{ "CY",     "EVENTTITLE:CONF",   SIMPLE,  LEVEL_MAIN },
	{ "CL",     "EVENTTITLE:CONF",   SIMPLE,  LEVEL_MAIN },
	{ "PU",     "PUBLISHER", SIMPLE, LEVEL_HOST },
	{ "PI",     "ADDRESS",   SIMPLE, LEVEL_HOST }, /* publisher city */
	{ "PA",     "ADDRESS",   SIMPLE, LEVEL_HOST }, /* publisher address */
	{ "C1",     "ADDRESS",   SIMPLE, LEVEL_MAIN}, /* author organisation and address */
	{ "RP",     "REPRINTADDRESS", SIMPLE, LEVEL_MAIN },
	{ "PY",     "PARTYEAR",  SIMPLE, LEVEL_MAIN },
	{ "PD",     "PARTMONTH", SIMPLE, LEVEL_MAIN },
	{ "VL",     "VOLUME",    SIMPLE, LEVEL_MAIN },
	{ "AR",     "ARTICLENUMBER", SIMPLE, LEVEL_MAIN }, /* AR=article number that Phys. Rev. B uses instead of page numbers */
	{ "BP",     "PAGESTART", SIMPLE, LEVEL_MAIN },
	{ "EP",     "PAGEEND",   SIMPLE, LEVEL_MAIN },
	{ "PG",     "TOTALPAGES", SIMPLE, LEVEL_MAIN },
	{ "IS",     "ISSUE",     SIMPLE, LEVEL_MAIN },
	{ "SN",     "SERIALNUMBER", SERIALNO, LEVEL_HOST },
	{ "EI",     "SERIALNUMBER", SERIALNO, LEVEL_HOST },
	{ "AB",     "ABSTRACT",  SIMPLE, LEVEL_MAIN },
	{ "NF",     "NOTES",     SIMPLE, LEVEL_MAIN },
	{ "DE",     "KEYWORD",   ISI_KEYWORD, LEVEL_MAIN }, /* author keywords */
	{ "LA",     "LANGUAGE",  SIMPLE, LEVEL_MAIN },
	{ "NR",     "NUMBERREFS", SIMPLE, LEVEL_MAIN },
	{ "CR",     "CITEDREFS",  SIMPLE, LEVEL_MAIN },
	{ "PT",     " ",         TYPE,   LEVEL_HOST },
	{ "DT",     "DOCUMENTTYPE", SIMPLE, LEVEL_MAIN },
	{ "GA",     "ISIDELIVERNUM", SIMPLE, LEVEL_MAIN}, /*ISI document delivery number */
	{ "UT",     "ISIREFNUM", SIMPLE, LEVEL_MAIN }, /* ISI unique article identifer */
	{ "DI",     "DOI",       SIMPLE, LEVEL_MAIN },
	{ "BN",     "ISBN", 	SIMPLE, LEVEL_MAIN }, /*ISBN number only occurs here if this is a proceeding paper which is published in a book  */
	{ "OI",     "ORCID",       SIMPLE, LEVEL_MAIN },
	{ "PM",     "PMID",       SIMPLE, LEVEL_MAIN },
	{ " ",      "INTERNAL_TYPE|ARTICLE",           ALWAYS, LEVEL_MAIN },
	{ " ",      "ISSUANCE|continuing",    ALWAYS, LEVEL_HOST },
	{ " ",      "RESOURCE|text",          ALWAYS, LEVEL_MAIN },
	{ " ",      "GENRE|periodical",       ALWAYS, LEVEL_HOST },
	{ " ",      "GENRE|academic journal", ALWAYS, LEVEL_HOST },
	{ " ",      "GENRE|article", ALWAYS, LEVEL_MAIN }
};

static lookups book[] = {
	{ "AU",     "AUTHOR",    PERSON, LEVEL_MAIN },
	{ "AF",     "AUTHOR",    PERSON, LEVEL_MAIN },
	{ "BE",     "EDITOR",    PERSON, LEVEL_MAIN },
	{ "TI",     "TITLE",     TITLE,  LEVEL_MAIN },
	{ "SO",     "TITLE",     TITLE,  LEVEL_HOST }, /* full journal name */
	{ "JI",     "SHORTTITLE",TITLE,  LEVEL_HOST }, /* abbr journal name */
	{ "J9",     "SHORTTITLE",TITLE,  LEVEL_HOST }, /* 29char journal name */
	{ "PU",     "PUBLISHER", SIMPLE, LEVEL_MAIN },
	{ "PI",     "ADDRESS",   SIMPLE, LEVEL_MAIN }, /* publisher city */
	{ "PA",     "ADDRESS",   SIMPLE, LEVEL_MAIN }, /* publisher address */
	{ "C1",     "ADDRESS",   SIMPLE, LEVEL_MAIN}, /* author organisation and address */
	{ "RP",     "REPRINTADDRESS", SIMPLE, LEVEL_MAIN },
	{ "PY",     "YEAR",      SIMPLE, LEVEL_MAIN },
	{ "PD",     "MONTH",     SIMPLE, LEVEL_MAIN },
	{ "VL",     "VOLUME",    SIMPLE, LEVEL_MAIN },
	{ "BP",     "PAGESTART", SIMPLE, LEVEL_MAIN },
	{ "EP",     "PAGEEND",   SIMPLE, LEVEL_MAIN },
	{ "PG",     "TOTALPAGES", SIMPLE, LEVEL_MAIN },
	{ "IS",     "ISSUE",     SIMPLE, LEVEL_MAIN },
	{ "SN",     "SERIALNUMBER", SERIALNO, LEVEL_HOST },
	{ "EI",     "SERIALNUMBER", SERIALNO, LEVEL_HOST },
	{ "AB",     "ABSTRACT",  SIMPLE, LEVEL_MAIN },
	{ "NF",     "NOTES",     SIMPLE, LEVEL_MAIN },
	{ "DE",     "KEYWORD",   ISI_KEYWORD, LEVEL_MAIN }, /* author keywords */
	{ "LA",     "LANGUAGE",  SIMPLE, LEVEL_MAIN },
	{ "NR",     "NUMBERREFS", SIMPLE, LEVEL_MAIN },
	{ "CR",     "CITEDREFS",  SIMPLE, LEVEL_MAIN },
	{ "PT",     " ",         TYPE,   LEVEL_MAIN },
	{ "DT",     "DOCUMENTTYPE", SIMPLE, LEVEL_MAIN },
	{ "GA",     "ISIDELIVERNUM", SIMPLE, LEVEL_MAIN}, /*ISI document delivery number */
	{ "UT",     "ISIREFNUM", SIMPLE, LEVEL_MAIN }, /* ISI unique article identifer */
	
	{ "PT",     " ",         TYPE,   LEVEL_HOST },
	{ "DI",     "DOI",       SIMPLE, LEVEL_MAIN },
	{ "OI",     "ORCID",       SIMPLE, LEVEL_MAIN },
	{ "PM",     "PMID",       SIMPLE, LEVEL_MAIN },
	{ " ",         "INTERNAL_TYPE|BOOK",       ALWAYS, LEVEL_MAIN },
	{ " ",         "ISSUANCE|monographic",    ALWAYS, LEVEL_MAIN },
	{ " ",         "RESOURCE|text",   ALWAYS, LEVEL_MAIN },
	{ " ",         "GENRE|book",      ALWAYS, LEVEL_MAIN }
};

static lookups inbook[] = {
	{ "AU",     "AUTHOR",    PERSON, LEVEL_MAIN },
	{ "AF",     "AUTHOR",    PERSON, LEVEL_MAIN },
	{ "BE",     "EDITOR",    PERSON, LEVEL_MAIN },
	{ "TI",     "TITLE",     TITLE,  LEVEL_MAIN },
	{ "SO",     "TITLE",     TITLE,  LEVEL_HOST }, /* full journal name */
	{ "JI",     "SHORTTITLE",TITLE,  LEVEL_HOST }, /* abbr journal name */
	{ "J9",     "SHORTTITLE",TITLE,  LEVEL_HOST }, /* 29char journal name */
	{ "PU",     "PUBLISHER", SIMPLE, LEVEL_HOST },
	{ "PI",     "ADDRESS",   SIMPLE, LEVEL_HOST }, /* publisher city */
	{ "PA",     "ADDRESS",   SIMPLE, LEVEL_HOST }, /* publisher address */
	{ "C1",     "ADDRESS",   SIMPLE, LEVEL_MAIN}, /* author organisation and address */
	{ "RP",     "REPRINTADDRESS", SIMPLE, LEVEL_MAIN },
	{ "PY",     "PARTYEAR",  SIMPLE, LEVEL_MAIN },
	{ "PD",     "PARTMONTH", SIMPLE, LEVEL_MAIN },
	{ "VL",     "VOLUME",    SIMPLE, LEVEL_MAIN },
	{ "BP",     "PAGESTART", SIMPLE, LEVEL_MAIN },
	{ "EP",     "PAGEEND",   SIMPLE, LEVEL_MAIN },
	{ "PG",     "TOTALPAGES", SIMPLE, LEVEL_HOST },
	{ "IS",     "ISSUE",     SIMPLE, LEVEL_MAIN },
	{ "SN",     "SERIALNUMBER", SERIALNO, LEVEL_HOST },
	{ "EI",     "SERIALNUMBER", SERIALNO, LEVEL_HOST },
	{ "AB",     "ABSTRACT",  SIMPLE, LEVEL_MAIN },
	{ "NF",     "NOTES",     SIMPLE, LEVEL_MAIN },
	{ "DE",     "KEYWORD",   ISI_KEYWORD, LEVEL_MAIN }, /* author keywords */
	{ "LA",     "LANGUAGE",  SIMPLE, LEVEL_MAIN },
	{ "NR",     "NUMBERREFS", SIMPLE, LEVEL_MAIN },
	{ "CR",     "CITEDREFS",  SIMPLE, LEVEL_MAIN },
	{ "PT",     " ",         TYPE,   LEVEL_HOST },
	{ "DT",     "DOCUMENTTYPE", SIMPLE, LEVEL_MAIN },
	{ "GA",     "ISIDELIVERNUM", SIMPLE, LEVEL_MAIN}, /*ISI document delivery number */
	{ "UT",     "ISIREFNUM", SIMPLE, LEVEL_MAIN }, /* ISI unique article identifer */
	{ "DI",     "DOI",       SIMPLE, LEVEL_MAIN },
	{ "OI",     "ORCID",       SIMPLE, LEVEL_MAIN },
	{ "PM",     "PMID",       SIMPLE, LEVEL_MAIN },
	{ " ",      "INTERNAL_TYPE|INBOOK",   ALWAYS, LEVEL_MAIN },
	{ " ",      "RESOURCE|text", ALWAYS, LEVEL_MAIN },
	{ " ",      "ISSUANCE|monographic",    ALWAYS, LEVEL_HOST },
	{ " ",         "GENRE|book",    ALWAYS, LEVEL_HOST },
	{ " ",         "GENRE|chapter",    ALWAYS, LEVEL_MAIN }
};

static lookups bookinseries[] = {
	{ "AU",     "AUTHOR",    PERSON, LEVEL_MAIN },
	{ "AF",     "AUTHOR",    PERSON, LEVEL_MAIN },
	{ "BE",     "EDITOR",    PERSON, LEVEL_MAIN },
	{ "TI",     "TITLE",     TITLE,  LEVEL_MAIN },
	{ "SO",     "TITLE",     TITLE,  LEVEL_HOST }, /* proceedings title */
	{ "BS",     "SUBTITLE",  TITLE,  LEVEL_HOST },
	{ "CT",     "EVENTTITLE:CONF",   SIMPLE,  LEVEL_MAIN },
	{ "CY",     "EVENTTITLE:CONF",   SIMPLE,  LEVEL_MAIN },
	{ "CL",     "EVENTTITLE:CONF",   SIMPLE,  LEVEL_MAIN },
	{ "PU",     "PUBLISHER", SIMPLE, LEVEL_HOST },
	{ "PI",     "ADDRESS",   SIMPLE, LEVEL_HOST }, /* publisher city */
	{ "PA",     "ADDRESS",   SIMPLE, LEVEL_HOST }, /* publisher address */
	{ "C1",     "ADDRESS",   SIMPLE, LEVEL_MAIN}, /* author organisation and address */
	{ "RP",     "REPRINTADDRESS", SIMPLE, LEVEL_MAIN },
	{ "PY",     "PARTYEAR",  SIMPLE, LEVEL_MAIN },
	{ "PD",     "PARTMONTH", SIMPLE, LEVEL_MAIN },
	{ "BP",     "PAGESTART", SIMPLE, LEVEL_MAIN },
	{ "EP",     "PAGEEND",   SIMPLE, LEVEL_MAIN },
	{ "IS",     "ISSUE",     SIMPLE, LEVEL_MAIN },
	{ "AR",     "ARTICLENUMBER", SIMPLE, LEVEL_MAIN }, /* AR=article number that Phys. Rev. B uses instead of page numbers */
	{ "BN",     "ISBN", SIMPLE, LEVEL_MAIN },
	{ "SE",     "TITLE",     TITLE,  LEVEL_SERIES }, /* series */
	{ "VL",     "SERIALNUMBER", SERIALNO, LEVEL_SERIES },
	{ "SN",     "ISSN", SIMPLE, LEVEL_SERIES },
	{ "EI",     "ISSN", SIMPLE, LEVEL_SERIES },
	{ "AB",     "ABSTRACT",  SIMPLE, LEVEL_MAIN },
	{ "NF",     "NOTES",     SIMPLE, LEVEL_MAIN },
	{ "DE",     "KEYWORD",   ISI_KEYWORD, LEVEL_MAIN }, /* author keywords */
	{ "LA",     "LANGUAGE",  SIMPLE, LEVEL_MAIN },
	{ "NR",     "NUMBERREFS", SIMPLE, LEVEL_MAIN },
	{ "CR",     "CITEDREFS",  SIMPLE, LEVEL_MAIN },
	{ "PT",     " ",         TYPE,   LEVEL_HOST },
	{ "DT",     "DOCUMENTTYPE", SIMPLE, LEVEL_MAIN },
	{ "GA",     "ISIDELIVERNUM", SIMPLE, LEVEL_MAIN}, /*ISI document delivery number */
	{ "UT",     "ISIREFNUM", SIMPLE, LEVEL_MAIN }, /* ISI unique article identifer */
	{ "DI",     "DOI",       SIMPLE, LEVEL_MAIN },
	{ "OI",     "ORCID",       SIMPLE, LEVEL_MAIN },
	{ "PM",     "PMID",       SIMPLE, LEVEL_MAIN },
	{ " ",      "INTERNAL_TYPE|INBOOK",      ALWAYS, LEVEL_MAIN },
	{ " ",      "ISSUANCE|monographic",   ALWAYS, LEVEL_HOST },
	{ " ",      "RESOURCE|text",          ALWAYS, LEVEL_MAIN },
	{ " ",      "GENRE|conferencePaper",       ALWAYS, LEVEL_MAIN }
};

#define ORIG(a) ( &(a[0]) )
#define SIZE(a) ( sizeof( a ) / sizeof( lookups ) )
#define REFTYPE(a,b) { a, ORIG(b), SIZE(b) }

variants isi_all[] = {
	REFTYPE( "Journal", article ),
	REFTYPE( "J", article ),
	REFTYPE( "Book", book ),
	REFTYPE( "B", book ),
	REFTYPE( "Chapter", inbook ),
	REFTYPE( "S", bookinseries ),
};

int isi_nall = sizeof( isi_all ) / sizeof( variants );