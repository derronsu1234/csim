// SPDX-License-Identifier: LGPL-2.1-or-later
#include "parse.h"  


namespace circuit {  

Node::Node(const std::string& n) : name(n) {}  

Node::~Node() {  
    for (auto child : children) {  
        delete child;  
    }  
}  

Subckt::~Subckt() {  
    for (auto child : children) {  
        delete child;  
    }  
}  

}

CircuitParser::CircuitParser() : top(new circuit::Node("top")), current_subckt(nullptr) {}  

CircuitParser::~CircuitParser() {  
    delete top;  
    for (auto& pair : subckt_map) {  
        delete pair.second;  
    }  
}  


bool CircuitParser::fun1(const std::string& line, std::string& component, std::vector<std::string>& node, std::string& type, std::string& data1)
{
    std::string value = "";
    std::istringstream iss(line);
    iss >> component;
    component = toLowerCase(component);
    std::string node1, node2, node3, node4;
    if (component.length() >= 3 && component.substr(0, 3) == "spk") {  // 扬声器：spk前缀
        iss >> node1 >> node2;
        type = "speaker";
        node.push_back(node1);
        node.push_back(node2);
    }
    else if (component[0] == 'r') {
        iss >> node1 >> node2;
        type = "resistor";
        node.push_back(node1);
        node.push_back(node2);
    }
    else if (component[0] == 'c') {
        iss >> node1 >> node2;
        type = "capacitor";
        node.push_back(node1);
        node.push_back(node2);
    }
    else if (component[0] == 'l') {
        iss >> node1 >> node2;
        type = "inductor";
        node.push_back(node1);
        node.push_back(node2);
    }
    else if (component[0] == 'v' || component[0] == 'i') {
        iss >> node1 >> node2;
        if (component[0] == 'v') {
            type = "voltage";
            node.push_back(node1);
            node.push_back(node2);
        }
        else { // 'i': 电流源
            type = "current";
            /* 库 IDC 模型的"正"端（电流注入端）是 terminal 1，
               为符合 SPICE 习惯（I n+ n- 时 n+ 为正），翻转节点顺序 */
            node.push_back(node2);
            node.push_back(node1);
        }
    }
    else if (component[0] == 's') {
        type = "sparameter";
        while (iss >> value) {
            node.push_back(value);
        }

        if (!node.empty()) {
            data1 = node.back();
            node.pop_back();
            node.pop_back();
        }

    }
    else if (component[0] == 'g') { // Gxxx n+ n-  in+ in- transconductance 
        auto line1 = toLowerCase(line);
        if (line1.find("vcr") != std::string::npos) {
            type = "vcr";  // 设置类型为vcr
            iss >> node1 >> node2;  
            node.push_back(node1);
            node.push_back(node2);
            iss >> value;
            iss >> value;
        }
        else {
            type = "vccs";
            /* VCCS: n+ n- in+ in- gain，四个节点全部记录，供 main 接线 */
            iss >> node1 >> node2 >> node3 >> node4;
            node.push_back(node1);
            node.push_back(node2);
            node.push_back(node3);
            node.push_back(node4);
        }
    }
    else if (component[0] == 'e') { // Exxx n+ n- in+ in- gain 
        type = "vcvs";
        /* VCVS: n+ n- in+ in- gain，四个节点全部记录，供 main 接线 */
        iss >> node1 >> node2 >> node3 >> node4;
        node.push_back(node1);
        node.push_back(node2);
        node.push_back(node3);
        node.push_back(node4);
    
    }
    else if (component[0] == 'f') { // Fxxx n+ n-  Vin gain
        type = "cccs";
        /* CCCS: 输出 n+ n-，控制电流源名 Vin 存入 data1（供 main 接线 P/Q 端） */
        iss >> node1 >> node2 >> node3;
        node.push_back(node1);
        node.push_back(node2);
        data1 = node3;
    }
    else if (component[0] == 'h') { // Hxxx n+ n-  Vin transresistance 
        type = "ccvs";
        /* CCVS: 输出 n+ n-，控制电流源名 Vin 存入 data1（供 main 接线 P/Q 端） */
        iss >> node1 >> node2 >> node3;
        node.push_back(node1);
        node.push_back(node2);
        data1 = node3;
    }
    else if (component[0] == 'x') {
        type = "subckt";
        while (iss >> value) {
            node.push_back(value);
        }

        if (!node.empty()) {
            data1 = node.back();
            node.pop_back();
        }
    }
    else  if (component[0] == 'k') {
        iss >> node1 >> node2;
        type = "mutual_inductance";
        node.push_back(node1);
        node.push_back(node2);
    }
    else  if (component[0] == 'd') {
        iss >> node1 >> node2 >> data1;
        type = "diode";
        node.push_back(node1);
        node.push_back(node2);
    }
    else  if (component[0] == 'p') {
        iss >> node1 >> node2;
        type = "Pelement";
        node.push_back(node1);
        node.push_back(node2);
    }
    else  if (component[0] == 'm') {
        iss >> node1 >> node2>>node3 >> node4;
        type = "mosfet";
        node.push_back(node1);
        node.push_back(node2);
        node.push_back(node3);
        node.push_back(node4);
    }
    else if (component[0] == 'w') {
        type = "w_element";
        while (iss >> value) {
            if (value.find('=') != std::string::npos) {
                data1 = data1 + " " + value;
            }
            else {
                node.push_back(value);
            }
        }

}
else{
    return false;
}
    while (iss >> value) {
        data1 = data1 + " " + value;
    }
    data1 = toLowerCase(data1);   // 统一小写（值 / 模型名 / 控制源名）
    node = toLowerCase(node);
    return true;
}


