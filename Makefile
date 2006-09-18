# Makefile.in generated by automake 1.7.8 from Makefile.am.
# Makefile.  Generated from Makefile.in by configure.

# Copyright 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003
# Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.



srcdir = .
top_srcdir = .

pkgdatadir = $(datadir)/davinci
pkglibdir = $(libdir)/davinci
pkgincludedir = $(includedir)/davinci
top_builddir = .

am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
INSTALL = /usr/bin/install -c
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_HEADER = $(INSTALL_DATA)
transform = $(program_transform_name)
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_triplet = i686-pc-linux-gnu
ACLOCAL = ${SHELL} /u/gorelick/davinci/config/missing --run aclocal-1.9
AMDEP_FALSE = #
AMDEP_TRUE = 
AMTAR = ${SHELL} /u/gorelick/davinci/config/missing --run tar
AR = ar
AUTOCONF = ${SHELL} /u/gorelick/davinci/config/missing --run autoconf
AUTOHEADER = ${SHELL} /u/gorelick/davinci/config/missing --run autoheader
AUTOMAKE = ${SHELL} /u/gorelick/davinci/config/missing --run automake-1.9
AWK = gawk
CC = gcc
CCDEPMODE = depmode=gcc3
CFLAGS = -g -O2  -I/usr/X11R6/include -I$(top_srcdir)/iomedley -I${top_srcdir}/libltdl -I$(top_srcdir)/vicar
CPP = gcc -E
CPPFLAGS =  -I/usr/local/isis/isisr/inc
CXX = g++
CXXCPP = g++ -E
CXXDEPMODE = depmode=gcc3
CXXFLAGS = -g -O2
CYGPATH_W = echo
DEFS = -DHAVE_CONFIG_H
DEPDIR = .deps
ECHO = echo
ECHO_C = 
ECHO_N = -n
ECHO_T = 
EGREP = grep -E
EXEEXT = 
F77 = g77
FFLAGS = -g -O2
HAVE_LIBMAGICK_FALSE = #
HAVE_LIBMAGICK_TRUE = 
HAVE_MOTIF_FALSE = #
HAVE_MOTIF_TRUE = 
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
LDFLAGS =  -L/usr/local/isis/isisr/lib
LIBOBJS = 
LIBS = -lXmu -lqmv -lcfitsio -lz -lreadline -lhdf5 -lltdl -ltermcap -lusds -lm -lXm -lXt -lX11  -lSM -lICE  -L/usr/X11R6/lib   -lisis -lisisbind -lisisgen -lg2c
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
LOCAL_IOMEDLEY_FALSE = #
LOCAL_IOMEDLEY_TRUE = 
LOCAL_READLINE_FALSE = 
LOCAL_READLINE_TRUE = #
LOCAL_VICAR_FALSE = #
LOCAL_VICAR_TRUE = 
LTLIBOBJS = 
MAINT = #
MAINTAINER_MODE_FALSE = 
MAINTAINER_MODE_TRUE = #
MAKEINFO = ${SHELL} /u/gorelick/davinci/config/missing --run makeinfo
MODULE_SUPPORT_FALSE = #
MODULE_SUPPORT_TRUE = 
MY_SUBDIRS =  iomedley modules libltdl modules/thm vicar modules/gui
OBJEXT = o
PACKAGE = davinci
PACKAGE_BUGREPORT = 
PACKAGE_NAME = davinci
PACKAGE_STRING = davinci 1.21
PACKAGE_TARNAME = davinci
PACKAGE_VERSION = 1.21
PATH_SEPARATOR = :
RANLIB = ranlib
SET_MAKE = 
SHELL = /bin/sh
STRIP = strip
VERSION = 1.21
X_CFLAGS =  -I/usr/X11R6/include
X_EXTRA_LIBS = 
X_LIBS =  -L/usr/X11R6/lib
X_PRE_LIBS =  -lSM -lICE
ac_ct_AR = ar
ac_ct_CC = gcc
ac_ct_CXX = g++
ac_ct_F77 = g77
ac_ct_RANLIB = ranlib
ac_ct_STRIP = strip
am__fastdepCC_FALSE = #
am__fastdepCC_TRUE = 
am__fastdepCXX_FALSE = #
am__fastdepCXX_TRUE = 
am__include = include
am__leading_dot = .
am__quote = 
am__tar = ${AMTAR} chof - "$$tardir"
am__untar = ${AMTAR} xf -
bindir = ${exec_prefix}/bin
build = i686-pc-linux-gnu
build_alias = 
build_cpu = i686
build_os = linux-gnu
build_vendor = pc
datadir = ${prefix}/share
exec_prefix = ${prefix}
host = i686-pc-linux-gnu
host_alias = 
host_cpu = i686
host_os = linux-gnu
host_vendor = pc
includedir = ${prefix}/include
infodir = ${prefix}/info
install_sh = /u/gorelick/davinci/config/install-sh
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localstatedir = ${prefix}/var
mandir = ${prefix}/man
mkdir_p = mkdir -p --
oldincludedir = /usr/include
prefix = /usr/local
program_transform_name = s,x,x,
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
subdirs =  iomedley libltdl
sysconfdir = ${prefix}/etc
target = i686-pc-linux-gnu
target_alias = 
target_cpu = i686
target_os = linux-gnu
target_vendor = pc

EXTRA_DIST = README.modules README_FF docs gplot.c lexer.l lib moddvmagick parser.y push.skel tests win32

MY_MODULES_LIB = libmodsupp.a

#MY_READLINE_LDADD = readline/libreadline.a

