# Generated automatically from Makefile.in by configure.
SHELL = /bin/sh


srcdir = .

prefix= /usr/local
exec_prefix = $(prefix)
BINDIR = $(exec_prefix)/bin
LIBDIR = $(prefix)/lib
mandir = $(prefix)/man/man1

INSTALL = ./install-sh -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

XINCLUDES=-I/usr/openwin/include $(XRTINCLUDE) -I/usr/local/include
XLIBS=-L/usr/openwin/lib $(XRTLIBS)

CPPFLAGS= -I/usr/include/X11
CFLAGS=$(XINCLUDES) -Ilib -g -I. 
LDFLAGS= -L/usr/openwin/lib -L${exec_prefix}/lib -Lreadline  -L.

XRTINCLUDE=  
XRTLIBS = 


CC     = gcc
DEFS   = -DHAVE_CONFIG_H 
LIBS   = $(XLIBS) -lplplotFX -lMagick -ltiff -lproj -lreadline -ltermcap  -ldpstk -ldps -lpng -ljpeg -lmsss_vis -lusds -lhdf5 -lz -lXm -lXext -lXt -lX11 -lm  -ldl  -lmodsupp
AR     = ar
RANLIB = ranlib

.c.o:
	$(CC) -c $(CPPFLAGS) $(DEFS) $(CFLAGS) $<

###
### If you are unable to compile readline, comment the following three lines
###
READLINE_OBJ=
READLINE_LIB=

#############################################################################
CONFIG=Makefile.in configure.in configure config.guess config.sub config.h.in install-sh
DIST=README README_FF *.[chyl] ${CONFIG}
DISTDIR=tests docs 

MODULE_SUPP_OBJS=ff_modules.o module_io.o

MOD_SUPP_LIB=libmodsupp.a
MOD_SUPP_LIB_OBJS=error.o ff_struct.o darray.o avl.o shared_globals.o

OBJ=p.o pp.o symbol.o \
	ff.o ff_ascii.o ff_cluster.o ff_display.o ff_gnoise.o ff_gplot.o \
	ff_load.o ff_rgb.o ff_random.o ff_source.o ff_version.o ff_write.o \
	reserved.o array.o string.o pp_math.o rpos.o init.o help.o \
	io_grd.o io_isis.o io_lablib3.o io_pnm.o io_specpr.o io_vicar.o \
	io_aviris.o io_imath.o io_magic.o io_themis.o\
	ff_moment.o io_ascii.o ff_interp.o ff_projection.o \
	lexer.o parser.o main.o fit.o system.o misc.o ufunc.o scope.o \
	ff_header.o ff_text.o io_ers.o io_goes.o ff_bbr.o ff_vignette.o \
	ff_pause.o printf.o ff_ifill.o ff_xfrm.o newfunc.o ff_ix.o ff_avg.o \
	ff_sort.o ff_fft.o fft.o matrix.o fft_mayer.o dct.o fft2f.o \
	x.o xrt_print_3d.o motif_tools.o ff_convolve.o apifunc.o \
	ff_plplot.o ff_pca.o ff_loadvan.o tools.o header.o \
	io_pds.o io_hdf.o ff_pbm.o ff_meta.o $(MODULE_SUPP_OBJS)

all:	 davinci gplot

