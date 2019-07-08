#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>

// Vector of lines
// lead of the bb
// end of the bb
// children
// parents?
class BasicBlock{
    public:
        BasicBlock()
        {}

        BasicBlock(std::vector<std::string> lines)
            :_lines{lines}, bb_id{bb_no++}
        {}

        void showLines(){
            std::cout << "====Basic block " << bb_id << "====" <<std::endl;
            for(auto l : _lines)
                std::cout << l << std::endl;
            std::cout << "=====================" << std::endl;
        }

        std::vector<std::string> getLines(){
            return _lines;
        }

        void addLine(std::string& l){
            _lines.push_back(l);
        }

    private:
        static int bb_no;
        int bb_id;
        std::vector<std::string> _lines;
        std::vector<BasicBlock*> _children;
};

int BasicBlock::bb_no = 0;

// 
std::vector<int> bb_idxs{1};

std::string slurp(std::ifstream& in){
    std::stringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}

// Parse tac from given file and construct basic blocks
std::vector<BasicBlock*> parse(const std::string& fname){
	std::ifstream	input;
	input.open(fname.c_str());
	std::string line;
        std::vector<std::string> buffer;

        int line_no = 0;
        if(input.is_open()){
            while(std::getline(input, line)){
                std::cout << "tellg() = "  << input.tellg() << std::endl;
                std::cout << "peek() = " << input.peek() << std::endl;
                buffer.push_back(line);
                line_no++;
                std::cout << line << std::endl;
                int idx = line.find("goto");
                if(idx != -1){
                    int i1 = line.find("(",idx) + 1;
                    bb_idxs.push_back(std::stoi(line.substr(i1)));
                    std::cout << "pb l+1" << std::endl;
                    if(input.peek() != -1)
                        bb_idxs.push_back(line_no+1);
                    else
                        bb_idxs.push_back(line_no);
                }
            }
	input.close();

        }


        std::sort(bb_idxs.begin(), bb_idxs.end());

        std::cout << "idxs = [";
        for(int i : bb_idxs)
            std::cout << i << ",";
        std::cout << "]" << std::endl;

        std::vector<BasicBlock*> blocks;
        std::transform(bb_idxs.begin(), bb_idxs.end() - 1, bb_idxs.begin() + 1,
                    std::back_inserter(blocks), [&](int i, int n) {  
                    std::cout << "i" << i << std::endl;
                    std::cout << "n" << n << std::endl;
                    return new BasicBlock(std::vector<std::string>(buffer.begin() + i - 1, buffer.begin() + n - 1));});

        for(auto b : blocks){
            std::string lastLine = b->getLines().back();
            int idx;
            if((idx = lastLine.rfind("goto")) != -1 ) {
                std::cout << "lastline" << lastLine << std::endl;
                int i1 = lastLine.find("(", idx) + 1;
                int jmpAddr = std::stoi(lastLine.substr(i1));
                // search for appropriate BB
                // TODO: explore more efficent ways to implement this
                //
                std::cout << "jmpAddr" << jmpAddr << std::endl;
            } // add children 
        }

        return blocks;

}

int main(){
        //std::cout << "Type in the name of the file:" << std::endl;
	std::string fname;
        //std::cin >> fname;
        std::cout << "npos:" << std::string::npos << std::endl;
        
        std::vector<BasicBlock*> blocks = parse("tac_example.txt");
        for(auto b : blocks)
            b->showLines();
        
	return 0;
}

