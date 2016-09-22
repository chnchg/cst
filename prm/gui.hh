/* prm/gui.hh
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
#ifndef PRM_GUI_HH
#define PRM_GUI_HH
#include "name_set.hh"
#include <prm/prm.hh>
#include <gltk.hh>
namespace prm {
	class Gltk;
	namespace gui {
		class Base
		{
			Taggable & tb;
			gltk::Widget * w;
			virtual gltk::Widget & get_widget() = 0;
		protected:
			bool linked; // gates the access to variable
			bool updating; // variable is being updated...
			virtual void redo() = 0; // actually write value to the variable
		public:
			Base(Taggable & v);
			virtual ~Base() {};
			virtual void load() = 0; // read value from the variable
			void save(); // write value to varibale, can be used for callback
			gltk::Widget & widget();
			void set_linked(bool set = true);
			mutable sigc::signal<void> changed; // value has been changed
			friend class prm::Gltk;
		};

		class Bool :
			public Base
		{
			Var<bool> & vb;
			gltk::CheckButton * cb;
			// override
			void load();
			void redo();
			gltk::Widget & get_widget();
		public:
			Bool(Var<bool> & v);
			~Bool();
		};

		class Int :
			public Base
		{
			Var<int> & vs;
			gltk::Box * hb;
			gltk::SpinButton * sb;
			// override
			void load();
			void redo();
			gltk::Widget & get_widget();
		public:
			Int(Var<int> & v);
			~Int();
		};

		class Select : // select from a NameSet
			public Base
		{
			Var<int> & vs;
			NameSet const * ns;
			gltk::Box * hb;
			gltk::ComboBoxText * cbt;
			// override
			void load();
			void redo();
			gltk::Widget & get_widget();
		public:
			Select(Var<int> & v);
			~Select();
		};

		class Size :
			public Base
		{
			Var<size_t> & vs;
			gltk::Box * hb;
			gltk::SpinButton * sb;
			// override
			void load();
			void redo();
			gltk::Widget & get_widget();
		public:
			Size(Var<size_t> & v);
			~Size();
		};

		class Double : // implement with spin button, need tag::Step
			public Base
		{
			Var<double> & vd;
			gltk::Box * hb;
			gltk::SpinButton * sb;
			// override
			void load();
			void redo();
			gltk::Widget & get_widget();
			// hacks...
			bool snap_to_0;
			double v_step;
		public:
			Double(Var<double> & v);
			~Double();
		};

		class File :
			public Base
		{
			Var<std::string> & vs;
			gltk::Box * hb;
			gltk::Entry * et;
			// override
			void load();
			void redo();
			gltk::Widget & get_widget();
		public:
			File(Var<std::string> & v);
			~File();
		};
	}
	class Gltk
	{
		std::vector<gui::Base *> ctrls;
		gltk::Box * vb;
		sigc::signal<void> * changed;
	public:
		Gltk();
		~Gltk();
		// Setup
		void add(Param & param); // create controls for the param
		void add_separator();
		void clear();
		// Use
		void sync(); // load data to controls
		void save(); // save data to variables
		gltk::Widget & widget();
		sigc::signal<void> & signal_changed();
		sigc::signal<void> & signal_changed(std::string const & var_name); // changed signal for certain item

		static gui::Base * make_ctrl(Taggable * var);
	};
}
#endif
