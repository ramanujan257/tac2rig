#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "basic_blocks.hpp"

void liveness_analysis(const std::vector<BasicBlock*>& bbs);

std::vector<BasicBlock*> basicBlocks;
BasicBlock* currBB;// = new BasicBlock();
int exit_id;

std::vector<int> leaders{1};
std::map<int, std::vector<BasicBlock*>> jumpers;

BasicBlock* getCurrentBB();
void setCurrentBB(BasicBlock* bb);
BasicBlock* bbLookup(int address);
void print_block(BasicBlock* bb);

BasicBlock* getCurrentBB(){
    return currBB;
}

void setCurrentBB(BasicBlock* bb){
    currBB = bb;
}

BasicBlock* bbLookup(int address){
    for(auto b : basicBlocks){
        for(auto l : b->getLines()){
            if(std::stoi(l.line()) == address)
                return b;
        }
    }
    
    return nullptr;
}

void print_block(BasicBlock* bb){
	std::cout << "BLOCK" << bb->getID() << std::endl;
	auto lines = bb->getLines();
	for (auto l : lines)
		std::cout << l << std::endl;
	std::cout << "BLOCK_END" << std::endl;
}

std::vector<BasicBlock*> parse(const std::string& fname){
	std::ifstream input;
	input.open(fname.c_str());
	std::string line;
    std::vector<std::string> buffer;

    int line_no = 0;
    if(input.is_open()){
        while(std::getline(input, line)){
            buffer.push_back(line);
            line_no++;
            //std::cout << line << std::endl;
            int idx = line.find("goto");
            if(idx != -1){
                int i1 = line.find("(",idx) + 1;
                leaders.push_back(std::stoi(line.substr(i1)));
                std::cout << "pb l+1" << std::endl;
            if(input.peek() != -1)
                leaders.push_back(line_no+1);
            else
                leaders.push_back(line_no);
            }
        }
        input.close();
    }


    std::sort(leaders.begin(), leaders.end());

    std::cout << "idxs = [";
    for(int i : leaders)
        std::cout << i << ",";
    std::cout << "]" << std::endl;
        
        for(int i = 0; i < leaders.size() - 1; i++){
            int from = leaders[i];
            int to = leaders[i+1];
            BasicBlock* newBB = new BasicBlock();
            for(int j = from; j < to; j++){
                newBB->addLine(buffer[j]);
            }
            basicBlocks.push_back(newBB);
        }
        
        for(auto bb : basicBlocks){
            
            std::string s;
            std::cout << "LINE " << bb->getLines().back().line() << std::endl;
            int idx = bb->getLines().back().line().find("goto");
            if(idx != -1){
                
                int i1 = line.find("(",idx) + 1;
                BasicBlock* gotoBlock = bbLookup(i1);
                if(gotoBlock){
                    bb->addChild(gotoBlock);
                } else {
                    // ERROR: all blocks are already constructed
                }
                
                std::cout << "This block ID " << bb->getID() << " basicBlocks[thisId] " << basicBlocks[bb->getID()-1]->getID() << std::endl;
            } else {
                
            }
                
            print_block(bb);
        }
        
        
        return std::vector<BasicBlock*>();
}

int main(){
    
    parse("bb_test");
    
    return 0;
}
