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

void liveness_analysis(const std::vector<BasicBlock*>& bbs);

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
			// Split on GOTO
            int idx = line.find("goto");
            if(idx != -1){
                int i1 = line.find("(",idx) + 1;
                leaders.push_back(std::stoi(line.substr(i1)));
            if(input.peek() != -1)
                leaders.push_back(line_no+1);
            else
                leaders.push_back(line_no);
            }

			// Split on RETURN
			idx = line.find("return");
			if(idx != -1){
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
            for(int j = from; j <= to; j++){
                newBB->addLine(buffer[j-1]);
            }
            if(newBB->getLines().back().line().find("goto"))
            	newBB->setRole(1);
           	else if(newBB->getLines().back().line().find("return"))
           		newBB->setRole(2);
			basicBlocks.push_back(newBB);
        }

		// Add exit block 
		BasicBlock* exitBB = new BasicBlock("exit");
		basicBlocks.push_back(exitBB);

        
		// Create CFG
        for(auto bb : basicBlocks){
            if(bb->getID() != -1){
		        std::string line_ = bb->getLines().back().line();
		        
		        /*
		        int idx = line_.find("goto");
		        if(idx != -1){
		           std::cout << "IF SUCC" << std::endl; 
		            int i1 = std::stoi(line_.substr(line_.find("(",idx) + 1));
				
		            BasicBlock* gotoBlock = bbLookup(i1);
					std::cout << i1 << " LOOKUP SUCC " << gotoBlock << std::endl;
		            if(gotoBlock){
		            	//THEN 
						std::cout << "GOTO FOUND" << std::endl;
		                bb->addChild(gotoBlock);
		                if(bb->getID() < basicBlocks.size() - 1){
		                	std::cout << "BALLOC" << std::endl;
		                	bb->addChild(basicBlocks[basicBlocks.size()]);
		                	}
		            } else {
						std::cout << "ERROR?" << std::endl;
		                // ERROR: all blocks are already constructed
		            }
		            
		        } */  
		        
		        switch(bb->getRole()){
		        	case 0:
		        		// Unconditional + unconditional
		        		if(basicBlocks[bb->getID()+1]->getRole() != 1){
		        			bb->addChild(basicBlocks[bb->getID()+1]);
		        		}
		        		break;
		        	case 1: {
		        		// Conditional
		        		std::string _line = bb->getLines().back().line();
		        		int jmp_addr = std::stoi(line_.substr(line_.rfind("(") + 1));
		        		BasicBlock *thenBB = bbLookup(jmp_addr);
		        		thenBB->setRole(3);
		        		
		        		BasicBlock *elseBB = bbLookup(std::stoi(line_));
		        		
		        		bb->addChild(thenBB);
		        		bb->addChild(elseBB);
		        		break; }
		        	case 2: {
		        		// Return
		        		bb->addChild(basicBlocks.back());
		        		break;
		        		}
		        		
		        }

				// Blocks containing return have no children other than EXIT block
				//idx = line_.find("return");
				//if(idx != -1)
				//	continue;
		
				// Non conditional
				//if(basicBlocks[bb->getID+1])

				// Add children to all but last block
				int ID = bb->getID();
				if(ID < basicBlocks.size() - 1){
					std::cout << ID << "SIZE " << basicBlocks.size() << std::endl;
					bb->addChild(basicBlocks[ID+1]);
				}	

		        print_block(bb);
        }
        }
		// Add exit block to CFG
		BasicBlock* exitBlock = new BasicBlock("exit");
		basicBlocks.push_back(exitBlock);
		print_block(exitBlock);
        
        return std::vector<BasicBlock*>();
}

void liveness_analysis(const std::vector<BasicBlock*>& bbs)
{
	std::vector<std::set<std::string>> prev_ins(bbs.size(),{""});
	std::vector<std::set<std::string>> curr_ins(bbs.size(),{""});
	
	bool in_changed = true;
	while(in_changed) {
		for (int i=0; i<bbs.size(); i++) {
			if (bbs[i]->getID() != exit_id) {
				auto out = bbs[i]->out_bb();
				auto in = bbs[i]->in_bb();
				curr_ins[i] = in;
			}
		}	
		in_changed = !(std::equal(prev_ins.cbegin(), prev_ins.cend(),
					curr_ins.cbegin()));

		// FIXME(mene, ne kod :-) ) debil zaboravio da azurira prethodne in-ove
		// plus jos milion drugih glupljih gresaka, ali da
		prev_ins = curr_ins;
	}
}


int main(){
    
    parse("bb_test");
    
    std::cout << "ANALYSING EXIT" << std::endl;
    for (int i=0; i<basicBlocks.size()-1; i++) {
		std::vector<BasicBlock*> children = basicBlocks[i]->getChildren();
		bool exit = true;
		for (auto c : children) {
			std::cout << "kidID() " << c->getID() << " bb ID " << basicBlocks[i]->getID() << std::endl;
			if(c->getID() < basicBlocks[i]->getID()){
				std::cout << "HEEY kidID() " << c->getID() << " bb ID " << basicBlocks[i]->getID() << std::endl;
				exit = false;
			}
		}
		
		if (exit) basicBlocks[i]->addChild(basicBlocks.back());
	}
	std::cout << "ANALYSING EXIT FINISHED" << std::endl;
	
	liveness_analysis(basicBlocks);
	
		std::cout << "ANALYSING LIVENESS FINISHED" << std::endl;

	for (auto b : basicBlocks) {
		std::cout << "B" << b->getID() << "addr: " << b << ":\n";
		std::cout << "in[B" << b->getID() << "] = {";
		b->print_in();
		std::cout << "}; out[B" << b->getID() << "] = {";
		b->print_out();
		std::cout << "}\n\n";
		for (Line l : b->getLines()) {
			std::cout << l.line() << "\t{";
			l.print_use();
			std::cout << "}{";
			l.print_def();
			std::cout << "}\n";
		}
		
		auto children = b->getChildren();
		auto found = std::find(children.cbegin(), children.cend(), basicBlocks.back());
		if (found != children.cend()) {
			std::cout << "->Exit" << std::endl;
		}

		std::cout << std::endl;
	}
    
    return 0;
}
