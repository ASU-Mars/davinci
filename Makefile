# Generated automatically from Makefile.in by configure.
SHELL = /bin/sh

CFLAGS=-g $(XINCLUDES)

srcdir = .

prefix= /usr/local
exec_prefix = $(prefix)
BINDIR = $(exec_prefix)/bin
LIBDIR = $(prefix)/lib
mandir = $(prefix)/man/man1

INSTALL = ./install-sh -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

XINCLUDES=-I/usr/openwin/include $(XRTINCLUDE)
XLIBS=-L/usr/openwin/lib $(XRTLIBS)

XRTINCLUDE=  
XRTLIBS = 


CC     = gcc
DEFS   = -DHAVE_CONFIG_H -Ilib
LIBS   = $(READLINE_LIB) -lplplotFX $(XLIBS) -lMagick -ltiff -lproj -lz -lreadline -ltermcap -ljpeg -lusds -lhdf5 -lXm -lXext -lXt -lX11 -lm 

.c.o:
	$(CC) -c $(CPPFLAGS) $(DEFS) $(CFLAGS) $<

###
### If you are unable to compile readline, comment the following three lines
###
READLINE_OBJ=
READLINE_LIB=

#############################################################################
CONFIG=Makefile.in configure.in configure config.h.in install-sh
DIST=README README_FF *.[chyl] ${CONFIG}
DISTDIR=tests docs 

OBJ=p.o pp.o symbol.o error.o \
	ff.o ff_ascii.o ff_cluster.o ff_display.o ff_gnoise.o ff_gplot.o \
	ff_load.o ff_rgb.o ff_random.o ff_source.o ff_version.o ff_write.o \
	reserved.o array.o string.o pp_math.o rpos.o init.o help.o \
	io_grd.o io_isis.o io_lablib3.o io_pnm.o io_specpr.o io_vicar.o \
	io_aviris.o io_imath.o io_magic.o io_themis.o\
	ff_moment.o io_ascii.o ff_interp.o ff_projection.o\
	lexer.o parser.o main.o fit.o system.o misc.o ufunc.o scope.o \
	ff_header.o ff_text.o io_ers.o io_goes.o ff_bbr.o ff_vignette.o \
	ff_pause.o printf.o ff_ifill.o ff_xfrm.o newfunc.o ff_ix.o ff_avg.o \
	ff_sort.o ff_fft.o fft.o matrix.o fft_mayer.o dct.o fft2f.o \
	x.o xrt_print_3d.o motif_tools.o ff_convolve.o ff_struct.o apifunc.o \
	ff_plplot.o ff_pca.o ff_loadvan.o tools.o header.o \
	io_pds.o io_hdf.o

all:	 davinci gplot

davinci:	$(OBJ)  $(READLINE_OBJ) 
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@ $(READLINE_LIB) $(LIBS)

readline/libreadline.a:
	@(cd readline ; make )

lexer.o:	lexer.c parser.o

lexer.c:	lexer.l
	flex lexer.l
	mv lex.yy.c lexer.c

parser.c:	parser.y
	/opt/local/alt/bin/bison -d parser.y
	mv parser.tab.c parser.c
	mv parser.tab.h y_tab.h

install:
	cp davinci $(BINDIR)
	-mkdir $(LIBDIR)
	cp docs/dv.gih $(LIBDIR)/dv.gih
	cp gplot $(BINDIR)

# 
# File specific dependancies
#
help.o:	help.c
	$(CC) $(CFLAGS) -DHELPFILE=\"$(LIBDIR)/dv.gih\" -c $*.c

clean:
	-rm -f *.o davinci dv core gplot TAGS config.cache config.h config.log
	-rm -f .dvlog */.dvlog .nfs* */.nfs* *~ */*~
	-(cd readline ; make clean)
	-(cd lib ; make clean)

dist:	clean
	echo davinci-`head -1 version.h | sed -e 's/.* Version #//;s/".*//'` > .ver
	rm -rf `cat .ver` `cat .ver`.tar.Z
	rm -rf readline/config.cache
	mkdir `cat .ver`
	ln $(DIST) `cat .ver`
	(cd `cat .ver` ; ln -s ../docs ../tests ../readline ../lib .)
	tar chf `cat .ver`.tar `cat .ver`
	compress `cat .ver`.tar
	rm -rf `cat .ver` .ver

win32dist:	clean
		echo davinci-`head -1 version.h | sed -e 's/.* Version #//;s/".*//'` > .ver
		rm -rf `cat .ver` `cat .ver`.zip
		mkdir `cat .ver`
		ln $(DIST) `cat .ver`
		(cd `cat .ver`; mkdir readline; cd readline; ln -s ../../readline/readline.h; ln \
			-s ../../readline/history.h)
		(cd `cat .ver` ; ln -s ../docs ../tests ../win32 .)
		zip -r `cat .ver`.zip `cat .ver`
		rm -rf `cat .ver` .ver

binary:	davinci gplot
	strip davinci
	strip gplot
	tar cf - davinci gplot | compress > `./config.guess`.tar.Z
	echo `./config.guess`