void CircuitParser::parseFile(const std::string& filename) { 
    // 提取主网表路径（供 .include 相对路径解析使用）；m_savename 去除扩展名
    m_savename = (filename.size() >= 3) ? filename.substr(0, filename.size() - 3) : filename;
    m_path = check_path(filename);
    std::function<void(const std::string& filename, std::vector<std::string>& res)> processor;
    processor = [&processor](const std::string& filename, std::vector<std::string>& res) ->void {
        // 当前文件的目录，用于解析相对 include 路径（支持嵌套 include）
        auto cur_path = check_path(filename);
        std::ifstream inFile;
        inFile.open(filename, std::ios::in);
        if (inFile.is_open()) {
            std::string line;
            std::getline(inFile, line);
            do {
                if (toLowerCase(line) == ".end") break; // .end 大小写不敏感
                if (line.empty() || line[0] == '*') continue;

                if (line[0] == '+') //+号开头的，拼接到上面的一行
                {
                    std::string subStr = line.substr(1, line.length() - 1);//去掉+号
                    res.back() += " " + subStr;
                }
                else if (toLowerCase(line.substr(0, 8)) == ".include") {

                    // 提取 ".include" 之后的部分并去除前导空白
                    std::string includePart = line.substr(8);
                    size_t partStart = includePart.find_first_not_of(" \t");
                    if (partStart == std::string::npos) continue; // 无有效内容

                    // 查找双引号位置
                    if (includePart[partStart] != '"') continue; // 首字符非引号
                    size_t firstQuote = partStart;
                    size_t secondQuote = includePart.find('"', firstQuote + 1);

                    if (secondQuote == std::string::npos) continue; // 引号未闭合

                    // 提取引号间内容
                    std::string filename2 = includePart.substr(
                        firstQuote + 1,
                        secondQuote - firstQuote - 1
                    );
                    // 相对路径基于当前文件所在目录解析
                    auto path2 = check_path(filename2);
                    if (!path2.first && !cur_path.second.empty()) {
                        filename2 = cur_path.second + "/" + filename2;
                    }
                    processor(filename2, res);
                    std::cout << "Found include file: " << filename2 << "\n";
                }
                else {

                    res.push_back(line);
                }
            } while (std::getline(inFile, line));
        }
        inFile.close();
    };
    
    std::vector<std::string> lines;
    processor(filename, lines);
    // 大小写统一在 parseLine / fun1 内部处理，不再做平台相关的整行小写化
    for (auto& line : lines) {
        parseLine(line);
    }

}  

