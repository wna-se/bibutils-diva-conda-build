POSTFIX=_osx_x86_64
CC = cc
RANLIB=ranlib -s
INSTALLDIR=/usr/local/bin

VERSION=4.9
DATE=4/15/10

PROGRAMS=bib2xml ris2xml end2xml endx2xml med2xml isi2xml copac2xml \
	biblatex2xml ebi2xml wordbib2xml \
	xml2ads xml2bib xml2end xml2isi xml2ris xml2wordbib modsclean

all : FORCE
	cd lib; make -k $(CC) -k $(RANLIB); cd ..
	cd bin; make -k $(CC) -k VERSION="$(VERSION)" -k DATE="$(DATE)"; cd ..

clean: FORCE
	cd lib     ; make clean ; cd ..
	cd bin     ; make clean ; cd ..
	cd test    ; make clean ; cd ..

realclean: FORCE
	cd lib     ; make realclean ; cd ..
	cd bin     ; make realclean ; cd ..
	cd test    ; make realclean ; cd ..
	rm -rf update lib/bibutils.pc

test: all FORCE
	cd lib    ; make test; cd ..
	cd bin    ; make test; cd ..

install: all FORCE
	sed 's/VERSION/${VERSION}/g' packageconfig_start > lib/bibutils.pc
	@for p in ${PROGRAMS}; \
		do ( cp bin/$$p ${INSTALLDIR}/$$p ); \
	done

package: all FORCE
	csh -f maketgz.csh ${VERSION} ${POSTFIX}

deb: all FORCE
	csh -f makedeb.csh ${VERSION} ${POSTFIX}

FORCE:
