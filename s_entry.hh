/*
 * gltk::Entry for streamable values
 */
#ifndef S_ENTRY_HH
#define S_ENTRY_HH
#include <gltk.hh>
#include <sstream>
#include <iostream>
template<typename SType>
class SEntry :
	public gltk::Entry
{
	bool updating;
	SType saved_value;
	std::string saved_text;
	void make_change()
	{
		if (updating) return;
		std::istringstream inp(get_text());
		SType value;
		inp >> value;
		if (inp.fail() || ! inp.eof()) {
			updating = true;
			set_text(saved_text);
			updating = false;
			// modify_text(Gtk::STATE_NORMAL, Gdk::Color(0));
			return;
		}
		set_value(value);
	}
public:
	SEntry() :
		updating(false)
	{
		set_value(SType());
		signal_activate().connect(sigc::mem_fun(* this, & SEntry::make_change));
		signal_stop().connect(sigc::mem_fun(* this, & SEntry::make_change));
	}

	void set_value(SType value)
	{
		std::ostringstream out;
		out << value;

		updating = true;
		set_text(out.str());
		updating = false;
		saved_value = value;
		saved_text = get_text();
	}

	SType get_value()
	{
		return saved_value;
	}
};
#endif
