#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <set>
#include <algorithm>

#include "line.hpp"

class BasicBlock{
    public:
        BasicBlock()
            :bb_id{bb_count++}, m_use{{""}},
				m_def{{""}},
				m_in{{""}},
				m_out{{""}},
				m_role{0} // TODO: enum
        {}

	BasicBlock(std::string s)
		:bb_id{-1}, m_use{{""}},
				m_def{{""}},
				m_in{{""}},
				m_out{{""}}
        {}

        void addLine(std::string& line){
			Line l(line);
            _lines.push_back(l);
        }

        std::vector<Line> getLines() const{
            return _lines;
        }

        void addChild(BasicBlock* c){
            if(!(std::find(_children.cbegin(), _children.cend(), c) != _children.end()))
                _children.push_back(c);
        }

        std::vector<BasicBlock*>& getChildren(){
            return _children;
        }

        void addParent(BasicBlock* c){
            _parents.push_back(c);
        }

        std::vector<BasicBlock*>& getParents(){
            return _parents;
        }

        void print(){
            std::cout << "BB" << bb_id << std::endl;
            for(auto l: _lines)
                std::cout << l << std::endl;
        }

        int getID(){
            return bb_id;
        }

	void setRole(int r){
		m_role = r;	
	}

	int getRole(){
		return m_role;	
	}

	static int const getBBCount(){
		return bb_count;	
	}

		friend std::set<std::string> line_in(const BasicBlock& bb, Line& line);
		friend std::set<std::string> line_out(const BasicBlock& bb, Line& line);

		std::set<std::string> use();
		std::set<std::string> def();

		std::set<std::string> in_bb();
		std::set<std::string> out_bb();

		std::set<std::string> c_in_bb() const;
		std::set<std::string> c_out_bb() const;

		void print_def() const;
		void print_use() const;
		void print_in() const;
		void print_out() const;

    private:
        static int bb_count;
        int bb_id;
		int m_role; // TODO: enum
		std::set<std::string> m_use;
		std::set<std::string> m_def;
		std::set<std::string> m_in;
		std::set<std::string> m_out;
        std::vector<Line> _lines;
        std::vector<BasicBlock*> _children;
        std::vector<BasicBlock*> _parents;
};

std::set<std::string> line_in(const BasicBlock& bb, std::string& line);
std::set<std::string> line_out(const BasicBlock& bb, std::string& line);
