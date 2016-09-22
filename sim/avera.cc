/* sim/avera.cc
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
#include "avera.hh"
#include <iostream>
#include <cstdlib>

#ifdef HAVE_HDF5
void Avera::AccDouble::h5_save(hid_t group) const
{
	std::string n = "sum_" + id;
	hid_t space = H5Screate(H5S_SCALAR);
	hid_t data = H5Dcreate(group, n.c_str(), H5T_NATIVE_DOUBLE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	H5Dwrite(data, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, & sum);
	H5Dclose(data);
	H5Sclose(space);
	if (rec.size()) {
		n = "rec_" + id;
		hsize_t len = rec.size();
		space = H5Screate_simple(1, & len, 0);
		double * d = new double[len];
		for (size_t i = 0; i < len; i ++) d[i] = rec[i];
		data = H5Dcreate(group, n.c_str(), H5T_NATIVE_DOUBLE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5Dwrite(data, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, d);
		H5Dclose(data);
		H5Sclose(space);
		delete [] d;
	}
}

void Avera::AccDouble::h5_load(hid_t group)
{
	std::string n = "sum_" + id;
	hid_t data = H5Dopen(group, n.c_str(), H5P_DEFAULT);
	hid_t space = H5Dget_space(data); // `space' should be scalar
	H5Dread(data, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, & sum);
	H5Dclose(data);
	H5Sclose(space);

	rec.clear();
	n = "rec_" + id;
	if (H5Lexists(group, n.c_str(), H5P_DEFAULT)) {
		data = H5Dopen(group, n.c_str(), H5P_DEFAULT);
		space = H5Dget_space(data);
		hsize_t len;
		H5Sget_simple_extent_dims(space, & len, 0);
		double * d = new double[len];
		H5Dread(data, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, d);
		for (size_t i = 0; i < len; i ++) rec.push_back(d[i]);
		H5Dclose(data);
		H5Sclose(space);
		delete [] d;
	}
}

void Avera::AccFVec::h5_save(hid_t group) const
{
	std::string n = "sum_" + id;
	hid_t space = H5Screate(H5S_SCALAR);
	hid_t type = sum.h5_type();
	hid_t data = H5Dcreate(group, n.c_str(), type, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	FVec::H5Data * buf = sum.h5_buff();
	sum.h5_make(buf);
	H5Dwrite(data, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
	H5Dclose(data);
	H5Sclose(space);
	sum.h5_free(buf);
	if (rec.size()) {
		n = "rec_" + id;
		hsize_t len = rec.size();
		space = H5Screate_simple(1, & len, 0);
		buf = sum.h5_buff(len);
		for (size_t i = 0; i < len; i ++) rec[i].h5_make(buf, i);
		data = H5Dcreate(group, n.c_str(), type, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5Dwrite(data, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
		H5Dclose(data);
		H5Sclose(space);
		sum.h5_free(buf);
	}
	H5Tclose(type);
}

void Avera::AccFVec::h5_load(hid_t group)
{
	std::string n = "sum_" + id;
	hid_t data = H5Dopen(group, n.c_str(), H5P_DEFAULT);
	hid_t space = H5Dget_space(data); // `space' should be scalar
	hid_t type = sum.h5_type();
	FVec::H5Data * buf = sum.h5_buff();
	H5Dread(data, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
	sum.h5_read(buf);
	H5Dclose(data);
	H5Sclose(space);
	sum.h5_free(buf);

	rec.clear();
	n = "rec_" + id;
	if (H5Lexists(group, n.c_str(), H5P_DEFAULT)) {
		data = H5Dopen(group, n.c_str(), H5P_DEFAULT);
		space = H5Dget_space(data);
		hsize_t len;
		H5Sget_simple_extent_dims(space, & len, 0);
		buf = sum.h5_buff(len);
		H5Dread(data, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
		for (size_t i = 0; i < len; i ++) {
			FVec v(sum.size());
			v.h5_read(buf, i);
			rec.push_back(v);
		}
		H5Dclose(data);
		H5Sclose(space);
		sum.h5_free(buf);
	}
	H5Tclose(type);
}

void Avera::AccVVec::h5_save(hid_t group) const
{
	std::string n = "sum_" + id;
	hid_t space = H5Screate(H5S_SCALAR);
	hid_t type = VVec::h5_type();
	hid_t data = H5Dcreate(group, n.c_str(), type, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	VVec::H5Data * buf = new VVec::H5Data;
	sum.h5_make(buf);
	H5Dwrite(data, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
	H5Dclose(data);
	H5Sclose(space);
	delete buf;
	if (rec.size()) {
		n = "rec_" + id;
		hsize_t len = rec.size();
		space = H5Screate_simple(1, & len, 0);
		buf = new VVec::H5Data[len];
		for (size_t i = 0; i < len; i ++) rec[i].h5_make(buf + i);
		data = H5Dcreate(group, n.c_str(), type, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5Dwrite(data, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
		H5Dclose(data);
		H5Sclose(space);
		delete [] buf;
	}
	H5Tclose(type);
}

void Avera::AccVVec::h5_load(hid_t group)
{
	std::string n = "sum_" + id;
	hid_t data = H5Dopen(group, n.c_str(), H5P_DEFAULT);
	hid_t space = H5Dget_space(data); // `space' should be scalar
	hid_t type = VVec::h5_type();
	VVec::H5Data * buf = new VVec::H5Data;
	H5Dread(data, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
	sum.h5_read(buf);
	H5Dvlen_reclaim(type, space, H5P_DEFAULT, buf);
	H5Dclose(data);
	H5Sclose(space);
	delete buf;

	n = "rec_" + id;
	if (H5Lexists(group, n.c_str(), H5P_DEFAULT)) {
		data = H5Dopen(group, n.c_str(), H5P_DEFAULT);
		space = H5Dget_space(data);
		hsize_t len;
		H5Sget_simple_extent_dims(space, & len, 0);
		buf = new VVec::H5Data[len];
		H5Dread(data, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
		for (size_t i = 0; i < len; i ++) {
			VVec v;
			v.h5_read(buf + i);
			rec.push_back(v);
		}
		H5Dvlen_reclaim(type, space, H5P_DEFAULT, buf);
		H5Dclose(data);
		H5Sclose(space);
		delete [] buf;
	}
	H5Tclose(type);
}

void Avera::AccIVec::h5_save(hid_t group) const
{
	std::string n = "sum_" + id;
	hid_t space = H5Screate(H5S_SCALAR);
	hid_t type = IVec::h5_type();
	hid_t data = H5Dcreate(group, n.c_str(), type, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	IVec::H5Data * buf = new IVec::H5Data;
	sum.h5_make(buf);
	H5Dwrite(data, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
	H5Dclose(data);
	H5Sclose(space);
	delete buf;
	if (rec.size()) {
		n = "rec_" + id;
		hsize_t len = rec.size();
		space = H5Screate_simple(1, & len, 0);
		buf = new IVec::H5Data[len];
		for (size_t i = 0; i < len; i ++) rec[i].h5_make(buf + i);
		data = H5Dcreate(group, n.c_str(), type, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5Dwrite(data, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
		H5Dclose(data);
		H5Sclose(space);
		delete [] buf;
	}
	H5Tclose(type);
}

void Avera::AccIVec::h5_load(hid_t group)
{
	std::string n = "sum_" + id;
	hid_t data = H5Dopen(group, n.c_str(), H5P_DEFAULT);
	hid_t space = H5Dget_space(data); // `space' should be scalar
	hid_t type = IVec::h5_type();
	IVec::H5Data * buf = new IVec::H5Data;
	H5Dread(data, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
	sum.h5_read(buf);
	H5Dvlen_reclaim(type, space, H5P_DEFAULT, buf);
	H5Dclose(data);
	H5Sclose(space);
	delete buf;

	rec.clear();
	n = "rec_" + id;
	if (H5Lexists(group, n.c_str(), H5P_DEFAULT)) {
		data = H5Dopen(group, n.c_str(), H5P_DEFAULT);
		space = H5Dget_space(data);
		hsize_t len;
		H5Sget_simple_extent_dims(space, & len, 0);
		buf = new IVec::H5Data[len];
		H5Dread(data, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
		for (size_t i = 0; i < len; i ++) {
			IVec v;
			v.h5_read(buf + i);
			rec.push_back(v);
		}
		H5Dvlen_reclaim(type, space, H5P_DEFAULT, buf);
		H5Dclose(data);
		H5Sclose(space);
		delete [] buf;
	}
	H5Tclose(type);
}
#endif

double Avera::get_ave(const double & sum, int backtrack) const
{
	double ave = 0;
	bool found = false;
	for (std::vector<Accumu *>::const_iterator i = acc_list.begin(); i != acc_list.end(); i ++) {
		const AccDouble * av = dynamic_cast<const AccDouble *>(* i);
		if (! (av && & sum == & av->get_sum())) continue;
		found = true;
		if (backtrack) {
			if (backtrack < 0) { // when backtrack is negative, take away early data
				int n = int(av->get_rec().size()) - 1 + backtrack;
				if (n < 0) {
					std::cerr << "Not so many generations to back track!\n";
					abort();
				}
				ave = (av->get_rec()[n + 1] - av->get_rec()[n]) / (lens[n + 1] - lens[n]);

			} else {
				int n = int(av->get_rec().size()) - 1 - backtrack;
				if (n < 0) {
					std::cerr << "Not so many generations to back track!\n";
					abort();
				}
				if (n >= int(lens.size())) {
					std::cerr << "lens is incorrect in data!\n";
					abort();
				}
				ave = av->get_rec()[n] / lens[n];
			}
		}
		else {
			ave = av->get_sum() / length;
		}
		break;
	}
	if (! found) {
		std::cerr << "Variable not found in averager!\n";
		abort();
	}
	return ave;
}

FVec Avera::get_ave(const FVec & sum, int backtrack) const
{
	FVec ave(sum.size());
	bool found = false;
	for (std::vector<Accumu *>::const_iterator i = acc_list.begin(); i != acc_list.end(); i ++) {
		AccFVec const * av = dynamic_cast<AccFVec const *>(* i);
		if (! (av && & sum == & av->get_sum())) continue;
		found = true;
		if (backtrack) {
			if (backtrack < 0) { // when backtrack is negative, take away early data
				int n = int(av->get_rec().size()) - 1 + backtrack;
				if (n < 0) {
					std::cerr << "Not so many generations to back track!\n";
					throw;
				}
				ave = av->get_rec()[n + 1];
				ave -= av->get_rec()[n];
				ave /= (lens[n + 1] - lens[n]);

			}
			else {
				int n = int(av->get_rec().size()) - 1 - backtrack;
				if (n < 0) {
					std::cerr << "Not so many generations to back track!\n";
					throw;
				}
				ave = av->get_rec()[n];
				ave /= lens[n];
			}
		}
		else if (length) {
			ave = av->get_sum();
			ave /= length;
		}
		break;
	}
	if (! found) {
		std::cerr << "Variable not found in averager!\n";
		throw;
	}
	return ave;
}

VVec Avera::get_ave(const VVec & sum, int backtrack) const
{
	VVec ave;
	bool found = false;
	for (std::vector<Accumu *>::const_iterator i = acc_list.begin(); i != acc_list.end(); i ++) {
		const AccVVec * av = dynamic_cast<const AccVVec *>(* i);
		if (! (av && & sum == & av->get_sum())) continue;
		found = true;
		if (backtrack) {
			if (backtrack < 0) { // when backtrack is negative, take away early data
				int n = int(av->get_rec().size()) - 1 + backtrack;
				if (n < 0) {
					std::cerr << "Not so many generations to back track!\n";
					abort();
				}
				ave = av->get_rec()[n + 1];
				ave -= av->get_rec()[n];
				ave /= (lens[n + 1] - lens[n]);

			}
			else {
				int n = int(av->get_rec().size()) - 1 - backtrack;
				if (n < 0) {
					std::cerr << "Not so many generations to back track!\n";
					abort();
				}
				ave = av->get_rec()[n];
				ave /= lens[n];
			}
		}
		else if (length) {
			ave = av->get_sum();
			ave /= length;
		}
		break;
	}
	if (! found) {
		std::cerr << "Variable not found in averager!\n";
		abort();
	}
	return ave;
}

IVec Avera::get_ave(const IVec & sum, int backtrack) const
{
	IVec ave;
	bool found = false;
	for (std::vector<Accumu *>::const_iterator i = acc_list.begin(); i != acc_list.end(); i ++) {
		const AccIVec * av = dynamic_cast<const AccIVec *>(* i);
		if (! (av && & sum == & av->get_sum())) continue;
		found = true;
		if (backtrack) {
			if (backtrack < 0) { // when backtrack is negative, take away early data
				int n = int(av->get_rec().size()) - 1 + backtrack;
				if (n < 0) {
					std::cerr << "Not so many generations to back track!\n";
					abort();
				}
				ave = av->get_rec()[n + 1];
				ave -= av->get_rec()[n];
				ave /= (lens[n + 1] - lens[n]);

			} else {
				int n = int(av->get_rec().size()) - 1 - backtrack;
				if (n < 0) {
					std::cerr << "Not so many generations to back track!\n";
					abort();
				}
				ave = av->get_rec()[n];
				ave /= lens[n];
			}
		}
		else {
			ave = av->get_sum();
			ave /= length;
		}
		break;
	}
	if (! found) {
		std::cerr << "Variable not found in averager!\n";
		abort();
	}
	return ave;
}

Avera::~Avera()
{
	while (acc_list.size())	{
		delete acc_list.back();
		acc_list.pop_back();
	}
}

void Avera::init()
{
	// initialize accumus
	for (std::vector<Accumu *>::iterator i = acc_list.begin(); i != acc_list.end(); i ++) (* i)->clear();

	length = 0;
	limit = 1;
}

void Avera::step(double increment)
{
	length += increment;
	if (length < limit) return;
	// push averages
	lens.push_back(length);
	for (std::vector<Accumu *>::iterator i = acc_list.begin(); i != acc_list.end(); i ++) (* i)->push_rec();
	do limit *= factor;
	while (length >= limit);
}

#ifdef HAVE_HDF5
void Avera::h5_save(hid_t group, const std::string & name) const
{
	hid_t g = name.size() ? H5Gcreate(group, name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT) : group;
	hid_t space = H5Screate(H5S_SCALAR);
	hid_t data;

	data = H5Acreate(g, "length", H5T_NATIVE_DOUBLE, space, H5P_DEFAULT, H5P_DEFAULT);
	H5Awrite(data, H5T_NATIVE_DOUBLE, & length);
	H5Aclose(data);

	data = H5Acreate(g, "limit", H5T_NATIVE_DOUBLE, space, H5P_DEFAULT, H5P_DEFAULT);
	H5Awrite(data, H5T_NATIVE_DOUBLE, & limit);
	H5Aclose(data);

	data = H5Acreate(g, "factor", H5T_NATIVE_DOUBLE, space, H5P_DEFAULT, H5P_DEFAULT);
	H5Awrite(data, H5T_NATIVE_DOUBLE, & factor);
	H5Aclose(data);

	H5Sclose(space);

	for (std::vector<Accumu *>::const_iterator i = acc_list.begin(); i != acc_list.end(); i ++) (* i)->h5_save(g);

	if (lens.size()) { // save lens
		hsize_t l = lens.size();
		space = H5Screate_simple(1, & l, 0);
		double * d = new double[l];
		for (size_t i = 0; i < l; i ++) d[i] = lens[i];
		data = H5Dcreate(g, "lens", H5T_NATIVE_DOUBLE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5Dwrite(data, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, d);
		H5Dclose(data);
		H5Sclose(space);
		delete [] d;
	}
	if (g != group) H5Gclose(g);
}

void Avera::h5_load(hid_t group, const std::string & name)
{
	hid_t g = name.size() ? H5Gopen(group, name.c_str(), H5P_DEFAULT) : group;
	hid_t data;

	data = H5Aopen_name(g, "factor");
	if (data < 0) throw;
	H5Aread(data, H5T_NATIVE_DOUBLE, & factor);
	H5Aclose(data);

	data = H5Aopen_name(g, "length");
	if (data < 0) throw;
	H5Aread(data, H5T_NATIVE_DOUBLE, & length);
	H5Aclose(data);

	data = H5Aopen_name(g, "limit");
	if (data < 0) throw;
	H5Aread(data, H5T_NATIVE_DOUBLE, & limit);
	H5Aclose(data);

	if (length >= limit) throw;

	// check if all sums are available
	bool have_all = true;
	for (std::vector<Accumu *>::iterator i = acc_list.begin(); i != acc_list.end(); i ++) {
		std::string n = "sum_" + (* i)->get_id();
		if (! H5Lexists(g, n.c_str(), H5P_DEFAULT)) {
			have_all = false;
			break;
		}
	}
	if (! have_all) { // don't have all sums, data are not compatible
		std::cerr << "Don't have all data, incompatible file!\n";
		throw;
	}

	// load all data
	for (std::vector<Accumu *>::iterator i = acc_list.begin(); i != acc_list.end(); i ++) (* i)->h5_load(g);

	lens.clear();
	if (H5Lexists(g, "lens", H5P_DEFAULT)) {
		data = H5Dopen(g, "lens", H5P_DEFAULT);
		hid_t space = H5Dget_space(data);
		hsize_t l;
		H5Sget_simple_extent_dims(space, & l, 0);
		double * d = new double[l];
		H5Dread(data, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, d);
		for (size_t i = 0; i < l; i ++) lens.push_back(d[i]);
		H5Dclose(data);
		H5Sclose(space);
		delete [] d;
	}
	if (g != group) H5Gclose(g);
}
#endif

void Avera::dump_info(std::ostream & output) const
{
	output << "length=" << length << '\n';
	output << "rec_size=" << lens.size() << '\n';
}

void Avera::compact(size_t keep)
{
	size_t sz = lens.size();
	if (sz) {
		if (! keep) lens.clear();
		else if (keep < sz) lens.erase(lens.begin(), lens.begin() + sz - keep);
	}
	for (std::vector<Accumu *>::iterator i = acc_list.begin(); i != acc_list.end(); i ++) (* i)->compact(keep);
}