MY_IOMEDLEY_LDADD = -Liomedley -liomedley

SUBDIRS =  iomedley modules libltdl modules/thm vicar modules/gui libltdl

bin_PROGRAMS = davinci

BUILT_SOURCES = config.h build.h

noinst_LIBRARIES = $(MY_MODULES_LIB)

libmodsupp_a_SOURCES = ff_modules.c module_io.c io_loadmod.c

davinci_LDADD = $(MY_READLINE_LDADD) $(MY_IOMEDLEY_LDADD) $(MY_MODULES_LIB) 
davinci_LDFLAGS = -export-dynamic

davinci_SOURCES = apifunc.c array.c avl.c binDCT.c \
	darray.c \
	dct.c deghost.C dvio.c dvio_ascii.c \
	dvio_aviris.c dvio_envi.c dvio_ers.c dvio_goes.c dvio_grd.c \
	dvio_hdf.c dvio_imath.c dvio_iomedley.c dvio_isis.c dvio_magic.c \
	dvio_pds.c dvio_pnm.c dvio_raw.c dvio_specpr.c \
	dvio_themis.c dvio_vicar.c endian_norm.c \
	error.c ff.c ff_ascii.c ff_avg.c \
	ff_bbr.c ff_cluster.c ff_convolve.c ff_deghost.c ff_display.c \
	ff_fft.c ff_filetype.c ff_fncc.c ff_gnoise.c ff_gplot.c \
	ff_header.c ff_hstretch.c ff_ifill.c ff_interp.c ff_ix.c \
	ff_load.c ff_loadvan.c ff_meta.c \
	ff_moment.c ff_pause.c ff_pbm.c ff_pca.c ff_plplot.c \
	ff_projection.c ff_random.c ff_raw.c ff_rgb.c ff_rice.c \
	ff_shade.c ff_slant.c ff_sort.c ff_source.c ff_struct.c \
	ff_text.c ff_version.c ff_vignette.c ff_write.c ff_xfrm.c \
	fft.c fft2f.c fft_mayer.c fit.c ff_coreg.c ff_boxfilter.c \
	ff_bin5.c \
	header.c \
	help.c init.c \
	isis_obj.c main.c matrix.c \
	misc.c motif_tools.c \
	newfunc.c p.c pp.c \
	pp_math.c printf.c reserved.c \
	rfunc.c rice.c rpos.c scope.c \
	spread.c string.c symbol.c \
	system.c tools.c \
	ufunc.c util.c \
	x.c xrt_print_3d.c \
	XformTable.h darray.h fit.h trigtbl.h api.h dos.h func.h \
	products.h ufunc.h api_extern_defs.h dvio.h header.h \
	proto.h version.h apidef.h dvio_specpr.h help.h rfunc.h \
	xrt_print_3d.h avl.h endian_norm.h io_loadmod.h scope.h \
	ff.h isis_obj.h system.h ff_modules.h module_io.h \
	toolbox.h configwin.h fft.h motif_tools.h tools.h \
	lexer.c parser.c parser.h y_tab.h \
	ff_gconvolve.c ff_warp.c ff_median.c ff_radial.c ff_window.c \
	dvio_fits.c ff_extract.c dvio_tdb.c

subdir = .
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
mkinstalldirs = $(SHELL) $(top_srcdir)/config/mkinstalldirs
CONFIG_HEADER = config.h
CONFIG_CLEAN_FILES =
LIBRARIES = $(noinst_LIBRARIES)

libmodsupp_a_AR = $(AR) cru
libmodsupp_a_LIBADD =
am_libmodsupp_a_OBJECTS = ff_modules.$(OBJEXT) module_io.$(OBJEXT) \
	io_loadmod.$(OBJEXT)
libmodsupp_a_OBJECTS = $(am_libmodsupp_a_OBJECTS)
bin_PROGRAMS = davinci$(EXEEXT)
PROGRAMS = $(bin_PROGRAMS)

