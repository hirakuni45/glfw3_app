#pragma once
#include <string>
// #include <boost/format.hpp>

namespace utils {

	class verbose {
	public:
		struct level {
			enum type {
				none,
				min,
				mid,
				max
			};
		};

	private:
		std::string	tab_;
		level::type	level_;
		int			nest_pos_;
		std::string	nest_str_;

	public:
		verbose() : tab_("  "), level_(level::none), nest_pos_(0) { }

		void set_tab(const std::string& tab) { tab_ = tab; }
		const std::string& get_tab() const { return tab_; }

		void nest_reset(int nest = 0) { nest_pos_ = nest; }

		void nest_down() {
			++nest_pos_;
		}

		void nest_up() {
			--nest_pos_;
			if(nest_pos_ < 0) nest_pos_ = 0;
		}

		void nest_out() const {
			if(level_ != level::none) {
				for(int i = 0; i < nest_pos_; ++i) {
					std::cout << tab_;
				}
			}
		}

		const std::string& get_nest() {
			nest_str_.clear();
			for(int i = 0; i < nest_pos_; ++i) {
				nest_str_ += tab_;
			}
			return nest_str_;
		}

		int get_nest_position() const { return nest_pos_; }

		void set_level(level::type l) { level_ = l; }

		level::type get_level() const { return level_; } 

		bool operator() () const { return (level_ != level::none); }

	};

}
