/* sim/gkviewt.hh
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
 * GUI to view a gkDraw
 */
#ifndef GKVIEWT_HH
#define GKVIEWT_HH
#include "runnable.hh"
#include "gkwin.hh"
#include <prm/gui.hh>
#include <prm/argu.hh>
#include <thread>
class gkViewT :
	// made virtual so it can be initialized early
	// but need to initialize it explicitly when gkViewT is inherited
	virtual public gkWin
{
	Runnable * sys;
	prm::Argu argu;
	prm::Gltk tk;
	// overrides
	void set_parser() final;
	void on_initialize() final;

	// run control
	bool running;
	std::thread * rthrd;
	bool dashed;
	sigc::signal<void> start_run;
	sigc::signal<void> stop_run;
	sigc::connection idle_run;
	bool idle_func();
	void toggle_run();
	void run_thread();

	gltk::Window * fdialog;
	gltk::Entry * fn_entry;
	bool file_loading;
	void make_file_dialog();
	void file_select(std::string name);
	void file_ok();
	void load_file();
	void save_file();

	gltk::Box acbox;
public:
	gkViewT(int argc = 0, char ** argv = 0, char ** envp = 0);
	~gkViewT();
	void set_system(Runnable * s);
	void add_action(std::string const & action, sigc::slot<void> const & slot);

	sigc::signal<void> & tk_changed(std::string const & name);
	sigc::signal<void> sys_changed; // system initialized or loaded
	sigc::signal<void> sys_loaded; // system loaded from file
	sigc::signal<void> sys_dashed;
	void tk_update(std::string const & name = "");
};
#endif