am_davinci_OBJECTS = apifunc.$(OBJEXT) array.$(OBJEXT) avl.$(OBJEXT) \
	binDCT.$(OBJEXT) darray.$(OBJEXT) dct.$(OBJEXT) \
	deghost.$(OBJEXT) dvio.$(OBJEXT) dvio_ascii.$(OBJEXT) \
	dvio_aviris.$(OBJEXT) dvio_envi.$(OBJEXT) dvio_ers.$(OBJEXT) \
	dvio_goes.$(OBJEXT) dvio_grd.$(OBJEXT) dvio_hdf.$(OBJEXT) \
	dvio_imath.$(OBJEXT) dvio_iomedley.$(OBJEXT) \
	dvio_isis.$(OBJEXT) dvio_magic.$(OBJEXT) dvio_pds.$(OBJEXT) \
	dvio_pnm.$(OBJEXT) dvio_raw.$(OBJEXT) dvio_specpr.$(OBJEXT) \
	dvio_themis.$(OBJEXT) dvio_vicar.$(OBJEXT) \
	endian_norm.$(OBJEXT) error.$(OBJEXT) ff.$(OBJEXT) \
	ff_ascii.$(OBJEXT) ff_avg.$(OBJEXT) ff_bbr.$(OBJEXT) \
	ff_cluster.$(OBJEXT) ff_convolve.$(OBJEXT) ff_deghost.$(OBJEXT) \
	ff_display.$(OBJEXT) ff_fft.$(OBJEXT) ff_filetype.$(OBJEXT) \
	ff_fncc.$(OBJEXT) ff_gnoise.$(OBJEXT) ff_gplot.$(OBJEXT) \
	ff_header.$(OBJEXT) ff_hstretch.$(OBJEXT) ff_ifill.$(OBJEXT) \
	ff_interp.$(OBJEXT) ff_ix.$(OBJEXT) ff_load.$(OBJEXT) \
	ff_loadvan.$(OBJEXT) ff_meta.$(OBJEXT) ff_moment.$(OBJEXT) \
	ff_pause.$(OBJEXT) ff_pbm.$(OBJEXT) ff_pca.$(OBJEXT) \
	ff_plplot.$(OBJEXT) ff_projection.$(OBJEXT) ff_random.$(OBJEXT) \
	ff_raw.$(OBJEXT) ff_rgb.$(OBJEXT) ff_rice.$(OBJEXT) \
	ff_shade.$(OBJEXT) ff_slant.$(OBJEXT) ff_sort.$(OBJEXT) \
	ff_source.$(OBJEXT) ff_struct.$(OBJEXT) ff_text.$(OBJEXT) \
	ff_version.$(OBJEXT) ff_vignette.$(OBJEXT) ff_write.$(OBJEXT) \
	ff_xfrm.$(OBJEXT) fft.$(OBJEXT) fft2f.$(OBJEXT) \
	fft_mayer.$(OBJEXT) fit.$(OBJEXT) ff_coreg.$(OBJEXT) \
	ff_boxfilter.$(OBJEXT) ff_bin5.$(OBJEXT) header.$(OBJEXT) \
	help.$(OBJEXT) init.$(OBJEXT) isis_obj.$(OBJEXT) main.$(OBJEXT) \
	matrix.$(OBJEXT) misc.$(OBJEXT) motif_tools.$(OBJEXT) \
	newfunc.$(OBJEXT) p.$(OBJEXT) pp.$(OBJEXT) pp_math.$(OBJEXT) \
	printf.$(OBJEXT) reserved.$(OBJEXT) rfunc.$(OBJEXT) \
	rice.$(OBJEXT) rpos.$(OBJEXT) scope.$(OBJEXT) spread.$(OBJEXT) \
	string.$(OBJEXT) symbol.$(OBJEXT) system.$(OBJEXT) \
	tools.$(OBJEXT) ufunc.$(OBJEXT) util.$(OBJEXT) x.$(OBJEXT) \
	xrt_print_3d.$(OBJEXT) lexer.$(OBJEXT) parser.$(OBJEXT) \
	ff_gconvolve.$(OBJEXT) ff_warp.$(OBJEXT) ff_median.$(OBJEXT) \
	ff_radial.$(OBJEXT) ff_window.$(OBJEXT) dvio_fits.$(OBJEXT) \
	ff_extract.$(OBJEXT) dvio_tdb.$(OBJEXT)
davinci_OBJECTS = $(am_davinci_OBJECTS)
davinci_DEPENDENCIES = \
	libmodsupp.a
#davinci_DEPENDENCIES =
#davinci_DEPENDENCIES = \
#	readline/libreadline.a \
#	libmodsupp.a
##davinci_DEPENDENCIES = \
##	readline/libreadline.a