gplot.o:	gplot.c
	$(CC) -c $(CPPFLAGS) $(DEFS) $(XINCLUDES) -Ilib $(CFLAGS) $< 

gplot:	gplot.o lib/libXfred.a system.o
	$(CC) $(CFLAGS) gplot.o system.o -o $@ -Llib -lXfred $(LIBS) $(XLIBS) -lX11

lib/libXfred.a:
	@cd lib ; $(MAKE)

depend:
	gcc -MM $(OBJ:.o=.c)

x.o:    x.c
	$(CC) -c $(CFLAGS) $(XRTINCLUDE) $?

xrt_print_3d.o: xrt_print_3d.c
	$(CC) -c $(CFLAGS) $(XRTINCLUDE) $?



#########################################################################
p.o: parser.h config.h system.h ufunc.h scope.h func.h
pp.o: parser.h config.h system.h ufunc.h scope.h func.h
symbol.o: parser.h config.h system.h ufunc.h scope.h func.h
ff.o: ff.h parser.h config.h system.h ufunc.h scope.h func.h apidef.h 
ff_ascii.o: parser.h config.h system.h ufunc.h scope.h func.h
error.o: 
ff_cluster.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_display.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_gnoise.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_gplot.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_load.o: parser.h config.h system.h ufunc.h scope.h func.h io_specpr.h
ff_rgb.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_random.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_source.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_version.o: parser.h config.h system.h ufunc.h scope.h func.h version.h
ff_write.o: parser.h config.h system.h ufunc.h scope.h func.h
reserved.o: parser.h config.h system.h ufunc.h scope.h func.h
array.o: parser.h config.h system.h ufunc.h scope.h func.h
string.o: parser.h config.h system.h ufunc.h scope.h func.h
pp_math.o: parser.h config.h system.h ufunc.h scope.h func.h
rpos.o: parser.h config.h system.h ufunc.h scope.h func.h
init.o: 
help.o: parser.h config.h system.h ufunc.h scope.h func.h help.h
io_grd.o: parser.h config.h system.h ufunc.h scope.h func.h
io_isis.o: parser.h config.h system.h ufunc.h scope.h func.h io_lablib3.h toolbox.h
io_lablib3.o: io_lablib3.h toolbox.h
io_pnm.o: parser.h config.h system.h ufunc.h scope.h func.h
io_specpr.o: parser.h config.h system.h ufunc.h scope.h func.h io_specpr.h
io_themis.o:parser.h config.h system.h ufunc.h scope.h func.h 
io_vicar.o: parser.h config.h system.h ufunc.h scope.h func.h io_vicar.h
io_aviris.o: parser.h config.h system.h ufunc.h scope.h func.h io_vicar.h
io_imath.o: parser.h config.h system.h ufunc.h scope.h func.h
input.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_moment.o: parser.h config.h system.h ufunc.h scope.h func.h
io_ascii.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_interp.o: parser.h config.h system.h ufunc.h scope.h func.h
lexer.o: parser.h config.h system.h ufunc.h scope.h func.h y_tab.h
parser.o: parser.h config.h system.h ufunc.h scope.h func.h
main.o: parser.h config.h system.h ufunc.h scope.h func.h
fit.o: parser.h config.h system.h ufunc.h scope.h func.h fit.h
system.o: config.h
misc.o: parser.h config.h system.h ufunc.h scope.h func.h
ufunc.o: parser.h config.h system.h ufunc.h scope.h func.h
scope.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_header.o: parser.h config.h system.h ufunc.h scope.h func.h io_specpr.h
ff_text.o: parser.h config.h system.h ufunc.h scope.h func.h
io_ers.o: parser.h config.h system.h ufunc.h scope.h func.h
io_goes.o: parser.h config.h system.h ufunc.h scope.h func.h io_goes.h
ff_bbr.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_vignette.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_pause.o: parser.h config.h system.h ufunc.h scope.h func.h
printf.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_ifill.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_xfrm.o: parser.h config.h system.h ufunc.h scope.h func.h
newfunc.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_ix.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_avg.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_sort.o: parser.h config.h system.h ufunc.h scope.h func.h
ff_fft.o: parser.h config.h system.h ufunc.h scope.h func.h fft.h
fft.o: fft.h
matrix.o: parser.h config.h system.h ufunc.h scope.h func.h
dct.o: parser.h config.h system.h ufunc.h scope.h func.h
fft_mayer.o: trigtbl.h
ff_projection.o: parser.h config.h system.h ufunc.h scope.h func.h
apifunc.o: parser.h config.h system.h ufunc.h scope.h func.h api.h apidef.h
ff_pca.o: ff_pca.c parser.h config.h system.h ufunc.h scope.h func.h 
io_pds.o: parser.h config.h system.h io_lablib3.h
ff_loadvan.o: ff_loadvan.c parser.h config.h system.h ufunc.h scope.h func.h 
io_pds.o: parser.h config.h system.h header.h io_lablib3.h
header.o: header.h tools.h parser.h config.h system.h io_lablib3.h
tools.o: tools.h 
