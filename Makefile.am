AUTOMAKE_OPTIONS = subdir-objects
CLEANFILES = *~ */*~
AM_CXXFLAGS =
CST_SRC = \
	sigc.hh arg.cc arg.hh val.cc val.hh s_entry.hh \
	prm/prm.cc prm/prm.hh prm/ctrl_tag.hh prm/argu.cc prm/argu.hh \
	prm/name_set.hh \
	sim/random.hh sim/executable.hh sim/runnable.hh sim/with_param.hh \
	sim/runner.cc sim/runner.hh \
	sim/avera.cc sim/avera.hh \
	sim/fvec.cc sim/fvec.hh sim/vvec.cc sim/vvec.hh \
	sim/ivec.cc sim/ivec.hh \
	sim/mt19937.cc sim/mt19937.hh sim/parser_add.hh

if HAVE_HDF5
CST_SRC += \
	prm/hdf5.cc prm/hdf5.hh sim/h5_file.cc sim/h5_file.hh
AM_CXXFLAGS += $(HDF5_CFLAGS) $(HDF5_CPPFLAGS)
endif

if HAVE_GLUT
CST_SRC += \
	gltk.cc gltk.hh prm/gui.cc prm/gui.hh \
	sim/gkwin.cc sim/gkwin.hh sim/gkview.cc sim/gkview.hh \
	sim/gkviewt.cc sim/gkviewt.hh
AM_CXXFLAGS += $(GLUT_CFLAGS)
endif

noinst_LIBRARIES = libcst.a
libcst_a_SOURCES = $(CST_SRC)