DEFAULT_INCLUDES =  -I. -I$(srcdir) -I.
depcomp = $(SHELL) $(top_srcdir)/config/depcomp
am__depfiles_maybe = depfiles
DEP_FILES = ./$(DEPDIR)/apifunc.Po ./$(DEPDIR)/array.Po \
	./$(DEPDIR)/avl.Po ./$(DEPDIR)/binDCT.Po \
	./$(DEPDIR)/darray.Po ./$(DEPDIR)/dct.Po \
	./$(DEPDIR)/deghost.Po ./$(DEPDIR)/dvio.Po \
	./$(DEPDIR)/dvio_ascii.Po \
	./$(DEPDIR)/dvio_aviris.Po ./$(DEPDIR)/dvio_envi.Po \
	./$(DEPDIR)/dvio_ers.Po ./$(DEPDIR)/dvio_fits.Po \
	./$(DEPDIR)/dvio_goes.Po ./$(DEPDIR)/dvio_grd.Po \
	./$(DEPDIR)/dvio_hdf.Po ./$(DEPDIR)/dvio_imath.Po \
	./$(DEPDIR)/dvio_iomedley.Po \
	./$(DEPDIR)/dvio_isis.Po ./$(DEPDIR)/dvio_magic.Po \
	./$(DEPDIR)/dvio_pds.Po ./$(DEPDIR)/dvio_pnm.Po \
	./$(DEPDIR)/dvio_raw.Po ./$(DEPDIR)/dvio_specpr.Po \
	./$(DEPDIR)/dvio_tdb.Po ./$(DEPDIR)/dvio_themis.Po \
	./$(DEPDIR)/dvio_vicar.Po \
	./$(DEPDIR)/endian_norm.Po ./$(DEPDIR)/error.Po \
	./$(DEPDIR)/ff.Po ./$(DEPDIR)/ff_ascii.Po \
	./$(DEPDIR)/ff_avg.Po ./$(DEPDIR)/ff_bbr.Po \
	./$(DEPDIR)/ff_bin5.Po ./$(DEPDIR)/ff_boxfilter.Po \
	./$(DEPDIR)/ff_cluster.Po \
	./$(DEPDIR)/ff_convolve.Po ./$(DEPDIR)/ff_coreg.Po \
	./$(DEPDIR)/ff_deghost.Po ./$(DEPDIR)/ff_display.Po \
	./$(DEPDIR)/ff_extract.Po ./$(DEPDIR)/ff_fft.Po \
	./$(DEPDIR)/ff_filetype.Po ./$(DEPDIR)/ff_fncc.Po \
	./$(DEPDIR)/ff_gconvolve.Po \
	./$(DEPDIR)/ff_gnoise.Po ./$(DEPDIR)/ff_gplot.Po \
	./$(DEPDIR)/ff_header.Po ./$(DEPDIR)/ff_hstretch.Po \
	./$(DEPDIR)/ff_ifill.Po ./$(DEPDIR)/ff_interp.Po \
	./$(DEPDIR)/ff_ix.Po ./$(DEPDIR)/ff_load.Po \
	./$(DEPDIR)/ff_loadvan.Po ./$(DEPDIR)/ff_median.Po \
	./$(DEPDIR)/ff_meta.Po ./$(DEPDIR)/ff_modules.Po \
	./$(DEPDIR)/ff_moment.Po ./$(DEPDIR)/ff_pause.Po \
	./$(DEPDIR)/ff_pbm.Po ./$(DEPDIR)/ff_pca.Po \
	./$(DEPDIR)/ff_plplot.Po \
	./$(DEPDIR)/ff_projection.Po \
	./$(DEPDIR)/ff_radial.Po ./$(DEPDIR)/ff_random.Po \
	./$(DEPDIR)/ff_raw.Po ./$(DEPDIR)/ff_rgb.Po \
	./$(DEPDIR)/ff_rice.Po ./$(DEPDIR)/ff_shade.Po \
	./$(DEPDIR)/ff_slant.Po ./$(DEPDIR)/ff_sort.Po \
	./$(DEPDIR)/ff_source.Po ./$(DEPDIR)/ff_struct.Po \
	./$(DEPDIR)/ff_text.Po ./$(DEPDIR)/ff_version.Po \
	./$(DEPDIR)/ff_vignette.Po ./$(DEPDIR)/ff_warp.Po \
	./$(DEPDIR)/ff_window.Po ./$(DEPDIR)/ff_write.Po \
	./$(DEPDIR)/ff_xfrm.Po ./$(DEPDIR)/fft.Po \
	./$(DEPDIR)/fft2f.Po ./$(DEPDIR)/fft_mayer.Po \
	./$(DEPDIR)/fit.Po ./$(DEPDIR)/header.Po \
	./$(DEPDIR)/help.Po ./$(DEPDIR)/init.Po \
	./$(DEPDIR)/io_loadmod.Po ./$(DEPDIR)/isis_obj.Po \
	./$(DEPDIR)/lexer.Po ./$(DEPDIR)/main.Po \
	./$(DEPDIR)/matrix.Po ./$(DEPDIR)/misc.Po \
	./$(DEPDIR)/module_io.Po ./$(DEPDIR)/motif_tools.Po \
	./$(DEPDIR)/newfunc.Po ./$(DEPDIR)/p.Po \
	./$(DEPDIR)/parser.Po ./$(DEPDIR)/pp.Po \
	./$(DEPDIR)/pp_math.Po ./$(DEPDIR)/printf.Po \
	./$(DEPDIR)/reserved.Po ./$(DEPDIR)/rfunc.Po \
	./$(DEPDIR)/rice.Po ./$(DEPDIR)/rpos.Po \
	./$(DEPDIR)/scope.Po ./$(DEPDIR)/spread.Po \
	./$(DEPDIR)/string.Po ./$(DEPDIR)/symbol.Po \
	./$(DEPDIR)/system.Po ./$(DEPDIR)/tools.Po \
	./$(DEPDIR)/ufunc.Po ./$(DEPDIR)/util.Po \
	./$(DEPDIR)/x.Po ./$(DEPDIR)/xrt_print_3d.Po
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
LTCXXCOMPILE = $(LIBTOOL) --mode=compile $(CXX) $(DEFS) \
	$(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
	$(AM_CXXFLAGS) $(CXXFLAGS)
CXXLD = $(CXX)
CXXLINK = $(LIBTOOL) --mode=link $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) \
	$(AM_LDFLAGS) $(LDFLAGS) -o $@
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) \
	$(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) \
	$(AM_LDFLAGS) $(LDFLAGS) -o $@
DIST_SOURCES = $(libmodsupp_a_SOURCES) $(davinci_SOURCES)

RECURSIVE_TARGETS = info-recursive dvi-recursive pdf-recursive \
	ps-recursive install-info-recursive uninstall-info-recursive \
	all-recursive install-data-recursive install-exec-recursive \
	installdirs-recursive install-recursive uninstall-recursive \
	check-recursive installcheck-recursive
DIST_COMMON = README $(srcdir)/Makefile.in $(srcdir)/configure \
	Makefile.am aclocal.m4 config.h.in config/config.guess \
	config/config.sub config/depcomp config/install-sh \
	config/ltmain.sh config/missing config/mkinstalldirs configure \
	configure.ac
DIST_SUBDIRS = $(SUBDIRS)
SOURCES = $(libmodsupp_a_SOURCES) $(davinci_SOURCES)

all: $(BUILT_SOURCES) config.h
	$(MAKE) $(AM_MAKEFLAGS) all-recursive

.SUFFIXES:
.SUFFIXES: .C .c .lo .o .obj

am__CONFIG_DISTCLEAN_FILES = config.status config.cache config.log \
 configure.lineno
$(srcdir)/Makefile.in: # Makefile.am  $(top_srcdir)/configure.ac $(ACLOCAL_M4)
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --gnu  Makefile
Makefile: # $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status $@ $(am__depfiles_maybe)

