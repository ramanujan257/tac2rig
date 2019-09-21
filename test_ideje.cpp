#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "basic_blocks.hpp"

std::vector<BasicBlock*> basicBlocks;
BasicBlock* currBB;// = new BasicBlock();
int exit_id;

std::vector<int> leaders{1};
std::map<int, std::vector<BasicBlock*>> jumpers;

BasicBlock* getCurrentBB();
void setCurrentBB(BasicBlock* bb);
BasicBlock* bbLookup(int address);
void print_block(BasicBlock* bb);

void test_fun(BasicBlock* bb);

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
            if(std::stoi(l->line()) == address)
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

void parse(const std::string& fname){
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
        
    for(int i = 0; i < leaders.size() - 1; i++){
        int from = leaders[i];
        int to = leaders[i+1];
        BasicBlock* newBB = new BasicBlock();
        for(int j = from; j < to; j++){
            newBB->addLine(buffer[j-1]);
        }
        basicBlocks.push_back(newBB);
    }
        
    // Add last line to last block
    basicBlocks.back()->addLine(buffer.back());

	// Add exit block 
	BasicBlock* exitBB = new BasicBlock("exit");
	basicBlocks.push_back(exitBB);

        
	// Create CFG
    for(auto bb : basicBlocks){
        if(bb->getID() != -1){
            std::string line_ = bb->getLines().back()->line();
            int idx = line_.rfind("goto");
            if(idx != -1){
                // Jump statement
                int jmp_addr = std::stoi(line_.substr(line_.rfind("(") + 1));
                BasicBlock* gotoBlock = bbLookup(jmp_addr);
                bb->addChild(gotoBlock);
                    
                if(bb->getID() < basicBlocks.size() - 1)
                    bb->addChild(basicBlocks[bb->getID()+1]);
                else
                    bb->addChild(exitBB);
            } else {
                // Falltrough
                if(bb->getID() < basicBlocks.size() - 1)
                    bb->addChild(basicBlocks[bb->getID()+1]);
                else
                    bb->addChild(exitBB);
            }
        }
    }
    
    
}

void liveness_analysis(const std::vector<BasicBlock*>& bbs)
{
	std::vector<std::set<std::string>> prev_ins(bbs.size(),{""});
	std::vector<std::set<std::string>> curr_ins(bbs.size(),{""});
	
	bool in_changed = true;
	while(in_changed) {
		for (int i=0; i<bbs.size(); i++) {
			if (bbs[i]->getID() != -1) {
				auto out = bbs[i]->out_bb();

				// iteriraj unazad kroz blok i odredi in i out za svaku liniju
				std::vector<Line*> lines = bbs[i]->getLines();
                const BasicBlock& bb = *bbs[i];
				for (auto l = lines.rbegin(); l != lines.rend(); l++) {
                    
                    std::set<std::string> l_o = line_out(bb, *l);
                	(*l)->set_out(l_o);
					(*l)->set_in(line_in(bb, *l));
				}
                
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
    
    std::string filename;
    std::cin >> filename;
    parse(filename);
	
	liveness_analysis(basicBlocks);

	for (auto b : basicBlocks) {
		std::cout << "in[B" << b->getID() << "] = {";
		b->print_in();
		std::cout << "}; out[B" << b->getID() << "] = {";
		b->print_out();
		std::cout << "}\n\n";
		for (Line* l : b->getLines()) {
			std::cout << l->line() << "\tin={";
			l->print_in();
			std::cout << "}, out={";
			l->print_out();
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
