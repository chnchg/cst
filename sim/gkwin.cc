/* sim/gkwin.cc
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
#include "gkwin.hh"

int gkProgram::on_execute()
{
	gltk::Window * w = get_window();
	if (w) main.run(* w);
	return 0;
}

gkProgram::gkProgram(int argc, char * * argv, char * * envp) :
	Executable(argc, argv, envp),
	main(argc, argv)
{
}

gkProgram::~gkProgram()
{
}

gltk::Window * gkWin::get_window()
{
	return this;
}

gkWin::gkWin(int argc, char * * argv, char * * envp) :
	gkProgram(argc, argv, envp)
{
}
