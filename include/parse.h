// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef PARSER_H  
#define PARSER_H  
#include <map>  
#include <unordered_map>
#include "helper.h"
#include <vector>
#include <string>  
#include "boost/regex.hpp"
#include "public.h"


namespace circuit {
class Node {  
public:  
    std::string name;
    std::vector<std::string> nodes;
    std::string value;
    std::vector<Node*> children;
    std::string subckt_name;
    std::string type;
	

    bool is_from_subckt = false;
    std::string subckt_suffix = "";
    std::string original_subckt_name = "";
    std::map<std::string, std::string> node_mapping;
	
    explicit Node(const std::string& n);  
    ~Node();  


    Node(const Node&) = delete;  
    Node& operator=(const Node&) = delete;  
};  


class Subckt {  
public:  
    std::string name;
    std::vector<std::string> external_nodes;
    std::vector<Node*> children;

    Subckt() = default;  
    ~Subckt();  


    Subckt(const Subckt&) = delete;  
    Subckt& operator=(const Subckt&) = delete;  
};  

} // namespace circuit

class CircuitParser {  
private:  
    circuit::Node* top;
    circuit::Subckt* current_subckt;   
   
public:
    std::string m_savename;
    std::map<std::string, circuit::Subckt*> subckt_map;
    std::unordered_multimap<std::string, std::vector<std::string>> m_keyword;
    std::vector<circuit::Edge> m_edges;
    std::vector<std::string> m_probe_names;
    std::unordered_multimap<std::string, std::string> m_analysis;
    std::map<std::string, std::string> m_models;
    std::pair<bool, std::string> m_path;
public:  
    CircuitParser();  
    ~CircuitParser();  


    CircuitParser(const CircuitParser&) = delete;  
    CircuitParser& operator=(const CircuitParser&) = delete;  

    void parseFile(const std::string& filename);  
    void parseLine(const std::string& line);  
    void expandSubckt(circuit::Node* node, const std::string& suffix);  
    void expandAllSubckts();  
    void printCircuit(circuit::Node* node = nullptr, int level = 0);  
    void genEdges(circuit::Node* node = nullptr, int level = 0);
    void printEdges();
  
    bool fun1(const std::string& line, std::string& component, std::vector<std::string>& node, std::string& type, std::string& data1);
    void process_();

    circuit::Node* get_top() {
        return top;
    }
};  

#endif // PARSER_H