void CircuitParser::parseLine(const std::string& line) { 
    if (line.empty() ) return; // 空行   
    std::istringstream iss(line);  
    std::string token;
    std::vector<std::string> data;
    if (!(iss >> token)) return; // 空行 
    token = toLowerCase(token);   // 统一小写，避免平台相关的 #ifdef
    if (token == ".subckt") {  
        // 解析子电路定义（子电路名与外部节点统一小写，与 X 实例引用保持一致）  
        std::string subckt_name;  
        iss >> subckt_name;  
        
        circuit::Subckt* subckt = new circuit::Subckt();  
        subckt->name = toLowerCase(subckt_name);  
        
        // 读取外部节点  
        std::string node;  
        while (iss >> node) {  
            subckt->external_nodes.push_back(toLowerCase(node));
        }  
        
        subckt_map[subckt->name] = subckt;  
        current_subckt = subckt;  
    }  
    else if (token == ".ends") {  
        current_subckt = nullptr;  
    }  
    else {  
        if (token[0]=='.')
        {
            std::string keyword_name = token.substr(1);
            while (iss >> token) {
                data.push_back(token);
            }
            m_keyword.insert(std::make_pair(keyword_name, data));
        }
        else if (token[0] == 'a') {
            std::string keyword_name = token;
            while (iss >> token) {
                data.push_back(token);
            }
            m_keyword.insert(std::make_pair(keyword_name, data));
        }
        else {
            std::string temp_component;
            std::vector<std::string> temp_node;
            std::string temp_type;
            std::string temp_data1;
            if(!fun1(line, temp_component, temp_node, temp_type, temp_data1))
                return ;
            circuit::Node* node = new circuit::Node(temp_component);
            node->nodes = temp_node;
            node->type = temp_type;
            node->value = temp_data1;
            // 如果是子电路实例，记录子电路名称  
            if (temp_component[0] == 'x') {
                node->subckt_name = node->value;
            }

            // 添加到相应的父节点  
            if (current_subckt) {
                current_subckt->children.push_back(node);
            }
            else {
                top->children.push_back(node);
            }
        }
    }  
}  

void CircuitParser::expandSubckt(circuit::Node* node, const std::string& suffix) {  
    if (node->name[0] != 'x') return;  

    circuit::Subckt* subckt = subckt_map[node->subckt_name];  
    if (!subckt) return;  

    // 创建节点映射  
    std::map<std::string, std::string> node_mapping;  
    for (size_t i = 0; i < subckt->external_nodes.size(); i++) {  
        node_mapping[subckt->external_nodes[i]] = node->nodes[i];  
    }  

    // 复制子电路中的元件并添加后缀  
    for (circuit::Node* child : subckt->children) {  
        circuit::Node* new_node = new circuit::Node(child->name + suffix);  
        // 设置子电路上下文信息
        new_node->is_from_subckt = true;
        new_node->subckt_suffix = suffix;
        new_node->original_subckt_name = node->subckt_name;
        new_node->node_mapping = node_mapping;
        // 映射节点  
        for (const std::string& n : child->nodes) {  
            auto it = node_mapping.find(n);  
            if (it != node_mapping.end() ) {
                new_node->nodes.push_back(it->second);
            }
            else {  

                if (n == "0") {
                    new_node->nodes.push_back(n);
                }
                else{
                new_node->nodes.push_back(n + suffix);
                }
            }  
        }  
        new_node->type = child->type;
        new_node->value = child->value; 
        
        if (child->name[0] == 'x') {
            new_node->subckt_name = child->subckt_name;
        }
        node->children.push_back(new_node);  

        // 递归处理嵌套的子电路  
        if (child->name[0] == 'x') {  
            expandSubckt(new_node, suffix + "_" + child->name);  
        }  
    }  
}  

