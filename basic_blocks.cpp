#include "basic_blocks.hpp"

int BasicBlock::bb_count = 0;

std::set<std::string> BasicBlock::cfg;

void BasicBlock::addLine(std::string& line){
		Line* l = new Line(line);
        _lines.push_back(l);
}

std::vector<Line*> BasicBlock::getLines() const{
    return _lines;
}

void BasicBlock::addChild(BasicBlock* c){
    if(!(std::find(_children.cbegin(), _children.cend(), c) != _children.end()))
        _children.push_back(c);
}

std::vector<BasicBlock*>& BasicBlock::getChildren(){
    return _children;
}

void BasicBlock::print(){
    std::cout << "BB" << bb_id << std::endl;
    for(auto l: _lines)
        std::cout << l->line() << std::endl;
}

int BasicBlock::getID(){
    return bb_id;
}

std::set<std::string> line_in(const BasicBlock& bb, Line* line)
{
	std::set<std::string> in_set({""});
	std::set<std::string> use_set = line->use();	
	std::set<std::string> def_set = line->def();
	std::set<std::string> out_set = line->out();
	std::set<std::string> diff_set({""});

	// provera da li je linija u bloku (sto je sigurno, sigurno je)
	auto found = std::find(bb._lines.cbegin(), bb._lines.cend(), line);
	if (found == bb._lines.end()) {
		std::cerr << "Line is not in this block" << std::endl;
		return std::set<std::string>(); 
	} 

	// diff[s] = out[s]\def[s]
	std::set_difference(out_set.begin(), out_set.end(),
				   def_set.begin(), def_set.end(),
				   std::inserter(diff_set, diff_set.end())
				   );

	// in[s] = use[s] U diff[s]
	std::set_union(use_set.begin(), use_set.end(),
			  diff_set.begin(), diff_set.end(),
			  std::inserter(in_set, in_set.end())
			);

	return std::set<std::string>(in_set);
}

std::set<std::string> line_out(const BasicBlock& bb, Line* line)
{
	std::set<std::string> out_set;
	
	// proveri da li je linija u bloku
	auto found = std::find(bb._lines.cbegin(), bb._lines.cend(), line);
	if (found == bb._lines.cend()) {
		std::cerr << "Line is not in this block" << std::endl;
		return std::set<std::string>();
	} else if (found == bb._lines.cend()-1) {
		// out[s] = out[B], s - poslednja linija u B
		auto set = bb.out_bb();
		std::set_union(out_set.begin(), out_set.end(),
					set.begin(), set.end(),
					std::inserter(out_set, out_set.end()));
	} else {
		// out[s] = in[s'], s' - naredna linija u bloku
		auto set = (*(found+1))->in();
		std::set_union(out_set.begin(), out_set.end(),
					set.begin(), set.end(),
					std::inserter(out_set, out_set.end()));
	}

	return std::set<std::string>(out_set);
}

std::set<std::string> BasicBlock::use() 
{
	std::vector<Line*> lines = getLines();
	if (lines.size() == 0) return std::set<std::string>();
	
	m_use = lines[0]->use();
	std::set<std::string> defs = lines[0]->def();
	std::set<std::string> diff{};

	// use[B] = Union{i=1;N} (use[s_i]\Union{k=1;i-1}(def[s_k]))
	for (int i=1; i<lines.size(); i++) {
		std::set<std::string> use = lines[i]->use();

		/* use_i = use[s_i] \ Union{k=1;i-1}(def[s_k]) */
		std::set_difference(use.begin(), use.end(),
							defs.begin(), defs.end(),
							std::inserter(diff, diff.end()));
		
		std::set_union(m_use.begin(), m_use.end(),
					   diff.begin(), diff.end(),
					   std::inserter(m_use, m_use.end()));
		
		std::set<std::string> def = lines[i]->def();
		std::set_union(defs.begin(), defs.end(),
					   def.begin(), def.end(),
					   std::inserter(defs, defs.end()));
	}

	return std::set<std::string>(m_use);
}

std::set<std::string> BasicBlock::def()
{
	if (bb_id == -1) return std::set<std::string>();

	// def[B] = Union{i=1;i=num_lines}def[s_i], s_i = i-ta linija
	std::vector<Line*> lines = getLines();
	for (auto l : lines){
		std::set<std::string> line_def(l->def());
		std::set_union(m_def.begin(), m_def.end(),
					   line_def.begin(), line_def.end(),
					   std::inserter(m_def, m_def.end()));
	}

	return std::set<std::string>(m_def);
}

