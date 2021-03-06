/* prm/gui.cc
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
#include "gui.hh"
// #include <gtkmm/separator.h>
#include <prm/ctrl_tag.hh>
#include <cmath>

using namespace prm;

// gui::Base
gui::Base::Base(Taggable const * t) :
	tb(t),
	w(0),
	linked(false),
	updating(false)
{
}

gltk::Widget & gui::Base::widget()
{
	if (! w) {
		w = & get_widget();
		// if (const tag::Desc * d = tb.find<tag::Desc>()) w->set_tooltip_text(d->text);
		if (linked) load();
	}
	return * w;
}

void gui::Base::save()
{
	if (! linked) return;
	if (updating) return;
	redo();
	changed.emit();
}

void gui::Base::set_linked(bool set)
{
	if (linked == set) return; // not changed
	linked = set;
	// if (linked) load();
}
// gui::Bool
void gui::Bool::load()
{
	if (cb) {
		updating = true;
		cb->set_active(vb->get());
		updating = false;
	}
}

void gui::Bool::redo()
{
	if (cb) vb->set(cb->get_active());
}

gltk::Widget & gui::Bool::get_widget()
{
	if (! cb) {
		std::string t = "[check]";
		if (auto n = tb->find<tag::Name>()) t = n->text;
		cb = new gltk::CheckButton(t);
		cb->signal_toggled().connect(sigc::mem_fun(* this, & Bool::save));
	}
	return * cb;
}

gui::Bool::Bool(Taggable const * t, std::shared_ptr<Var<bool>> v) :
	Base(t),
	vb(v),
	cb(0)
{
}

gui::Bool::~Bool()
{
	if (cb) delete cb;
}

// gui::Int
void gui::Int::load()
{
	if (hb) {
		updating = true;
		sb->set_value(vs->get());
		updating = false;
	}
}

void gui::Int::redo()
{
	if (hb) vs->set(int(sb->get_value()));
}

gltk::Widget & gui::Int::get_widget()
{
	if (! hb) {
		hb = new gltk::Box(gltk::ORIENTATION_HORIZONTAL);
		sb = manage(new gltk::SpinButton);
		if (auto n = tb->find<tag::Name>()) {
			gltk::Label * lb = new gltk::Label(n->text + ':');
			hb->pack_start(lb);
			lb->show();
		}
		if (auto s = tb->find<tag::Step>()) {
			sb->set_increments(1, floor(s->page));
		}
		if (auto r = tb->find<tag::Range>()) sb->set_range(ceil(r->min), floor(r->max));
		sb->signal_value_changed().connect(sigc::mem_fun(* this, & Int::save));
		hb->pack_start(sb);
		sb->show();
	}
	return * hb;
}

gui::Int::Int(Taggable const * t, std::shared_ptr<Var<int>> v) :
	Base(t),
	vs(v),
	hb(0)
{
}

gui::Int::~Int()
{
	delete hb;
}

// gui::Select
void gui::Select::load()
{
	if (hb) {
		updating = true;
		cbt->set_active_text(ns->get_name(vs->get()));
		updating = false;
	}
}

void gui::Select::redo()
{
	if (hb) {
		vs->set(ns->get_key(cbt->get_active_text()));
	}
}

gltk::Widget & gui::Select::get_widget()
{
	if (! hb) {
		hb = new gltk::Box(gltk::ORIENTATION_HORIZONTAL);
		cbt = new gltk::ComboBoxText;
		if (auto n = tb->find<tag::Name>()) {
			gltk::Label * lb = new gltk::Label(n->text + ':');
			hb->pack_start(lb);
			lb->show();
		}
		std::vector<std::string> n = ns->get_names();
		for (auto i: ns->get_names()) cbt->append(i);
		cbt->signal_changed().connect(sigc::mem_fun(* this, & Select::save));
		hb->pack_start(cbt);
		cbt->show();
	}
	return * hb;
}

gui::Select::Select(Taggable const * t, std::shared_ptr<Var<int>> v) :
	Base(t),
	vs(v),
	hb(0)
{
	auto s = tb->find<tag::Select>();
	ns = & (s->names);
}

gui::Select::~Select()
{
	if (hb) {
		delete hb;
		delete cbt;
	}
}

// gui::Size
void gui::Size::load()
{
	if (hb) {
		updating = true;
		sb->set_value(vs->get());
		updating = false;
	}
}

void gui::Size::redo()
{
	if (hb) vs->set(size_t(sb->get_value()));
}

gltk::Widget & gui::Size::get_widget()
{
	if (! hb) {
		hb = new gltk::Box(gltk::ORIENTATION_HORIZONTAL);
		sb = new gltk::SpinButton;
		if (auto n = tb->find<tag::Name>()) {
			gltk::Label * lb = new gltk::Label(n->text + ':');
			hb->pack_start(lb);
			lb->show();
		}
		if (auto s = tb->find<tag::Step>()) {
			sb->set_increments(1, floor(s->page));
		}
		if (auto r = tb->find<tag::Range>()) sb->set_range(ceil(r->min), floor(r->max));
		sb->signal_value_changed().connect(sigc::mem_fun(* this, & Size::save));
		hb->pack_start(sb);
		sb->show();
	}
	return * hb;
}

gui::Size::Size(Taggable const * t, std::shared_ptr<Var<size_t>> v) :
	Base(t),
	vs(v),
	hb(0)
{
}

gui::Size::~Size()
{
	if (hb) {
		delete hb;
		delete sb;
	}
}
// gui::Double
void gui::Double::load()
{
	if (hb) {
		updating = true;
		sb->set_value(vd->get());
		updating = false;
	}
}

void gui::Double::redo()
{
	if (hb) {
		double v = sb->get_value();
		if (snap_to_0 && fabs(v / v_step) < 0.01) v = 0; // snap to zero
		vd->set(v);
	}
}

gltk::Widget & gui::Double::get_widget()
{
	if (! hb) {
		hb = new gltk::Box(gltk::ORIENTATION_HORIZONTAL);
		sb = new gltk::SpinButton;
		if (auto n = tb->find<tag::Name>()) {
			gltk::Label * lb = new gltk::Label(n->text + ':');
			hb->pack_start(lb);
			lb->show();
		}
		if (auto s = tb->find<tag::Step>()) {
			v_step = s->step;
			if (v_step > 0) snap_to_0 = true;
			sb->set_increments(v_step, s->page);
			double r = log10(s->step);
			if (r < 0) sb->set_digits(int(ceil(- r)));
		}
		if (auto r = tb->find<tag::Range>()) sb->set_range(r->min, r->max);
		sb->signal_value_changed().connect(sigc::mem_fun(* this, & Double::save));
		hb->pack_start(sb);
		sb->show();
	}
	return * hb;
}

gui::Double::Double(Taggable const * t, std::shared_ptr<Var<double>> v) :
	Base(t),
	vd(v),
	hb(0),
	snap_to_0(false)
{
}

gui::Double::~Double()
{
	if (hb) {
		delete hb;
		delete sb;
	}
}
// gui::File
void gui::File::load()
{
	if (hb) {
		updating = true;
		et->set_text(vs->get());
		updating = false;
	}
}

void gui::File::redo()
{
	if (hb) vs->set(et->get_text());
}

gltk::Widget & gui::File::get_widget()
{
	if (! hb) {
		hb = new gltk::Box(gltk::ORIENTATION_HORIZONTAL);
		et = new gltk::Entry;
		if (auto n = tb->find<tag::Name>()) {
			gltk::Label * lb = new gltk::Label(n->text + ':');
			hb->pack_start(lb);
			lb->show();
		}
		et->signal_activate().connect(sigc::mem_fun(* this, & File::save));
		et->signal_stop().connect(sigc::mem_fun(* this, & File::save));
		hb->pack_start(et);
		et->show();
	}
	return * hb;
}

gui::File::File(Taggable const * t, std::shared_ptr<Var<std::string>> v) :
	Base(t),
	vs(v),
	hb(0)
{
}

gui::File::~File()
{
	if (hb) {
		delete hb;
		delete et;
	}
}

// Gltk
Gltk::Gltk() :
	vb(0),
	changed(0)
{
}

Gltk::~Gltk()
{
	clear();
}

void Gltk::add(Param & param)
{
	for (auto i: param.vars) if (i.tags.find<tag::Control>()) {
		gui::Base * c = make_ctrl(i);
		if (c) {
			ctrls.push_back(c);
			c->set_linked(); // the Var should be valid for Param
		}
		else {
			std::cerr << "can not add control for " << i.name << '\n';
		}
	}
}

void Gltk::add_separator()
{
	// ctrls.push_back(0);
}

void Gltk::clear()
{
	while (ctrls.size()) {
		if (ctrls.back()) delete ctrls.back();
		ctrls.pop_back();
	}
	if (vb) {
		delete vb;
		vb = 0;
	}
	if (changed) {
		delete changed;
		changed = 0;
	}
}

void Gltk::sync()
{
	for (std::vector<gui::Base *>::iterator i = ctrls.begin(); i != ctrls.end(); i ++) {
		if (* i) (* i)->load();
	}
}

void Gltk::save()
// Normally, Gltk put the value back to Value when it's changed.
// But, Value can change without Gltk.
// save() reapply the changes to Value
{
	for (std::vector<gui::Base *>::iterator i = ctrls.begin(); i != ctrls.end(); i ++) {
		if (* i) (* i)->save();
	}
}

gltk::Widget & Gltk::widget()
{
	if (! vb) {
		vb = new gltk::Box(gltk::ORIENTATION_VERTICAL);
		gltk::Box * ib = vb;
		size_t wc = 0; // widget count
		for (std::vector<gui::Base *>::iterator i = ctrls.begin(); i != ctrls.end(); i ++) {
			if (* i) {
				if (wc >= 12) {
					if (ib == vb) {
						vb = new gltk::Box(gltk::ORIENTATION_HORIZONTAL);
						vb->pack_start(* ib);
						ib->show();
					}
					ib = new gltk::Box(gltk::ORIENTATION_VERTICAL);
					vb->pack_start(* ib);
					ib->show();
					wc = 0;
				}
				ib->pack_start(& (* i)->widget());
				(* i)->widget().show();
				wc ++;
			}
			else { // Separator
				/*
				gltk::HSeparator * hs = gltk::manage(new gltk::HSeparator);
				vb->pack_start(* hs, gltk::PACK_SHRINK);
				hs->show();
				*/
			}
		}
	}
	return * vb;
}

