/* sim/runner.cc
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
#include "runner.hh"
#include <val.hh>
#include <fstream>
#include <csignal>
#include <cstdio>
#include <ctime>
#include <cerrno>
#include <cstdlib>
#include <cstring>
extern "C" {
#include <fcntl.h>
#ifdef WIN32
#include <io.h>
#include <winsock2.h>
#include <process.h>
#define F_OK 0
#define R_OK 4
#define W_OK 2
#define access _access
#define unlink _unlink
#define open _open
#define snprintf _snprintf
#define getpid _getpid
#define NO_FCNTL_LOCK
#else
#include <unistd.h>
#endif
#ifdef __MINGW32__
#include <winsock2.h>
#endif
}

namespace {
	volatile bool stop_run;
	volatile bool sig_term;
	volatile bool sig_save;

	// SIGTERM to stop the run
	void handle_term(int)
	{
		sig_term = true;
		stop_run = true;
	}

	// SIGUSER2 to trigger a save of data
	void handle_usr2(int)
	{
		sig_save = true;
	}

	// call back to stop run
	/* void set_stop_run()
	{
		stop_run = true;
	} */
}

void Runner::set_parser()
{
	dump_set["help"] = "Available data for --dump:";
	dump_set["info"] = "display information of the data file";
	dump_set["param"] = "display parameter values of the data file";
	dump_set["count"] = "integer representation of the amount of running";

	dump_type = "help";

	std::string header = "Usage: " + exec_name + " <Command> [Options] <File>\n";
	if (description.size()) header += description + '\n';
	parser.set_header(header);

	parser.add_help("Commands:");
	parser.add_opt('I', "init")
		.set(& run_type, RT_INIT)
		.once(RT_NONE)
		.help("initailize data file");
	parser.add_opt('R', "run")
		.store(new arg::RelValue(run_count, run_relative))
		.set(& run_type, RT_DASH)
		.once(RT_NONE)
		.help("run until count=VALUE or count+VALUE", "[+]VALUE")
		.show_default();
	arg::TermValue * tv = new arg::TermValue(dump_type);
	for (auto const & e: dump_set) {
		if (e.first == "help") tv->add_help(e.second);
		else tv->add(e.first, e.second);
	}
	parser.add_opt('D', "dump")
		.store(tv)
		.set(& run_type, RT_DUMP)
		.once(RT_NONE)
		.help("dump DATA in the data file, use 'help' for a list of available data", "DATA");

	parser.add_help("Options:");
	parser.add_opt('O',"overwrite")
		.set(& overwrite, true)
		.help("Overwrite old data file");
	parser.add_opt('K', "nolock")
		.set(& use_lock, false)
		.help("do not use lock file");
	parser.add_opt('T', "period")
		.stow(period)
		.help("save state of calculation when it's INT seconds after the last save", "INT")
		.show_default();
	parser.add_opt('L', "limit")
		.stow(limit)
		.help("limit total run time to INT seconds", "INT")
		.show_default();

	parser.add_opt("no-save-on-term")
		.set(& save_on_term, false)
		.help("Do not save data when TERM signal is received");
	parser.add_opt("dry-run")
		.set(& dry_run, true)
		.help("Do not save data at all");
	// adding parameters from Runnable
	argu.clear();
	argu.add(param);
	parser.add_opt('p', "param")
		.store(argu.make_parser())
		.help("system parameters, `help' for a list", "PARAM[,PARAM...]");
	addto_parser(parser); // additional parser options from subclasses
	parser.add_opt_help();
}

void Runner::on_initialize()
{
	if (parser.args().size() != 1) {
		std::cerr << "One filename is required!\n";
		abort();
	}
	data_file = parser.args()[0];
}