void CircuitParser::expandAllSubckts() {  
    for (circuit::Node* node : top->children) {  
        if (node->name[0] == 'x') {  
            expandSubckt(node, "_" + node->name);  
        }  
    }  
}  

void CircuitParser::printCircuit(circuit::Node* node, int level) {  
    if (!node) node = top;  

    std::string indent(level * 2, ' ');  
    std::cout << indent << node->name;  
    if (!node->nodes.empty()) {  
        std::cout << " Nodes: ";  
        for (const auto& n : node->nodes) {  
            std::cout << n << " ";  
        }  
        std::cout << " Type: " << node->type << " ";
        std::cout << " Value: " << node->value;
    }  
 
    std::cout << "\n";

    for (circuit::Node* child : node->children) {  
        printCircuit(child, level + 1);  
    }  
}

void CircuitParser::genEdges(circuit::Node* node, int level) {
    if (!node) {
        node = top;
    }
    else {
        if (node->type != "subckt") {
            //m_edges.push_back({ node->nodes, node->name, node->type, node->value });
			circuit::Edge edge;
            edge.nodes = node->nodes;
            edge.component_name = node->name;
            edge.type = node->type;
            edge.value = node->value;
            
            // 从Node复制子电路信息到Edge
            edge.is_in_subckt = node->is_from_subckt;
            edge.subckt_suffix = node->subckt_suffix;
            edge.subckt_name = node->original_subckt_name;
            edge.node_mapping = node->node_mapping;
            
            m_edges.push_back(edge);
        }
    }
	
	
    for (circuit::Node* child : node->children) {
        genEdges(child, level + 1);
    }
}

void CircuitParser::printEdges()
{
    int index = 1;
    for (auto& ii : m_edges) {
        std::cout << index << ":";
        std::cout << "name:" <<ii.component_name << ";";
        std::cout << "node:" ;
        for (auto& node : ii.nodes) {
            std::cout  << node << ",";
        }
        std::cout << "type:" << ii.type << ";";
        std::cout << "value:" << ii.value << ";";
        std::cout << "\n";
        index++;
    }
}

void CircuitParser::process_()
{
    for (auto it = m_keyword.begin(); it != m_keyword.end(); ) {
        auto vec1 = it->second;

        if (it->first == "ac" || it->first == "tran" || it->first == "op" || it->first == "stateye" || it->first == "dc" ||
            it->first == "eyediagram" || it->first == "eye") {
            std::string temp = "";
            for (size_t i = 0; i < vec1.size(); i++) {
                if (vec1[i] != ",") {
                    temp = temp + vec1[i] + " ";
                }
            }
    temp = toLowerCase(temp);   // 统一小写（如 .ac LIN -> lin）
    m_analysis.emplace(it->first, temp); 
            it = m_keyword.erase(it);
        } 
        else {
            ++it;
        }
    }
  
    for (auto it = m_keyword.begin(); it != m_keyword.end(); ) {
        
        if (it->first == "model") {
            auto vec1 = it->second;
            std::string temp;
            for (auto jj : vec1)
            {
                temp += jj;
                temp += " ";
            }
            //auto temp2=std::make_pair(vec1[0], temp);
            //m_models.insert(std::make_pair(vec1[0], temp));
            std::string name = toLowerCase(vec1[0]);   // 模型名统一小写，与 D 元件引用一致
            m_models.emplace(name, temp);
              it = m_keyword.erase(it);
        }
        else {
            ++it;
        }
    }

    for (auto it = m_keyword.begin(); it != m_keyword.end();) {
        if (it->first == "probe") {            
            auto vec1 = it->second;
            for (size_t i = 0; i < vec1.size(); i++) {
                m_probe_names.push_back(toLowerCase(vec1[i]));   // 探针名统一小写（v()/i() 判断依赖）
            }
            it = m_keyword.erase(it);

        }
        else {
            ++it;
        }
    }


}

