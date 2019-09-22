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
	std::string lval_re =
	   	"([a-zA-Z_][a-zA-Z0-9_]*)(?:\\[(?:([a-zA-Z_][a-zA-Z0-9_]*)|(?:(?:[0-9]+)))\\])?";

	std::string rval_re = "(?:([a-zA-Z_][a-zA-Z0-9_]*)(?:\\[(?:([a-zA-Z_][a-zA-Z0-9_]*)|(?:(?:[0-9]+)))\\])?|(?:(?:[0-9]+)))";

	std::string optional_re = "(?:\\s*[+*/-]\\s*" + rval_re + ")?";
	std::string line_num_re = "[1-9][0-9]*:\\s*";

	std::string str_assign_re = line_num_re + lval_re + 
							"\\s*:=\\s*" + rval_re + optional_re;

	std::regex assign_re(str_assign_re);

	std::set<std::string> def_set;

	if (std::regex_match(s, assign_re)) {
		def_set.insert(std::regex_replace(s, assign_re, "$1"));

		return std::set<std::string>(def_set);
	}

	return std::set<std::string>();
}

std::set<std::string> Line::use(const std::string& s)
{
	std::string lval_re( 
	   	"([a-zA-Z_][a-zA-Z0-9_]*)(?:\\[(?:([a-zA-Z_][a-zA-Z0-9_]*)|(?:(?:[0-9]+)))\\])?");

	std::string rval_re("(?:([a-zA-Z_][a-zA-Z0-9_]*)(?:\\[(?:([a-zA-Z_][a-zA-Z0-9_]*)|(?:(?:[0-9]+)))\\])?|(?:(?:[0-9]+)))");

	std::string optional_re("(?:\\s*[+*/-]\\s*" + rval_re + ")?");
	std::string line_num_re("[1-9][0-9]*:\\s*");

	std::string str_assign_re(line_num_re + lval_re + 
							"\\s*:=\\s*" + rval_re + optional_re);

	std::string str_cond_re(line_num_re + "if\\s+" + lval_re + 
				"\\s*[<>][=]?\\s*" + rval_re + "\\s+goto\\s+B[0-9][0-9]*");

	std::string str_return_re(line_num_re + "\\s*return\\s+" + rval_re);

	std::regex assign_re(str_assign_re);
	std::regex cond_re(str_cond_re);
	std::regex return_re(str_return_re);

	std::string result;
	std::set<std::string> use_set;

	if (std::regex_match(s, return_re)) {
		use_set.insert(std::regex_replace(s, return_re, "$1"));
		use_set.insert(std::regex_replace(s, return_re, "$2"));
		return std::set<std::string>(use_set);
	} else if (std::regex_match(s, assign_re)) {
		use_set.insert(std::regex_replace(s, assign_re, "$2"));
		use_set.insert(std::regex_replace(s, assign_re, "$3"));
		use_set.insert(std::regex_replace(s, assign_re, "$4"));
		use_set.insert(std::regex_replace(s, assign_re, "$5"));
		use_set.insert(std::regex_replace(s, assign_re, "$6"));
		return std::set<std::string>(use_set);
	} else if (std::regex_match(s, cond_re)) {
		use_set.insert(std::regex_replace(s, cond_re, "$1"));
		use_set.insert(std::regex_replace(s, cond_re, "$2"));
		use_set.insert(std::regex_replace(s, cond_re, "$3"));
		use_set.insert(std::regex_replace(s, cond_re, "$4"));
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
