/* sim/avera.hh
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
  Keep history of numerical averages at exponential increasing time intervals.
  This is useful for accessing the stationary state of a system.
 */
#ifndef AVERA_HH
#define AVERA_HH

#include "fvec.hh"
#include "vvec.hh"
#include "ivec.hh"
#include <arg.hh>
#ifdef HAVE_HDF5
extern "C" {
#include <hdf5.h>
}
#endif
#if __cplusplus < 201103L
#define override
#define final
#endif
class Avera
{
private:
	// persistent
	double length; // run length
	double limit;  // next run length to save
	double factor; // multiplication factor for limit

	class Accumu
	{
	protected:
		std::string id; // for storage
	public:
		Accumu(const std::string & name) : id(name) {}
		virtual ~Accumu() {}
		virtual void clear() = 0;
		virtual void push_rec() = 0;
#ifdef HAVE_HDF5
		virtual void h5_save(hid_t group) const = 0;
		virtual void h5_load(hid_t group) = 0;
#endif
		virtual void compact(size_t keep) = 0;
		const std::string & get_id() const {return id;}
	};

	class AccDouble :
		public Accumu
	{
		double & sum;
		std::vector<double> rec;
	public:
		AccDouble(double & summation, const std::string & name) :
			Accumu(name), sum(summation) {}
		void clear() override {rec.clear(); sum = 0;}
		void push_rec() override {rec.push_back(sum);}
#ifdef HAVE_HDF5
		void h5_save(hid_t group) const override;
		void h5_load(hid_t group) override;
#endif
		void compact(size_t keep) override
		{
			size_t sz = rec.size();
			if (sz) {
				if (! keep) rec.clear();
				else if (keep < sz) rec.erase(rec.begin(), rec.begin() + sz - keep);
			}
		}
		const double & get_sum() const {return sum;}
		const std::vector<double> & get_rec() const {return rec;}
	};

	class AccFVec :
		public Accumu
	{
		FVec & sum;
		std::vector<FVec> rec;
	public:
		AccFVec(FVec & summation, const std::string & name) :
			Accumu(name), sum(summation) {}
		void clear() override {rec.clear(); sum.clear();}
		void push_rec() override {rec.push_back(sum);}
#ifdef HAVE_HDF5
		void h5_save(hid_t group) const override;
		void h5_load(hid_t group) override;
#endif
		void compact(size_t keep) override
		{
			size_t sz = rec.size();
			if (sz) {
				if (! keep) rec.clear();
				else if (keep < sz) rec.erase(rec.begin(), rec.begin() + sz - keep);
			}
		}
		const FVec & get_sum() const {return sum;}
		const std::vector<FVec> & get_rec() const {return rec;}
	};

	class AccVVec :
		public Accumu
	{
		VVec & sum;
		std::vector<VVec> rec;
	public:
		AccVVec(VVec & summation, const std::string & name) :
			Accumu(name), sum(summation) {}
		void clear() override {rec.clear(); sum.clear();}
		void push_rec() override {rec.push_back(sum);}
#ifdef HAVE_HDF5
		void h5_save(hid_t group) const override;
		void h5_load(hid_t group) override;
#endif
		void compact(size_t keep) override
		{
			size_t sz = rec.size();
			if (sz) {
				if (! keep) rec.clear();
				else if (keep < sz) rec.erase(rec.begin(), rec.begin() + sz - keep);
			}
		}
		const VVec & get_sum() const {return sum;}
		const std::vector<VVec> & get_rec() const {return rec;}
	};

	class AccIVec :
		public Accumu
	{
		IVec & sum;
		std::vector<IVec> rec;
	public:
		AccIVec(IVec & summation, const std::string & name) :
			Accumu(name), sum(summation) {}
		void clear() override {rec.clear(); sum.clear();}
		void push_rec() override {rec.push_back(sum);}
#ifdef HAVE_HDF5
		void h5_save(hid_t group) const override;
		void h5_load(hid_t group) override;
#endif
		void compact(size_t keep) override
		{
			size_t sz = rec.size();
			if (sz) {
				if (! keep) rec.clear();
				else if (keep < sz) rec.erase(rec.begin(), rec.begin() + sz - keep);
			}
		}
		const IVec & get_sum() const {return sum;}
		const std::vector<IVec> & get_rec() const {return rec;}
	};

	std::vector<double> lens;
	std::vector<Accumu *> acc_list;
public:
	void add_accumu(double & sum, const std::string & name)
	{
		acc_list.push_back(new AccDouble(sum, name));
	}

	void add_accumu(FVec & sum, const std::string & name)
	{
		acc_list.push_back(new AccFVec(sum, name));
	}

	void add_accumu(VVec & sum, const std::string & name)
	{
		acc_list.push_back(new AccVVec(sum, name));
	}

	void add_accumu(IVec & sum, const std::string & name)
	{
		acc_list.push_back(new AccIVec(sum, name));
	}

	double get_ave(const double & sum, int backtrack = 0) const;
	FVec get_ave(const FVec & sum, int backtrack = 0) const;
	VVec get_ave(const VVec & sum, int backtrack = 0) const;
	IVec get_ave(const IVec & sum, int backtrack = 0) const;

public:
	Avera() : factor(2) {}
	virtual ~Avera();

	void init();
	void step(double increment = 1.0);
#ifdef HAVE_HDF5
	void h5_save(hid_t group, const std::string & name = std::string()) const;
	void h5_load(hid_t group, const std::string & name = std::string());
#endif
	void dump_info(std::ostream & output) const;
	size_t rec_size() const {return lens.size();}
	double rec_len(size_t i) const {return lens[i];}
	double get_length() const {return length;}
	void compact(size_t keep = 0);
};
#endif // AVERAGER_HH
