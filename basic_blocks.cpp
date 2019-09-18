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

std::set<std::string> BasicBlock::use() 
{
	// FIXME: bb_id 4 je za exit blok; napraviti konstruktor koji prepoznaje 
	// 		  Exit blok
	if (bb_id == 4) return std::set<std::string>();
	std::vector<Line> lines = getLines();
	m_use = lines[0].use();
	std::set<std::string> defs = lines[0].def();
	std::set<std::string> diff;
	for (int i=1; i<lines.size(); i++) {
		std::set<std::string> use = lines[i].use();
		/* use[s_i] \ Union{k=1;i}(def[s_k]) */
		std::set_difference(use.begin(), use.end(),
							defs.begin(), defs.end(),
							std::inserter(diff, diff.end()));

		
		std::set_union(m_use.begin(), m_use.end(),
					   diff.begin(), diff.end(),
					   std::inserter(m_use, m_use.end()));

		
		std::set<std::string> def = lines[i].def();
		std::set_union(defs.begin(), defs.end(),
					   def.begin(), def.end(),
					   std::inserter(defs, defs.end()));
		
	}

	return std::set<std::string>(m_use);
}

std::set<std::string> BasicBlock::def()
{
	// FIXME: bb_id 4 je za exit blok; napraviti konstruktor koji prepoznaje 
	// 		  Exit blok
	if (bb_id == 4) return std::set<std::string>();

	// def[B] = Union{i=1;i=num_lines}def[s_i], s_i = i-ta linija
	std::vector<Line> lines = getLines();
	for (auto l : lines){
		std::set<std::string> line_def(l.def());
		std::set_union(m_def.begin(), m_def.end(),
					   line_def.begin(), line_def.end(),
					   std::inserter(m_def, m_def.end()));
	}

	return std::set<std::string>(m_def);
}

void BasicBlock::print_def() const
{
	std::copy(m_def.cbegin(), m_def.cend(),
			std::ostream_iterator<std::string>(std::cout, " "));
}

void BasicBlock::print_use() const
{
	std::copy(m_use.cbegin(), m_use.cend(),
			std::ostream_iterator<std::string>(std::cout, " "));
}

std::set<std::string> BasicBlock::in_bb() 
{
	std::set<std::string> diff{""};
	std::set<std::string> def_set(def());
	std::set<std::string> out = m_out;
	std::set_difference(out.cbegin(), out.cend(),
						def_set.cbegin(), def_set.cend(),
						std::inserter(diff, diff.end()));

	std::set<std::string> use_set(use());
	std::set_union(use_set.cbegin(), use_set.cend(),
				   diff.cbegin(), diff.cend(),
				   std::inserter(m_in, m_in.end()));
	//print_in();

	return std::set<std::string>(m_in);
}

std::set<std::string> BasicBlock::out_bb()
{
	for (auto c : _children){
		auto tmp_in = c->in_bb();
		std::set_union(m_out.begin(), m_out.end(),
					   tmp_in.cbegin(), tmp_in.cend(),
					   std::inserter(m_out, m_out.end()));
	}

	return std::set<std::string>(m_out);
}

void BasicBlock::print_in() const
{
	std::copy(m_in.cbegin(), m_in.cend(),
			std::ostream_iterator<std::string>(std::cout, " "));
}

void BasicBlock::print_out() const
{
	std::copy(m_out.cbegin(), m_out.cend(),
			std::ostream_iterator<std::string>(std::cout, " "));
}
