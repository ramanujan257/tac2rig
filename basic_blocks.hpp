#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <regex>

class BasicBlock{
    public:
        BasicBlock()
            :bb_id{bb_count++}
        {}

        void addLine(std::string& line){
            _lines.push_back(line);
        }

        std::vector<std::string> getLines(){
            return _lines;
        }

        void addChild(BasicBlock* c){
            _children.push_back(c);
        }

        void print(){
            std::cout << "BB" << bb_id << std::endl;
            for(auto l: _lines)
                std::cout << l << std::endl;
        }

        int getID(){
            return bb_id;
        }

		friend std::set<std::string> use(const std::string& line);
		friend std::set<std::string> def(const std::string& line);

		friend std::set<std::string> in(const std::string& line);
		friend std::set<std::string> out(const std::string& line);

		std::set<std::string> in_bb() const;
		std::set<std::string> out_bb() const;

    private:
        static int bb_count;
        int bb_id;
        std::vector<std::string> _lines;
        std::vector<BasicBlock*> _children;
};

std::set<std::string> use(const std::string& line);
std::set<std::string> def(const std::string& line);
                                                         
std::set<std::string> in(const std::string& line);
std::set<std::string> out(const std::string& line);
