/* sim/runner.hh
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
 *  Calculation Runner
 *
 *  - Assume the running state and data are stored in a single data file which
 *    is in HDF5 format
 *  - Provides locking on the data file
 *  - Runs are split into dashes which is a finite but non-trivial amount of
 *    calculations
 *  - Between dashes, program subclasses should be able to save the running
 *    state to the data file
 *
 */
#ifndef RUNNER_HH
#define RUNNER_HH
#include "executable.hh"
#include "runnable.hh"
#include "parser_add.hh"
#include <prm/argu.hh>
#include <iostream>

class Runner :
	public Executable,
	virtual private Runnable,
	virtual private ParserAdd
{
	enum RunType {
		RT_NONE, // nothing to do?
		RT_INIT, // initialize
		RT_DASH, // regular runs
		RT_DUMP  // exporting data
	};
	int run_type;
	int dump_type;
	std::string data_file;
	int overwrite;
	int use_lock;
	int save_on_term;
	int dry_run;
	// int stop_on_done;
	double run_count; // count to run
	bool run_relative; // count to run is relative?
	unsigned long period; // saving period in seconds
	unsigned long limit; // run time limit
	bool altered; // Does system differ from file? (to decide whether to save)

	prm::Argu argu;

	// overrides, not to be overriden again
	void set_parser();
	void on_initialize();
	int on_execute();

	// internal
	void save_data();
public:
	Runner(int argc, char ** argv, char ** envp);
};
#endif // RUNNER_HH
