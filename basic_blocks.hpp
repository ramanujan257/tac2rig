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
				m_out{{""}}
        {}

        void addLine(std::string& line){
			Line l(line);
            _lines.push_back(l);
        }

        std::vector<Line> getLines() const
		{
            return _lines;
        }

        void addChild(BasicBlock* c){
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

		friend std::set<std::string> in(const BasicBlock& bb, const Line& line);
		friend std::set<std::string> out(const BasicBlock& bb, const Line& line);

		std::set<std::string> use();
		std::set<std::string> def();

		std::set<std::string> in_bb();
		std::set<std::string> out_bb();

		void print_def() const;
		void print_use() const;
		void print_in() const;
		void print_out() const;

    private:
        static int bb_count;
        int bb_id;
		std::set<std::string> m_use;
		std::set<std::string> m_def;
		std::set<std::string> m_in;
		std::set<std::string> m_out;
        std::vector<Line> _lines;
        std::vector<BasicBlock*> _children;
        std::vector<BasicBlock*> _parents;
};

std::set<std::string> in(const std::string& line);
std::set<std::string> out(const std::string& line);
