/* GENERATED, DO NOT EDIT! See http://gltk.ccdw.org/ for source. */
/* gltk_all.hh: adjustment align bin box button checkbox checkbutton comboboxtext container entry filebrowser focusable frame glarea glutwindow idle label labelbin listview main menu orientable orientation packoptions popup positiontype pushbox scale scrollbar separator shape slider spinbutton textdraw timeout widget window
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
#ifndef GLTK_GLTK_HH
#define GLTK_GLTK_HH
#include <list>
#include <map>
#include <string>
#include <vector>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#ifndef FREEGLUT
#include <cstdlib>
#endif
#include <GL/glut.h>
#endif
#include "sigc.hh"
namespace gltk {
	class Adjustment
	{
		sigc::signal<void> changed;
		sigc::signal<void> value_changed;
	protected:
		double value;
		double lower;
		double upper;
		double step_increment;
		double page_increment;
		double page_size;
		virtual void on_value_changed() {};
	public:
		Adjustment(
			double value,
			double lower,
			double upper,
			double step_increment = 1,
			double page_increment = 10,
			double page_size = 0
		);
		static Adjustment * create(
			double value,
			double lower,
			double upper,
			double step_increment = 1,
			double page_increment = 10,
			double page_size = 0
		);
		void configure(
			double value,
			double lower,
			double upper,
			double step_increment=1,
			double page_increment=10,
			double page_size=0
		);
		double get_value() const;
		void set_value(double value);
		void step_up();
		void step_down();
		void page_up();
		void page_down();
		void set_step_increment(double step_increment);
		void set_page_increment(double page_increment);
		double get_upper() const {return upper;}
		void set_upper (double upper);
		double get_lower() const {return lower;}
		void set_lower (double lower);
		double get_page_size() const {return page_size;}
		void set_page_size(double ps);
		sigc::signal<void> & signal_changed();
		sigc::signal<void> & signal_value_changed();
	};
	enum Align {
		ALIGN_FILL,
		ALIGN_START,
		ALIGN_END,
		ALIGN_CENTER
	};
	class Container;
	class Window;
	struct Allocation
	{
		int x;
		int y;
		int width;
		int height;
	};
	template<typename T>
	T * manage(T * obj)
	{
		obj->set_managed();
		return obj;
	}
	class Widget
	{
		Container * parent;
		Window * window;
		bool managed;
		bool grabbing;
		bool shown;
		bool configured;
		int alloc_x;
		int alloc_y;
		int alloc_w;
		int alloc_h;
	protected:
		virtual void on_set_parent(Container * p) {(void)p;}
		virtual void on_set_window(Window * window) {(void)window;}
		virtual void on_realize() {}
		virtual void on_unrealize() {}
		virtual void on_draw() {}
		virtual void on_size_allocate(int width, int height) {(void)width;(void)height;}
		virtual void on_configure_event() {}
		virtual void on_button_event(int button, int state, int x, int y) {(void)button;(void)state;(void)x;(void)y;}
		virtual void on_motion_event(int x, int y) {(void)x;(void)y;}
		virtual void on_passive_event(int x, int y) {(void)x;(void)y;}
		virtual bool on_key_event(unsigned char key) {(void)key;return false;}
		virtual bool on_special_event(int key) {(void)key;return false;}
		virtual void on_entry_event(bool entered) {(void)entered;}
		virtual void on_show() {}
		virtual void on_hide() {}
		virtual bool get_focus();
	public:
		Widget();
		virtual ~Widget();
		virtual void get_preferred_height(int & min, int & nat) const;
		virtual void get_preferred_height_for_width(int width, int & min, int & nat) const;
		virtual void get_preferred_width(int & min, int & nat) const;
		virtual void get_preferred_width_for_height(int height, int & min, int & nat) const;
		virtual bool focus_make();
		virtual bool focus_next();
		virtual void focus_end() {}
		void set_parent(Container * p);
		void set_managed(bool v = true);
		bool get_managed() const;
		void realize();
		void unrealize();
		void size_allocate(int x, int y, int width, int height);
		void move_allocate(int x, int y);
		bool contains(int x, int y) const;
		void draw();
		void show();
		void hide();
		void make_focus();
		void button_event(int button, int state, int x, int y);
		void motion_event(int x, int y);
		bool keyboard_event(unsigned char key);
		bool special_event(int key);
		void passive_event(int x, int y);
		void entry_event(bool enterred);
		void set_grabbing(bool v = true);
		bool get_realized() const;
		Window * get_window() const;
		void set_window(Window * window);
		int get_shown() const {return shown;}
		bool get_configured() const {return configured;}
		int get_width() const {return alloc_w;}
		int get_height() const {return alloc_h;}
		Allocation get_allocation() const;
		void set_size_request(int width = - 1, int height = - 1);
		void get_absolute(int & x, int & y) const;
		void queue_draw();
		void queue_resize();
		void set_can_default() {}
		void grab_default() {make_focus();}
	};
	class Container :
		public virtual Widget
	{
		void on_set_window(Window * window);
		void on_realize();
		void on_unrealize();
		void on_draw();
		void on_button_event(int button, int state, int x, int y);
		void on_motion_event(int x, int y);
		void on_passive_event(int x, int y);
		bool on_key_event(unsigned char key);
		bool on_special_event(int key);
		void on_entry_event(bool entered);
		bool focus_make();
		bool focus_next();
		void focus_end();
	protected:
		Widget * get_child(int x, int y);
		std::vector<Widget *> child_list;
		Widget * grabber;
		Widget * focus;
		Widget * tracker;
		virtual void on_add(Widget *) {}
		virtual void on_remove(Widget *) {}
		void draw_children();
		void set_children_window(Window * window);
		void realize_children();
		void unrealize_children();
	public:
		Container();
		~Container();
		void add(Widget * child);
		void add(Widget & child) {add(& child);}
		void remove(Widget * child);
		void show_all();
		void show_all_children(bool recursive = true);
		void set_grabber(Widget * child, bool grab = true);
		virtual bool focus_on(Widget * child);
	};
	class Bin :
		public virtual Container
	{
		int child_x;
		int child_y;
	protected:
		void on_configure_event() ;
		void get_preferred_width(int & min, int & nat) const;
		void get_preferred_height(int & min, int & nat) const;
		void get_preferred_width_for_height(int height, int & min, int & nat) const;
		void get_preferred_height_for_width(int width, int & min, int & nat) const;
		void on_add(Widget *);
		void on_remove(Widget *);
		Widget * child;
		int reducible_x;
		int reducible_y;
		int reserved_x;
		int reserved_y;
	public:
		Bin();
		void set_reducible(int x, int y);
		void set_reserved(int x, int y);
		void place_bin(int x, int y);
		void set_border_width(unsigned w);
	};
	enum Orientation {
		ORIENTATION_HORIZONTAL,
		ORIENTATION_VERTICAL
	};
	class Orientable
	{
	protected:
		bool hori;
	public:
		Orientable(Orientation orientation = ORIENTATION_HORIZONTAL);
		Orientation get_orientation() const;
		void set_orientation(Orientation);
		bool get_hori() const {return hori;}
	};
	enum PackOptions {
		PACK_SHRINK,
		PACK_EXPAND_PADDING,
		PACK_EXPAND_WIDGET
	};
	class Box :
		public Orientable,
		public Container
	{
	private:
		void on_configure_event();
		void get_preferred_width(int & min, int & nat) const;
		void get_preferred_height(int & min, int & nat) const;
		void get_preferred_width_for_height(int height, int & min, int & nat) const;
		void get_preferred_height_for_width(int width, int & min, int & nat) const;
	protected:
		unsigned border_width;
		int spacing;
		bool homogeneous;
	public:
		Box(
			Orientation orientabtion = ORIENTATION_HORIZONTAL,
			int spacing = 0
		);
		void pack_start(Widget * child);
		void pack_start(Widget & child, PackOptions options = PACK_EXPAND_WIDGET, unsigned padding = 0) {
			(void)options;
			(void)padding;
			pack_start(& child);
		}
		void set_border_width(unsigned border_width);
	};
	class VBox :
		public Box
	{
	public:
		VBox(bool  = false, int spacing = 0) :
			Box(ORIENTATION_VERTICAL, spacing)
		{}
	};
	class HBox :
		public Box
	{
	public:
		HBox(bool  = false, int spacing = 0) :
			Box(ORIENTATION_HORIZONTAL, spacing)
		{}
	};
	class Focusable :
		virtual public Widget
	{
	protected:
		bool focused;
		bool focus_make();
		bool focus_next();
		void focus_end();
		virtual void draw_focus_box();
	public:
		Focusable();
	};
	class PushBox :
		public virtual Focusable
	{
		sigc::signal<void> pushed;
		sigc::signal<void> clicked;
	protected:
		void on_draw();
		void get_preferred_width(int & min, int & nat) const;
		void get_preferred_height(int & min, int & nat) const;
		void on_button_event(int button, int state, int x, int y);
		bool on_key_event(unsigned char key);
		int box_width;
		bool depressed;
		void draw_box();
	public:
		PushBox();
		sigc::signal<void> & signal_clicked() {return clicked;}
		sigc::signal<void> & signal_pushed() {return pushed;}
		void set_box_width (int bw) {box_width = bw;}
	};
	class LabelBin :
		virtual public Bin
	{
		bool own_child;
	public:
		LabelBin(Widget * child);
		LabelBin(std::string const & name);
		~LabelBin();
		void set_text(std::string name);
	};
	class Button :
		virtual public PushBox,
		virtual public LabelBin
	{
		void on_draw();
		void get_preferred_width(int & min, int & nat) const;
		void get_preferred_height(int & min, int & nat) const;
		void get_preferred_width_for_height(int height, int & min, int & nat) const;
		void get_preferred_height_for_width(int width, int & min, int & nat) const;
		bool focus_make();
		bool focus_next();
		void focus_end();
		void on_button_event(int button, int state, int x, int y);
		bool on_key_event(unsigned char key);
	public:
		Button(std::string const & name);
		Button(Widget * widget);
	};
	class CheckBox :
		virtual public Focusable
	{
		sigc::signal<void> toggled;
		bool active;
		void on_draw();
		void get_preferred_width(int & min, int & nat) const;
		void get_preferred_height(int & min, int & nat) const;
	protected:
		void on_button_event(int button, int state, int x, int y);
		bool on_key_event(unsigned char key);
		void draw_checkbox(int x, int y, int w, int h);
		static const int checkbox_size;
		static const int checkbox_area;
	public:
		CheckBox();
		sigc::signal<void> & signal_toggled();
		bool get_active() const;
		void set_active(bool val = true);
	};
	class CheckButton :
		virtual public CheckBox,
		virtual public LabelBin
	{
		int cb_width;
		void on_configure_event();
		void on_draw();
		void get_preferred_width(int & min, int & nat) const;
		void get_preferred_height(int & min, int & nat) const;
		void get_preferred_width_for_height(int height, int & min, int & nat) const;
		void get_preferred_height_for_width(int width, int & min, int & nat) const;
		void on_button_event(int button, int state, int x, int y);
		bool on_key_event(unsigned char key);
		bool focus_make();
		bool focus_next();
		void focus_end();
	public:
		CheckButton(std::string const & label, bool mnemonic = false);
	};
	class Popup :
		public virtual Widget
	{
		int menu_id;
		static void menu_callback(int item);
		static void menu_status(int status, int x, int y);
		void on_entry_event(bool entered);
		void on_unrealized();
	protected:
		std::vector<std::string> menu;
		virtual void on_menu_select(int item) {(void)item;}
		void release_menu();
	public:
		Popup();
	};
	class TextDraw
	{
		std::string text;
		void * font;
	protected:
		void draw_text(
			Align xalign = ALIGN_START,
			Align yalign = ALIGN_START,
			int width_limit = 0
		);
		int text_height() const;
		int text_width() const;
		int text_width(
			std::string const & string
		) const;
		virtual void on_set_text() {}
	public:
		TextDraw();
		void set_text(std::string const & text);
		std::string const & get_text() const {return text;}
		void * get_font() const {return font;}
		int get_ascent() const;
	};
	class ComboBoxText :
		public virtual TextDraw,
		public virtual Popup
	{
		sigc::signal<void> changed;
		void on_draw();
		void get_preferred_height(int & min, int & nat) const;
		void get_preferred_width(int & min, int & nat) const;
		void on_menu_select(int item);
		int active_item;
	public:
		ComboBoxText();
		void append(std::string const & text);
		std::string get_active_text() const;
		void remove_all();
		void set_active_text(std::string const & text);
		sigc::signal<void> & signal_changed() {return changed;}
	};
	class Entry :
		public virtual Widget
	{
		std::string text;
		size_t max_length;
		bool focused;
		bool editable;
		bool visibility;
		size_t cursor;
		size_t vstart;
		size_t mark;
		bool marked;
		void * font;
		int width_chars;
		void on_draw();
		void on_button_event(int button, int state, int x, int y);
		void on_motion_event(int x, int y);
		bool on_key_event(unsigned char key);
		bool on_special_event(int key);
		bool focus_make();
		void focus_end();
		void adjust_vstart();
		int find_x(size_t p);
		size_t find_position(int x);
		void delete_marked();
		int bw;
		int as;
		bool end_arrow;
		sigc::signal<void> activate;
		sigc::signal<void> stop;
	public:
		Entry();
		void get_preferred_height(int & min, int & nat) const;
		void get_preferred_width(int & min, int & nat) const;
		void set_text(std::string const &);
		std::string const & get_text() const;
		size_t get_text_length() const;
		void select_all();
		void select_none();
		void set_max_length(size_t length);
		void set_width_chars(int n_chars);
		void set_editable(bool v = true);
		void set_visibility(bool v = true);
		sigc::signal<void> & signal_activate();
		sigc::signal<void> & signal_stop();
	};
	struct ViewableList
	{
		struct ItemBase
		{
			virtual std::string str() const;
		};
		struct RenderBase
		{
			virtual int get_height() = 0;
			virtual void render(ItemBase * item, int x, int y, int width, int height)
			{
				(void)item;(void)x;(void)y;(void)width;(void)height;
			}
		};
		virtual size_t size() const = 0;
		virtual ItemBase * get_item(size_t index) = 0;
		sigc::signal<void> changed;
	};
	struct RenderItemStr :
		public ViewableList::RenderBase
	{
		void * font;
		int get_height();
		void render(ViewableList::ItemBase * item, int x, int y, int width, int height);
		RenderItemStr();
	};
	class ListView :
		public virtual Widget,
		public virtual Adjustment,
		public virtual Focusable
	{
		sigc::signal<void, int> selected;
		ViewableList * list;
		ViewableList::RenderBase * render;
		int select;
		bool last_click;
		void clear_click();
		void on_draw();
		void on_configure_event();
		void on_button_event(int button, int state, int x, int y);
		bool on_key_event(unsigned char key);
		bool on_special_event(int key);
		void on_value_changed();
		void get_preferred_height(int & min, int & nat) const;
		void get_preferred_width(int & min, int & nat) const;
		int line_start;
		int line_end;
		int line_height;
		int line_visible;
		void list_change();
	public:
		ListView();
		void set_list(ViewableList * list);
		void add_render(ViewableList::RenderBase * render);
		sigc::signal<void, int> & signal_selected();
	};
	class FileBrowser :
		public Box
	{
		ViewableList * list;
	public:
		FileBrowser();
		void set_path(std::string const & path);
		std::string get_path() const;
		sigc::signal<void, std::string> & signal_selected();
	};
	class Frame :
		public virtual TextDraw,
		public virtual Bin
	{
		void on_configure_event();
		void on_draw();
	public:
		Frame(std::string const & title);
	};
	class glutWindow
	{
		friend class _glutWindow;
		int win_id;
		unsigned event_mask;
	protected:
		virtual void on_display_func() {}
		virtual void on_reshape_func(int width, int height) {(void)width;(void)height;}
		virtual void on_mouse_func(int button, int state, int x, int y) {(void)button;(void)state;(void)x;(void)y;}
		virtual void on_motion_func(int x, int y) {(void)x;(void)y;}
		virtual void on_passive_func(int x, int y) {(void)x;(void)y;}
		virtual void on_keyboard_func(unsigned char key, int x, int y) {(void)key;(void)x;(void)y;}
		virtual void on_special_func(int key, int x, int y) {(void)key;(void)x;(void)y;}
		virtual void on_entry_func(int state) {(void)state;}
		enum EventMask {
			EVENT_MASK_NONE = 0,
			DISPLAY_MASK = 1 << 0,
			RESHAPE_MASK = 1 << 1,
			MOUSE_MASK = 1 << 2,
			MOTION_MASK = 1 << 3,
			PASSIVE_MASK = 1 << 4,
			KEYBOARD_MASK = 1 << 5,
			SPECIAL_MASK = 1 << 6,
			ENTRY_MASK = 1 << 7,
			EVENT_MASK_ALL = (1 << 8) - 1
		};
		virtual void on_create() {}
		virtual void on_destroy() {}
		glutWindow();
		virtual ~glutWindow();
		void set_events(unsigned mask);
		void add_events(unsigned mask);
		void create(int parent_win_id, int x, int y, int width, int height);
		void create(std::string const & name, int width, int height);
		void destroy();
		void destroy_all();
		int get_win_id() const {return win_id;}
		void set_current();
		void move(int x, int y);
		void resize(int width, int height);
		void redisplay();
	};
	class glArea :
		public glutWindow,
		public Widget
	{
		void on_display_func();
		void on_reshape_func(int width, int height);
		void on_draw();
		void on_unrealize() ;
		void on_configure_event() ;
		void on_show() ;
		void on_hide() ;
		int area_width;
		int area_height;
		void get_preferred_width(int & min, int & nat) const;
		void get_preferred_height(int & min, int & nat) const;
		void make_realized();
	protected:
		virtual void gl_setup() {}
		virtual void gl_new_size(int w, int h) {(void)w;(void)h;}
		virtual void gl_draw() {}
		virtual void gl_button_event(int button, int state, int x, int y) {(void)button;(void)state;(void)x;(void)y;}
		virtual void gl_motion_event(int x, int y) {(void)x;(void)y;}
	public:
		glArea();
		~glArea();
		void set_size(int width, int height);
	};
	class SignalIdle
	{
	public:
		static sigc::connection connect(sigc::slot0<bool> const & slot);
		static void connect_once(sigc::slot0<void> const & slot);
	};
	class Label :
		virtual public TextDraw,
		virtual public Widget
	{
		void on_draw();
		void get_preferred_height(int & min, int & nat) const;
		void get_preferred_width(int & min, int & nat) const;
		void on_set_text();
	public:
		Label();
		Label(std::string const & label, bool mnemonic = false);
	};
	class Window :
		glutWindow,
		public virtual Bin
	{
		static size_t visible_count;
		bool visible;
		bool to_reconf;
		void on_display_func();
		void on_reshape_func(int width, int height);
		void on_mouse_func(int button, int state, int x, int y);
		void on_motion_func(int x, int y);
		void on_passive_func(int x, int y);
		void on_keyboard_func(unsigned char key, int x, int y);
		void on_special_func(int key, int x, int y);
		void on_create();
		void on_destroy();
		void on_add(Widget * widget);
		bool get_focus();
		int default_w;
		int default_h;
		std::string title;
		int win_width;
		int win_height;
	public:
		Window();
		~Window();
		void set_default_size(int w, int h);
		void set_title(std::string const & title);
		void resize(int width = 0, int height = 0);
		void refresh();
		void reconfigure();
		void make_current();
		void show();
		void hide();
		bool is_realized() const;
		int get_id() const;
	};
	class Main
	{
	public:
		Main(int & argc, char * * (& argv));
		static void run(Window & window);
	};
	class MenuItem
	{
		std::string name;
		sigc::signal<void> activate;
	public:
		MenuItem(std::string const & name);
		sigc::signal<void> & signal_activate();
	};
	class Menu :
		public virtual Label,
		public virtual Popup
	{
	public:
	};
	enum PositionType {
		POS_LEFT,
		POS_RIGHT,
		POS_TOP,
		POS_BOTTOM
	};
	class Slider :
		public virtual Orientable,
		public virtual Widget
	{
		Adjustment * adjust;
		void on_draw();
		void on_button_event(int button, int state, int x, int y);
		void on_motion_event(int x, int y);
		void get_preferred_width(int & min, int & nat) const;
		void get_preferred_height(int & min, int & nat) const;
		int slider_z0;
		int slider_sz;
		bool sliding;
		int sliding_start;
		double sliding_vs;
	public:
		Slider(
			Adjustment * adjust = 0,
			Orientation orient = ORIENTATION_HORIZONTAL
		);
		Adjustment * get_adjustment();
		void set_value(double value);
		double get_value() const;
		int get_slider_pos() const;
		int get_slider_size() const;
		sigc::signal<void> & signal_value_changed();
	};
	class Scale :
		TextDraw,
		public virtual Bin
	{
		Slider sl;
		PositionType pos;
		int digits;
		int wtxt;
		bool draw_value;
		void update_label();
		void update_wtxt();
		void on_configure_event();
		void on_draw();
		void get_preferred_width(int & min, int & nat) const;
		void get_preferred_width_for_height(int height, int & min, int & nat) const;
	public:
		Scale(
			Adjustment * adjust = 0,
			Orientation orient = ORIENTATION_HORIZONTAL
		);
		void set_digits(int digits);
		void set_draw_value(bool draw_value = true);
		void set_value_pos(PositionType pos);
	};
	class VScale :
		public Scale
	{
	public:
		VScale(Adjustment * adjust) :
			Scale(adjust, ORIENTATION_VERTICAL)
		{}
	};
	class HScale :
		public Scale
	{
	public:
		HScale(Adjustment * adjust) :
			Scale(adjust, ORIENTATION_HORIZONTAL)
		{}
	};
	class Scrollbar :
		virtual public Container
	{
		Adjustment * adjust;
		bool hori;
		Button up;
		Button dn;
		void on_configure_event();
		void on_draw();
		void on_button_event(int button, int state, int x, int y);
		void on_motion_event(int x, int y);
		unsigned spin_wait;
		unsigned spin_interval;
		bool held;
		bool spinning;
		void button_held();
		void pushed_up();
		void pushed_dn();
		int slider_z0;
		int slider_sz;
		bool sliding;
		int sliding_start;
		double sliding_vs;
		void get_preferred_width(int & min, int & nat) const;
		void get_preferred_height(int & min, int & nat) const;
	public:
		Scrollbar(Adjustment * adjust, Orientation orientation = ORIENTATION_HORIZONTAL);
	};
	class VScrollbar :
		public Scrollbar
	{
	public:
		VScrollbar(Adjustment * adjust) :
			Scrollbar(adjust, ORIENTATION_VERTICAL)
		{}
	};
	class HScrollbar :
		public Scrollbar
	{
	public:
		HScrollbar(Adjustment * adjust) :
			Scrollbar(adjust, ORIENTATION_HORIZONTAL)
		{}
	};
	class Separator :
		public Widget
	{
		bool hori;
		void on_draw();
		void get_preferred_width(int & min, int & nat) const;
		void get_preferred_height(int & min, int & nat) const;
	public:
		Separator(Orientation orientation = ORIENTATION_HORIZONTAL);
	};
	class VSeparator :
		public Separator
	{
	public:
		VSeparator() : Separator(ORIENTATION_VERTICAL) {}
	};
	class HSeparator :
		public Separator
	{
	public:
		HSeparator() : Separator(ORIENTATION_HORIZONTAL) {}
	};
	class Shape :
		public virtual Widget
	{
	public:
		enum Predefined {
			UP_TRIANGLE,
			DOWN_TRIANGLE,
			LEFT_TRIANGLE,
			RIGHT_TRIANGLE
		};
	private:
		void on_draw();
		void get_preferred_width(int & minimum, int & natural) const;
		void get_preferred_height(int & minimum, int & natural) const;
		Predefined shape;
		int width;
		int height;
	public:
		Shape(
			Predefined shape,
			int width,
			int height
		);
	};
	class SpinButton :
		virtual public Container,
		virtual public Adjustment
	{
		Entry tx;
		Button up;
		Button dn;
		void on_configure_event();
		void on_draw();
		void on_value_changed();
		void on_button_event(int button, int state, int x, int y);
		bool on_special_event(int key);
		void value_enter();
		void value_stop();
		unsigned spin_wait;
		unsigned spin_interval;
		double climb_value;
		bool held;
		sigc::connection held_timeout;
		bool spinning;
		bool button_held();
		void pushed_up();
		void pushed_dn();
	protected:
		double climb_rate;
		unsigned digits;
		bool snap_to_ticks;
		bool wrap;
	public:
		SpinButton(
			double climb_rate = 0.0,
			unsigned digits = 0
		);
		void get_preferred_width(int & min, int & nat) const;
		void get_preferred_height(int & min, int & nat) const;
		int get_value_as_int() const;
		unsigned get_digits() const;
		void set_digits(unsigned digits);
		void set_snap_to_ticks(bool snap_to_ticks);
		void set_increments(double step, double page);
		void set_range(double min, double max);
		void set_wrap(bool wrap = true);
		void set_width_chars(int n_chars);
	};
	class SignalTimeout
	{
	public:
		static sigc::connection connect(
			sigc::slot0<bool> const & slot,
			unsigned interval
		);
		static void connect_once(sigc::slot0<void> const & slot, unsigned interval);
	};
}
#endif
