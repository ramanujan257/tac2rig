#include "basic_blocks.hpp"

int BasicBlock::bb_count = 0;

std::set<std::string> use(const std::string& line)
{
	std::string var_regex = "([a-zA-Z_][a-zA-Z0-9_]*)";
	std::string line_num_regex = "[1-9][0-9]*:";

	std::regex return_regex(line_num_regex + "\s*return\s+" + var_regex);

	std::regex assign_regex(
			line_num_regex + var_regex + "\s*:=\s*" + 
			var_regex + "\s*[+*/-]\s*" + var_regex);

	std::regex cond_regex(
				line_num_regex + "if\s+"+var_regex+"\s*<\s*" + 
				"([1-9][0-9]*|"+var_regex+")\s+goto\s+B[1-9][0-9]*");

	std::string result;
	std::set<std::string> use_set;

	// add appropriate variables to the use set, according to matched regex
	if (std::regex_match(line, return_regex)) {
		result = std::regex_replace(line, return_regex, "$1");
		use_set.insert(result);

		return std::set<std::string>(use_set);
	} else if (std::regex_match(line, assign_regex)) {
		use_set.insert(std::regex_replace(line, assign_regex, "$2"));
		use_set.insert(std::regex_replace(line, assign_regex, "$3"));

		return std::set<std::string>(use_set);
	} else if (std::regex_match(line, cond_regex)) {
		use_set.insert(std::regex_replace(line, cond_regex, "$1"));
		use_set.insert(std::regex_replace(line, cond_regex, "$2"));

		return std::set<std::string>(use_set);
	}

	return std::set<std::string>();
}

std::set<std::string> def(const std::string& line)
{
	std::string var_regex = "([a-zA-Z_][a-zA-Z0-9_]*)";

	std::regex assign_regex(
			var_regex + "\s*:=\s*" + var_regex + "\s*[+*/-]\s*" + var_regex);

	std::set<std::string> def_set;

	if (std::regex_match(line, assign_regex)) {
		def_set.insert(std::regex_replace(line, assign_regex, "$1"));

		return std::set<std::string>(def_set);
	}

	return std::set<std::string>();
}

std::set<std::string> in(const std::string& line)
{
	std::set<std::string> in_set;
	std::set<std::string> use_set = use(line);	
	std::set<std::string> def_set = def(line);
	std::set<std::string> out_set = out(line);
	std::set<std::string> diff_set;

	auto found = std::find(_lines.cbegin(), _lines.cend(), line);
	if (found == _lines.end()) {
		std::cerr << "Line is not in this block" << std::endl;
		return std::set<std::string>();
	} else if (found == *(_lines.rbegin())) {
		// TODO
	} 
	
	diff_set.reserve(out_set.size());
	set_difference(out_set.begin(), out_set.end(),
				   def_set.begin(), def_set.end(),
				   std::inserter(diff_set, diff_set.end())
				   );

	in_set.reserve(use_set.size() + diff_set.size());
	set_union(use_set.begin(), use_set.end(),
			  diff_set.begin(), diff_set.end(),
			  std::inserter(in_set, in_set.end())
			);

	return std::set<std::string>(in_set);
}

std::set<std::string> out(const std::string& line)
{
	std::set<std::string> out_set;

	// TODO

	return out_set;
}