int Runner::on_execute()
{
	if (run_type == RT_NONE) { // nothing to do
		std::cerr << "nothing to do?\n";
		abort();
	}
	// use `if' instead of `switch' for more flexibility
	if (run_type == RT_INIT) {
		bool have_old = access(data_file.c_str(), F_OK) == 0; // old data file exists?
		if (have_old && ! overwrite) {
			std::cerr << "data file exists! use \"-O\" to overwrite\n";
			abort();
		}
		alter(); // commit command line parameters for construction
		init(); // initialize/construct the system
		argu.alter(); // commit running parameters from command line
		// save file
		std::string tmp_file = data_file + ".new~";
#ifdef HAVE_HDF5
		hid_t file = H5Fcreate(tmp_file.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if (file >= 0) {
			hid_t group = H5Gopen(file, "/", H5P_DEFAULT);
			h5_save(group);
			H5Gclose(group);
			H5Fclose(file);
		}
		else {
			std::cerr << "fail to open hdf5 file: " << tmp_file << '\n';
			throw;
		}
#endif
		if (have_old) {
			std::string bak = data_file + "~";
			int ret = rename(data_file.c_str(), bak.c_str()); // backup old file
			if (ret && access(bak.c_str(), F_OK) == 0) { // error renaming file, and old backup exists
				unlink(bak.c_str()); // try removing old backup first...
				rename(data_file.c_str(), bak.c_str()); // do again...
			}
		}
		rename(tmp_file.c_str(), data_file.c_str());
		return 0;
	}

	// load data file for all other cases
	if (access(data_file.c_str(), R_OK) != 0) { // have no read access to file?
		int en = errno;
		std::cerr << "Can not read file `" << data_file << "' due to error: \"" << strerror(en) << "\".\n";
		abort();
	}
#ifdef HAVE_HDF5
	hid_t file = H5Fopen(data_file.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
	if (file >= 0) {
		hid_t group = H5Gopen(file, "/", H5P_DEFAULT);
		h5_load(group);
		H5Gclose(group);
		H5Fclose(file);
	}
	else {
		std::cerr << "Failed to open `" << data_file << "' as a HDF5 file\n";
		throw;
	}
#endif
	altered = argu.alter() || alter(); // commit command line changes
	// data dump
	if (run_type == RT_DUMP) {
		std::streamsize sp = std::cout.precision();
		std::cout.precision(0);
		std::cout.setf(std::ios::fixed, std::ios::floatfield);
		if (dump_type == "info") {
			std::cout << "data_file=" << data_file << '\n';
			std::cout << "count=" << count() << '\n';
		}
		else if (dump_type == "param") {
			param.write(std::cout);
		}
		else if (dump_type == "count") {
			std::cout << count() << '\n';
		}
		// restore default formating
		std::cout.unsetf(std::ios::floatfield);
		std::cout.precision(sp);
		dump(dump_type, std::cout);
		return 0;
	}

	// normal run
	if (run_type != RT_DASH) { // sanity check
		std::cerr << "Not RT_DASH when it must be!\n";
		throw;
	}

	if (dry_run && use_lock) use_lock = false;
	// creating lock file

	std::string lock_file = data_file + ".lock";
	if (use_lock) {
		// msg(DBG_INFO) << "openning lock file\n";
		int fd = open(lock_file.c_str(), O_RDWR | O_CREAT, 00644);
		if (fd < 0) {
			std::cerr << "can't open lock file '" << lock_file << "'!\n";
			abort();
		}
#ifndef NO_FCNTL_LOCK
		flock lk;
		// first aquire a read lock
		lk.l_type = F_RDLCK;
		lk.l_whence = SEEK_SET;
		lk.l_start = 0;
		lk.l_len = 0;
		if (fcntl(fd, F_SETLK, & lk) < 0) {
			std::cerr << "can't get read lock on lock file '" << lock_file << "'!\n";
			close(fd);
			abort();
		}
#endif
		char b[100];
		int l = read(fd, b, 100);
		if (l > 0) { // read sucessful, file exists with info, quit!
			b[l] = 0;
			for (int i = 0; i < l; i ++) if (b[i] == 10) b[i] = 0; // show only first line
			std::cerr << "according to `" << lock_file << "', process `"
				  << b << "' is locking the data file!\n";
			close(fd);
			abort();
		}
#ifndef NO_FCNTL_LOCK
		// it seems to be necessary to cancel the read lock before getting
		// a write lock on cygwin... (an infinitesimal window for race condition)
		lk.l_type = F_UNLCK;
		if (fcntl(fd, F_SETLK, & lk) < 0) {
			std::cerr << "can't unlock\n";
			close(fd);
			abort();
		}

		// aquire a write lock
		lk.l_type = F_WRLCK;
		lk.l_whence = SEEK_SET;
		lk.l_start = 0;
		lk.l_len = 0;
		if (fcntl(fd, F_SETLK, & lk) < 0) { // can't get write lock, quit!
			std::cerr << "can't obtain write lock on lock file `" << lock_file << "'\n";
			close(fd);
			abort();
		}
#endif
		// write pid to the lock file
		char h[30];
		gethostname(h, 30);
		l = snprintf(b, 100, "%s: %d\n", h, int(getpid()));
		if (write(fd, b, l) <= 0) {
			std::cerr << "fail to write to lock file!\n";
			abort();
		}
		close(fd);
	}
	// enable signal control
	signal(SIGTERM, & handle_term);
#ifdef SIGXCPU
	signal(SIGXCPU, & handle_term);
#endif
#ifdef SIGUSR2
	signal(SIGUSR2, & handle_usr2);
#endif
	sig_term = false;
	sig_save = false;
	stop_run = false;
	time_t start_time = time(0);
	time_t last_save = start_time;
	if (run_relative) run_count += count();

	// main running loop
	while (count() < run_count) {
		if (stop_run) {
			if (sig_term && ! save_on_term) goto remove_lock;
			break;
		}
		dash();
		altered = true;
		time_t curr_time = time(0);
		if (sig_save || (period && curr_time - last_save >= long(period))) {
			save_data();
			last_save = curr_time;
			std::string stop_file = data_file + ".stop~";
			if (access(stop_file.c_str(), F_OK) == 0) { // stop file exists?
				unlink(stop_file.c_str());
				goto remove_lock;
			}
			if (limit && curr_time - start_time > long(limit)) goto remove_lock;
		}
	}
	if (altered) save_data();
 remove_lock:
	if (use_lock) unlink(lock_file.c_str()); // unlock
	return 0;
}

void Runner::save_data()
{
	if (dry_run) return;
	std::string tmp_file = data_file + ".new~";
#ifdef HAVE_HDF5
	hid_t file = H5Fcreate(tmp_file.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	hid_t group = H5Gopen(file, "/", H5P_DEFAULT);
	h5_save(group);
	H5Gclose(group);
	H5Fclose(file);
#endif
	// old file should exist
	// FIXME: If the program is interrupted between the following
	// system calls, the expected data_file will be left missing....
	std::string bak = data_file + "~";
	int ret = rename(data_file.c_str(), bak.c_str()); // backup old data file
	if (ret && access(bak.c_str(), F_OK) == 0) { // error renaming file, and old backup exists
		unlink(bak.c_str()); // try removing old backup first...
		rename(data_file.c_str(), bak.c_str()); // do again...
	}
	rename(tmp_file.c_str(), data_file.c_str());

	sig_save = false;
	altered = false;
}

Runner::Runner(int argc, char ** argv, char ** envp) :
	Executable(argc, argv, envp),
	run_type(RT_NONE),
	overwrite(false),
	use_lock(true),
	save_on_term(true),
	dry_run(false),
	run_count(0),
	run_relative(true),
	period(3600),
	limit(0) // unlimited
{
}
