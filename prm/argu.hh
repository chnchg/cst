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
		protected:
			std::string name;
			Taggable tags;
			int set_altering;
			virtual void addto_option(arg::Option & opt) = 0;
			virtual void do_altering() = 0;
			virtual std::shared_ptr<Sable> get_var() = 0;
			virtual std::shared_ptr<Sable const> get_var() const = 0;
		public:
			Base(Param::VarEntry const & v);
			virtual ~Base() {}
			void dump(std::ostream & output) const;
			void set(const std::string & str);
			void addto_parser(arg::Parser & parser);
			bool alter();
			std::string const & get_name() const;
		};

		template <typename Type> std::string type_word() {return "VALUE";}
		template <> std::string type_word<bool>();
		template <> std::string type_word<int>();
		template <> std::string type_word<unsigned>();
		template <> std::string type_word<size_t>();
		template <> std::string type_word<double>();

		template <typename Type>
		class Arg :
			virtual public Base
		{
			std::shared_ptr<Var<Type>> var;
			Type altering;
			void addto_option(arg::Option & opt) override
			{
				opt.stow(altering);
				if (auto w = tags. template find<tag::Word>()) opt.help_word(w->text);
				else opt.help_word(type_word<Type>());
			}
			void do_altering() override {var->set(altering);}
			std::shared_ptr<Sable> get_var() override {return var;}
			std::shared_ptr<Sable const> get_var() const override {return var;}
		public:
			Arg(Param::VarEntry const & v) :
				Base(v),
				var(std::dynamic_pointer_cast<Var<Type>>(v.var))
			{
				if (! var) throw;
				altering = var->get();
			}
			static std::shared_ptr<Arg<Type>> make(Param::VarEntry const & v)
			{
				if (std::dynamic_pointer_cast<Var<Type>>(v.var)) return std::make_shared<Arg<Type>>(v);
				return 0;
			}
		};
	}

	class Argu
	{
		std::vector<std::shared_ptr<argu::Base>> argus;
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