void BasicBlock::print_def() const
{
	std::copy(m_def.cbegin(), m_def.cend(),
			std::ostream_iterator<std::string>(std::cout, " "));
}

void BasicBlock::print_use() const
{
	std::copy(m_use.cbegin(), m_use.cend(),
			std::ostream_iterator<std::string>(std::cout, " "));
}

std::set<std::string> BasicBlock::in_bb() 
{
	// in[B] = use[B] U (out[B]\def[B])
	std::set<std::string> diff{""};
	std::set<std::string> def_set(def());
	std::set<std::string> out = m_out;
	
	std::set_difference(out.cbegin(), out.cend(),
						def_set.cbegin(), def_set.cend(),
						std::inserter(diff, diff.end()));

	std::set<std::string> use_set(use());
	std::set_union(use_set.cbegin(), use_set.cend(),
				   diff.cbegin(), diff.cend(),
				   std::inserter(m_in, m_in.end()));

	return std::set<std::string>(m_in);
}

std::string has_return(const std::string& l)
{
	int idx = l.rfind("return");
	if (idx != -1) {
		auto var = l.substr(l.rfind(" ")+1);
		auto found = std::find_if(var.cbegin(), var.cend(), 
								[] (char c) { return isalpha(c); });

		return (found == var.cend()) ? "" : var;
	}

	return "";
}

std::set<std::string> BasicBlock::out_bb()
{
	// out[B] = Union{S-sledbenik od B}(in[S])
	for (auto c : this->getChildren()){
		const auto tmp_in = c->in_bb();
		std::set_union(m_out.begin(), m_out.end(),
					   tmp_in.cbegin(), tmp_in.cend(),
					   std::inserter(m_out, m_out.end()));
	}

	// ostavljamo zivu promenljivu koja je povratna vrednost
	auto lines = _lines;
	auto return_ = has_return(lines.back()->line());
	if (return_ != "") {
		m_out.insert(return_);
	}

	return std::set<std::string>(m_out);
}

void BasicBlock::print_in() const
{
	std::copy(m_in.cbegin(), m_in.cend(),
			std::ostream_iterator<std::string>(std::cout, " "));
}

void BasicBlock::print_out() const
{
	std::copy(m_out.cbegin(), m_out.cend(),
			std::ostream_iterator<std::string>(std::cout, " "));
}

std::set<std::string> BasicBlock::in_bb() const 
{
	return m_in;
}

std::set<std::string> BasicBlock::out_bb() const
{
	return m_out;
}

void BasicBlock::clean_sets(){
    for (auto l : _lines)
        l->clean();
}

void BasicBlock::setVisited(){
    visited = true;
}

bool BasicBlock::isVisited(){
    return visited;
}

// Graphing
std::string BasicBlock::toGraphNode(){
    std::string result = "";
    if(bb_id != -1){
        result += "BB" + std::to_string(bb_id) + " [label = \"" ;
        for(auto line : _lines){
            result += line->line() + "\\l ";
        }
        result += "\"]\n";
    } else {
        result += "Exit [label = \"Exit\"]";
    }
        
    return result;
}

void BasicBlock::toGraph(BasicBlock* root){
        

        if(!root->isVisited()){
            cfg.insert(root->toGraphNode());
            root->setVisited();
            auto children = root->getChildren();
            for(auto c : children){
                if(c->getID() != -1){
                    cfg.insert(c->toGraphNode());
                    cfg.insert("BB" + std::string(std::to_string(root->getID())) + " -> BB" + std::to_string(c->getID()) + "\n");
                    toGraph(c);
                } else {
                    cfg.insert(c->toGraphNode());
                    cfg.insert("BB" + std::string(std::to_string(root->getID())) + " -> Exit" + "\n");
                }
            }
        }
    }
    
void BasicBlock::saveGraph(std::string outputFilePath){
    
    std::ofstream outfile;
    outfile.open(outputFilePath);
    
    outfile << "digraph G {\nnode [shape=box]\n";
    
    for(auto el : cfg)
        outfile << el << std::endl;
    
    outfile << "}";
    outfile.close();
    
}

void BasicBlock::displayGraph(std::string filePath){
    
    std::stringstream command1;
    std::stringstream command2;
    
    command1 << "dot -Tpng " << filePath << " -o " << filePath << "_graph.png &";
    command2 << "xdg-open " << filePath << "_graph.png &";
    
    std::system(command1.str().c_str());
    std::system(command2.str().c_str());
    
}
