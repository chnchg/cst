/* sim/gkwin.hh
Copyright (C) 2016 Chun-Chung Chen <cjj@u.washington.edu>

This file is part of cst.

cst is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with cst.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * Executable gltk application
 */
#ifndef SIM_GKWIN_HH
#define SIM_GKWIN_HH
#include "executable.hh"
#include <gltk.hh>

class gkProgram :
	public Executable
{
	gltk::Main main;
	int on_execute();
protected:
	virtual gltk::Window * get_window() = 0;
public:
	gkProgram(int argc = 0, char ** argv = 0, char ** envp = 0);
	virtual ~gkProgram();
};

class gkWin :
	public gkProgram,
	public gltk::Window
{
	gltk::Window * get_window();
public:
	gkWin(int argc = 0, char ** argv = 0, char ** envp = 0);
};
#endif
