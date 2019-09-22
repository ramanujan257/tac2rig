#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "basic_blocks.hpp"

std::vector<BasicBlock*> basicBlocks;
BasicBlock* currBB;
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

		prev_ins = curr_ins;
	}
}

void find_edges(std::set<std::set<std::string>>& live_vars,
	  std::vector<std::pair<std::string,std::string>>& edges)
{
	std::set<std::string>::iterator it;
	std::set<std::string>::iterator it2;

	for (auto ms: live_vars){
        if (ms.size () == 1){
			edges.push_back({*ms.begin(),""});
        	continue;
        }

        for (it = ms.begin (); it != ms.end (); ++it) {

        	std::string a = *it;
        	it2 = std::set<std::string>::iterator(it);
        	it2++;
        	for (; it2 != ms.end (); it2++) {
            	std::string b = *it2;
				if (a <= b) {
					auto find = std::find(edges.cbegin(), edges.cend(), 
							std::pair<std::string,std::string>({a,b}));
					if (find == edges.cend()) {
						edges.push_back({a,b});
					}
				} else {
					auto find = std::find(edges.cbegin(), edges.cend(), 
							std::pair<std::string,std::string>({b,a}));
					if (find == edges.cend()) {
						edges.push_back({b,a});
					}
				}
            }
        }
    }
}

void draw_graph(std::set<std::set<std::string>>& live_vars, std::string outputFilePath){

    std::ofstream outfile;
    outfile.open(outputFilePath);

	// construct edge pairs
    std::vector<std::pair<std::string,std::string>> edges;
	find_edges(live_vars, edges);

	// "draw" graph (dot format)
    outfile << "graph {\n";
    outfile << "graph[dpi=300]\ngraph[concentrate=true]\n";

	for (auto e : edges) {
		if (e.second != "") {
			outfile << e.first << "--" << e.second << ";\n";
		} else {
			outfile << e.first << ";\n";
		}
	}
    
    outfile << "}";
    
    outfile.close();
    
	// run commands for graph construction
    std::stringstream command1;
    std::stringstream command2;
    std::string graphName = outputFilePath.substr(0,outputFilePath.rfind("."));
    command1 << "dot -Tpng " << outputFilePath << " -o " << graphName << "_graph.png";
    command2 << "gwenview " << graphName << "_graph.png";
    
    std::cout << command1.str() << " C1\n";
    std::cout << command2.str() << " C2\n";
    
    std::system(command1.str().c_str());
    std::system(command2.str().c_str());
}

// cleans all empty elements ("") from a set
void clean_sets(const std::vector<BasicBlock*>& bbs)
{
	for (auto bb : bbs) {
		bb->clean_sets();
	}
}

void construct_live_vars(std::set<std::set<std::string>>& live_vars,
					std::vector<BasicBlock*>& bbs)
{
	for (auto bb : bbs) {
		for (auto l : bb->getLines()) {
			auto set = l->in();
			live_vars.insert(set);
			if (l == bb->getLines().back()) {
				auto set = l->out();
				live_vars.insert(set);
			}
		}
	}
}

int main(){
    
    std::string filename;
    std::cin >> filename;
    parse(filename);
	
	liveness_analysis(basicBlocks);

	std::set<std::set<std::string>> live_vars{};
	clean_sets(basicBlocks);
	construct_live_vars(live_vars, basicBlocks);

	// print basic blocks with their in and out sets
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
		
	BasicBlock::toGraph(*basicBlocks.begin());
    BasicBlock::saveGraph(filename+"CFG.dot");

	std::string fOut = filename + "RIG.dot";
	draw_graph(live_vars, fOut);

    return 0;
}
