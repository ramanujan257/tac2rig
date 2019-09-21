#include "line.hpp"

Line::Line(const std::string& s)
	: m_content(s), m_in{{""}}, m_out{{""}}
{
	m_def = def(s);
	m_use = use(s);
}

std::ostream& operator<< (std::ostream& out, const Line& l)
{
	out << l.m_content;
	return out;
}

std::set<std::string> Line::def(const std::string& s)
{
	std::string var_re = "([a-zA-Z_][a-zA-Z0-9_]*)";
	// TODO: dodaj mogucnost nizova
	std::string array_re = var_re + "\\[" + var_re + "\\]";
	std::string num_re = "([0-9]+)"; // TODO: proveri da li ovo treba "(?:[0-9]+)
	std::string varnum_re = "(" + var_re + "|" + num_re + ")";
	std::string line_num_re = "[1-9][0-9]*:\\s*";

	std::regex assign_regex(
			line_num_re +
			var_re + "\\s*:=\\s*" + 
			varnum_re + "\\s*([+*/-]\\s*" + 
			varnum_re + ")?");

	std::set<std::string> def_set;

	if (std::regex_match(s, assign_regex)) {
		def_set.insert(std::regex_replace(s, assign_regex, "$1"));

		return std::set<std::string>(def_set);
	}

	return std::set<std::string>();
}

std::set<std::string> Line::use(const std::string& s)
{
	std::string var_re = "([a-zA-Z_][a-zA-Z0-9_]*)";
	// TODO: dodaj mogucnost nizova
	std::string array_re = var_re + "\\[" + var_re + "\\]";
	std::string varnum_re = "(?:[1-9][0-9]*|"+var_re+")";
	std::string line_num_re= "[1-9][0-9]*:\\s*";

	std::regex return_regex(line_num_re + "\\s*return\\s+" + varnum_re);

	std::regex assign_regex(
			line_num_re + var_re + "\\s*:=\\s*" + 
			varnum_re + "(?:\\s*[+*/-]\\s*" + varnum_re + ")?");

	std::regex cond_regex(
				line_num_re + "if\\s+" + var_re + "\\s*<=\\s*" + 
				varnum_re + "\\s+goto\\s+B[0-9][0-9]*");

	std::string result;
	std::set<std::string> use_set;

	if (std::regex_match(s, return_regex)) {
		use_set.insert(std::regex_replace(s, return_regex, "$1"));
		return std::set<std::string>(use_set);
	} else if (std::regex_match(s, assign_regex)) {
		use_set.insert(std::regex_replace(s, assign_regex, "$2"));
		use_set.insert(std::regex_replace(s, assign_regex, "$3"));
		return std::set<std::string>(use_set);
	} else if (std::regex_match(s, cond_regex)) {
		use_set.insert(std::regex_replace(s, cond_regex, "$1"));
		use_set.insert(std::regex_replace(s, cond_regex, "$2"));
		return std::set<std::string>(use_set);
	}
	
	return std::set<std::string>();
}

std::set<std::string> Line::use() const
{
	return m_use;
}

std::set<std::string> Line::def() const
{
	return m_def;
}

std::set<std::string> Line::in() const
{
	return m_in;
}

std::set<std::string> Line::out() const  
{
	return m_out;
}

void Line::set_in(const std::set<std::string>& in){
	for (auto el : in) 
		m_in.insert(el);
}

void Line::set_out(const std::set<std::string>& out){
	for (auto el : out)
		m_out.insert(el);
}

std::string Line::line() const{
	return m_content;
}

void Line::print_use() const{
	std::copy(m_use.cbegin(), m_use.cend(),
			  std::ostream_iterator<std::string>(std::cout, " "));
}

void Line::print_def() const
{
	std::copy(m_def.cbegin(), m_def.cend(),
			std::ostream_iterator<std::string>(std::cout, " "));
}

void Line::print_in() const
{
	std::copy(m_in.cbegin(), m_in.cend(),
			std::ostream_iterator<std::string>(std::cout, " "));
}

void Line::print_out() const
{
	std::copy(m_out.cbegin(), m_out.cend(),
			std::ostream_iterator<std::string>(std::cout, " "));
}

bool Line::operator== (const Line& l) const
{
	return (m_content == l.line()) ? true : false;
}

bool Line::operator!= (const Line& l) const
{
	return (m_content != l.line()) ? true : false;
}

void Line::clean()
{
	auto find = std::find_if(m_in.begin(), m_in.end(),
				[] (std::string s) { return s.empty(); });
	m_in.erase(find);
	
	find = std::find_if(m_out.begin(), m_out.end(),
				[] (std::string s) { return s.empty(); });
	m_out.erase(find);
}
