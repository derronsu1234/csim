#ifndef PUBLIC_H  
#define PUBLIC_H  
#include <string>  
#include <vector> 
#include <map>

namespace circuit {  

    struct Edge {
        std::vector<std::string> nodes;
        std::string component_name;
        std::string type;
        std::string value;

		
        bool is_in_subckt = false;
        std::string subckt_suffix = ""; 
        std::string subckt_name = "";
        std::map<std::string, std::string> node_mapping;
		
		Edge(std::vector<std::string> node_list, 
        const std::string& comp_name, 
        const std::string& t, 
        const std::string& val)
        : nodes(node_list), component_name(comp_name), type(t), value(val) {}
		
    // 显式声明所有默认特殊成员函数
        Edge() = default;
        Edge(const Edge&) = default;
        Edge& operator=(const Edge&) = default;
        Edge(Edge&&) = default;
        Edge& operator=(Edge&&) = default;
        ~Edge() = default;

        bool operator==(const Edge& other) const {
            return (component_name == other.component_name &&
                nodes == other.nodes &&
                type == other.type &&
                value == other.value &&
                is_in_subckt == other.is_in_subckt &&
                subckt_suffix == other.subckt_suffix);
        }

    };
}

#endif // PUBLIC_H