#pragma once

#include <set>
#include <string>
#include <regex>
#include <vector>
#include <iostream>
#include <iterator>


class Line {
public:
	Line(const std::string& s);

	std::string line() const;
	
	std::set<std::string> in() const;
	std::set<std::string> out() const;

	std::set<std::string> use() const;
	std::set<std::string> def() const;

	void set_in(const std::set<std::string>& in);
	void set_out(const std::set<std::string>& out);

	void print_use() const;
	void print_def() const;
	void print_in() const;
	void print_out() const;

	bool operator== (const Line& l) const;
	bool operator!= (const Line& l) const;

	friend std::ostream& operator<< (std::ostream& out, const Line& l);

private:
	std::set<std::string> def(const std::string& s);
	std::set<std::string> use(const std::string& s);
	std::string m_content;
	std::set<std::string> m_in;
	std::set<std::string> m_out;
	std::set<std::string> m_use;
	std::set<std::string> m_def;
};

std::ostream& operator<< (std::ostream& out, const Line& l);