sigc::signal<void> & Gltk::signal_changed()
{
	if (! changed) {
		changed = new sigc::signal<void>;
		for (std::vector<gui::Base *>::iterator i = ctrls.begin(); i != ctrls.end(); i ++) {
			if (* i) (* i)->changed.connect(changed->make_slot());
		}
	}
	return * changed;
}

sigc::signal<void> & Gltk::signal_changed(std::string const & name)
{
	for (auto i: ctrls) if (i) {
		if (auto n = i->tb->find<tag::Name>()) {
			if (n->text == name) return i->changed;
		}
	}
	throw;
}

gui::Base * Gltk::make_ctrl(Param::VarEntry & e) // The Magic happens here!
{
	gui::Base * c = 0;
	if (auto v = std::dynamic_pointer_cast<Var<bool>>(e.var)) {
		c = new gui::Bool(&e.tags, v);
	}
	else if (auto v = std::dynamic_pointer_cast<Var<int>>(e.var)) {
		if (e.tags.find<tag::Select>()) {
			c = new gui::Select(&e.tags, v);
		}
		else c = new gui::Int(&e.tags, v);
	}
	else if (auto v = std::dynamic_pointer_cast<Var<size_t>>(e.var)) {
		c = new gui::Size(&e.tags, v);
	}
	else if (auto v = std::dynamic_pointer_cast<Var<double>>(e.var)) {
		if (e.tags.find<tag::Step>()) c = new gui::Double(&e.tags, v);
	}
	else if (auto v = std::dynamic_pointer_cast<Var<std::string>>(e.var)) {
		if (e.tags.find<tag::File>()) c = new gui::File(&e.tags, v);
	}
	/*
	else if (TagSable * v = dynamic_cast<TagSable *>(var)) {
		c = new gui::Sable(* v);
	}
	*/
	return c;
}
