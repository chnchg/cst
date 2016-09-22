/* GENERATED, DO NOT EDIT! See http://gltk.ccdw.org/ for source. */
/* gltk_all.cc: adjustment align bin box button checkbox checkbutton comboboxtext container entry filebrowser focusable frame glarea glutwindow idle label labelbin listview main menu orientable orientation packoptions popup positiontype pushbox scale scrollbar separator shape slider spinbutton textdraw timeout widget window
 *
 * Copyright (C) 2011 Chun-Chung Chen <cjj@u.washington.edu>
 *
 * This file was generated from gltk.
 *
 * gltk is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with arg.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */
#include "gltk.hh"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace gltk;
namespace gltk {
	int glut_get_font_height(void * font);
	int glut_get_font_ascent(void * font);
	int glut_get_font_descent(void * font);
	void glut_draw_string(void * font, std::string const & text);
	void glut_draw_string(void * font, std::string const & text, int limit);
	int glut_get_string_width(void * font, std::string const & text);
	void glut_draw_raised_box(int x, int y, int w, int h);
	int glut_raised_box_width();
	void glut_draw_sunken_box(int x, int y, int w, int h);
	int glut_sunken_box_width();
	inline int round_int(double x) {return x > 0 ? int(x + 0.5) : int(x - 0.5);}
	template<typename T>
	inline T min_of(T a, T b) {return a > b ? b : a;}
	template<typename T>
	inline T max_of(T a, T b) {return a > b ? a : b;}
	struct Error
	{
		std::string msg;
		Error(std::string const & s) : msg(s) {}
	};
}
Adjustment::Adjustment(double val, double lo, double up, double st, double pg, double psz) :
	value(val),
	lower(lo),
	upper(up),
	step_increment(st),
	page_increment(pg),
	page_size(psz)
{}
Adjustment * Adjustment::create(double val, double lo, double up, double st, double pg, double ps)
{
	return new Adjustment(val, lo, up, st, pg, ps);
}
void Adjustment::configure(double val, double lo, double up, double st, double pg, double ps)
{
	struct _local {
		bool change;
		_local() : change(false) {}
		void md(double & var, double val)
		{
			if (var != val) {
				var = val;
				change |= true;
			}
		}
	} local;
	local.md(lower, lo);
	local.md(upper, up);
	local.md(step_increment, st);
	local.md(page_increment, pg);
	local.md(page_size, ps);
	if (local.change) changed();
	if (value != val) {
		value = val;
		on_value_changed();
		value_changed();
	}
}
double Adjustment::get_value() const
{
	return value;
}
void Adjustment::set_value(double v)
{
	if (v + page_size > upper) v = upper - page_size;
	if (v < lower) v = lower;
	if (v != value) {
		value = v;
		on_value_changed();
		value_changed();
	}
}
void Adjustment::step_up()
{
	set_value(value + step_increment);
}
void Adjustment::step_down()
{
	set_value(value - step_increment);
}
void Adjustment::page_up()
{
	set_value(value + page_increment);
}
void Adjustment::page_down()
{
	set_value(value - page_increment);
}
void Adjustment::set_step_increment(double inc)
{
	step_increment = inc;
}
void Adjustment::set_page_increment(double inc)
{
	page_increment = inc;
}
void Adjustment::set_upper(double v)
{
	upper = v;
	double ll = upper - page_size;
	if (lower > ll) lower = ll;
	if (value > ll) set_value(ll);
}
void Adjustment::set_lower(double v)
{
	lower = v;
	double uu = lower + page_size;
	if (upper < uu) upper = uu;
	if (value < lower) set_value(lower);
}
void Adjustment::set_page_size(double ps)
{
	page_size = ps;
	assert(lower + page_size <= upper);
	if (value + page_size > upper) set_value(upper - page_size);
}
sigc::signal<void> & Adjustment::signal_changed()
{
	return changed;
}
sigc::signal<void> & Adjustment::signal_value_changed()
{
	return value_changed;
}
void Bin::on_configure_event()
{
	if (! child) return;
	int width = get_width();
	int height = get_height();
	if (width < reserved_x && height < reserved_y) {
		child->size_allocate(0, 0, 0, 0);
		return;
	}
	width -= reserved_x;
	height -= reserved_y;
	int m;
	int n;
	child->get_preferred_width(m, n);
	int w = min_of(width, max_of(n, width - reducible_x));
	child->get_preferred_height_for_width(w, m, n);
	int h = min_of(height, max_of(n, height - reducible_y));
	child_x = (width - w) / 2;
	child_y = (height - h) / 2;
	child->size_allocate(child_x, child_y, w, h);
}
void Bin::get_preferred_width(int & min, int & nat) const
{
	min = reserved_x;
	nat = reserved_x + reducible_x;
	if (child) {
		int m;
		int n;
		child->get_preferred_width(m, n);
		min += m;
		nat += n;
	}
}
void Bin::get_preferred_height(int & min, int & nat) const
{
	min = reserved_y;
	nat = reserved_y + reducible_y;
	if (child) {
		int m;
		int n;
		child->get_preferred_height(m, n);
		min += m;
		nat += n;
	}
}
void Bin::get_preferred_width_for_height(int height, int & min, int & nat) const
{
	min = reserved_x;
	nat = reserved_x + reducible_x;
	if (child) {
		int m;
		int n;
		child->get_preferred_height(m, n);
		int child_h = max_of(0, height - reserved_y);
		if (child_h > n) child_h = max_of(n, n - reducible_y);
		child->get_preferred_width_for_height(child_h, m, n);
		min += m;
		nat += n;
	}
}
void Bin::get_preferred_height_for_width(int width, int & min, int & nat) const
{
	min = reserved_y;
	nat = reserved_y + reducible_y;
	if (child) {
		int m;
		int n;
		child->get_preferred_width(m, n);
		int child_w = max_of(0, width - reserved_x);
		if (child_w > n) child_w = max_of(n, n - reducible_x);
		child->get_preferred_height_for_width(child_w, m, n);
		min += m;
		nat += n;
	}
}
void Bin::on_add(Widget * w)
{
	if (child) throw Error("add to Bin with a child");
	child = w;
}
void Bin::on_remove(Widget * w)
{
	if (child != w) throw Error("removed child not in Bin");
	child = 0;
}
Bin::Bin() :
	child(0),
	reducible_x(0),
	reducible_y(0),
	reserved_x(0),
	reserved_y(0)
{}
void Bin::set_reducible(int red_x, int red_y)
{
	reducible_x = red_x;
	reducible_y = red_y;
}
void Bin::set_reserved(int x, int y)
{
	reserved_x = x;
	reserved_y = y;
}
void Bin::place_bin(int x, int y)
{
	if (child) child->move_allocate(child_x + x, child_y + y);
}
void Bin::set_border_width(unsigned w)
{
	set_reducible(w * 2, w * 2);
}
void Box::on_configure_event()
{
	int width = get_width();
	int height = get_height();
	if (hori) {
		int min = 0;
		int nat = 0;
		std::vector<Widget *>::const_iterator e = child_list.end();
		int cnt = 0;
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != e; i ++) {
			int m;
			int n;
			(* i)->get_preferred_width_for_height(width, m, n);
			min += m;
			nat += n;
			if (min > width) {
				min -= m;
				nat -= n;
				e = i;
				break;
			}
			cnt ++;
		}
		int pad = width > nat && cnt > 1 ? (width - nat) / (cnt - 1) : 0;
		int ext = width - min - (cnt - 1) * pad;
		int x = 0;
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != e; i ++) {
			(* i)->get_preferred_width_for_height(height, min, nat);
			int w = min_of(nat, min + ext);
			(* i)->size_allocate(x, 0, w, height);
			x += w + pad;
			ext -= (w - min);
		}
		while (e != child_list.end()) {
			(* e)->size_allocate(width, 0, 0, height);
			e ++;
		}
	}
	else {
		int min = 0;
		int nat = 0;
		std::vector<Widget *>::const_iterator e = child_list.end();
		int cnt = 0;
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != e; i ++) {
			int m;
			int n;
			(* i)->get_preferred_height_for_width(width, m, n);
			min += m;
			nat += n;
			if (min > height) {
				min -= m;
				nat -= n;
				e = i;
				break;
			}
			cnt ++;
		}
		int pad = height > nat && cnt > 1 ? (height - nat) / (cnt - 1) : 0;
		int ext = height - min - (cnt - 1) * pad;
		int y = 0;
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != e; i ++) {
			(* i)->get_preferred_height_for_width(width, min, nat);
			int h = min_of(nat, min + ext);
			(* i)->size_allocate(0, y, width, h);
			y += h + pad;
			ext -= (h - min);
		}
		while (e != child_list.end()) {
			(* e)->size_allocate(0, height, width, 0);
			e ++;
		}
	}
}
void Box::get_preferred_width(int & min, int & nat) const
{
	min = 0;
	nat = 0;
	if (hori) {
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != child_list.end(); i ++) {
			int m;
			int n;
			(* i)->get_preferred_width(m, n);
			min += m;
			nat += n;
		}
	}
	else {
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != child_list.end(); i ++) {
			int m;
			int n;
			(* i)->get_preferred_width(m, n);
			min = max_of(min, m);
			nat = max_of(nat, n);
		}
	}
	nat += border_width;
}
void Box::get_preferred_height(int & min, int & nat) const
{
	min = 0;
	nat = 0;
	if (hori) {
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != child_list.end(); i ++) {
			int m;
			int n;
			(* i)->get_preferred_height(m, n);
			min = max_of(min, m);
			nat = max_of(nat, n);
		}
	}
	else {
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != child_list.end(); i ++) {
			int m;
			int n;
			(* i)->get_preferred_height(m, n);
			min += m;
			nat += n;
		}
	}
	nat += border_width;
}
void Box::get_preferred_width_for_height(int height, int & min, int & nat) const
{
	min = 0;
	nat = 0;
	if (hori) {
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != child_list.end(); i ++) {
			int m;
			int n;
			(* i)->get_preferred_width_for_height(height, m, n);
			min += m;
			nat += n;
		}
	}
	else {
		std::vector<Widget *>::const_iterator e = child_list.end();
		int cnt = 0;
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != e; i ++) {
			int m;
			int n;
			(* i)->get_preferred_height(m, n);
			min += m;
			nat += n;
			if (min > height) {
				min -= m;
				nat -= n;
				e = i;
				break;
			}
			cnt ++;
		}
		int ext = height - min;
		min = 0;
		nat = 0;
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != e; i ++) {
			int mh;
			int nh;
			(* i)->get_preferred_height(mh, nh);
			int h = min_of(nh, mh + ext);
			int mw;
			int nw;
			(* i)->get_preferred_width_for_height(h, mw, nw);
			ext -= (h - mh);
			min = max_of(min, mw);
			nat = max_of(nat, nw);
		}
	}
	nat += border_width;
}
void Box::get_preferred_height_for_width(int width, int & min, int & nat) const
{
	if (hori) {
		min = 0;
		nat = 0;
		std::vector<Widget *>::const_iterator e = child_list.end();
		int cnt = 0;
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != e; i ++) {
			int m;
			int n;
			(* i)->get_preferred_width(m, n);
			min += m;
			nat += n;
			if (min > width) {
				min -= m;
				nat -= n;
				e = i;
				break;
			}
			cnt ++;
		}
		int ext = width - min;
		min = 0;
		nat = 0;
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != e; i ++) {
			int mw;
			int nw;
			(* i)->get_preferred_width(mw, nw);
			int w = min_of(nw, mw + ext);
			int mh;
			int nh;
			(* i)->get_preferred_height_for_width(w, mh, nh);
			ext -= (w - mw);
			min = max_of(min, mh);
			nat = max_of(nat, nh);
		}
	}
	else {
		min = 0;
		nat = 0;
		for (std::vector<Widget *>::const_iterator i = child_list.begin(); i != child_list.end(); i ++) {
			int m;
			int n;
			(* i)->get_preferred_height_for_width(width, m, n);
			min += m;
			nat += n;
		}
	}
}
Box::Box(Orientation ori, int sp) :
	Orientable(ori),
	border_width(0),
	spacing(sp)
{}
void Box::pack_start(Widget * child)
{
	add(child);
}
void Box::set_border_width(unsigned bw)
{
	border_width = bw;
}
void Button::on_draw()
{
	PushBox::draw_box();
	if (child) {
		int d = box_width + depressed;
		place_bin(d, d);
		draw_children();
	}
}
void Button::get_preferred_height(int & min, int & nat) const
{
	Bin::get_preferred_height(min, nat);
}
void Button::get_preferred_width(int & min, int & nat) const
{
	Bin::get_preferred_width(min, nat);
}
void Button::get_preferred_width_for_height(int height, int & min, int & nat) const
{
	Bin::get_preferred_width_for_height(height, min, nat);
}
void Button::get_preferred_height_for_width(int width, int & min, int & nat) const
{
	Bin::get_preferred_height_for_width(width, min, nat);
}
bool Button::focus_make()
{
	return PushBox::focus_make();
}
bool Button::focus_next()
{
	return PushBox::focus_next();
}
void Button::focus_end()
{
	PushBox::focus_end();
}
void Button::on_button_event(int button, int state, int x, int y)
{
	PushBox::on_button_event(button, state, x, y);
	get_focus();
}
bool Button::on_key_event(unsigned char key)
{
	return PushBox::on_key_event(key);
}
Button::Button(std::string const & name) :
	LabelBin(name)
{
	int rs = box_width * 2 + 1;
	set_reserved(rs, rs);
}
Button::Button(Widget * w) :
	LabelBin(w)
{
	int rs = box_width * 2 + 1;
	set_reserved(rs, rs);
}
const int CheckBox::checkbox_size = 7;
const int CheckBox::checkbox_area = 20;
void CheckBox::on_draw()
{
	draw_focus_box();
	draw_checkbox(0, 0, get_width(), get_height());
}
void CheckBox::get_preferred_width(int & min, int & nat) const
{
	min = checkbox_size;
	nat = checkbox_area;
}
void CheckBox::get_preferred_height(int & min, int & nat) const
{
	min = checkbox_size;
	nat = checkbox_area;
}
void CheckBox::on_button_event(int button, int state, int , int )
{
	if (button != GLUT_LEFT_BUTTON) return;
	if (state == GLUT_DOWN) {
		active = ! active;
		toggled();
	}
	queue_draw();
}
bool CheckBox::on_key_event(unsigned char key)
{
	if (key == 0xd || key == 0x20) {
		active = ! active;
		toggled();
		queue_draw();
		return true;
	}
	return false;
}
void CheckBox::draw_checkbox(int , int , int w, int h)
{
	int checkbox_size = 7;
	if (w > checkbox_size && h > checkbox_size) {
		float dx = (w - checkbox_size) / 2 - 0.5;
		float dy = (h - checkbox_size) / 2 - 0.5;
		glColor3ub(0, 0, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(dx, dy);
		glVertex2f(dx + checkbox_size, dy);
		glVertex2f(dx + checkbox_size, dy + checkbox_size);
		glVertex2f(dx, dy + checkbox_size);
		glEnd();
		if (active) {
			glColor3ub(0, 0, 0);
			glBegin(GL_LINES);
			glVertex2f(dx, dy);
			glVertex2f(dx + checkbox_size, dy + checkbox_size);
			glVertex2f(dx + checkbox_size, dy);
			glVertex2f(dx, dy + checkbox_size);
			glEnd();
		}
	}
}
CheckBox::CheckBox() :
	active(false)
{
}
sigc::signal<void> & CheckBox::signal_toggled()
{
	return toggled;
}
bool CheckBox::get_active() const
{
	return active;
}
void CheckBox::set_active(bool val)
{
	active = val;
}
void CheckButton::on_configure_event()
{
	Bin::on_configure_event();
	if (child) {
		int ch = child->get_height();
		child->move_allocate(min_of(checkbox_area, get_width()), (get_height() - ch) / 2);
	}
}
void CheckButton::on_draw()
{
	int w = get_width();
	int h = get_height();
	draw_focus_box();
	w = min_of(w, checkbox_area);
	draw_checkbox(0, 0, w, h);
	draw_children();
}
void CheckButton::get_preferred_width(int & min, int & nat) const
{
	Bin::get_preferred_width(min, nat);
	min = checkbox_size;
}
void CheckButton::get_preferred_height(int & min, int & nat) const
{
	Bin::get_preferred_height(min, nat);
	min = checkbox_size;
	nat = max_of(nat, checkbox_area);
}
void CheckButton::get_preferred_width_for_height(int height, int & min, int & nat) const
{
	Bin::get_preferred_width_for_height(height, min, nat);
	min = checkbox_size;
}
void CheckButton::get_preferred_height_for_width(int width, int & min, int & nat) const
{
	Bin::get_preferred_height_for_width(width, min, nat);
	min = checkbox_size;
	nat = max_of(nat, checkbox_area);
}
void CheckButton::on_button_event(int button, int state, int x, int y)
{
	CheckBox::on_button_event(button, state, x, y);
}
bool CheckButton::on_key_event(unsigned char key)
{
	return CheckBox::on_key_event(key);
}
bool CheckButton::focus_make()
{
	return CheckBox::focus_make();
}
bool CheckButton::focus_next()
{
	return CheckBox::focus_next();
}
void CheckButton::focus_end()
{
	CheckBox::focus_end();
}
CheckButton::CheckButton(std::string const & label, bool ) :
	LabelBin(label)
{
	set_reserved(checkbox_area + 1, 2);
}
void ComboBoxText::on_draw()
{
	int w = get_width();
	int h = get_height();
	glut_draw_raised_box(0, 0, w, h);
	int bw = glut_raised_box_width();
	glColor3f(0.9, 0.9, 0.9);
	glBegin(GL_QUADS);
	glVertex2i(bw, bw);
	glVertex2i(w - bw, bw);
	glVertex2i(w - bw, h - bw);
	glVertex2i(bw, h - bw);
	glEnd();
	int y = h / 2;
	int x = bw + 1;
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2i(x, y);
	draw_text(ALIGN_START, ALIGN_CENTER, max_of(1, get_width() - bw * 2 - 14));
	y = h / 2;
	x = w - bw - 1;
	glBegin(GL_TRIANGLES);
	glVertex2f(x, y + 1);
	glVertex2f(x - 8, y + 1);
	glVertex2f(x - 4, y + 5);
	glVertex2f(x, y - 1);
	glVertex2f(x - 8, y - 1);
	glVertex2f(x - 4, y - 5);
	glEnd();
}
void ComboBoxText::get_preferred_height(int & min, int & nat) const
{
	int h = text_height();
	min = h + glut_raised_box_width() * 2;
	min = max_of(min, 12);
	nat = min + 6;
}
void ComboBoxText::get_preferred_width(int & min, int & nat) const
{
	int w = 0;
	for (std::vector<std::string>::const_iterator i = menu.begin(); i != menu.end(); i ++) {
		w = max_of(w, text_width(* i));
	}
	min = w + glut_raised_box_width() * 2 + 14;
	nat = min;
}
void ComboBoxText::on_menu_select(int item)
{
	set_text(menu[item]);
	active_item = item;
	changed();
	queue_draw();
}
ComboBoxText::ComboBoxText() :
	active_item(0)
{
}
void ComboBoxText::append(std::string const & text)
{
	menu.push_back(text);
	if (active_item == int(menu.size()) - 1) {
		set_text(text);
		queue_draw();
	}
	release_menu();
}
std::string ComboBoxText::get_active_text() const
{
	if (active_item >= 0 && active_item < (int) menu.size()) return menu[active_item];
	return std::string();
}
void ComboBoxText::remove_all()
{
	menu.clear();
	release_menu();
	active_item = 0;
}
void ComboBoxText::set_active_text(std::string const & text)
{
	for (int i = 0; i < (int) menu.size(); i ++)
	if (menu[i] == text) {
		active_item = i;
	}
	set_text(text);
	queue_draw();
}
void Container::on_set_window(Window * window)
{
	set_children_window(window);
}
void Container::on_realize()
{
	realize_children();
}
void Container::on_unrealize()
{
	unrealize_children();
}
void Container::on_draw()
{
	draw_children();
}
void Container::on_button_event(int button, int state, int x, int y)
{
	if (grabber) grabber->button_event(button, state, x, y);
	else if (Widget * w = get_child(x, y)) {
		w->button_event(button, state, x, y);
	}
	if (! grabber) on_passive_event(x, y);
}
void Container::on_motion_event(int x, int y)
{
	if (grabber) grabber->motion_event(x, y);
	else on_passive_event(x, y);
}
void Container::on_passive_event(int x, int y)
{
	if (Widget * w = get_child(x, y)) {
		if (tracker != w) {
			if (tracker) tracker->entry_event(false);
			tracker = w;
			w->entry_event(true);
		}
		w->passive_event(x, y);
	}
	else if (tracker) {
		tracker->entry_event(false);
		tracker = 0;
	}
}
bool Container::on_key_event(unsigned char key)
{
	if (focus) return focus->keyboard_event(key);
	return false;
}
bool Container::on_special_event(int key)
{
	if (focus) return focus->special_event(key);
	return false;
}
void Container::on_entry_event(bool entered)
{
	if ((! entered) && tracker) {
		tracker->entry_event(false);
		tracker = 0;
	}
}
bool Container::focus_make()
{
	if (focus) {
		if (focus->focus_make()) return true;
		focus = 0;
	}
	for (std::vector<Widget *>::iterator i = child_list.begin(); i != child_list.end(); i ++) if ((* i)->focus_make()) {
		focus = * i;
		return true;
	}
	return false;
}
bool Container::focus_next()
{
	std::vector<Widget *>::iterator i = child_list.begin();
	if (focus) {
		if (focus->focus_next()) return true;
		do if (i == child_list.end()) {
			focus = 0;
			return false;
		} while (focus != * (i ++));
	}
	while (i != child_list.end()) {
		if ((* i)->focus_make()) {
			focus = * i;
			return true;
		}
		i ++;
	}
	focus = 0;
	return false;
}
void Container::focus_end()
{
	if (focus) {
		focus->focus_end();
		focus = 0;
	}
}
Widget * Container::get_child(int x, int y)
{
	for (std::vector<Widget *>::iterator i = child_list.begin(); i != child_list.end(); i ++) {
		if ((* i)->contains(x, y)) return * i;
	}
	return 0;
}
void Container::draw_children()
{
	for (std::vector<Widget *>::iterator i = child_list.begin(); i != child_list.end(); i ++) {
		glPushMatrix();
		(* i)->draw();
		glPopMatrix();
	}
}
void Container::set_children_window(Window * window)
{
	for (std::vector<Widget *>::iterator i = child_list.begin(); i != child_list.end(); i ++) {
		(* i)->set_window(window);
	}
}
void Container::realize_children()
{
	for (std::vector<Widget *>::iterator i = child_list.begin(); i != child_list.end(); i ++) {
		(* i)->realize();
	}
}
void Container::unrealize_children()
{
	for (std::vector<Widget *>::iterator i = child_list.begin(); i != child_list.end(); i ++) {
		(* i)->unrealize();
	}
}
Container::Container() :
	grabber(0),
	focus(0),
	tracker(0)
{}
Container::~Container()
{
	for (std::vector<Widget *>::iterator i = child_list.begin(); i != child_list.end(); i ++) {
		(* i)->set_window(0);
		(* i)->set_parent(0);
		if ((* i)->get_managed()) delete * i;
	}
}
void Container::add(Widget * child)
{
	child_list.push_back(child);
	child->set_parent(this);
	child->set_window(get_window());
	on_add(child);
	if (get_realized()) child->realize();
}
void Container::remove(Widget * child)
{
	for (std::vector<Widget *>::iterator i = child_list.begin(); i != child_list.end(); i ++)
	if (* i == child) {
		if (grabber == child) {
			grabber = 0;
			set_grabbing(false);
		}
		if (tracker == child) {
			tracker->entry_event(false);
			tracker = 0;
		}
		if (focus == child) {
			focus->focus_end();
			focus = 0;
		}
		child_list.erase(i);
		if (get_realized()) child->unrealize();
		on_remove(child);
		child->set_window(0);
		child->set_parent(0);
		return;
	}
	throw Error("child not found");
}
void Container::show_all()
{
	show();
	show_all_children(true);
}
void Container::show_all_children(bool recursive)
{
	for (std::vector<Widget *>::iterator i = child_list.begin(); i != child_list.end(); i ++) {
		Container * c = dynamic_cast<Container *>(* i);
		if (recursive && c) c->show_all();
		else (* i)->show();
	}
}
void Container::set_grabber(Widget * child, bool grab)
{
	if (grabber) {
		if (grab) throw("already grabbed!");
		if (grabber != child) throw("child not the grabber!");
		grabber = 0;
		set_grabbing(false);
		return;
	}
	grabber = child;
	set_grabbing(true);
}
bool Container::focus_on(Widget * child)
{
	if (! get_focus()) return false;
	if (focus && focus != child) focus->focus_end();
	focus = child;
	return true;
}
void Entry::on_draw()
{
	int w = get_width();
	int h = get_height();
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	glVertex2i(0, 0);
	glVertex2i(w, 0);
	glVertex2i(w, h);
	glVertex2i(0, h);
	glEnd();
	glut_draw_sunken_box(0, 0, w, h);
	int cx = - 1;
	int ex = (h - glut_get_font_height(font)) / 2;
	if (marked) {
		if (cx < 0) cx = find_x(cursor);
		int mx = find_x(mark);
		glColor3f(0.8, 0.8, 1.0);
		glBegin(GL_QUADS);
		glVertex2i(mx, ex);
		glVertex2i(cx, ex);
		glVertex2i(cx, ex + glut_get_font_height(font));
		glVertex2i(mx, ex + glut_get_font_height(font));
		glEnd();
	}
	if (vstart) {
		glColor3f(0.5, 0.25, 0.25);
		glBegin(GL_TRIANGLES);
		glVertex2f(bw + as - 1, ex);
		glVertex2f(bw + as - 1, ex + glut_get_font_height(font));
		glVertex2f(bw - 1, h / 2.0);
		glEnd();
	}
	if (visibility) glColor3f(0, 0, 0);
	else glColor3f(1.0, 0.4, 0.4);
	int xs = vstart ? bw + as : bw;
	int mx = w - bw;
	glRasterPos2i(xs, ex + glut_get_font_ascent(font));
	bool check = false;
	end_arrow = false;
	for (size_t i = vstart; i < text.size(); i ++) {
		char c = visibility ? text[i] : '*';
		xs += glutBitmapWidth(font, c);
		if (xs > mx - as && ! check) {
			size_t j = i;
			int xss = xs;
			check = true;
			do {
				if (xss > mx) {
					glColor3f(0.5, 0.25, 0.25);
					glBegin(GL_TRIANGLES);
					glVertex2f(w - bw - as + 1, ex);
					glVertex2f(w - bw - as + 1, ex + glut_get_font_height(font));
					glVertex2f(w - bw + 1, h / 2.0);
					glEnd();
					end_arrow = true;
					break;
				}
				if (++ j > text.size()) break;
				xss += glutBitmapWidth(font, visibility ? text[j] : '*');
			} while (true);
			if (end_arrow) break;
		}
		glutBitmapCharacter(font, c);
	}
	if (focused) {
		if (cx < 0) cx = find_x(cursor);
		glBegin(GL_LINES);
		if (editable) glColor3f(0, 0, 1.0);
		else glColor3f(0.6, 0.6, 0.0);
		glVertex2f(0.5 + cx, ex);
		glVertex2f(0.5 + cx, ex + glut_get_font_height(font));
		glEnd();
	}
}
void Entry::on_button_event(int button, int state, int x, int )
{
	if (! focused) {
		if (get_focus()) {
			focused = true;
			queue_draw();
		}
	}
	if (button != GLUT_LEFT_BUTTON) return;
	if (state == GLUT_DOWN) {
		set_grabbing();
		cursor = find_position(x);
		marked = true;
		mark = cursor;
	}
	else {
		set_grabbing(false);
	}
	queue_draw();
}
void Entry::on_motion_event(int x, int )
{
	if (! marked) return;
	if (x < 0 || x > get_width() - bw - 2) return;
	size_t p = find_position(x);
	if (p != cursor) {
		cursor = p;
		queue_draw();
	}
}
bool Entry::on_key_event(unsigned char key)
{
	if (isprint(key)) {
		if (editable) {
			if (marked) delete_marked();
			text.insert(cursor, 1, char(key));
			cursor ++;
			adjust_vstart();
		}
		marked = false;
	}
	else switch (key) {
	case 0x8:
		if (editable) {
			if (marked) delete_marked();
			else if (cursor > 0) {
				cursor --;
				if (vstart > cursor) vstart = cursor;
				text.erase(cursor, 1);
			}
		}
		break;
	case 0xb:
		if (editable) {
			if (marked) delete_marked();
			else if (cursor < text.size()) text.erase(cursor);
		}
		break;
	case 0xd:
		activate();
		select_all();
		return true;
	case 0x7f:
		if (editable) {
			if (marked) delete_marked();
			else if (cursor < text.size()) text.erase(cursor, 1);
		}
		break;
	default:
		return false;
	}
	marked = false;
	queue_draw();
	return true;
}
bool Entry::on_special_event(int key)
{
	if (! marked && glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
		mark = cursor;
		marked = true;
	}
	switch (key) {
	case GLUT_KEY_LEFT:
		if (cursor > 0) {
			cursor --;
			if (vstart > cursor) vstart = cursor;
		}
		break;
	case GLUT_KEY_RIGHT:
		if (cursor < text.size()) {
			cursor ++;
			adjust_vstart();
		}
		break;
	case GLUT_KEY_HOME:
		vstart = 0;
		cursor = 0;
		break;
	case GLUT_KEY_END:
		cursor = text.size();
		adjust_vstart();
		break;
	default:
		return false;
	}
	if (glutGetModifiers() != GLUT_ACTIVE_SHIFT) marked = false;
	queue_draw();
	return true;
}
bool Entry::focus_make()
{
	focused = true;
	select_all();
	return true;
}
void Entry::focus_end()
{
	if (focused) {
		focused = false;
		stop();
		select_none();
		queue_draw();
	}
}
void Entry::adjust_vstart()
{
	int aw = get_width() - bw * 2;
	if (visibility)	{
		int w = 0;
		for (size_t i = vstart; i < cursor; i ++) w += glutBitmapWidth(font, text[i]);
		int ce = 0;
		for (size_t j = cursor; j < text.size() && ce <= as; j ++) {
			ce += glutBitmapWidth(font, text[j]);
		}
		if (vstart) w += as;
		while (w > aw - as && w + ce > aw) {
			w -= glutBitmapWidth(font, text[vstart]);
			if (! vstart) w += as;
			vstart ++;
		}
	}
	else {
		int w = glutBitmapWidth(font, '*') * (cursor - vstart);
		if ((vstart ? w + as : w) > aw) {
			size_t ac = (aw - as) / glutBitmapWidth(font, '*');
			vstart = ac > cursor ? 0 : cursor - ac;
		}
	}
}
int Entry::find_x(size_t p)
{
	int x = vstart ? bw + as : bw;
	if (p <= vstart) return x;
	if (p > text.size()) p = text.size();
	if (visibility) for (size_t i = vstart; i < p; i ++) x += glutBitmapWidth(font, text[i]);
	else x += glutBitmapWidth(font, '*') * (p - vstart);
	return min_of(x, get_width() - bw);
}
size_t Entry::find_position(int x)
{
	int w = vstart ? bw + as : bw;
	int mx = get_width() - (end_arrow ? bw + as : bw);
	for (size_t i = vstart; i < text.size(); i ++) {
		int cw = glutBitmapWidth(font, text[i]);
		if (w + cw / 2 > x) return i;
		w += cw;
		if (w > mx) return i;
	}
	return text.size();
}
void Entry::delete_marked()
{
	if (mark < cursor) {
		text.erase(mark, cursor - mark);
		cursor = mark;
		if (vstart > cursor) vstart = cursor;
	}
	else text.erase(cursor, mark - cursor);
	marked = false;
}
Entry::Entry() :
	focused(false),
	editable(true),
	visibility(true),
	cursor(0),
	vstart(0),
	marked(false),
	font(GLUT_BITMAP_HELVETICA_12),
	width_chars(0)
{
	bw = glut_sunken_box_width() + 1;
	as = 5;
	end_arrow = false;
}
void Entry::get_preferred_height(int & min, int & nat) const
{
	int h = glut_get_font_height(font);
	min = h + bw * 2;
	nat = h + bw * 2 + 4;
}
void Entry::get_preferred_width(int & min, int & nat) const
{
	if (width_chars) {
		nat = width_chars * glutBitmapWidth(font, 'W') + bw * 2;
		min = nat;
	}
	else {
		min = 10 + bw * 2;
		nat = 100 + bw * 2;
	}
}
void Entry::set_text(std::string const & t)
{
	text = t;
	if (cursor > t.size()) {
		cursor = t.size();
		if (vstart > cursor) vstart = cursor;
	}
	if (marked && mark > t.size()) mark = t.size();
	queue_draw();
}
std::string const & Entry::get_text() const
{
	return text;
}
size_t Entry::get_text_length() const
{
	return text.size();
}
void Entry::select_all()
{
	mark = text.size();
	marked = true;
	cursor = 0;
	vstart = 0;
	queue_draw();
}
void Entry::select_none()
{
	marked = false;
	cursor = 0;
	vstart = 0;
	queue_draw();
}
void Entry::set_max_length(size_t length)
{
	max_length = length;
}
void Entry::set_width_chars(int n_chars)
{
	width_chars = n_chars;
}
void Entry::set_editable(bool v)
{
	editable = v;
}
void Entry::set_visibility(bool v)
{
	if (v != visibility) {
		visibility = v;
		if (get_realized()) adjust_vstart();
	}
}
sigc::signal<void> & Entry::signal_activate()
{
	return activate;
}
sigc::signal<void> & Entry::signal_stop()
{
	return stop;
}
#ifdef __GNUC__
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#elif defined _WIN32
#include <windows.h>
#endif
namespace {
	struct FileItem :
		ViewableList::ItemBase
	{
		enum Type {
			FILE,
			DIRECTORY
		};
		std::string name;
		Type type;
		FileItem(std::string const & n, Type t) : name(n), type(t) {}
		bool operator<(FileItem const & f) const
		{
			if (type != f.type) return type == DIRECTORY;
			return name < f.name;
		}
	};
	struct RenderFileItem :
		public ViewableList::RenderBase
	{
		void * font;
		int get_height()
		{
			return glut_get_font_height(font) + 2;
		}
		void render(ViewableList::ItemBase * item, int x, int y, int width, int )
		{
			if (FileItem * i = dynamic_cast<FileItem *>(item)) {
				if (i->type == FileItem::DIRECTORY) glColor3f(0, 0.8, 0);
				else glColor3f(0, 0, 0);
				glRasterPos2i(x + 1, y + 1 + glut_get_font_ascent(font));
				glut_draw_string(font, i->name, width - 2);
			}
			else throw Error("unexpected ItemBase is not a FileItem");
		}
		RenderFileItem() :
			font(GLUT_BITMAP_HELVETICA_12)
		{}
	};
	class FileList :
		public ViewableList
	{
		std::list<FileItem> filelist;
		std::vector<std::list<FileItem>::iterator> fl_index;
		size_t size() const
		{
			return fl_index.size();
		}
		ItemBase * get_item(size_t i)
		{
			return & (* fl_index[i]);
		}
		void read_filelist()
		{
			filelist.clear();
#ifdef __GNUC__
			DIR * dir;
			struct dirent * dirp;
			struct stat dr;
			if (! (dir = opendir("."))) throw Error("Fail to opendir .");
			while ((dirp = readdir(dir))) {
				filelist.push_back(FileItem(dirp->d_name, ! stat(dirp->d_name, & dr) && S_ISDIR(dr.st_mode) ? FileItem::DIRECTORY : FileItem::FILE));
			}
			closedir(dir);
#elif defined _WIN32
			WIN32_FIND_DATA fdata;
			HANDLE fhndl = FindFirstFile("*", & fdata);
			if (fhndl == INVALID_HANDLE_VALUE) throw Error("Fail to list current directory");
			do {
				filelist.push_back(FileItem(fdata.cFileName, fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? FileItem::DIRECTORY : FileItem::FILE));
			} while (FindNextFile(fhndl, & fdata) != 0);
			if (GetLastError() == ERROR_NO_MORE_FILES) FindClose(fhndl);
			else throw Error("Error reading current directory ");
#endif
			filelist.sort();
		}
		void index_filelist()
		{
			fl_index.clear();
			for (std::list<FileItem>::iterator i = filelist.begin(); i != filelist.end(); i ++) {
				fl_index.push_back(i);
			}
		}
	public:
		FileList()
		{
			read_filelist();
			index_filelist();
		}
		void item_selected(int item)
		{
			FileItem & fi = * fl_index[item];
			if (fi.type == FileItem::DIRECTORY) {
#ifdef __GNUC__
				if (chdir(fi.name.c_str())) {std::cerr << "error changing directory to " << fi.name << '\n';}
#elif defined _WIN32
				SetCurrentDirectory(fi.name.c_str());
#endif
				read_filelist();
				index_filelist();
				changed();
			}
			else select_file(fi.name);
		}
		sigc::signal<void, std::string> select_file;
	};
}
FileBrowser::FileBrowser() :
	Box(ORIENTATION_HORIZONTAL)
{
	FileList * fl = new FileList;
	ListView * lv = new ListView;
	Scrollbar * sb = new Scrollbar(lv, ORIENTATION_VERTICAL);
	RenderFileItem * rfi = new RenderFileItem;
	pack_start(lv);
	pack_start(sb);
	lv->set_list(fl);
	lv->add_render(rfi);
	lv->signal_selected().connect(sigc::mem_fun(fl, & FileList::item_selected));
	list = fl;
}
void FileBrowser::set_path(std::string const & path)
{
#ifdef __GNUC__
	if (chdir(path.c_str())) {std::cerr << "error changing directory to " << path << '\n';}
#elif defined _WIN32
	SetCurrentDirectory(path.c_str());
#endif
}
std::string FileBrowser::get_path() const
{
	std::string path;
#ifdef __GNUC__
	size_t len = 32;
	char * buf = new char [len];
	while (! getcwd(buf, len)) {
		delete [] buf;
		len *= 2;
		buf = new char[len];
	}
	path = buf;
#elif defined _WIN32
	size_t len = GetCurrentDirectory(0, 0);
	char * buf = new char [len + 1];
	GetCurrentDirectory(len + 1, buf);
	path = buf;
	delete [] buf;
#endif
	return path;
}
sigc::signal<void, std::string> & FileBrowser::signal_selected()
{
	FileList * fl = dynamic_cast<FileList *>(list);
	return fl->select_file;
}
bool Focusable::focus_make()
{
	focused = true;
	queue_draw();
	return true;
}
bool Focusable::focus_next()
{
	focus_end();
	return false;
}
void Focusable::focus_end()
{
	focused = false;
	queue_draw();
}
void Focusable::draw_focus_box()
{
	if (focused) {
		int w = get_width();
		int h = get_height();
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0x5555);
		glColor3f(0., 0., 0.);
		glBegin(GL_LINE_LOOP);
		glVertex2f(0.5, 0.5);
		glVertex2f(w - 0.5, 0.5);
		glVertex2f(w - 0.5, h - 0.5);
		glVertex2f(0.5, h - 0.5);
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}
}
Focusable::Focusable() :
	focused(false)
{}
void Frame::on_configure_event()
{
	Bin::on_configure_event();
	place_bin(3, text_height() + 2);
}
void Frame::on_draw()
{
	draw_children();
	int w = get_width();
	int h = get_height();
	float htop = 1.5 + get_ascent();
	glColor3f(0.6, 0.6, 0.6);
	glBegin(GL_LINE_LOOP);
	glVertex2f(1.5, htop);
	glVertex2f(w - 1.5, htop);
	glVertex2f(w - 1.5, h - 1.5);
	glVertex2f(1.5, h - 1.5);
	glEnd();
	glColor3f(0, 0, 0);
	glRasterPos2i(1, 1);
	draw_text(ALIGN_START, ALIGN_START, w - 2);
}
Frame::Frame(std::string const & title)
{
	set_text(title);
	set_reserved(6, text_height() + 5);
}
void glArea::on_display_func()
{
	assert(glutGetWindow() == get_win_id());
	gl_draw();
	glutSwapBuffers();
}
void glArea::on_reshape_func(int width, int height)
{
	gl_new_size(width, height);
}
void glArea::on_draw()
{
	if (int wid = get_win_id()) {
		int oid = glutGetWindow();
		glutSetWindow(wid);
		glutPostRedisplay();
		glutSetWindow(oid);
	}
}
void glArea::on_unrealize()
{
	if (get_win_id()) destroy();
}
void glArea::on_configure_event()
{
	if (! get_shown()) return;
	Allocation a = get_allocation();
	if (int wid = get_win_id()) {
		int oid = glutGetWindow();
		glutSetWindow(wid);
		glutReshapeWindow(a.width, a.height);
		glutPositionWindow(a.x, a.y);
		glutSetWindow(oid);
	}
	else {
		Window * w = get_window();
		assert(w);
		int pwin = w->get_id();
		assert(pwin);
		create(pwin, a.x, a.y, a.width, a.height);
		gl_setup();
	}
}
void glArea::on_show()
{
	if (int pwin = get_realized()) {
		if (int wid = get_win_id()) {
			glutSetWindow(wid);
			glutShowWindow();
		}
		else {
			Allocation a = get_allocation();
			create(pwin, a.x, a.y, a.width, a.height);
			gl_setup();
		}
	}
}
void glArea::on_hide()
{
	if (int wid = get_win_id()) {
		glutSetWindow(wid);
		glutHideWindow();
	}
}
void glArea::get_preferred_height(int & min, int & nat) const
{
	min = 0;
	nat = area_height;
}
void glArea::get_preferred_width(int & min, int & nat) const
{
	min = 0;
	nat = area_width;
}
glArea::glArea() :
	area_width(200),
	area_height(200)
{
	set_events(DISPLAY_MASK | RESHAPE_MASK | MOUSE_MASK | MOTION_MASK);
}
glArea::~glArea()
{
	if (get_win_id()) destroy();
}
void glArea::set_size(int width, int height)
{
	area_width = width;
	area_height = height;
}
namespace gltk {
	class _glutWindow
	{
		static std::map<int, glutWindow *> win_list;
		typedef void (* cb_setter)(glutWindow *, bool);
		static cb_setter cb_set_list[];
		static void display_func();
		static void reshape_func(int width, int height);
		static void mouse_func(int button, int state, int x, int y);
		static void motion_func(int x, int y);
		static void passive_func(int x, int y);
		static void keyboard_func(unsigned char key, int x, int y);
		static void special_func(int key, int x, int y);
		static void entry_func(int state);
	public:
		static void create(glutWindow * w, int parent, int x, int y, int width, int height);
		static void create(glutWindow * w, std::string const & name, int width, int height);
		static void destroy(glutWindow * w);
		static void destroy_all();
		static void cb_set_display(glutWindow *, bool);
		static void cb_set_reshape(glutWindow *, bool);
		static void cb_set_mouse(glutWindow *, bool);
		static void cb_set_motion(glutWindow *, bool);
		static void cb_set_passive(glutWindow *, bool);
		static void cb_set_keyboard(glutWindow *, bool);
		static void cb_set_special(glutWindow *, bool);
		static void cb_set_entry(glutWindow *, bool);
		static void setup_cb(glutWindow * g, unsigned origin = 0);
	};
}
std::map<int, glutWindow *> _glutWindow::win_list;
_glutWindow::cb_setter _glutWindow::cb_set_list[] = {
	& _glutWindow::cb_set_display,
	& _glutWindow::cb_set_reshape,
	& _glutWindow::cb_set_mouse,
	& _glutWindow::cb_set_motion,
	& _glutWindow::cb_set_passive,
	& _glutWindow::cb_set_keyboard,
	& _glutWindow::cb_set_special,
	& _glutWindow::cb_set_entry
};
void _glutWindow::display_func()
{
	glutWindow & w = * win_list[glutGetWindow()];
	w.on_display_func();
}
void _glutWindow::reshape_func(int width, int height)
{
	glutWindow & w = * win_list[glutGetWindow()];
	w.on_reshape_func(width, height);
}
void _glutWindow::mouse_func(int button, int state, int x, int y)
{
	glutWindow & w = * win_list[glutGetWindow()];
	w.on_mouse_func(button, state, x, y);
}
void _glutWindow::motion_func(int x, int y)
{
	glutWindow & w = * win_list[glutGetWindow()];
	w.on_motion_func(x, y);
}
void _glutWindow::passive_func(int x, int y)
{
	glutWindow & w = * win_list[glutGetWindow()];
	w.on_passive_func(x, y);
}
void _glutWindow::keyboard_func(unsigned char key, int x, int y)
{
	glutWindow & w = * win_list[glutGetWindow()];
	w.on_keyboard_func(key, x, y);
}
void _glutWindow::special_func(int key, int x, int y)
{
	glutWindow & w = * win_list[glutGetWindow()];
	w.on_special_func(key, x, y);
}
void _glutWindow::entry_func(int state)
{
	glutWindow & w = * win_list[glutGetWindow()];
	w.on_entry_func(state);
}
void _glutWindow::create(glutWindow * g, int p, int x, int y, int w, int h)
{
	assert(! g->win_id);
	if (p) {
		g->win_id = glutCreateSubWindow(p, x, y, w, h);
	}
	else {
		glutInitWindowSize(w, h);
		glutInitWindowPosition(x, y);
		g->win_id = glutCreateWindow("gltk Window");
	}
	win_list[g->win_id] = g;
	g->on_create();
}
void _glutWindow::create(glutWindow * g, std::string const & name, int w, int h)
{
	glutInitWindowSize(w, h);
	g->win_id = glutCreateWindow(name.c_str());
	g->on_create();
	win_list[g->win_id] = g;
}
void _glutWindow::destroy(glutWindow * g)
{
	win_list.erase(g->win_id);
	g->on_destroy();
	glutDestroyWindow(g->win_id);
	g->win_id = 0;
}
void _glutWindow::cb_set_display(glutWindow * , bool v)
{
	if (v) glutDisplayFunc(& display_func);
	else glutDisplayFunc(0);
}
void _glutWindow::cb_set_reshape(glutWindow * , bool v)
{
	if (v) glutReshapeFunc(& reshape_func);
	else glutReshapeFunc(0);
}
void _glutWindow::cb_set_mouse(glutWindow * , bool v)
{
	if (v) glutMouseFunc(& mouse_func);
	else glutMouseFunc(0);
}
void _glutWindow::cb_set_motion(glutWindow * , bool v)
{
	if (v) glutMotionFunc(& motion_func);
	else glutMotionFunc(0);
}
void _glutWindow::cb_set_passive(glutWindow * , bool v)
{
	if (v) glutPassiveMotionFunc(& passive_func);
	else glutPassiveMotionFunc(0);
}
void _glutWindow::cb_set_keyboard(glutWindow * , bool v)
{
	if (v) glutKeyboardFunc(& keyboard_func);
	else glutKeyboardFunc(0);
}
void _glutWindow::cb_set_special(glutWindow * , bool v)
{
	if (v) glutSpecialFunc(& special_func);
	else glutSpecialFunc(0);
}
void _glutWindow::cb_set_entry(glutWindow * , bool v)
{
	if (v) glutEntryFunc(& entry_func);
	else glutEntryFunc(0);
}
void _glutWindow::setup_cb(glutWindow * g, unsigned origin)
{
	for (size_t i = 0; i < 8; i ++) {
		unsigned mask = 1 << i;
		bool f0 = origin & mask;
		bool f1 = g->event_mask & mask;
		if (f1 != f0) (* cb_set_list[i])(g, f1);
	}
}
glutWindow::glutWindow() :
	win_id(0),
	event_mask(EVENT_MASK_NONE)
{
}
glutWindow::~glutWindow()
{
}
void glutWindow::set_events(unsigned mask)
{
	if (win_id) {
		unsigned om = event_mask;
		event_mask = mask;
		_glutWindow::setup_cb(this, om);
	}
	else event_mask = mask;
}
void glutWindow::add_events(unsigned mask)
{
	if (win_id) {
		unsigned om = event_mask;
		event_mask |= mask;
		_glutWindow::setup_cb(this, om);
	}
	else event_mask |= mask;
}
void glutWindow::create(int parent_win_id, int x, int y, int width, int height)
{
	_glutWindow::create(this, parent_win_id, x, y, width, height);
	_glutWindow::setup_cb(this);
}
void glutWindow::create(std::string const & name, int width, int height)
{
	_glutWindow::create(this, name, width, height);
	_glutWindow::setup_cb(this);
}
void glutWindow::destroy()
{
	assert(win_id);
	_glutWindow::destroy(this);
}
void glutWindow::set_current()
{
	assert(win_id);
	glutSetWindow(win_id);
}
void glutWindow::move(int x, int y)
{
	glutSetWindow(win_id);
	glutPositionWindow(x, y);
}
void glutWindow::resize(int width, int height)
{
	assert(win_id);
	glutSetWindow(win_id);
	glutReshapeWindow(width, height);
}
void glutWindow::redisplay()
{
	if (win_id) {
		glutSetWindow(win_id);
		glutPostRedisplay();
	}
}
namespace {
	unsigned idle_unique_id = 0;
	struct IdleEvent
	{
		unsigned idle_id;
		sigc::functor_base * fun;
	};
	std::list<IdleEvent> idle_event_list;
	void idle_func()
	{
		if (idle_event_list.size()) {
			IdleEvent e = idle_event_list.front();
			idle_event_list.pop_front();
			if (sigc::functor0<bool> * f = dynamic_cast<sigc::functor0<bool> *>(e.fun)) {
				if (f->call()) {
					idle_event_list.push_back(e);
					return;
				}
			}
			else if (sigc::functor0<void> * f = dynamic_cast<sigc::functor0<void> *>(e.fun)) {
				f->call();
			}
			if (! idle_event_list.size()) glutIdleFunc(0);
		}
		else glutIdleFunc(0);
	}
	struct _Idle :
		public sigc::signal_base
	{
		unsigned add_handle(sigc::slot_base const & slot)
		{
			(void)slot;
			throw Error("add_handle() called on idle_signal!");
		}
		void remove_handle(unsigned hid)
		{
			if (! idle_event_list.size()) return;
			for (std::list<IdleEvent>::iterator i = idle_event_list.begin(); i != idle_event_list.end(); i ++)
			if (i->idle_id == hid) {
				delete i->fun;
				idle_event_list.erase(i);
				break;
			}
			if (! idle_event_list.size()) {
				glutIdleFunc(0);
			}
		}
	} idle;
}
sigc::connection SignalIdle::connect(sigc::slot0<bool> const & slot)
{
	IdleEvent e;
	e.idle_id = idle_unique_id;
	e.fun = slot.fun->dup();
	idle_event_list.push_back(e);
	glutIdleFunc(& idle_func);
	return sigc::connection(& idle, unsigned(idle_unique_id ++));
}
void SignalIdle::connect_once(sigc::slot0<void> const & slot)
{
	IdleEvent e;
	e.idle_id = idle_unique_id;
	e.fun = slot.fun->dup();
	idle_event_list.push_back(e);
	glutIdleFunc(& idle_func);
	idle_unique_id ++;
}
namespace {
	struct glutFontInfo
	{
		void * font;
		int height;
		int ascent;
		int descent;
	} gf_info [] = {
		{GLUT_BITMAP_8_BY_13, 13, 11, 2},
		{GLUT_BITMAP_9_BY_15, 15, 13, 2},
		{GLUT_BITMAP_TIMES_ROMAN_10, 10, 7, 3},
		{GLUT_BITMAP_TIMES_ROMAN_24, 24, 18, 6},
		{GLUT_BITMAP_HELVETICA_10, 10, 8, 2},
		{GLUT_BITMAP_HELVETICA_12, 12, 9, 3},
		{GLUT_BITMAP_HELVETICA_18, 18, 15, 3},
		{0, 0, 0, 0}
	};
	glutFontInfo const & get_info(void * font)
	{
		size_t i = 0;
		while (gf_info[i].font && font != gf_info[i].font) i ++;
		return gf_info[i];
	}
}
int gltk::glut_get_font_height(void * font)
{
	return get_info(font).height;
}
int gltk::glut_get_font_ascent(void * font)
{
	return get_info(font).ascent;
}
int gltk::glut_get_font_descent(void * font)
{
	return get_info(font).descent;
}
void gltk::glut_draw_string(void * font, std::string const & text)
{
	for (size_t i = 0; i < text.size(); i ++) glutBitmapCharacter(font, text[i]);
}
void gltk::glut_draw_string(void * font, std::string const & text, int limit)
{
	int w = 0;
	for (size_t i = 0; i < text.size(); i ++) {
		w += glutBitmapWidth(font, text[i]);
		if (w > limit) return;
		glutBitmapCharacter(font, text[i]);
	}
}
int gltk::glut_get_string_width(void * font, std::string const & text)
{
	int w = 0;
	for (size_t i = 0; i < text.size(); i ++) w += glutBitmapWidth(font, text[i]);
	return w;
}
void gltk::glut_draw_raised_box(int x, int y, int w, int h)
{
	w = w + x;
	h = h + y;
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(x + 0.5, h - 0.5);
	glVertex2f(x + 0.5, y + 0.5);
	glVertex2f(w - 0.5, y + 0.5);
	glEnd();
	glColor3f(0.8, 0.8, 0.8);
	glBegin(GL_LINE_STRIP);
	glVertex2f(x + 1.5, h - 1.5);
	glVertex2f(x + 1.5, y + 1.5);
	glVertex2f(w - 1.5, y + 1.5);
	glEnd();
	glColor3f(0.4, 0.4, 0.4);
	glBegin(GL_LINE_STRIP);
	glVertex2f(w - 0.5, y + 0.5);
	glVertex2f(w - 0.5, h - 0.5);
	glVertex2f(x + 0.5, h - 0.5);
	glEnd();
	glColor3f(0.8, 0.8, 0.8);
	glBegin(GL_LINE_STRIP);
	glVertex2f(w - 1.5, y + 1.5);
	glVertex2f(w - 1.5, h - 1.5);
	glVertex2f(x + 1.5, h - 1.5);
	glEnd();
}
int gltk::glut_raised_box_width()
{
	return 2;
}
void gltk::glut_draw_sunken_box(int x, int y, int w, int h)
{
	w = w + x;
	h = h + y;
	glColor3f(0.8, 0.8, 0.8);
	glBegin(GL_LINE_STRIP);
	glVertex2f(x + 0.5, h - 0.5);
	glVertex2f(x + 0.5, y + 0.5);
	glVertex2f(w - 0.5, y + 0.5);
	glEnd();
	glColor3f(0.4, 0.4, 0.4);
	glBegin(GL_LINE_STRIP);
	glVertex2f(x + 1.5, h - 1.5);
	glVertex2f(x + 1.5, y + 1.5);
	glVertex2f(w - 1.5, y + 1.5);
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(w - 0.5, y + 0.5);
	glVertex2f(w - 0.5, h - 0.5);
	glVertex2f(x + 0.5, h - 0.5);
	glEnd();
	glColor3f(0.8, 0.8, 0.8);
	glBegin(GL_LINE_STRIP);
	glVertex2f(w - 1.5, y + 1.5);
	glVertex2f(w - 1.5, h - 1.5);
	glVertex2f(x + 1.5, h - 1.5);
	glEnd();
}
int gltk::glut_sunken_box_width()
{
	return 2;
}
void Label::on_draw()
{
	int y = get_height() / 2;
	int w = min_of(text_width(), get_width());
	int x = (get_width() - w) / 2;
	glColor3f(0, 0, 0);
	glRasterPos2i(x, y);
	draw_text(ALIGN_START, ALIGN_CENTER, w);
}
void Label::get_preferred_height(int & min, int & nat) const
{
	min = text_height();
	nat = min + 2;
}
void Label::get_preferred_width(int & min, int & nat) const
{
	min = text_width();
	nat = min + 2;
}
void Label::on_set_text()
{
	queue_resize();
}
Label::Label()
{}
Label::Label(std::string const & label, bool )
{
	set_text(label);
}
LabelBin::LabelBin(Widget * child) :
	own_child(false)
{
	add(child);
}
LabelBin::LabelBin(std::string const & name) :
	own_child(true)
{
	add(manage(new Label(name)));
}
LabelBin::~LabelBin()
{
}
void LabelBin::set_text(std::string name)
{
	if (Label * l = dynamic_cast<Label *>(child)) l->set_text(name);
}
std::string ViewableList::ItemBase::str() const
{
	return "item";
}
int RenderItemStr::get_height()
{
	return glut_get_font_height(font) + 2;
}
void RenderItemStr::render(ViewableList::ItemBase * item, int x, int y, int width, int )
{
	glColor3f(0, 0, 0);
	glRasterPos2i(x + 1, y + 1 + glut_get_font_ascent(font));
	glut_draw_string(font, item->str(), width - 2);
}
RenderItemStr::RenderItemStr() :
	font(GLUT_BITMAP_HELVETICA_12)
{
}
void ListView::clear_click()
{
	last_click = false;
}
void ListView::on_draw()
{
	int h = get_height();
	int w = get_width();
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex2i(0, 0);
	glVertex2i(w, 0);
	glVertex2i(w, h);
	glVertex2i(0, h);
	glEnd();
	glut_draw_sunken_box(0, 0, w, h);
	int bw = glut_sunken_box_width();
	int aw = w - bw;
	if (list) {
		glColor3f(0, 0, 0);
		int y = bw;
		if (line_start && line_start + line_visible > (int) list->size()) line_start = max_of(0, int(list->size()) - line_visible);
		line_end = min_of((int) list->size(), line_start + line_visible);
		if (select >= 0 && select < (int) list->size() && select >= line_end) {
			line_start += select + 1 - line_end;
			line_end = select + 1;
		}
		for (int i = line_start; i < line_end; i ++) {
			if (i == select) {
				int sy0 = (i - line_start) * line_height + bw;
				int sy1 = sy0 + line_height;
				if (focused) {
					glColor3f(0.8, 0.9, 1.0);
					glBegin(GL_QUADS);
					glVertex2i(bw, sy0);
					glVertex2i(w - bw, sy0);
					glVertex2i(w - bw, sy1);
					glVertex2i(bw, sy1);
					glEnd();
				}
				else {
					glColor3f(0.6, 0.7, 0.8);
					glBegin(GL_LINE_LOOP);
					glVertex2f(bw + 0.5, sy0 + 0.5);
					glVertex2f(w - bw - 0.5, sy0 + 0.5);
					glVertex2f(w - bw - 0.5, sy1 - 0.5);
					glVertex2f(bw + 0.5, sy1 - 0.5);
					glEnd();
				}
			}
			if (render) render->render(list->get_item(i), bw, y, aw, line_height);
			y += line_height;
		}
		value = line_start;
		upper = list->size();
		page_increment = line_visible - 1;
		page_size = line_end - line_start;
	}
}
void ListView::on_configure_event()
{
	line_visible = (get_height() - glut_sunken_box_width() * 2) / line_height;
}
void ListView::on_button_event(int button, int state, int , int y)
{
	if (! focused) {
		if (get_focus()) {
			focused = true;
			queue_draw();
		}
	}
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			int sel = line_start + (int) floor(double(y - 1) / line_height);
			if (sel >= line_start && sel < line_end) {
				if (! last_click) {
					last_click = true;
					gltk::SignalTimeout::connect_once(sigc::mem_fun(this, & ListView::clear_click), 300);
					if (select == sel) return;
					select = sel;
				}
				else {
					last_click = false;
					if (select == sel) {
						selected(select);
						select = - 1;
					}
					else select = sel;
				}
			}
			else select = - 1;
			queue_draw();
		}
	}
	else if (button == 3) {
		if (state != GLUT_DOWN) return;
		if (select < 0) {
			if (line_start) line_start --;
			else return;
		}
		else if (select > 0) {
			select --;
			if (select >= 0 && select < line_start) line_start = select;
		}
		else return;
		queue_draw();
	}
	else if (button == 4) {
		if (state != GLUT_DOWN) return;
		assert(list);
		if (select < 0) {
			if (line_start + line_visible < int(list->size())) line_start ++;
			else return;
		}
		else if (select < int(list->size()) - 1) {
			select ++;
			if (select >= line_end) {
				line_start += select - line_end + 1;
			}
		}
		else return;
		queue_draw();
	}
}
bool ListView::on_key_event(unsigned char key)
{
	switch (key) {
	case 0xd:
		if (select >= 0) {
			selected(select);
			select = - 1;
		}
		break;
	case 0x20:
		if (select < 0) {
			select = 0;
			line_start = select;
		}
		else select = - 1;
		break;
	default:
		return false;
	}
	queue_draw();
	return true;
}
bool ListView::on_special_event(int key)
{
	if (! list) return false;
	switch (key) {
	case GLUT_KEY_UP:
		if (select > 0) {
			select --;
		}
		else select = 0;
		break;
	case GLUT_KEY_DOWN:
		if (select >= 0 && select < int(list->size()) - 1) {
			select ++;
		}
		else select = 0;
		break;
	case GLUT_KEY_PAGE_UP:
		if (select < line_visible) select = 0;
		else select -= line_visible;
		break;
	case GLUT_KEY_PAGE_DOWN:
		if (select < 0) select = 0;
		else {
			select += line_visible;
			if (select >= (int) list->size()) select = list->size() - 1;
		}
		break;
	default:
		return false;
	}
	if (select >= 0) {
		if (select < line_start) line_start = select;
		else if (select >= line_end) {
			line_start += select - line_end + 1;
		}
	}
	queue_draw();
	return true;
}
void ListView::on_value_changed()
{
	if (! list) throw Error("No list!");
	int v = value;
	if (v > line_start) {
		line_start = min_of(int(list->size()) - line_visible, v);
		if (line_start < 0) line_start = 0;
		value = line_start;
		if (select >= 0 && select < line_start) select = line_start;
	}
	else if (v < line_start) {
		line_start = v;
		if (select >= 0 && select >= line_start + line_visible) select = line_start + line_visible - 1;
	}
	queue_draw();
}
void ListView::get_preferred_height(int & min, int & nat) const
{
	if (list && render) {
		min = line_height + 4;
		nat = max_of(min, min_of((int(list->size()) + 1) * line_height, 400));
	}
	else {
		min = 20;
		nat = 200;
	}
}
void ListView::get_preferred_width(int & min, int & nat) const
{
	min = 100;
	nat = 200;
}
void ListView::list_change()
{
	queue_draw();
}
ListView::ListView() :
	Adjustment(0, 0, 10, 1, 10, 1),
	list(0),
	select(- 1),
	last_click(false)
{
	line_start = 0;
	line_end = 0;
	lower = 0;
	step_increment = 1;
	page_size = 1;
}
void ListView::set_list(ViewableList * l)
{
	list = l;
}
void ListView::add_render(ViewableList::RenderBase * rndr)
{
	render = rndr;
	line_height = render->get_height();
}
sigc::signal<void, int> & ListView::signal_selected()
{
	return selected;
}
Main::Main(int & argc, char * * (& argv))
{
	glutInit(& argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
}
void Main::run(Window & w)
{
	w.show();
	glutMainLoop();
}
Orientable::Orientable(Orientation orientation) :
	hori(orientation == ORIENTATION_HORIZONTAL)
{}
Orientation Orientable::get_orientation() const
{
	return hori ? ORIENTATION_HORIZONTAL : ORIENTATION_VERTICAL;
}
void Orientable::set_orientation(Orientation orientation)
{
	hori = orientation == ORIENTATION_HORIZONTAL;
}
std::map<int, Popup *> popup_list;
void Popup::menu_callback(int item)
{
	Popup & p = * popup_list[glutGetMenu()];
	p.on_menu_select(item);
}
void Popup::menu_status(int , int , int )
{
}
void Popup::on_entry_event(bool ent)
{
	get_window()->make_current();
	if (ent) {
		if (menu_id) glutSetMenu(menu_id);
		else {
			menu_id = glutCreateMenu(& menu_callback);
			popup_list[menu_id] = this;
			for (int i = 0; i < (int) menu.size(); i ++) {
				glutAddMenuEntry(menu[i].c_str(), i);
			}
		}
		glutAttachMenu(GLUT_LEFT_BUTTON);
		glutSetCursor(GLUT_CURSOR_UP_DOWN);
	}
	else {
		if (menu_id) {
			glutSetMenu(menu_id);
			glutDetachMenu(GLUT_LEFT_BUTTON);
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
	}
}
void Popup::on_unrealized()
{
	release_menu();
}
void Popup::release_menu()
{
	if (menu_id) {
		popup_list.erase(menu_id);
		glutDestroyMenu(menu_id);
		menu_id = 0;
	}
}
Popup::Popup() :
	menu_id(0)
{}
void PushBox::on_draw()
{
	draw_box();
}
void PushBox::get_preferred_width(int & min, int & nat) const
{
	min = box_width * 2;
	nat = min + 10;
}
void PushBox::get_preferred_height(int & min, int & nat) const
{
	min = box_width * 2;
	nat = min + 10;
}
void PushBox::on_button_event(int button, int state, int x, int y)
{
	if (button != GLUT_LEFT_BUTTON) return;
	if (state == GLUT_DOWN) {
		depressed = true;
		set_grabbing();
		pushed();
	}
	else if (depressed) {
		set_grabbing(false);
		depressed = false;
		if (x >= 0 && y >= 0 && x < get_width() && y < get_height()) clicked();
	}
	queue_draw();
}
bool PushBox::on_key_event(unsigned char key)
{
	if (key == 0xd) {
		pushed();
		clicked();
		return true;
	}
	return false;
}
void PushBox::draw_box()
{
	int w = get_width();
	int h = get_height();
	int bw = glut_raised_box_width();
	glColor3f(0.9, 0.9, 0.9);
	glBegin(GL_QUADS);
	glVertex2i(bw, bw);
	glVertex2i(w - bw, bw);
	glVertex2i(w - bw, h - bw);
	glVertex2i(bw, h - bw);
	glEnd();
	if (depressed) {
		glColor3f(0.6, 0.6, 0.6);
		glBegin(GL_LINE_LOOP);
		glVertex2f(0.5, 0.5);
		glVertex2f(w - 0.5, 0.5);
		glVertex2f(w - 0.5 , h - 0.5);
		glVertex2f(0.5, h - 0.5);
		glEnd();
	}
	else glut_draw_raised_box(0, 0, w, h);
	if (focused) {
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0x5555);
		glColor3f(0., 0., 0.);
		glBegin(GL_LINE_LOOP);
		double d = bw + 0.5;
		glVertex2f(d, d);
		glVertex2f(w - d, d);
		glVertex2f(w - d, h - d);
		glVertex2f(d, h - d);
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}
}
PushBox::PushBox() :
	box_width(4),
	depressed(false)
{}
void Scale::update_label()
{
	std::ostringstream s;
	double v = sl.get_value();
	if (v < 0) {
		s << '-';
		v = - v;
	}
	int t = v;
	s << t;
	v -= t;
	if (digits) {
		s << '.';
		for (int i = 0; i < digits; i ++) {
			v *= 10;
			t = v;
			s << t;
			v -= t;
		}
	}
	set_text(s.str());
	std::istringstream ss(s.str());
	ss >> v;
	sl.set_value(v);
}
void Scale::update_wtxt()
{
	if (! draw_value) return;
	std::ostringstream s;
	s << int(sl.get_adjustment()->get_lower());
	if (digits) {
		s << '.';
		s << std::string(digits, '0');
	}
	wtxt = text_width(s.str());
	s.str("");
	s << int(sl.get_adjustment()->get_upper());
	if (digits) {
		s << '.';
		s << std::string(digits, '0');
	}
	wtxt = max_of(wtxt, text_width(s.str()));
}
void Scale::on_configure_event()
{
	Bin::on_configure_event();
	if (! draw_value) return;
	int cw = child->get_width();
	int ch = child->get_height();
	int w = get_width();
	int h = get_height();
	switch (pos) {
	case POS_TOP:
		place_bin(0, h - ch);
		break;
	case POS_BOTTOM:
		place_bin(0, 0);
		break;
	case POS_LEFT:
		place_bin(w - cw, 0);
		break;
	case POS_RIGHT:
		place_bin(0, 0);
	}
}
void Scale::on_draw()
{
	draw_children();
	if (draw_value) {
		int w = get_width();
		int h = get_height();
		int tw = text_width();
		int th = text_height();
		int c = sl.get_slider_pos() + sl.get_slider_size() / 2;
		if (sl.get_hori()) {
			if (pos == POS_TOP || pos == POS_BOTTOM) {
				c = max_of(c, tw / 2);
				c = min_of(c, w - tw / 2);
			}
			else {
				c = h / 2;
			}
		}
		else {
			if (pos == POS_LEFT || pos == POS_RIGHT) {
				c = max_of(c, th / 2);
				c = min_of(c, w - th / 2);
			}
			else {
				c = w / 2;
			}
		}
		glColor3f(0, 0, 0);
		switch (pos) {
		case POS_TOP:
			glRasterPos2i(c, 0);
			draw_text(ALIGN_CENTER, ALIGN_START, tw);
			break;
		case POS_BOTTOM:
			glRasterPos2i(c, h);
			draw_text(ALIGN_CENTER, ALIGN_END, tw);
			break;
		case POS_LEFT:
			glRasterPos2i(0, c);
			draw_text(ALIGN_START, ALIGN_CENTER, tw);
			break;
		case POS_RIGHT:
			glRasterPos2i(w, c);
			draw_text(ALIGN_END, ALIGN_CENTER, tw);
			break;
		}
	}
}
void Scale::get_preferred_width(int & min, int & nat) const
{
	Bin::get_preferred_width(min, nat);
	if (draw_value) {
		if (min < wtxt) {
			min = wtxt;
			if (nat < min) nat = min;
		}
	}
}
void Scale::get_preferred_width_for_height(int height, int & min, int & nat) const
{
	Bin::get_preferred_width_for_height(height, min, nat);
	if (draw_value) {
		if (min < wtxt) {
			min = wtxt;
			if (nat < min) nat = min;
		}
	}
}
Scale::Scale(Adjustment * adjust, Orientation orient) :
	sl(adjust, orient),
	pos(POS_TOP),
	digits(3),
	draw_value(true)
{
	update_label();
	update_wtxt();
	set_reserved(0, text_height());
	adjust->signal_value_changed().connect(sigc::mem_fun(this, & Scale::update_label));
	adjust->signal_changed().connect(sigc::mem_fun(this, & Scale::update_wtxt));
	add(sl);
	sl.show();
}
void Scale::set_digits(int dgt)
{
	digits = dgt;
	update_label();
	update_wtxt();
	if (draw_value && (pos == POS_LEFT || pos == POS_RIGHT)) {
		set_reserved(wtxt, 0);
	}
	queue_resize();
}
void Scale::set_draw_value(bool dv)
{
	draw_value = dv;
	update_wtxt();
	if (draw_value) {
		if (pos == POS_TOP || pos == POS_BOTTOM) set_reserved(0, text_height());
		else set_reserved(wtxt, 0);
	}
	else set_reserved(0, 0);
	queue_resize();
}
void Scale::set_value_pos(PositionType p)
{
	pos = p;
	if (draw_value) {
		if (pos == POS_TOP || pos == POS_BOTTOM) set_reserved(0, text_height());
		else set_reserved(wtxt, 0);
		queue_resize();
	}
}
void Scrollbar::on_configure_event()
{
	int width = get_width();
	int height = get_height();
	if (hori) {
		if (width < 20) {
			up.size_allocate(0, 0, 0, height);
			dn.size_allocate(width, 0, 0, height);
		}
		else {
			up.size_allocate(0, 0, 10, height);
			dn.size_allocate(width - 10, 0, 10, height);
		}
	}
	else {
		if (height < 20) {
			up.size_allocate(0, 0, width, 0);
			dn.size_allocate(0, height, width, 0);
		}
		else {
			up.size_allocate(0, 0, width, 10);
			dn.size_allocate(0, height - 10, width, 10);
		}
	}
}
void Scrollbar::on_draw()
{
	int h = get_height();
	int w = get_width();
	if ((hori ? w : h) < 20) return;
	draw_children();
	double upper = adjust->get_upper();
	double lower = adjust->get_lower();
	double value = adjust->get_value();
	double psize = adjust->get_page_size();
	glBegin(GL_LINES);
	if (hori) {
		glColor3f(0.8, 0.8, 0.8);
		glVertex2f(10, 0.5);
		glVertex2f(w - 10, 0.5);
		glColor3f(1.0, 1.0, 1.0);
		glVertex2f(10, h - 0.5);
		glVertex2f(w - 10, h - 0.5);
	}
	else {
		glColor3f(0.8, 0.8, 0.8);
		glVertex2f(0.5, 10);
		glVertex2f(0.5, h - 10);
		glColor3f(1.0, 1.0, 1.0);
		glVertex2f(w - 0.5, 10);
		glVertex2f(w - 0.5, h - 10);
	}
	glEnd();
	int z = (hori ? w : h) - 20;
	slider_sz = max_of(round_int(psize * z / (upper - lower)), 10);
	slider_z0 = 10;
	if (slider_sz < z) slider_z0 += round_int((value - lower) * (z - slider_sz) / (upper - lower - psize));
	if (hori) glut_draw_raised_box(slider_z0, 0, slider_sz, h);
	else glut_draw_raised_box(0, slider_z0, w, slider_sz);
}
void Scrollbar::on_button_event(int button, int state, int x, int y)
{
	assert(adjust);
	if (button == GLUT_LEFT_BUTTON) {
		if (grabber) {
			assert(! sliding);
			assert(state == GLUT_UP);
			grabber->button_event(button, state, x, y);
		}
		else if (sliding) {
			assert(state == GLUT_UP);
			set_grabbing(false);
			sliding = false;
		}
		else if (Widget * w = get_child(x, y)) {
			w->button_event(button, state, x, y);
		}
		else if (state == GLUT_DOWN) {
			int z = hori ? x : y;
			if (z < slider_z0) adjust->page_down();
			else if (z >= slider_z0 + slider_sz) adjust->page_up();
			else {
				set_grabbing();
				sliding = true;
				sliding_start = z;
				sliding_vs = adjust->get_value();
			}
		}
		if (grabber && ! held) {
			spin_interval = spin_wait / 2;
			SignalTimeout::connect_once(sigc::mem_fun(this, & Scrollbar::button_held), spin_wait);
			held = true;
			spinning = false;
		}
	}
	else if (button == 3) {
		if (state == GLUT_DOWN) adjust->step_down();
	}
	else if (button == 4) {
		if (state == GLUT_DOWN) adjust->step_up();
	}
}
void Scrollbar::on_motion_event(int x, int y)
{
	if (grabber) {
		grabber->motion_event(x, y);
	}
	else {
		assert(sliding);
		int dz = (hori ? x : y) - sliding_start;
		int rg = (hori ? get_width() : get_height()) - 20 - slider_sz;
		if (! rg) return;
		double vr = adjust->get_upper() - adjust->get_lower() - adjust->get_page_size();
		assert(vr > 0);
		double dv = dz * vr / rg;
		adjust->set_value(sliding_vs + dv);
	}
}
void Scrollbar::button_held()
{
	if (! grabber) {
		held = false;
		return;
	}
	spinning = true;
	if (! adjust) throw Error("No adjust!");
	if (grabber == & up) adjust->step_down();
	else if (grabber == & dn) adjust->step_up();
	else throw Error("Unknown grabber...");
	spin_interval -= (spin_interval / 3);
	spin_interval = max_of(spin_interval, spin_wait / 10);
	SignalTimeout::connect_once(sigc::mem_fun(this, & Scrollbar::button_held), spin_interval);
}
void Scrollbar::pushed_up()
{
	if (! adjust) throw Error("No adjust!");
	adjust->step_down();
}
void Scrollbar::pushed_dn()
{
	if (! adjust) throw Error("No adjust!");
	adjust->step_up();
}
void Scrollbar::get_preferred_width(int & min, int & nat) const
{
	if (hori) {
		min = 30;
		nat = 200;
	}
	else {
		min = 16;
		nat = 16;
	}
}
void Scrollbar::get_preferred_height(int & min, int & nat) const
{
	if (hori) {
		min = 16;
		nat = 16;
	}
	else {
		min = 30;
		nat = 200;
	}
}
Scrollbar::Scrollbar(Adjustment * adj, Orientation orient) :
	adjust(adj),
	hori(orient == ORIENTATION_HORIZONTAL),
	up(new Shape((hori ? Shape::LEFT_TRIANGLE : Shape::UP_TRIANGLE), 15, 8)),
	dn(new Shape((hori ? Shape::RIGHT_TRIANGLE : Shape::DOWN_TRIANGLE), 15, 8)),
	spin_wait(700),
	held(false),
	spinning(false),
	sliding(false)
{
	add(& up);
	add(& dn);
	up.set_reserved(5, 5);
	dn.set_reserved(5, 5);
	up.set_box_width(2);
	dn.set_box_width(2);
	up.signal_pushed().connect(sigc::mem_fun(this, & Scrollbar::pushed_up));
	dn.signal_pushed().connect(sigc::mem_fun(this, & Scrollbar::pushed_dn));
}
void Separator::on_draw()
{
	int w = get_width();
	int h = get_height();
	glBegin(GL_LINES);
	if (hori) {
		glColor3f(0.8, 0.8, 0.8);
		glVertex2f(0, (h - 1) / 2.0);
		glVertex2f(w, (h - 1) / 2.0);
		glColor3f(1.0, 1.0, 1.0);
		glVertex2f(0, (h + 1) / 2.0);
		glVertex2f(w, (h + 1) / 2.0);
	}
	else {
		glColor3f(0.8, 0.8, 0.8);
		glVertex2f((w - 1) / 2.0, 0);
		glVertex2f((w - 1) / 2.0, h);
		glColor3f(1.0, 1.0, 1.0);
		glVertex2f((w + 1) / 2.0, 0);
		glVertex2f((w + 1) / 2.0, h);
	}
	glEnd();
}
void Separator::get_preferred_width(int & min, int & nat) const
{
	if (hori) {
		min = 0;
		nat = 4;
	}
	else {
		min = 4;
		nat = 4;
	}
}
void Separator::get_preferred_height(int & min, int & nat) const
{
	if (hori) {
		min = 4;
		nat = 4;
	}
	else {
		min = 0;
		nat = 4;
	}
}
Separator::Separator(Orientation orientation) :
	hori(orientation == ORIENTATION_HORIZONTAL)
{
}
void Shape::on_draw()
{
	int w = get_width();
	int h = get_height();
	switch (shape) {
	case UP_TRIANGLE:
		glColor3f(0, 0, 0);
		glBegin(GL_TRIANGLES);
		glVertex2f(0, h);
		glVertex2f(w / 2.0, 0);
		glVertex2f(w, h);
		glEnd();
		break;
	case DOWN_TRIANGLE:
		glColor3f(0, 0, 0);
		glBegin(GL_TRIANGLES);
		glVertex2f(0, 0);
		glVertex2f(w / 2.0, h);
		glVertex2f(w, 0);
		glEnd();
		break;
	case LEFT_TRIANGLE:
		glColor3f(0, 0, 0);
		glBegin(GL_TRIANGLES);
		glVertex2f(0, h / 2.0);
		glVertex2f(w, h);
		glVertex2f(w, 0);
		glEnd();
		break;
	case RIGHT_TRIANGLE:
		glColor3f(0, 0, 0);
		glBegin(GL_TRIANGLES);
		glVertex2f(0, 0);
		glVertex2f(w, h / 2.0);
		glVertex2f(0, h);
		glEnd();
		break;
	default:
		throw Error("Unknown shape");
	}
}
void Shape::get_preferred_width(int & min, int & nat) const
{
	min = 0;
	nat = width;
}
void Shape::get_preferred_height(int & min, int & nat) const
{
	min = 0;
	nat = height;
}
Shape::Shape(Predefined s, int w, int h) :
	shape(s),
	width(w),
	height(h)
{
}
void Slider::on_draw()
{
	int w = get_width();
	int h = get_height();
	int z = hori ? w : h;
	if (adjust) {
		double upper = adjust->get_upper() - adjust->get_page_size();
		double lower = adjust->get_lower();
		double value = adjust->get_value();
		if (upper > lower) {
			slider_z0 = (value - lower) * (z - slider_sz) / (upper - lower);
			glBegin(GL_QUADS);
			glColor3f(0.8, 0.8, 1.0);
			if (hori) {
				glVertex2i(0, (h - slider_sz) / 2);
				glVertex2i(slider_z0, (h - slider_sz) / 2);
				glVertex2i(slider_z0, (h + slider_sz) / 2);
				glVertex2i(0, (h + slider_sz) / 2);
			}
			else {
				glVertex2i((w - slider_sz) / 2, 0);
				glVertex2i((w - slider_sz) / 2, slider_z0);
				glVertex2i((w + slider_sz) / 2, slider_z0);
				glVertex2i((w + slider_sz) / 2, 0);
			}
			glEnd();
		}
		else slider_z0 = - 1;
	}
	int mz = (hori ? h : w) / 2;
	glBegin(GL_LINES);
	glColor3f(0.7, 0.7, 0.7);
	if (hori) {
		glVertex2f(slider_sz / 2.0, mz - 0.5);
		glVertex2f(w - slider_sz / 2.0, mz - 0.5);
	}
	else {
		glVertex2f(mz - 0.5, slider_sz / 2.0);
		glVertex2f(mz - 0.5, h - slider_sz / 2.0);
	}
	glColor3f(1.0, 1.0, 1.0);
	if (hori) {
		glVertex2f(slider_sz / 2.0, mz + 0.5);
		glVertex2f(w - slider_sz / 2.0, mz + 0.5);
	}
	else {
		glVertex2f(mz + 0.5, slider_sz / 2.0);
		glVertex2f(mz + 0.5, h - slider_sz / 2.0);
	}
	glEnd();
	if (adjust && slider_z0 >= 0) {
		glBegin(GL_QUADS);
		glColor3f(0.9, 0.9, 0.9);
		if (hori) {
			glVertex2i(slider_z0, (h - slider_sz) / 2);
			glVertex2i(slider_z0 + slider_sz, (h - slider_sz) / 2);
			glVertex2i(slider_z0 + slider_sz, (h + slider_sz) / 2);
			glVertex2i(slider_z0, (h + slider_sz) / 2);
		}
		else {
			glVertex2i((w - slider_sz) / 2, slider_z0);
			glVertex2i((w - slider_sz) / 2, slider_z0 + slider_sz);
			glVertex2i((w + slider_sz) / 2, slider_z0 + slider_sz);
			glVertex2i((w + slider_sz) / 2, slider_z0);
		}
		glEnd();
		if (hori) glut_draw_raised_box(slider_z0, (h - slider_sz) / 2, slider_sz, slider_sz);
		else glut_draw_raised_box((w - slider_sz) / 2, slider_z0, slider_sz, slider_sz);
	}
}
void Slider::on_button_event(int button, int state, int x, int y)
{
	assert(adjust);
	if (button == GLUT_LEFT_BUTTON) {
		if (sliding) {
			assert(state == GLUT_UP);
			set_grabbing(false);
			sliding = false;
		}
		else if (state == GLUT_DOWN && slider_z0 >= 0) {
			int z = hori ? x : y;
			if (z < slider_z0) adjust->page_down();
			else if (z >= slider_z0 + slider_sz) adjust->page_up();
			else {
				set_grabbing();
				sliding = true;
				sliding_start = z;
				sliding_vs = adjust->get_value();
			}
		}
	}
	else if (button == 3) {
		if (state == GLUT_DOWN) adjust->step_down();
	}
	else if (button == 4) {
		if (state == GLUT_DOWN) adjust->step_up();
	}
}
void Slider::on_motion_event(int x, int y)
{
	assert(sliding);
	int dz = (hori ? x : y) - sliding_start;
	int rg = (hori ? get_width() : get_height()) - slider_sz;
	if (! rg) return;
	double vr = adjust->get_upper() - adjust->get_lower() - adjust->get_page_size();
	assert(vr > 0);
	double dv = dz * vr / rg;
	adjust->set_value(sliding_vs + dv);
}
void Slider::get_preferred_width(int & min, int & nat) const
{
	if (hori) {
		min = 30;
		nat = 120;
	}
	else {
		min = slider_sz;
		nat = slider_sz;
	}
}
void Slider::get_preferred_height(int & min, int & nat) const
{
	if (hori) {
		min = slider_sz;
		nat = slider_sz;
	}
	else {
		min = 30;
		nat = 120;
	}
}
Slider::Slider(Adjustment * adj, Orientation ori) :
	Orientable(ori),
	adjust(adj),
	slider_sz(16),
	sliding(false)
{
	if (! adjust) adjust = new Adjustment(0, 0, 10);
	adjust->signal_value_changed().connect(sigc::mem_fun(this, & Slider::queue_draw));
}
Adjustment * Slider::get_adjustment()
{
	return adjust;
}
void Slider::set_value(double value)
{
	assert(adjust);
	adjust->set_value(value);
}
double Slider::get_value() const
{
	return adjust->get_value();
}
int Slider::get_slider_pos() const
{
	return slider_z0;
}
int Slider::get_slider_size() const
{
	return slider_sz;
}
sigc::signal<void> & Slider::signal_value_changed()
{
	assert(adjust);
	return adjust->signal_value_changed();
}
void SpinButton::on_configure_event()
{
	int height = get_height();
	int width = get_width();
	int h2 = height / 2;
	int h = min_of(h2, 7);
	if (width < 10) {
		tx.size_allocate(0, 0, width, height);
		up.size_allocate(width, h2 - h, 0, h);
		dn.size_allocate(width, h2, 0, h);
	}
	else {
		int x = width - 10;
		tx.size_allocate(0, 0, x, height);
		up.size_allocate(x, h2 - h, 10, h);
		dn.size_allocate(x, h2, 10, h);
	}
}
void SpinButton::on_draw()
{
	draw_children();
}
void SpinButton::on_value_changed()
{
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(digits) << get_value();
	tx.set_text(ss.str());
	queue_draw();
}
void SpinButton::on_button_event(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (grabber) {
			assert(state == GLUT_UP);
			grabber->button_event(button, state, x, y);
			assert(! grabber);
			if (held) {
				held_timeout.disconnect();
				held = false;
			}
		}
		else if (Widget * w = get_child(x, y)) {
			w->button_event(button, state, x, y);
		}
		if (grabber && ! held && grabber != & tx) {
			spin_interval = spin_wait / 2;
			climb_value = step_increment;
			held_timeout = SignalTimeout::connect(sigc::mem_fun(this, & SpinButton::button_held), spin_wait);
			held = true;
			spinning = false;
		}
	}
	else if (button == 3) {
		if (state == GLUT_DOWN) pushed_up();
	}
	else if (button == 4) {
		if (state == GLUT_DOWN) pushed_dn();
	}
}
bool SpinButton::on_special_event(int key)
{
	if (! focus) return false;
	if (focus == & tx) {
		switch (key) {
		case GLUT_KEY_UP:
			pushed_up();
			break;
		case GLUT_KEY_DOWN:
			pushed_dn();
			break;
		case GLUT_KEY_PAGE_UP:
			page_up();
			break;
		case GLUT_KEY_PAGE_DOWN:
			page_down();
			break;
		default:
			return focus->special_event(key);
		}
		tx.select_all();
		return true;
	}
	return focus->special_event(key);
}
void SpinButton::value_enter()
{
	std::istringstream s(tx.get_text());
	double v;
	s >> v;
	if (snap_to_ticks) v = step_increment * round_int(v / step_increment);
	if (v != get_value()) set_value(v);
	else {
		std::ostringstream ss;
		ss << std::fixed << std::setprecision(digits) << get_value();
		tx.set_text(ss.str());
		queue_draw();
	}
}
void SpinButton::value_stop()
{
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(digits) << get_value();
	tx.set_text(ss.str());
}
bool SpinButton::button_held()
{
	if (! grabber) {
		held = false;
		return false;
	}
	spinning = true;
	double v = get_value();
	if (grabber == & up) {
		v += climb_value;
		if (wrap && v > upper) v = lower;
	}
	else if (grabber == & dn) {
		v -= climb_value;
		if (wrap && v < lower) v = upper;
	}
	else throw Error("Unknown grabber...");
	if (snap_to_ticks) v = step_increment * round_int(v / step_increment);
	set_value(v);
	spin_interval -= (spin_interval / 3);
	spin_interval = max_of(spin_interval, spin_wait / 10);
	climb_value += climb_value * climb_rate / 10;
	if (climb_value > page_increment) climb_value = (page_increment + climb_value) / 2;
	held_timeout = SignalTimeout::connect(sigc::mem_fun(this, & SpinButton::button_held), spin_interval);
	return false;
}
void SpinButton::pushed_up()
{
	double v = get_value() + step_increment;
	if (wrap && v > upper) v = lower;
	if (snap_to_ticks) v = step_increment * round_int(v / step_increment);
	set_value(v);
}
void SpinButton::pushed_dn()
{
	double v = get_value() - step_increment;
	if (wrap && v < lower) v = upper;
	if (snap_to_ticks) v = step_increment * round_int(v / step_increment);
	set_value(v);
}
SpinButton::SpinButton(double c_rate, unsigned dgts) :
	Adjustment(0, 0, 10),
	up(new Shape(Shape::UP_TRIANGLE, 8, 4)),
	dn(new Shape(Shape::DOWN_TRIANGLE, 8, 4)),
	spin_wait(700),
	held(false),
	spinning(false),
	climb_rate(c_rate),
	digits(dgts),
	snap_to_ticks(false),
	wrap(false)
{
	add(& tx);
	add(& up);
	add(& dn);
	up.set_reserved(3, 3);
	dn.set_reserved(3, 3);
	up.set_box_width(1);
	dn.set_box_width(1);
	up.signal_pushed().connect(sigc::mem_fun(this, & SpinButton::pushed_up));
	dn.signal_pushed().connect(sigc::mem_fun(this, & SpinButton::pushed_dn));
	std::ostringstream s;
	s << std::fixed << std::setprecision(digits) << get_value();
	tx.set_text(s.str());
	tx.signal_activate().connect(sigc::mem_fun(this, & SpinButton::value_enter));
	tx.signal_stop().connect(sigc::mem_fun(this, & SpinButton::value_stop));
}
void SpinButton::get_preferred_width(int & min, int & nat) const
{
	tx.get_preferred_width(min, nat);
	min += 10;
	nat += 10;
}
void SpinButton::get_preferred_height(int & min, int & nat) const
{
	tx.get_preferred_height(min, nat);
	min = max_of(min, 10);
	nat = max_of(nat, 16);
}
int SpinButton::get_value_as_int() const
{
	int v = get_value();
	return v;
}
unsigned SpinButton::get_digits() const
{
	return digits;
}
void SpinButton::set_digits(unsigned d)
{
	digits = d;
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(digits) << get_value();
	tx.set_text(ss.str());
	tx.select_none();
	queue_draw();
}
void SpinButton::set_snap_to_ticks(bool snap)
{
	snap_to_ticks = snap;
	if (snap) {
		double v = step_increment * round_int(get_value() / step_increment);
		set_value(v);
	}
}
void SpinButton::set_increments(double step, double page)
{
	set_step_increment(step);
	set_page_increment(page);
	if (snap_to_ticks) {
		double v = step_increment * round_int(get_value() / step_increment);
		set_value(v);
	}
}
void SpinButton::set_range(double min, double max)
{
	set_upper(max);
	set_lower(min);
}
void SpinButton::set_wrap(bool w)
{
	wrap = w;
}
void SpinButton::set_width_chars(int n_chars)
{
	tx.set_width_chars(n_chars);
}
void TextDraw::draw_text(Align xl, Align yl, int wlimit)
{
	int x = 0;
	int y = 0;
	switch (xl) {
	case ALIGN_END:
		x = - text_width();
		break;
	case ALIGN_CENTER:
		x = - text_width() / 2;
		break;
	default:
		break;
	}
	switch (yl) {
	case ALIGN_CENTER:
		y -= glut_get_font_height(font) / 2;
	case ALIGN_START:
		y += glut_get_font_ascent(font);
		break;
	case ALIGN_END:
		y = - glut_get_font_descent(font);
		break;
	default:
		break;
	}
	glBitmap(0, 0, 0, 0, x, - y, 0);
	if (wlimit > 0) glut_draw_string(font, text, wlimit);
	else glut_draw_string(font, text);
}
int TextDraw::text_height() const
{
	return glut_get_font_height(font);
}
int TextDraw::text_width() const
{
	return text_width(text);
}
int TextDraw::text_width(std::string const & tx) const
{
	int w = 0;
	for (size_t i = 0; i < tx.size(); i ++) w += glutBitmapWidth(font, tx[i]);
	return w;
}
TextDraw::TextDraw() :
	font(GLUT_BITMAP_HELVETICA_12)
{}
void TextDraw::set_text(std::string const & t)
{
	text = t;
	on_set_text();
}
int TextDraw::get_ascent() const
{
	return glut_get_font_ascent(font);
}
namespace {
	int timer_unique_id = 0;
	struct TimerEvent
	{
		sigc::functor_base * fun;
		unsigned interval;
	};
	std::map<int, TimerEvent> timer_event_list;
	void timer_func(int v)
	{
		std::map<int, TimerEvent>::iterator i = timer_event_list.find(v);
		if (i == timer_event_list.end()) return;
		if (sigc::functor0<bool> * f = dynamic_cast<sigc::functor0<bool> *>(i->second.fun)) {
			if (f->call()) {
				glutTimerFunc(i->second.interval, & timer_func, v);
				return;
			}
		}
		else if (sigc::functor0<void> * f = dynamic_cast<sigc::functor0<void> *>(i->second.fun)) {
			f->call();
		}
		delete i->second.fun;
		timer_event_list.erase(i);
	}
	struct _Timeout :
		public sigc::signal_base
	{
		unsigned add_handle(sigc::slot_base const & )
		{
			throw Error("add_handle() called on timeout_signal!");
		}
		void remove_handle(unsigned hid)
		{
			int v = hid;
			std::map<int, TimerEvent>::iterator i = timer_event_list.find(v);
			if (i == timer_event_list.end()) return;
			delete i->second.fun;
			timer_event_list.erase(i);
		}
	} timeout;
}
sigc::connection SignalTimeout::connect(sigc::slot0<bool> const & slot, unsigned interval)
{
	TimerEvent & e = timer_event_list[timer_unique_id];
	e.fun = slot.fun->dup();
	e.interval = interval;
	glutTimerFunc(interval, & timer_func, timer_unique_id);
	return sigc::connection(& timeout, unsigned(timer_unique_id ++));
}
void SignalTimeout::connect_once(sigc::slot0<void> const & slot, unsigned interval)
{
	TimerEvent & e = timer_event_list[timer_unique_id];
	e.fun = slot.fun->dup();
	e.interval = interval;
	glutTimerFunc(interval, & timer_func, timer_unique_id);
	timer_unique_id ++;
}
Widget::Widget() :
	parent(0),
	window(0),
	managed(false),
	grabbing(false),
	shown(false),
	alloc_x(- 1),
	alloc_y(- 1),
	alloc_w(- 1),
	alloc_h(- 1)
{}
Widget::~Widget()
{
	if (parent) parent->remove(this);
}
void Widget::get_preferred_width(int & min, int & nat) const
{
	min = 0;
	nat = 0;
}
void Widget::get_preferred_height(int & min, int & nat) const
{
	min = 0;
	nat = 0;
}
void Widget::get_preferred_height_for_width(int , int & min, int & nat) const
{
	get_preferred_height(min, nat);
}
void Widget::get_preferred_width_for_height(int , int & min, int & nat) const
{
	get_preferred_width(min, nat);
}
bool Widget::focus_make()
{
	return false;
}
bool Widget::focus_next()
{
	focus_end();
	return false;
}
bool Widget::get_focus()
{
	if (! parent) return false;
	return parent->focus_on(this);
}
void Widget::set_parent(Container * p)
{
	parent = p;
	on_set_parent(p);
}
void Widget::set_managed(bool v)
{
	managed = v;
}
bool Widget::get_managed() const
{
	return managed;
}
void Widget::realize()
{
	configured = false;
	on_realize();
}
void Widget::unrealize()
{
	on_unrealize();
}
void Widget::size_allocate(int x, int y, int width, int height)
{
	alloc_x = x;
	alloc_y = y;
	if (alloc_w != width || alloc_h != height) {
		alloc_w = width;
		alloc_h = height;
	}
	configured = true;
	on_configure_event();
}
void Widget::move_allocate(int x, int y)
{
	assert(configured);
	alloc_x = x;
	alloc_y = y;
	on_configure_event();
}
bool Widget::contains(int x, int y) const
{
	return x >= alloc_x && y >= alloc_y && x - alloc_x < alloc_w && y - alloc_y < alloc_h;
}
void Widget::draw()
{
	assert(get_window()->get_id() == glutGetWindow());
	if (alloc_w > 0 && alloc_h > 0) {
		if (alloc_x > 0 || alloc_y > 0) glTranslatef(alloc_x, alloc_y, 0);
		on_draw();
	}
}
void Widget::show()
{
	shown = true;
	on_show();
}
void Widget::hide()
{
	on_hide();
	shown = false;
}
void Widget::make_focus()
{
	if (focus_make()) {
		if (! get_focus()) focus_end();
	}
}
void Widget::button_event(int button, int state, int x, int y)
{
	if (alloc_x > 0 || alloc_y > 0) {
		x -= alloc_x;
		y -= alloc_y;
	}
	on_button_event(button, state, x, y);
}
void Widget::motion_event(int x, int y)
{
	if (alloc_x > 0 || alloc_y > 0) {
		x -= alloc_x;
		y -= alloc_y;
	}
	on_motion_event(x, y);
}
bool Widget::keyboard_event(unsigned char key)
{
	return on_key_event(key);
}
bool Widget::special_event(int key)
{
	return on_special_event(key);
}
void Widget::passive_event(int x, int y)
{
	if (alloc_x > 0 || alloc_y > 0) {
		x -= alloc_x;
		y -= alloc_y;
	}
	on_passive_event(x, y);
}
void Widget::entry_event(bool entered)
{
	on_entry_event(entered);
}
void Widget::set_grabbing(bool v)
{
	if (grabbing == v) return;
	grabbing = v;
	if (parent) parent->set_grabber(this, v);
}
bool Widget::get_realized() const
{
	return window && window->is_realized();
}
Window * Widget::get_window() const
{
	return window;
}
void Widget::set_window(Window * w)
{
	window = w;
	on_set_window(w);
}
Allocation Widget::get_allocation() const
{
	Allocation a = {alloc_x, alloc_y, alloc_w, alloc_h};
	return a;
}
void Widget::set_size_request(int , int )
{
}
void Widget::get_absolute(int & x, int & y) const
{
	assert(window);
	if (parent && parent->window == window) {
		parent->get_absolute(x, y);
		assert(alloc_x >= 0 &&  alloc_y >= 0);
		x += alloc_x;
		y += alloc_y;
	}
	else {
		x = alloc_x;
		y = alloc_y;
	}
}
void Widget::queue_draw()
{
	if (window) window->refresh();
}
void Widget::queue_resize()
{
	if (window) window->reconfigure();
}
size_t Window::visible_count = 0;
void Window::on_display_func()
{
	if (to_reconf) {
		size_allocate(0, 0, win_width, win_height);
		to_reconf = false;
	}
	glDisable(GL_LIGHTING);
	glViewport(0, 0, win_width, win_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, win_width, win_height, 0, 1, - 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(0.9, 0.9, 0.9);
	glBegin(GL_QUADS);
	glVertex2d(0, 0);
	glVertex2d(0, win_height);
	glVertex2d(win_width, win_height);
	glVertex2d(win_width, 0);
	glEnd();
	draw_children();
	glutSwapBuffers();
}
void Window::on_reshape_func(int width, int height)
{
	win_width = width;
	win_height = height;
	size_allocate(0, 0, width, height);
	to_reconf = false;
}
void Window::on_mouse_func(int button, int state, int x, int y)
{
	button_event(button, state, x, y);
}
void Window::on_motion_func(int x, int y)
{
	motion_event(x, y);
}
void Window::on_passive_func(int x, int y)
{
	passive_event(x, y);
}
void Window::on_keyboard_func(unsigned char key, int , int )
{
	if (key == 0x9) {
		((Widget *) this)->focus_next();
		return;
	}
	if (! keyboard_event(key)) std::cerr << "unhandled key: 0x" << std::hex << int(key) << '\n';
}
void Window::on_special_func(int key, int , int )
{
	if (! special_event(key)) std::cerr << "unhandled special: 0x" << std::hex << key << '\n';
}
void Window::on_create()
{
	assert(get_win_id());
	realize_children();
}
void Window::on_destroy()
{
	unrealize_children();
	if (visible) {
		visible = false;
		visible_count --;
	}
}
void Window::on_add(Widget * child)
{
	Bin::on_add(child);
	child->set_window(this);
}
bool Window::get_focus()
{
	return true;
}
Window::Window() :
	visible(false),
	to_reconf(false),
	default_w(0),
	default_h(0)
{
	set_events(
		DISPLAY_MASK |
		RESHAPE_MASK |
		MOUSE_MASK |
		MOTION_MASK |
		PASSIVE_MASK |
		KEYBOARD_MASK |
		SPECIAL_MASK
	);
}
Window::~Window()
{
	if (visible) visible_count --;
}
void Window::set_default_size(int w, int h)
{
	default_w = w;
	default_h = h;
}
void Window::set_title(std::string const & txt)
{
	title = txt;
	if (get_realized()) {
		glutSetWindow(get_win_id());
		glutSetWindowTitle(title.c_str());
	}
}
void Window::resize(int w, int h)
{
	if (! get_win_id()) return;
	if (w < 1 || h < 1) {
		int mw;
		int nw;
		get_preferred_width(mw, nw);
		if (default_w) nw = max_of(mw, default_w);
		else if (! nw) nw = 200;
		int mh;
		int nh;
		get_preferred_height_for_width(nw, mh, nh);
		if (default_h) nh = max_of(mh, default_h);
		else if (! nh) nh = 200;
		w = nw;
		h = nh;
	}
	glutWindow::resize(w, h);
}
void Window::refresh()
{
	redisplay();
}
void Window::reconfigure()
{
	to_reconf = true;
	redisplay();
}
void Window::make_current()
{
	set_current();
}
void Window::show()
{
	if (int wid = get_win_id()) glutSetWindow(wid);
	else {
		int mw;
		int nw;
		get_preferred_width(mw, nw);
		if (default_w) nw = max_of(mw, default_w);
		else if (! nw) nw = 200;
		int mh;
		int nh;
		get_preferred_height_for_width(nw, mh, nh);
		if (default_h) nh = max_of(mh, default_h);
		else if (! nh) nh = 200;
		create(title, nw, nh);
	}
	glutShowWindow();
	if (! visible) {
		visible = true;
		visible_count ++;
	}
}
void Window::hide()
{
	if (int wid = get_win_id()) {
		glutSetWindow(wid);
		glutHideWindow();
		if (visible) {
			visible = false;
			visible_count --;
			if (! visible_count) {
				destroy();
				exit(0);
			}
		}
	}
}
bool Window::is_realized() const
{
	return get_win_id();
}
int Window::get_id() const
{
	return get_win_id();
}
