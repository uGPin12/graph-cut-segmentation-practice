#pragma once

#include <string>
#include <iostream>


#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <boost/format.hpp>

namespace fs = boost::filesystem;

// boostを用いたディレクトリ作成(エラーチェック付き)
void mkdir_boost(std::string dir)
{
	const fs::path path(dir);
	if (fs::exists(dir)) {
		return;
	}

	boost::system::error_code error;
	const bool result = fs::create_directories(path, error);
	if (!result || error) {
		std::cerr << "Fail to create dictionary : " << dir
			<< "\n Hit any key to exit..."
			<< std::endl;
		system("pause");
	}
}

// boostを用いたパラメータ読み込み
template<class RET>
RET read_param_boost(std::string path, std::string section, std::string key)
{
	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(path, pt);

	if (boost::optional<RET> ret = pt.get_optional<RET>(section + "." + key)) {
		return ret.get();
	}
	else {
		std::cerr 
			<< key << " is nothing "
			<< "\n Hit any key to exit..."
			<< std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}

}
