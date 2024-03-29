#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <set>
#include <algorithm>
#include <fstream>

#include "line.hpp"

class BasicBlock{
    public:
        BasicBlock()
            : bb_id{bb_count++}, 
				m_use{{""}}, m_def{{""}},
				m_in{{""}},	m_out{{""}}
        {}

	BasicBlock(std::string s)
		: bb_id{-1}, 
		 	m_use{{""}}, m_def{{""}}, 
			m_in{{""}}, m_out{{""}}
        {}

    void addLine(std::string& line);
    std::vector<Line*> getLines() const;

    void addChild(BasicBlock* c);
    std::vector<BasicBlock*>& getChildren();

    void print();
    int getID();

	friend std::set<std::string> line_in(const BasicBlock& bb, Line* line);
	friend std::set<std::string> line_out(const BasicBlock& bb, Line* line);

	std::set<std::string> use();
	std::set<std::string> def();

	std::set<std::string> in_bb();
	std::set<std::string> out_bb();

	std::set<std::string> in_bb() const;
	std::set<std::string> out_bb() const;

	void print_def() const;
	void print_use() const;
	void print_in() const;
	void print_out() const;
    
    // CFG construction and manipulation
    static std::set<std::string> cfg;
    std::string toGraphNode();
    static void toGraph(BasicBlock* root);
    static void saveGraph(std::string outputFilePath);
    static void displayGraph(std::string filePath);
    
    void setVisited();
    bool isVisited();
    
    void clean_sets();

    private:
        static int bb_count;
        int bb_id;
		std::set<std::string> m_use;
		std::set<std::string> m_def;
		std::set<std::string> m_in;
		std::set<std::string> m_out;
        std::vector<Line*> _lines;
        std::vector<BasicBlock*> _children;
        bool visited; // HACK: for graphing
};

std::set<std::string> line_in(const BasicBlock& bb, std::string& line);
std::set<std::string> line_out(const BasicBlock& bb, std::string& line);