$(top_builddir)/config.status: $(srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	$(SHELL) ./config.status --recheck
$(srcdir)/configure: # $(srcdir)/configure.ac $(ACLOCAL_M4) $(CONFIGURE_DEPENDENCIES)
	cd $(srcdir) && $(AUTOCONF)

$(ACLOCAL_M4): # configure.ac 
	cd $(srcdir) && $(ACLOCAL) $(ACLOCAL_AMFLAGS)

config.h: stamp-h1
	@if test ! -f $@; then \
	  rm -f stamp-h1; \
	  $(MAKE) stamp-h1; \
	else :; fi

stamp-h1: $(srcdir)/config.h.in $(top_builddir)/config.status
	@rm -f stamp-h1
	cd $(top_builddir) && $(SHELL) ./config.status config.h

$(srcdir)/config.h.in: # $(top_srcdir)/configure.ac $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOHEADER)
	touch $(srcdir)/config.h.in

distclean-hdr:
	-rm -f config.h stamp-h1

clean-noinstLIBRARIES:
	-test -z "$(noinst_LIBRARIES)" || rm -f $(noinst_LIBRARIES)
libmodsupp.a: $(libmodsupp_a_OBJECTS) $(libmodsupp_a_DEPENDENCIES) 
	-rm -f libmodsupp.a
	$(libmodsupp_a_AR) libmodsupp.a $(libmodsupp_a_OBJECTS) $(libmodsupp_a_LIBADD)
	$(RANLIB) libmodsupp.a
binPROGRAMS_INSTALL = $(INSTALL_PROGRAM)
install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  p1=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  if test -f $$p \
	     || test -f $$p1 \
	  ; then \
	    f=`echo "$$p1" | sed 's,^.*/,,;$(transform);s/$$/$(EXEEXT)/'`; \
	   echo " $(INSTALL_PROGRAM_ENV) $(LIBTOOL) --mode=install $(binPROGRAMS_INSTALL) $$p $(DESTDIR)$(bindir)/$$f"; \
	   $(INSTALL_PROGRAM_ENV) $(LIBTOOL) --mode=install $(binPROGRAMS_INSTALL) $$p $(DESTDIR)$(bindir)/$$f || exit 1; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  f=`echo "$$p" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  echo " rm -f $(DESTDIR)$(bindir)/$$f"; \
	  rm -f $(DESTDIR)$(bindir)/$$f; \
	done

clean-binPROGRAMS:
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  f=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  echo " rm -f $$p $$f"; \
	  rm -f $$p $$f ; \
	done
davinci$(EXEEXT): $(davinci_OBJECTS) $(davinci_DEPENDENCIES) 
	@rm -f davinci$(EXEEXT)
	$(CXXLINK) $(davinci_LDFLAGS) $(davinci_OBJECTS) $(davinci_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT) core *.core

distclean-compile:
	-rm -f *.tab.c

include ./$(DEPDIR)/apifunc.Po
include ./$(DEPDIR)/array.Po
include ./$(DEPDIR)/avl.Po
include ./$(DEPDIR)/binDCT.Po
include ./$(DEPDIR)/darray.Po
include ./$(DEPDIR)/dct.Po
include ./$(DEPDIR)/deghost.Po
include ./$(DEPDIR)/dvio.Po
include ./$(DEPDIR)/dvio_ascii.Po
include ./$(DEPDIR)/dvio_aviris.Po
include ./$(DEPDIR)/dvio_envi.Po
include ./$(DEPDIR)/dvio_ers.Po
include ./$(DEPDIR)/dvio_fits.Po
include ./$(DEPDIR)/dvio_goes.Po
include ./$(DEPDIR)/dvio_grd.Po
include ./$(DEPDIR)/dvio_hdf.Po
include ./$(DEPDIR)/dvio_imath.Po
include ./$(DEPDIR)/dvio_iomedley.Po
include ./$(DEPDIR)/dvio_isis.Po
include ./$(DEPDIR)/dvio_magic.Po
include ./$(DEPDIR)/dvio_pds.Po
include ./$(DEPDIR)/dvio_pnm.Po
include ./$(DEPDIR)/dvio_raw.Po
include ./$(DEPDIR)/dvio_specpr.Po
include ./$(DEPDIR)/dvio_tdb.Po
include ./$(DEPDIR)/dvio_themis.Po
include ./$(DEPDIR)/dvio_vicar.Po
include ./$(DEPDIR)/endian_norm.Po
include ./$(DEPDIR)/error.Po
include ./$(DEPDIR)/ff.Po
include ./$(DEPDIR)/ff_ascii.Po
include ./$(DEPDIR)/ff_avg.Po
include ./$(DEPDIR)/ff_bbr.Po
include ./$(DEPDIR)/ff_bin5.Po
include ./$(DEPDIR)/ff_boxfilter.Po
include ./$(DEPDIR)/ff_cluster.Po
include ./$(DEPDIR)/ff_convolve.Po
include ./$(DEPDIR)/ff_coreg.Po
include ./$(DEPDIR)/ff_deghost.Po
include ./$(DEPDIR)/ff_display.Po
include ./$(DEPDIR)/ff_extract.Po
include ./$(DEPDIR)/ff_fft.Po
include ./$(DEPDIR)/ff_filetype.Po
include ./$(DEPDIR)/ff_fncc.Po
include ./$(DEPDIR)/ff_gconvolve.Po
include ./$(DEPDIR)/ff_gnoise.Po
include ./$(DEPDIR)/ff_gplot.Po
include ./$(DEPDIR)/ff_header.Po
include ./$(DEPDIR)/ff_hstretch.Po
include ./$(DEPDIR)/ff_ifill.Po
include ./$(DEPDIR)/ff_interp.Po
include ./$(DEPDIR)/ff_ix.Po
include ./$(DEPDIR)/ff_load.Po
include ./$(DEPDIR)/ff_loadvan.Po
include ./$(DEPDIR)/ff_median.Po
include ./$(DEPDIR)/ff_meta.Po
include ./$(DEPDIR)/ff_modules.Po
include ./$(DEPDIR)/ff_moment.Po
include ./$(DEPDIR)/ff_pause.Po
include ./$(DEPDIR)/ff_pbm.Po
include ./$(DEPDIR)/ff_pca.Po
include ./$(DEPDIR)/ff_plplot.Po
include ./$(DEPDIR)/ff_projection.Po
include ./$(DEPDIR)/ff_radial.Po
include ./$(DEPDIR)/ff_random.Po
include ./$(DEPDIR)/ff_raw.Po
include ./$(DEPDIR)/ff_rgb.Po
include ./$(DEPDIR)/ff_rice.Po
include ./$(DEPDIR)/ff_shade.Po
include ./$(DEPDIR)/ff_slant.Po
include ./$(DEPDIR)/ff_sort.Po
include ./$(DEPDIR)/ff_source.Po
include ./$(DEPDIR)/ff_struct.Po
include ./$(DEPDIR)/ff_text.Po
include ./$(DEPDIR)/ff_version.Po
include ./$(DEPDIR)/ff_vignette.Po
include ./$(DEPDIR)/ff_warp.Po
include ./$(DEPDIR)/ff_window.Po
include ./$(DEPDIR)/ff_write.Po
include ./$(DEPDIR)/ff_xfrm.Po
include ./$(DEPDIR)/fft.Po
include ./$(DEPDIR)/fft2f.Po
include ./$(DEPDIR)/fft_mayer.Po
include ./$(DEPDIR)/fit.Po
include ./$(DEPDIR)/header.Po
include ./$(DEPDIR)/help.Po
include ./$(DEPDIR)/init.Po
include ./$(DEPDIR)/io_loadmod.Po
include ./$(DEPDIR)/isis_obj.Po
include ./$(DEPDIR)/lexer.Po
include ./$(DEPDIR)/main.Po
include ./$(DEPDIR)/matrix.Po
include ./$(DEPDIR)/misc.Po
include ./$(DEPDIR)/module_io.Po
include ./$(DEPDIR)/motif_tools.Po
include ./$(DEPDIR)/newfunc.Po
include ./$(DEPDIR)/p.Po
include ./$(DEPDIR)/parser.Po
include ./$(DEPDIR)/pp.Po
include ./$(DEPDIR)/pp_math.Po
include ./$(DEPDIR)/printf.Po
include ./$(DEPDIR)/reserved.Po
include ./$(DEPDIR)/rfunc.Po
include ./$(DEPDIR)/rice.Po
include ./$(DEPDIR)/rpos.Po
include ./$(DEPDIR)/scope.Po
include ./$(DEPDIR)/spread.Po
include ./$(DEPDIR)/string.Po
include ./$(DEPDIR)/symbol.Po
include ./$(DEPDIR)/system.Po
include ./$(DEPDIR)/tools.Po
include ./$(DEPDIR)/ufunc.Po
include ./$(DEPDIR)/util.Po
include ./$(DEPDIR)/x.Po
include ./$(DEPDIR)/xrt_print_3d.Po

.C.o:
	if $(CXXCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" \
	  -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; \
	else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; \
	fi
#	source='$<' object='$@' libtool=no \
#	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
#	$(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<

.C.obj:
	if $(CXXCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" \
	  -c -o $@ `if test -f '$<'; then $(CYGPATH_W) '$<'; else $(CYGPATH_W) '$(srcdir)/$<'; fi`; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; \
	else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; \
	fi
#	source='$<' object='$@' libtool=no \
#	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
#	$(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ `if test -f '$<'; then $(CYGPATH_W) '$<'; else $(CYGPATH_W) '$(srcdir)/$<'; fi`

.C.lo:
	if $(LTCXXCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" \
	  -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Plo"; \
	else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; \
	fi
#	source='$<' object='$@' libtool=yes \
#	depfile='$(DEPDIR)/$*.Plo' tmpdepfile='$(DEPDIR)/$*.TPlo' \
#	$(CXXDEPMODE) $(depcomp) \
#	$(LTCXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<

.c.o:
	if $(COMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" \
	  -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; \
	else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; \
	fi
#	source='$<' object='$@' libtool=no \
#	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
#	$(CCDEPMODE) $(depcomp) \
#	$(COMPILE) -c `test -f '$<' || echo '$(srcdir)/'`$<

.c.obj:
	if $(COMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" \
	  -c -o $@ `if test -f '$<'; then $(CYGPATH_W) '$<'; else $(CYGPATH_W) '$(srcdir)/$<'; fi`; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; \
	else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; \
	fi
#	source='$<' object='$@' libtool=no \
#	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
#	$(CCDEPMODE) $(depcomp) \
#	$(COMPILE) -c `if test -f '$<'; then $(CYGPATH_W) '$<'; else $(CYGPATH_W) '$(srcdir)/$<'; fi`

.c.lo:
	if $(LTCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" \
	  -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Plo"; \
	else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; \
	fi
#	source='$<' object='$@' libtool=yes \
#	depfile='$(DEPDIR)/$*.Plo' tmpdepfile='$(DEPDIR)/$*.TPlo' \
#	$(CCDEPMODE) $(depcomp) \
#	$(LTCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:
	-rm -f libtool
uninstall-info-am:

# This directory's subdirectories are mostly independent; you can cd
# into them and run `make' without going through this Makefile.
# To change the values of `make' variables: instead of editing Makefiles,
# (1) if the variable is set in `config.status', edit `config.status'
#     (which will cause the Makefiles to be regenerated when you run `make');
# (2) otherwise, pass the desired values on the `make' command line.
$(RECURSIVE_TARGETS):
	@set fnord $$MAKEFLAGS; amf=$$2; \
	dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) $(AM_MAKEFLAGS) "$$target-am" || exit 1; \
	fi; test -z "$$fail"

