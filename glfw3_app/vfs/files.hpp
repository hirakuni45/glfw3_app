#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include "tree_unit.hpp"
#include "handle_set.hpp"

namespace utils {

	enum class open_mode {
		none,
		read,
		write	
	};

	enum class seek_mode {
		set,
		cur,
		end,
	};


	struct file {
		char	path_[256];
		uint32_t	size_;
		std::vector<uint16_t>	idxs_;
		file() : size_(0), idxs_() { }
	};

	class files {
		std::string	base_;
		uint32_t	idx_;
		tree_unit				tree_unit_;
		handle_set<uint32_t>	handle_set_;
		std::vector<file>		dirs_;

	public:
		files(const std::string& base) : base_(base), idx_(0),
										 tree_unit_(), handle_set_(1000) {
			std::cout << "Construct files: '" << base << "'" << std::endl;
		}

		~files() {
		}

		void start() {
			if(idx_ != 0) {

				++idx_;
				return;
			}

			// ディレクトリー情報を読み込み
		}

		// ファイルがあるか調べる
		bool probe(const std::string& path) const {
			return tree_unit_.find(path);
		}

		// ディレクトリーか検査（子供があるか？）
		bool is_directory(const std::string& path) const {
			return tree_unit_.is_directory(path);
		}

		bool set_current_directory(const std::string& path) {
			return tree_unit_.set_current_path(path);
		}

		uint32_t mkdir(const std::string& path) {
			return tree_unit_.make_directory(path);
		}

		uint32_t rmdir(const std::string& path) {
			return tree_unit_.remove_directory(path);
		}

		// ファイルを作成してハンドルを返す（０なら失敗）
		uint32_t install(const std::string& path) {
			return tree_unit_.install(path);
		}


		bool erase(const std::string& path) {
			return tree_unit_.erase(path);
		}


		// ディレクトリー情報の同期
		void sync() {

		}


		// ディレクリー情報の書き込み
		void final() {
			if(idx_ <= 1) {
				return;
			}
		}

		void list() {
			tree_unit_.list();
		}
	};
}
