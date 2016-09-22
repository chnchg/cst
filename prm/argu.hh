/* prm/argu.hh
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
  Generate command line argument from Param
  TODO: NameSet support

  The command-line values parsed by Argu do not affect the parameters right away.
  To commit the values to actual parameters, you need to call "alter()";
*/
#ifndef ARGU_HH
#define ARGU_HH 1
#include "prm.hh"
#include <arg.hh>
namespace prm {
	namespace tag {
		class Key : // an expressive word to short for
			virtual public Base
		{
			Base * dup() const;
		public:
			int key;
			Key(int a_key);
		};
	}

	namespace argu {
		class Base
		{
			std::string name;
			TagSable & ts;
			int set_altering;
		protected:
			virtual void addto_option(arg::Option & opt) = 0;
			virtual void do_altering() = 0;
		public:
			Base(const std::string & n, TagSable & t);
			virtual ~Base() {}
			void dump(std::ostream & output) const;
			void set(const std::string & str);
			void addto_parser(arg::Parser & parser);
			bool alter();
			const std::string & get_name() const {return name;}
		};

		template <typename Type> std::string type_word() {return "VALUE";}
		template <> std::string type_word<bool>();
		template <> std::string type_word<int>();
		template <> std::string type_word<size_t>();
		template <> std::string type_word<double>();

		template <typename Type>
		class Arg :
			virtual public Base
		{
			Var<Type> & var;
			Type altering;
			void addto_option(arg::Option & opt)
			{
				opt.stow(altering);
				if (const tag::Word * w = var.template find<tag::Word>()) opt.help_word(w->text);
				else opt.help_word(type_word<Type>());
			}

			void do_altering()
			{
				var.set(altering);
			}
		public:
			Arg(const std::string name, Var<Type> & v) :
				Base(name, v),
				var(v),
				altering(v.get())
			{
			}
		};
	}

	class Argu
	{
		std::vector<argu::Base *> argus;
	public:
		~Argu();
		// Setup
		void add(Param & p);
		void clear(); // clear argus
		// Use
		arg::SubParser * make_parser(std::string title = "");
		void addto_parser(arg::Parser & parser);
		bool alter();
		// Info
		size_t size() const;
	};
}
#endif // ARGU_HH