mostlyclean-recursive clean-recursive distclean-recursive \
maintainer-clean-recursive:
	@set fnord $$MAKEFLAGS; amf=$$2; \
	dot_seen=no; \
	case "$@" in \
	  distclean-* | maintainer-clean-*) list='$(DIST_SUBDIRS)' ;; \
	  *) list='$(SUBDIRS)' ;; \
	esac; \
	rev=''; for subdir in $$list; do \
	  if test "$$subdir" = "."; then :; else \
	    rev="$$subdir $$rev"; \
	  fi; \
	done; \
	rev="$$rev ."; \
	target=`echo $@ | sed s/-recursive//`; \
	for subdir in $$rev; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"
tags-recursive:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test "$$subdir" = . || (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) tags); \
	done
ctags-recursive:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test "$$subdir" = . || (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) ctags); \
	done

ETAGS = etags
ETAGSFLAGS =

CTAGS = ctags
CTAGSFLAGS =

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique

TAGS: tags-recursive $(HEADERS) $(SOURCES) config.h.in $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	if (etags --etags-include --version) >/dev/null 2>&1; then \
	  include_option=--etags-include; \
	else \
	  include_option=--include; \
	fi; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  if test "$$subdir" = .; then :; else \
	    test -f $$subdir/TAGS && \
	      tags="$$tags $$include_option=$$here/$$subdir/TAGS"; \
	  fi; \
	done; \
	list='$(SOURCES) $(HEADERS) config.h.in $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$tags$$unique" \
	  || $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	     $$tags $$unique

