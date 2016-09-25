# cst
Common Simulation Tools
* arg.cc arg.hh val.cc val.hh  
from arg package http://ccdw.org/~cjj/prog/arg/
* gltk.cc gltk.hh sigc.hh  
are generated files from gltk package http://gltk.ccdw.org/
* prm/  
parameter manager
* sim/  
simulation framework

cst is a set of tools to take care of common tasks that are usually encountered in simulations or numerical calculations of computational models.

## Requirements
(recommanded versions are parenthesized)
* autoconf (2.69), automake (1.15), libtool (2.4.6)  
for building configuration script
* hdf5 (1.8.16)
for data storage
* glut or freeglut3 (2.8.1)
for GUI
