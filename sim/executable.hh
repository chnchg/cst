/* sim/executable.hh
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
 * Executable program
 */
#ifndef EXECUTABLE_HH
#define EXECUTABLE_HH
#include <arg.hh>
class Executable
{
protected:
	int argc;
	char ** argv;
	char ** envp;

	arg::Parser parser; // the common parser
	std::string exec_name;

	virtual void on_construct() {}
	virtual void set_parser() {}
	virtual void on_initialize() {}
	virtual int on_execute() {return 0;}
public:
	Executable(int argc, char ** argv, char ** envp) : argc(argc), argv(argv), envp(envp)
	{
		exec_name = argc ? argv[0] : "exec";
	}

	virtual ~Executable() {}

	int execute()
	{
		on_construct();
		set_parser();
		parser.parse(argc, argv);
		on_initialize();
		return on_execute();
	}
};
#endif // EXECUTABLE_HH
