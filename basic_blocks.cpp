#include "basic_blocks.hpp"

int BasicBlock::bb_count = 0;

// FIXME: izmeni tako da odgovara klasi Line
std::set<std::string> in(const BasicBlock& bb, const Line& line)
{
	std::set<std::string> in_set;
	std::set<std::string> use_set = line.use();	
	std::set<std::string> def_set = line.def();
	std::set<std::string> out_set = out(bb, line);
	std::set<std::string> diff_set;

	/*
	auto found = std::find(bb._lines.cbegin(), bb._lines.cend(), line);
	if (found == bb._lines.end()) {
		std::cerr << "Line is not in this block" << std::endl;
		return std::set<std::string>();
	} 

	set_difference(out_set.begin(), out_set.end(),
				   def_set.begin(), def_set.end(),
				   std::inserter(diff_set, diff_set.end())
				   );

	set_union(use_set.begin(), use_set.end(),
			  diff_set.begin(), diff_set.end(),
			  std::inserter(in_set, in_set.end())
			);
	*/

	return std::set<std::string>(in_set);
}

//FIXME: ne valja out fja (napraviti je tako da odgovara fji out_bb
// izmeni tako da odgovara klasi Line
std::set<std::string> out(const BasicBlock& bb, const Line& line)
{
	std::set<std::string> out_set;
	
/*
	auto found = std::find(bb._lines.cbegin(), bb._lines.cend(), line);
	if (found == bb._lines.cend()) {
		std::cerr << "Line is not in this block" << std::endl;
		return std::set<std::string>();
	} else if (found == bb._lines.cend()-1) {
		out_set.insert(bb.out_bb());
	} else {
		//FIXME: nije line, vec sledbenik od line
		out_set = in(line);
	}
*/

	return out_set;
}

std::set<std::string> BasicBlock::in_bb() const
{
	return std::set<std::string>();
}

std::set<std::string> BasicBlock::out_bb() const
{
	return std::set<std::string>();
}
