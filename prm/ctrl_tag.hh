/* prm/ctrl_tag.hh
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
#ifndef CTRL_TAG_HH
#define CTRL_TAG_HH 1
#include "prm.hh"
#include "name_set.hh"
namespace prm {
	namespace tag {
		class Control : // value can be interactively controlled
			virtual public Base
		{
			Base * dup() const {return new Control;}
		};

		class Step : // a numerical value
			virtual public Control
		{
			Base * dup() const {return new Step(step, page);}
		public:
			const double step;
			const double page;
			Step(double st, double pg) : step(st), page(pg) {}
		};

		class Select :
			virtual public Control
		{
			Base * dup() const {return new Select(names);}
		public:
			NameSet & names;
			Select(NameSet & ns) : names(ns) {}
		};
	}
}
#endif // CTRL_TAG_HH
