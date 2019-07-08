#pragma once

#include <iostream>
#include <vector>
#include <string>

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

    private:
        static int bb_count;
        int bb_id;
        std::vector<std::string> _lines;
        std::vector<BasicBlock*> _children;
};

int BasicBlock::bb_count = 0;
