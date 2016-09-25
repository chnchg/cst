AUTOMAKE_OPTIONS = subdir-objects
CLEANFILES = *~ */*~
AM_CXXFLAGS =
CST_SRC = \
	prm/prm.cc prm/argu.cc sim/runner.cc sim/avera.cc sim/fvec.cc \
	sim/vvec.cc sim/ivec.cc sim/mt19937.cc
CST_HDR = \
	sigc.hh arg.cc arg.hh val.cc val.hh s_entry.hh \
	prm/prm.hh prm/ctrl_tag.hh prm/argu.hh \
	prm/name_set.hh \
	sim/random.hh sim/executable.hh sim/runnable.hh sim/with_param.hh \
	sim/runner.hh \
	sim/avera.hh \
	sim/fvec.hh sim/vvec.hh \
	sim/ivec.hh \
	sim/mt19937.hh sim/parser_add.hh

if HAVE_HDF5
CST_SRC += \
	prm/hdf5.cc sim/h5_file.cc
CST_HDR += \
	prm/hdf5.hh sim/h5_file.hh
AM_CXXFLAGS += $(HDF5_CFLAGS) $(HDF5_CPPFLAGS)
endif

if HAVE_GLUT
CST_SRC += \
	gltk.cc prm/gui.cc sim/gkwin.cc sim/gkview.cc sim/gkviewt.cc
CST_HDR += \
	gltk.hh prm/gui.hh sim/gkwin.hh sim/gkview.hh sim/gkviewt.hh
AM_CXXFLAGS += $(GLUT_CFLAGS)
endif

lib_LIBRARIES = libcst.a
libcst_a_SOURCES = $(CST_SRC)
cstincludedir = $(includedir)/$(CST_MODULE_NAME)
nobase_cstinclude_HEADERS = $(CST_HDR)