ctags: CTAGS
CTAGS: ctags-recursive $(HEADERS) $(SOURCES) config.h.in $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS) config.h.in $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(CTAGS_ARGS)$$tags$$unique" \
	  || $(CTAGS) $(CTAGSFLAGS) $(AM_CTAGSFLAGS) $(CTAGS_ARGS) \
	     $$tags $$unique

GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH tags
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

top_distdir = .
distdir = $(PACKAGE)-$(VERSION)

am__remove_distdir = \
  { test ! -d $(distdir) \
    || { find $(distdir) -type d ! -perm -200 -exec chmod u+w {} ';' \
         && rm -fr $(distdir); }; }

GZIP_ENV = --best
distuninstallcheck_listfiles = find . -type f -print
distcleancheck_listfiles = find . -type f -print

distdir: $(DISTFILES)
	$(am__remove_distdir)
	mkdir $(distdir)
	$(mkinstalldirs) $(distdir)/config
	@srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`; \
	topsrcdirstrip=`echo "$(top_srcdir)" | sed 's|.|.|g'`; \
	list='$(DISTFILES)'; for file in $$list; do \
	  case $$file in \
	    $(srcdir)/*) file=`echo "$$file" | sed "s|^$$srcdirstrip/||"`;; \
	    $(top_srcdir)/*) file=`echo "$$file" | sed "s|^$$topsrcdirstrip/|$(top_builddir)/|"`;; \
	  esac; \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    dir="/$$dir"; \
	    $(mkinstalldirs) "$(distdir)$$dir"; \
	  else \
	    dir=''; \
	  fi; \
	  if test -d $$d/$$file; then \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -pR $(srcdir)/$$file $(distdir)$$dir || exit 1; \
	    fi; \
	    cp -pR $$d/$$file $(distdir)$$dir || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  if test "$$subdir" = .; then :; else \
	    test -d $(distdir)/$$subdir \
	    || mkdir $(distdir)/$$subdir \
	    || exit 1; \
	    (cd $$subdir && \
	      $(MAKE) $(AM_MAKEFLAGS) \
	        top_distdir="$(top_distdir)" \
	        distdir=../$(distdir)/$$subdir \
	        distdir) \
	      || exit 1; \
	  fi; \
	done
	-find $(distdir) -type d ! -perm -777 -exec chmod a+rwx {} \; -o \
	  ! -type d ! -perm -444 -links 1 -exec chmod a+r {} \; -o \
	  ! -type d ! -perm -400 -exec chmod a+r {} \; -o \
	  ! -type d ! -perm -444 -exec $(SHELL) $(install_sh) -c -m a+r {} {} \; \
	|| chmod -R a+r $(distdir)
dist-gzip: distdir
	$(AMTAR) chof - $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)

dist dist-all: distdir
	$(AMTAR) chof - $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)

# This target untars the dist file and tries a VPATH configuration.  Then
# it guarantees that the distribution is self-contained by making another
# tarfile.
distcheck: dist
	$(am__remove_distdir)
	GZIP=$(GZIP_ENV) gunzip -c $(distdir).tar.gz | $(AMTAR) xf -
	chmod -R a-w $(distdir); chmod a+w $(distdir)
	mkdir $(distdir)/_build
	mkdir $(distdir)/_inst
	chmod a-w $(distdir)
	dc_install_base=`$(am__cd) $(distdir)/_inst && pwd | sed -e 's,^[^:\\/]:[\\/],/,'` \
	  && dc_destdir="$${TMPDIR-/tmp}/am-dc-$$$$/" \
	  && cd $(distdir)/_build \
	  && ../configure --srcdir=.. --prefix="$$dc_install_base" \
	    $(DISTCHECK_CONFIGURE_FLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) dvi \
	  && $(MAKE) $(AM_MAKEFLAGS) check \
	  && $(MAKE) $(AM_MAKEFLAGS) install \
	  && $(MAKE) $(AM_MAKEFLAGS) installcheck \
	  && $(MAKE) $(AM_MAKEFLAGS) uninstall \
	  && $(MAKE) $(AM_MAKEFLAGS) distuninstallcheck_dir="$$dc_install_base" \
	        distuninstallcheck \
	  && chmod -R a-w "$$dc_install_base" \
	  && ({ \
	       (cd ../.. && $(mkinstalldirs) "$$dc_destdir") \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" install \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" uninstall \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" \
	            distuninstallcheck_dir="$$dc_destdir" distuninstallcheck; \
	      } || { rm -rf "$$dc_destdir"; exit 1; }) \
	  && rm -rf "$$dc_destdir" \
	  && $(MAKE) $(AM_MAKEFLAGS) dist-gzip \
	  && rm -f $(distdir).tar.gz \
	  && $(MAKE) $(AM_MAKEFLAGS) distcleancheck
	$(am__remove_distdir)
	@echo "$(distdir).tar.gz is ready for distribution" | \
	  sed 'h;s/./=/g;p;x;p;x'
distuninstallcheck:
	@cd $(distuninstallcheck_dir) \
	&& test `$(distuninstallcheck_listfiles) | wc -l` -le 1 \
	   || { echo "ERROR: files left after uninstall:" ; \
	        if test -n "$(DESTDIR)"; then \
	          echo "  (check DESTDIR support)"; \
	        fi ; \
	        $(distuninstallcheck_listfiles) ; \
	        exit 1; } >&2
distcleancheck: distclean
	@if test '$(srcdir)' = . ; then \
	  echo "ERROR: distcleancheck can only run from a VPATH build" ; \
	  exit 1 ; \
	fi
	@test `$(distcleancheck_listfiles) | wc -l` -eq 0 \
	  || { echo "ERROR: files left in build directory after distclean:" ; \
	       $(distcleancheck_listfiles) ; \
	       exit 1; } >&2
check-am: all-am
check: $(BUILT_SOURCES)
	$(MAKE) $(AM_MAKEFLAGS) check-recursive
all-am: Makefile $(LIBRARIES) $(PROGRAMS) config.h
installdirs: installdirs-recursive
installdirs-am:
	$(mkinstalldirs) $(DESTDIR)$(bindir)

install: $(BUILT_SOURCES)
	$(MAKE) $(AM_MAKEFLAGS) install-recursive
install-exec: install-exec-recursive
install-data: install-data-recursive
uninstall: uninstall-recursive

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-recursive
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  INSTALL_STRIP_FLAG=-s \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
	-test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)
clean: clean-recursive

clean-am: clean-binPROGRAMS clean-generic clean-libtool \
	clean-noinstLIBRARIES mostlyclean-am

distclean: distclean-recursive
	-rm -f $(am__CONFIG_DISTCLEAN_FILES)
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
distclean-am: clean-am distclean-compile distclean-generic distclean-hdr \
	distclean-libtool distclean-tags

dvi: dvi-recursive

dvi-am:

info: info-recursive

info-am:

install-data-am:

install-exec-am: install-binPROGRAMS

install-info: install-info-recursive

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-recursive
	-rm -f $(am__CONFIG_DISTCLEAN_FILES)
	-rm -rf $(top_srcdir)/autom4te.cache
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-recursive

mostlyclean-am: mostlyclean-compile mostlyclean-generic \
	mostlyclean-libtool

pdf: pdf-recursive

pdf-am:

ps: ps-recursive

ps-am:

uninstall-am: uninstall-binPROGRAMS uninstall-info-am

uninstall-info: uninstall-info-recursive

.PHONY: $(RECURSIVE_TARGETS) CTAGS GTAGS all all-am check check-am clean \
	clean-binPROGRAMS clean-generic clean-libtool \
	clean-noinstLIBRARIES clean-recursive ctags ctags-recursive \
	dist dist-all dist-gzip distcheck distclean distclean-compile \
	distclean-generic distclean-hdr distclean-libtool \
	distclean-recursive distclean-tags distcleancheck distdir \
	distuninstallcheck dvi dvi-am dvi-recursive info info-am \
	info-recursive install install-am install-binPROGRAMS \
	install-data install-data-am install-data-recursive \
	install-exec install-exec-am install-exec-recursive \
	install-info install-info-am install-info-recursive install-man \
	install-recursive install-strip installcheck installcheck-am \
	installdirs installdirs-am installdirs-recursive \
	maintainer-clean maintainer-clean-generic \
	maintainer-clean-recursive mostlyclean mostlyclean-compile \
	mostlyclean-generic mostlyclean-libtool mostlyclean-recursive \
	pdf pdf-am pdf-recursive ps ps-am ps-recursive tags \
	tags-recursive uninstall uninstall-am uninstall-binPROGRAMS \
	uninstall-info-am uninstall-info-recursive uninstall-recursive


build.h: Makefile
	echo 'char *build = "'`date`'";' > $@
	echo 'char *builder = "'`id | sed -e 's/[^(]*(\([^)]*\)).*/\1/'`'@'`hostname`'";' >> $@
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