davinci:	$(OBJ)  $(READLINE_OBJ) $(MOD_SUPP_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@ $(READLINE_LIB) $(LIBS)

$(MOD_SUPP_LIB): $(MOD_SUPP_LIB_OBJS)
	$(AR) qv $(MOD_SUPP_LIB) $(MOD_SUPP_LIB_OBJS)

ff_version.o:	build.h

build.h:	$(OBJ:ff_version.o=)
	echo 'char *build = "'`date`'";' > build.h
	echo 'char *builder = "'`whoami`'@'`hostname`'";' >> build.h

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
	-(cd modules ; make clean)

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
	$(CC) $(CFLAGS) gplot.o system.o -o $@ $(LDFLAGS) -Llib -lXfred $(LIBS) $(XLIBS) -lX11

lib/libXfred.a:
	@cd lib ; $(MAKE)

depend:
	@gcc -MM $(OBJ:.o=.c)

x.o:    x.c
	$(CC) -c $(CFLAGS) $(XRTINCLUDE) $<

xrt_print_3d.o: xrt_print_3d.c
	$(CC) -c $(CFLAGS) $(XRTINCLUDE) $<



#########################################################################
p.o: p.c parser.h config.h system.h darray.h avl.h ff_modules.h \
 ufunc.h scope.h func.h
pp.o: pp.c parser.h config.h system.h darray.h avl.h ff_modules.h \
 ufunc.h scope.h func.h
symbol.o: symbol.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
error.o: error.c
ff.o: ff.c ff.h parser.h config.h system.h darray.h avl.h ff_modules.h \
 ufunc.h scope.h func.h apidef.h 
ff_ascii.o: ff_ascii.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_cluster.o: ff_cluster.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_display.o: ff_display.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_gnoise.o: ff_gnoise.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_gplot.o: ff_gplot.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_load.o: ff_load.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h io_specpr.h
ff_rgb.o: ff_rgb.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_random.o: ff_random.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_source.o: ff_source.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_version.o: ff_version.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h version.h
ff_write.o: ff_write.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
reserved.o: reserved.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
array.o: array.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
string.o: string.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
pp_math.o: pp_math.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
rpos.o: rpos.c parser.h config.h system.h darray.h avl.h ff_modules.h \
 ufunc.h scope.h func.h
init.o: init.c
help.o: help.c parser.h config.h system.h darray.h avl.h ff_modules.h \
 ufunc.h scope.h func.h help.h
io_grd.o: io_grd.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
io_isis.o: io_isis.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h io_lablib3.h toolbox.h
io_lablib3.o: io_lablib3.c header.h tools.h parser.h config.h system.h \
 darray.h avl.h ff_modules.h ufunc.h scope.h func.h proto.h \
 io_lablib3.h toolbox.h
io_pnm.o: io_pnm.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
io_specpr.o: io_specpr.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h io_specpr.h
io_vicar.o: io_vicar.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h io_vicar.h
io_aviris.o: io_aviris.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h io_vicar.h
io_imath.o: io_imath.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
io_magic.o: io_magic.c config.h parser.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
io_themis.o: io_themis.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_moment.o: ff_moment.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
io_ascii.o: io_ascii.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_interp.o: ff_interp.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_projection.o: ff_projection.c parser.h config.h system.h darray.h \
 avl.h ff_modules.h ufunc.h scope.h func.h \
 /usr/local/include/projects.h /usr/local/include/my_list.h
lexer.o: lexer.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h y_tab.h
parser.o: parser.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
main.o: main.c parser.h config.h system.h darray.h avl.h ff_modules.h \
 ufunc.h scope.h func.h y_tab.h
fit.o: fit.c parser.h config.h system.h darray.h avl.h ff_modules.h \
 ufunc.h scope.h func.h fit.h
system.o: system.c config.h
misc.o: misc.c parser.h config.h system.h darray.h avl.h ff_modules.h \
 ufunc.h scope.h func.h
ufunc.o: ufunc.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
scope.o: scope.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_header.o: ff_header.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h io_specpr.h
ff_text.o: ff_text.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
io_ers.o: io_ers.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
io_goes.o: io_goes.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h io_goes.h
ff_bbr.o: ff_bbr.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_vignette.o: ff_vignette.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_pause.o: ff_pause.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
printf.o: printf.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_ifill.o: ff_ifill.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_xfrm.o: ff_xfrm.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
newfunc.o: newfunc.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_ix.o: ff_ix.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_avg.o: ff_avg.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_sort.o: ff_sort.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_fft.o: ff_fft.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h fft.h
fft.o: fft.c fft.h
matrix.o: matrix.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
fft_mayer.o: fft_mayer.c trigtbl.h
dct.o: dct.c parser.h config.h system.h darray.h avl.h ff_modules.h \
 ufunc.h scope.h func.h
fft2f.o: fft2f.c
x.o: x.c parser.h config.h system.h darray.h avl.h ff_modules.h \
 ufunc.h scope.h func.h
xrt_print_3d.o: xrt_print_3d.c
motif_tools.o: motif_tools.c
ff_convolve.o: ff_convolve.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_struct.o: ff_struct.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
apifunc.o: apifunc.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h api.h apidef.h
ff_plplot.o: ff_plplot.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h api_extern_defs.h
ff_pca.o: ff_pca.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
ff_loadvan.o: ff_loadvan.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
tools.o: tools.c tools.h parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
header.o: header.c header.h tools.h parser.h config.h system.h \
 darray.h avl.h ff_modules.h ufunc.h scope.h func.h proto.h \
 io_lablib3.h toolbox.h
io_pds.o: io_pds.c header.h tools.h parser.h config.h system.h \
 darray.h avl.h ff_modules.h ufunc.h scope.h func.h proto.h \
 io_lablib3.h toolbox.h
io_hdf.o: io_hdf.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
darray.o: darray.c darray.h avl.h
avl.o: avl.c avl.h
ff_modules.o: ff_modules.c parser.h config.h system.h darray.h avl.h \
 ff_modules.h ufunc.h scope.h func.h
