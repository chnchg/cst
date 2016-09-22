/* sim/gkviewt.cc
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
#include "gkviewt.hh"
#include <sim/h5_file.hh>
#include <sim/parser_add.hh>
#include <prm/argu.hh>

void gkViewT::set_parser()
{
	parser.add_opt('p', "param")
		.store(argu.make_parser())
		.help("system parameters, `help' for a list", "PARAM[, PARAM...]");
	if (ParserAdd * pa = dynamic_cast<ParserAdd *>(sys)) {
		pa->addto_parser(parser);
	}
	parser.add_opt_help();
}

void gkViewT::on_initialize()
{
	if (! sys) throw; // a system is required
	if (parser.args().size()) {
#ifdef HAVE_HDF5
		H5File f(parser.args()[0]);
		sys->h5_load(f.open_load());
		sys_loaded();
		tk.sync();
#endif
	}
	else {
		argu.alter();
		sys->init();
		tk.sync();
	}
	if (gltk::Box * vb = dynamic_cast<gltk::Box *>(child)) {
		tk.widget().show();
		vb->pack_start(& tk.widget());
	}
	sys_changed.emit();
}

bool gkViewT::idle_func()
{
	if (dashed) {
		sys_dashed.emit();
		dashed = false;
	}
	return true;
}

void gkViewT::toggle_run()
{
	running = ! running;
	if (running) {
		idle_run = gltk::SignalIdle::connect(sigc::mem_fun(this, & gkViewT::idle_func));
		rthrd = new std::thread(& gkViewT::run_thread, this);
		start_run.emit();
	}
	else {
		idle_run.disconnect();
		rthrd->join();
		delete rthrd;
		rthrd = 0;
		stop_run.emit();
	}
}

void gkViewT::run_thread()
{
	while (running) {
		sys->dash();
		dashed = true;
	}
}

void gkViewT::make_file_dialog()
{
	using namespace gltk;
	fdialog = new Window;
	Box * bx = new Box(ORIENTATION_VERTICAL);
	fdialog->add(manage(bx));

	FileBrowser * bsr = new FileBrowser;
	bx->pack_start(manage(bsr));
	bsr->signal_selected().connect(sigc::mem_fun(this, & gkViewT::file_select));

	Box * hb = new Box(ORIENTATION_HORIZONTAL);
	bx->pack_start(manage(hb));
	hb->pack_start(manage(new Label("Filename:")));
	fn_entry = new Entry;
	hb->pack_start(manage(fn_entry));

	Button * b;
	bx->pack_start(manage(b = new Button("Cancel")));
	b->signal_clicked().connect(sigc::mem_fun(fdialog, & Window::hide));

	bx->pack_start(manage(b = new Button("OK")));
	b->signal_clicked().connect(sigc::mem_fun(this, & gkViewT::file_ok));
}

void gkViewT::file_select(std::string name)
{
	fn_entry->set_text(name);
}

void gkViewT::file_ok()
{
	std::string fn = fn_entry->get_text();
	if (file_loading) {
#ifdef HAVE_HDF5
		H5File f(fn);
		sys->h5_load(f.open_load());
		tk.sync();
		sys_changed();
		sys_loaded();
		std::cerr << "system loaded\n";
#endif
	}
	else {
#ifdef HAVE_HDF5
		H5File f(fn);
		sys->h5_save(f.open_save());
		f.done_save();
#endif
	}
	fdialog->hide();
}

void gkViewT::load_file()
{
	if (! fdialog) make_file_dialog();
	fdialog->set_title("Open file...");
	file_loading = true;
	fdialog->show();
}

void gkViewT::save_file()
{
	if (! fdialog) make_file_dialog();
	fdialog->set_title("Save file...");
	file_loading = false;
	fdialog->show();
}

gkViewT::gkViewT(int argc, char ** argv, char ** envp) :
	gkWin(argc, argv, envp),
	sys(0),
	running(false),
	rthrd(0),
	dashed(false),
	fdialog(0)
{
	using namespace gltk;
	Box * bx = new Box(ORIENTATION_VERTICAL);
	add(manage(bx));
	Box * vb = new Box(ORIENTATION_HORIZONTAL);
	bx->pack_start(manage(vb));
	vb->show();
	vb->pack_start(acbox);
	acbox.show();
	Button * b;

	b = new Button("Run");
	acbox.pack_start(manage(b));
	b->signal_clicked().connect(sigc::mem_fun(this, & gkViewT::toggle_run));
	LabelBin * lb = b;
	start_run.connect(sigc::bind(sigc::mem_fun(lb, & LabelBin::set_text), std::string("Stop")));
	stop_run.connect(sigc::bind(sigc::mem_fun(lb, & LabelBin::set_text), std::string("Run")));

	b = new Button("Load");
	acbox.pack_start(manage(b));
	b->signal_clicked().connect(sigc::mem_fun(this, & gkViewT::load_file));

	b = new Button("Save");
	acbox.pack_start(manage(b));
	b->signal_clicked().connect(sigc::mem_fun(this, & gkViewT::save_file));

	b = new Button("Quit");
	vb->pack_start(manage(b));
	b->signal_clicked().connect(sigc::bind(sigc::ptr_fun(& exit), 0));
}

gkViewT::~gkViewT()
{
	if (rthrd) rthrd->join();
}

void gkViewT::set_system(Runnable * s)
{
	if (sys) {
		argu.clear();
		tk.clear();
	}
	sys = s;
	if (sys) {
		argu.add(sys->param);
		tk.add(sys->param);
	}
}

void gkViewT::add_action(std::string const & action, sigc::slot<void> const & slot)
{
	gltk::Button * b = new gltk::Button(action);
	acbox.pack_start(manage(b));
	b->signal_clicked().connect(slot);
	b->show();
}

sigc::signal<void> & gkViewT::tk_changed(std::string const & name)
{
	return tk.signal_changed(name);
}

void gkViewT::tk_update(std::string const & /*name*/)
{
	tk.sync();
}
