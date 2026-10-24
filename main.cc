
#include <stdio.h>
#include "csim/utils/errors.h"
#include "csim/internal/Analyzers.h"
#include "csim/internal/Circuit.h"
#include "csim/internal/Dataset.h"
#include "csim/internal/Netlist.h"
#include "csim/internal/ModelLoader.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iomanip>
#include "parse.h"  
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include "modelmanager.h"

// ==================== Helper Functions ====================

/**
 * 将输入的字符串转换为大写形式
 * @param str 输入的字符串常量引用
 * @return 返回转换后的大写字符串
 */
std::string toUpperCase(const std::string& str) {
    std::string result = str; // 创建输入字符串的副本
    // 使用transform算法和toupper函数将字符串中的所有字符转换为大写
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result; // 返回转换后的大写字符串
}

/**
 * 将字符串解析为 double；去除首尾空白后若不能转换为数字则返回默认值
 */
double parseNumericValue(const std::string& s, double default_value) {
    std::string t = s;
    size_t first = t.find_first_not_of(" \t");
    size_t last = t.find_last_not_of(" \t");
    if (first == std::string::npos) return default_value;
    t = t.substr(first, last - first + 1);
    if (canConvertToDouble(t)) return std::stod(t);
    return default_value;
}

struct ComponentConnection {
    std::string component_name;
    int pin_index;
};

enum ProbeType { VOLTAGE_PROBE, CURRENT_PROBE };

struct ProbeInfo {
    std::string component_name;
    int pin_index;
    ProbeType type;
};

// ==================== Component Setup ====================
void addComponentsToCircuit(csim::Circuit* circuit, 
                            std::vector<circuit::Edge>& edges,
                            const std::map<std::string, std::string>& models,
                            ModelManager& modelMgr,
                            std::vector<std::pair<std::string, std::string>>& ctrl_sources) {
    for (auto& edge : edges) {
        edge.component_name = toUpperCase(edge.component_name);
        int ret = 0;
        
        if (edge.type == "resistor") {
            ret = circuit->netlist()->addComponent(edge.component_name.c_str(), modelMgr.getResistor());
            ret = circuit->netlist()->configComponent(edge.component_name.c_str(), "R", 
                csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(std::stod(edge.value)));
        }
        else if (edge.type == "voltage") {
            ret = circuit->netlist()->addComponent(edge.component_name.c_str(), modelMgr.getVDC());
            ret = circuit->netlist()->configComponent(edge.component_name.c_str(), "value", 
                csimModel::Variant(csimModel::Variant::VariantString).setString(edge.value.c_str()));
        }
        else if (edge.type == "current") {
            // Ixxx n+ n- [DC] 值，value 形如 "DC 1" 或 "1"
            ret = circuit->netlist()->addComponent(edge.component_name.c_str(), modelMgr.getIDC());
            double ival = 0.0;
            if (toLowerCase(edge.value).find("dc") != std::string::npos)
                ival = getValue(edge.value, "dc");
            else
                ival = parseNumericValue(edge.value, 0.0);
            ret = circuit->netlist()->configComponent(edge.component_name.c_str(), "I", 
                csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(ival));
        }
        else if (edge.type == "capacitor") {
            ret = circuit->netlist()->addComponent(edge.component_name.c_str(), modelMgr.getCapacitor());
            ret = circuit->netlist()->configComponent(edge.component_name.c_str(), "C", 
                csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(std::stod(edge.value)));
        }
        else if (edge.type == "inductor") {
            ret = circuit->netlist()->addComponent(edge.component_name.c_str(), modelMgr.getInductor());
            ret = circuit->netlist()->configComponent(edge.component_name.c_str(), "L", 
                csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(std::stod(edge.value)));
        }
        else if (edge.type == "diode") {
            std::string model_temp = models.at(edge.value);
            ret = circuit->netlist()->addComponent(edge.component_name.c_str(), modelMgr.getDiode());
            //std::cout << "diode:" << edge.component_name << "," << edge.value << "," << model_temp << std::endl;
        }
        else if (edge.type == "vccs") {
            // Gxxx n+ n- in+ in- 跨导，value = 增益
            ret = circuit->netlist()->addComponent(edge.component_name.c_str(), modelMgr.getVCCS());
            ret = circuit->netlist()->configComponent(edge.component_name.c_str(), "G", 
                csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(parseNumericValue(edge.value, 1.0)));
        }
        else if (edge.type == "vcvs") {
            // Exxx n+ n- in+ in- 电压增益，value = 增益
            ret = circuit->netlist()->addComponent(edge.component_name.c_str(), modelMgr.getVCVS());
            ret = circuit->netlist()->configComponent(edge.component_name.c_str(), "Mu", 
                csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(parseNumericValue(edge.value, 1.0)));
        }
        else if (edge.type == "cccs" || edge.type == "ccvs") {
            // Fxxx/Hxxx n+ n- Vctrl 增益，value 形如 "Vctrl gain"
            ret = circuit->netlist()->addComponent(edge.component_name.c_str(),
                edge.type == "cccs" ? modelMgr.getCCCS() : modelMgr.getCCVS());
            auto toks = split_space(edge.value);
            std::string ctrl_src = toUpperCase(toks.empty() ? "" : toks[0]);
            double gain = (toks.size() > 1) ? parseNumericValue(toks[1], 1.0) : 1.0;
            ret = circuit->netlist()->configComponent(edge.component_name.c_str(),
                edge.type == "cccs" ? "alpha" : "r",
                csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(gain));
            if (!ctrl_src.empty())
                ctrl_sources.push_back({edge.component_name, ctrl_src});
        }
        else {
            throw std::invalid_argument("Invalid model type: " + edge.type);
        }
        
        if (ret != 0) {
            throw std::runtime_error("Failed to add/configure component: " + edge.component_name);
        }
    }
}

// ==================== Node Connection ====================
std::map<std::string, std::vector<ComponentConnection>>
buildNodeConnectionMap(const std::vector<circuit::Edge>& edges, std::set<std::string>& all_nodes) {
    all_nodes.clear();
    for (const auto& edge : edges) {
        for (const auto& node : edge.nodes) {
            all_nodes.insert(node);
        }
    }
    
    std::map<std::string, std::vector<ComponentConnection>> node_connection_map;
    for (const auto& edge : edges) {
        for (size_t i = 0; i < edge.nodes.size(); ++i) {
            ComponentConnection conn{edge.component_name, static_cast<int>(i)};
            node_connection_map[edge.nodes[i]].push_back(conn);
        }
    }
    
    return node_connection_map;
}

// ==================== Probe Setup ====================
std::map<std::string, ProbeInfo> setupProbes(
    const std::vector<std::string>& probe_names,
    const std::map<std::string, std::vector<ComponentConnection>>& node_map) {
    
    std::map<std::string, ProbeInfo> probe_map;
    
    for (const auto& probe_name : probe_names) {
        std::string substring = toLowerCase(probe_name.substr(2, probe_name.size() - 3));
        if (!isValid(probe_name)) continue;
        
        ProbeType type = VOLTAGE_PROBE;
        if (probe_name[0] == 'i') {
            type = CURRENT_PROBE;
            probe_map[substring] = {substring, 0, type};
        } else {
            auto it = node_map.find(substring);
            if (it != node_map.end()) {
                const auto& connections = it->second;
                probe_map[substring] = {connections[0].component_name, connections[0].pin_index, type};
            }
        }
    }
    
    // Add ground node
    const auto& gnd_connections = node_map.at("0");
    probe_map["0"] = {gnd_connections[0].component_name, gnd_connections[0].pin_index, VOLTAGE_PROBE};
    
    return probe_map;
}


void wireComponents(csim::Circuit* circuit, 
                    const std::set<std::string>& all_nodes,
                    const std::map<std::string, std::vector<ComponentConnection>>& node_map) {
    for (const auto& node : all_nodes) {
        const auto& connections = node_map.at(node);
        for (size_t i = 0; i < connections.size() - 1; i++) {
            auto comp1_name = toUpperCase(connections[i].component_name);
            auto comp2_name = toUpperCase(connections[i + 1].component_name);
            
            std::cout << "wire:" << comp1_name << "," << connections[i].pin_index 
                     << "," << comp2_name << "," << connections[i + 1].pin_index << std::endl;
            
            circuit->netlist()->wire(comp1_name.c_str(), connections[i].pin_index,
                                    comp2_name.c_str(), connections[i + 1].pin_index);
        }
    }
}

// ==================== Controlled Source Wiring ====================
/**
 * CCCS/CCVS 的控制端（P/Q 引脚 2、3）需要接到控制源 Vctrl 的两个端子（0、1）上。
 * 库的 CCCS/CCVS 模型通过 P/Q 之间一个 0V 支路（安培计）测量控制电流，
 * 因此把安培计接到控制源的端子上以读取其电流。此步必须在 generateNodes() 之前完成。
 * 注意：当控制源为理想电压源时，并联安培计会导致矩阵奇异（库模型限制），
 * 建议用电流源作控制（参见 test/cccs_isc.sp）。
 */
void wireControlledSources(csim::Circuit* circuit,
                           const std::vector<std::pair<std::string, std::string>>& ctrl_sources) {
    for (const auto& [comp, src] : ctrl_sources) {
        int ret = circuit->netlist()->wire(comp.c_str(), 2, src.c_str(), 0);
        ret = circuit->netlist()->wire(comp.c_str(), 3, src.c_str(), 1);
        if (ret != 0) {
            std::cerr << "wire controlled source " << comp << " -> " << src << " failed, ret=" << ret << std::endl;
        }
    }
}

// ==================== Save To CSV ====================
enum class ComplexOutputMode {
    REAL,    // 保存实部
    ABS      // 保存绝对值
};

// 从 probe_map 提取探针名列（按迭代顺序，跳过地节点"0"），与数据列一一对应
std::vector<std::string> buildColumnNames(const std::map<std::string, ProbeInfo>& probe_map) {
    std::vector<std::string> names;
    for (const auto& [name, probe] : probe_map) {
        if (name == "0") continue;
        names.push_back(name);
    }
    return names;
}

void saveToCSV(const std::string& filename, 
               const std::vector<std::vector<std::complex<double>>>& data,
               const std::vector<std::string>& column_names,
               const csim::Variable* sweep = nullptr,
               ComplexOutputMode mode = ComplexOutputMode::ABS) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法创建输出文件: " + filename);
    }
    
    // 根据模式选择相应的获取值的函数
    auto getValue = [mode](const std::complex<double> c) {
        return (mode == ComplexOutputMode::REAL) ? c.real() : std::abs(c);
    };

    // 第一行写入列名：序号, [扫描变量], 探针名...
    file << std::setprecision(17); // 全精度输出，避免不同时间点被舍入为相同显示
    if (sweep) {
        file << sweep->getName();
    } else if (!column_names.empty()) {
        file << column_names[0];
    }
    for (size_t k = (sweep ? 0 : 1); k < column_names.size(); k++) {
        file << "," << column_names[k];
    }
    file << std::endl;

    for (size_t i = 0; i < data.size(); i++) {
        //file << i + 1 << ",";
        if (sweep) {
            file << sweep->at(i).real() << ",";
        }
        for (size_t j = 0; j < data[i].size(); j++) {
            file << getValue(data[i][j]) << ",";
        }
        file << std::endl;
    }
    file.close();
}

void collectProbeData(csim::Dataset& dset, 
                     std::vector<std::vector<std::complex<double>>>& data,
                     const csim::Variable &Vgnd,
                     const csim::AnalyzerBase* analyzer,
                     const std::map<std::string, ProbeInfo>& probe_map) {

}

// ==================== Analysis Execution ====================
void runOPAnalysis(csim::AnalyzerBase* analyzer, csim::Circuit* circuit,
                   const std::map<std::string, ProbeInfo>& probe_map,
                   const std::string& out_base) {
    csim::Dataset dset;
    analyzer->analyze(&dset);
    
    unsigned int gnd_node_idx;
    circuit->netlist()->getTermlNode(probe_map.at("0").component_name.c_str(), 
                                     probe_map.at("0").pin_index, &gnd_node_idx);
    std::vector<std::vector<std::complex<double>>> data(1, std::vector<std::complex<double>>(probe_map.size()-1,0));
    size_t j = 0;
    for (const auto& [name, probe] : probe_map) {
        if (name == "0") continue;
        
        unsigned int node_idx = 0;
        if (probe.type == VOLTAGE_PROBE) {
            if (circuit->netlist()->getTermlNode(probe.component_name.c_str(), 
                                                 probe.pin_index, &node_idx) == 0) {
                auto voltage = circuit->getNodeVolt(node_idx) - circuit->getNodeVolt(gnd_node_idx);
                std::cout << name << ": " << voltage.real() << " V" << std::endl;
                data[0][j] = std::complex<double>(voltage.real(),voltage.imag());
            }
        } else if (probe.type == CURRENT_PROBE) {
            if (circuit->netlist()->getBranch(probe.component_name.c_str(), 
                                              probe.pin_index, &node_idx) == 0) {
                auto current = circuit->getBranchCurrent(node_idx);
                std::cout << name << ": " << current.real() << " A" << std::endl;
                data[0][j] = std::complex<double>(current.real(),current.imag());
            }
        }
        j++;
    }

    saveToCSV(out_base + "_op.csv", data, buildColumnNames(probe_map));
}

void runDCAnalysis(csim::AnalyzerBase* analyzer, csim::Circuit* circuit,
                   const std::string& param, 
                   const std::map<std::string, ProbeInfo>& probe_map,
                   const std::string& out_base) {
    auto params = split_space(toUpperCase(param));
    
    analyzer->property().setProperty("vstart", csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(std::stod(params[1])));
    analyzer->property().setProperty("vstop", csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(std::stod(params[2])));
    analyzer->property().setProperty("vincr", csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(std::stod(params[3])));
    analyzer->property().setProperty("srcname", csimModel::Variant(csimModel::Variant::VariantString).setString(params[0].c_str()));
    
    unsigned int gnd_node_idx;
    circuit->netlist()->getTermlNode(probe_map.at("0").component_name.c_str(), 
                                     probe_map.at("0").pin_index, &gnd_node_idx);
    analyzer->addInterestNode(gnd_node_idx);
    
    std::vector<unsigned int> node_indices(probe_map.size() - 1);
    size_t i = 0;
    for (const auto& [name, probe] : probe_map) {
        if (name == "0") continue;
        
        if (probe.type == VOLTAGE_PROBE) {
            circuit->netlist()->getTermlNode(probe.component_name.c_str(), probe.pin_index, &node_indices[i]);
            analyzer->addInterestNode(node_indices[i]);
        } else if (probe.type == CURRENT_PROBE) {
            circuit->netlist()->getBranch(probe.component_name.c_str(), probe.pin_index, &node_indices[i]);
            analyzer->addInterestBranch(node_indices[i]);
        }
        i++;
    }
    
    csim::Dataset dset;
    analyzer->analyze(&dset);
    
    // 独立变量名 = 网表中的扫描源名（params[0]，已统一大写），不再硬编码 "V1"
    const csim::Variable& sweep = dset.getIndependentVar(params[0].c_str());
    const csim::Variable& Vgnd = dset.getDependentVar("voltage", analyzer->makeVarName("V", gnd_node_idx));
    
    std::vector<std::vector<std::complex<double>>> data(sweep.getNumValues(), std::vector<std::complex<double>>(node_indices.size()));
    size_t j = 0;
    for (const auto& [name, probe] : probe_map) {
        if (name == "0") continue;
        
        if (probe.type == VOLTAGE_PROBE) {
            const csim::Variable& Vn = dset.getDependentVar("voltage", analyzer->makeVarName("V", node_indices[j]));
            for (size_t k = 0; k < sweep.getNumValues(); k++) {
                double real = Vn.at(k).real() - Vgnd.at(k).real();
                double imag = Vn.at(k).imag() - Vgnd.at(k).imag();
                data[k][j] = std::complex<double>(real, imag);
            }
        } else if (probe.type == CURRENT_PROBE) {
            const csim::Variable& Ib = dset.getDependentVar("current", analyzer->makeVarName("I", node_indices[j]));
            for (size_t k = 0; k < sweep.getNumValues(); k++) {
                double real = Ib.at(k).real();
                double imag = Ib.at(k).imag();
                data[k][j] = std::complex<double>(real, imag);
            }
        }
        j++;
    }
    saveToCSV(out_base + "_dc.csv", data, buildColumnNames(probe_map), &sweep, ComplexOutputMode::REAL);
    std::cout << "DC analysis done -> " << out_base << "_dc.csv" << std::endl;

}


void runACAnalysis(csim::AnalyzerBase* analyzer, csim::Circuit* circuit, const std::string& param, 
                   const std::map<std::string, ProbeInfo>& probe_map,
                   const std::string& out_base){
    auto value1 = split_space(param);
        analyzer->property().setProperty("fspace", csimModel::Variant(csimModel::Variant::VariantString).setString(value1[0].c_str()));
        // 点数取自网表参数（value1[1]，如 .ac dec 10 1k 1Meg 中的 10）；缺失/非法时回退默认 50
        uint32_t fpoints = 50;
        if (value1.size() > 1 && !value1[1].empty()) {
            try { fpoints = static_cast<uint32_t>(std::stoul(value1[1])); }
            catch (...) {}
        }
        analyzer->property().setProperty("fpoints", csimModel::Variant(csimModel::Variant::VariantUint32).setUint32(fpoints));
        analyzer->property().setProperty("fstart", csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(std::stod(value1[2])));
        analyzer->property().setProperty("fstop", csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(std::stod(value1[3])));
        unsigned int gnd_node_idx ;// 获取地节点索引
        int ret = circuit->netlist()->getTermlNode(probe_map.at("0").component_name.c_str(), probe_map.at("0").pin_index, &gnd_node_idx );
        analyzer->addInterestNode(gnd_node_idx);   
        int i = 0;
        unsigned int node_idx = 0;
        std::vector<unsigned int> node_indices(probe_map.size()-1);
        for (const auto& probe : probe_map) {
            if (probe.first == "0") continue;  // 跳过地节点
            if(probe.second.type == VOLTAGE_PROBE){
                circuit->netlist()->getTermlNode(probe.second.component_name.c_str(),probe.second.pin_index,&node_indices[i]);    
                analyzer->addInterestNode(node_indices[i]);    
            }
            else if(probe.second.type == CURRENT_PROBE){
                circuit->netlist()->getBranch(probe.second.component_name.c_str(),probe.second.pin_index,&node_indices[i]);
                analyzer->addInterestBranch(node_indices[i]);
            }
  
            i++;
        }
        csim::Dataset dset;
        ret = analyzer->analyze(&dset);
        const csim::Variable &sweep = dset.getIndependentVar("frequency");
        const csim::Variable &Vgnd = dset.getDependentVar("voltage", analyzer->makeVarName("V", gnd_node_idx));
        std::vector<std::vector<std::complex<double>>> data(sweep.getNumValues(), std::vector<std::complex<double>>(node_indices.size()));
        size_t j=0;
        for(auto probe:probe_map){
            if(probe.first == "0") continue;  // 跳过地节点
            if(probe.second.type == VOLTAGE_PROBE){
                const csim::Variable &Vn1 = dset.getDependentVar("voltage", analyzer->makeVarName("V", node_indices[j]));
                for(size_t i=0;i<sweep.getNumValues();i++)
                {
                    double real = Vn1.at(i).real() - Vgnd.at(i).real();
                    double imag = Vn1.at(i).imag()- Vgnd.at(i).imag();
                    data[i][j] = std::complex<double>(real, imag);
                    //data[i][j]= sqrt(real*real+imag*imag);
                }
            } 
            else if(probe.second.type == CURRENT_PROBE){
                const csim::Variable &Ib2 = dset.getDependentVar("current", analyzer->makeVarName("I",  node_indices[j]));
                for(size_t i=0;i<sweep.getNumValues();i++)
                {
                    //data[i][j] = Ib2.at(i).real() ;
                    data[i][j] =std::complex<double>(Ib2.at(i).real(), Ib2.at(i).imag());               
                }
            }

            j++;
        }

        saveToCSV(out_base + "_ac.csv", data, buildColumnNames(probe_map), &sweep);
        std::cout << "AC analysis done -> " << out_base << "_ac.csv" << std::endl;
}

void runTRANAnalysis(csim::AnalyzerBase* analyzer, csim::Circuit* circuit, const std::string& param,
                   const std::map<std::string, ProbeInfo>& probe_map,
                   const std::string& out_base){
    auto value1 = split_space(param);
        double tstep = std::stod(value1[0]);
        double tstop = std::stod(value1[1]);
        analyzer->property().setProperty("tstop", csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(tstop));
        analyzer->property().setProperty("tstep", csimModel::Variant(csimModel::Variant::VariantDouble).setDouble(tstep));
        unsigned int gnd_node_idx ;// 获取地节点索引
        int ret = circuit->netlist()->getTermlNode(probe_map.at("0").component_name.c_str(), probe_map.at("0").pin_index, &gnd_node_idx );
        analyzer->addInterestNode(gnd_node_idx);   
        int i = 0;
        std::vector<unsigned int> node_indices(probe_map.size()-1);
        for (const auto& probe : probe_map) {
            if (probe.first == "0") continue;  // 跳过地节点                      
            if(probe.second.type == VOLTAGE_PROBE){
                circuit->netlist()->getTermlNode(probe.second.component_name.c_str(),probe.second.pin_index,&node_indices[i]);    
                analyzer->addInterestNode(node_indices[i]);
            }
            else if(probe.second.type == CURRENT_PROBE){
                circuit->netlist()->getBranch(probe.second.component_name.c_str(),probe.second.pin_index,&node_indices[i]);
                analyzer->addInterestBranch(node_indices[i]);
            }
            i++;
        }

        csim::Dataset dset;
        ret = analyzer->analyze(&dset);
        const csim::Variable &sweep = dset.getIndependentVar("time");
        unsigned int N = sweep.getNumValues();
        std::vector<std::vector<std::complex<double>>> data(N, std::vector<std::complex<double>>(node_indices.size()));
        const csim::Variable &Vgnd = dset.getDependentVar("voltage", analyzer->makeVarName("V", gnd_node_idx));
        size_t j=0;
        for(auto probe:probe_map){
            if(probe.first == "0") continue;  // 跳过地节点
            if(probe.second.type == VOLTAGE_PROBE){            
                const csim::Variable &Vn1 = dset.getDependentVar("voltage", analyzer->makeVarName("V", node_indices[j]));
                for(size_t i=0;i<N;i++)
                {
                    double real = Vn1.at(i).real() - Vgnd.at(i).real();
                    double imag = Vn1.at(i).imag() - Vgnd.at(i).imag();
                    data[i][j]= std::complex<double>(real, imag);    
                }
            } else if(probe.second.type == CURRENT_PROBE){
                const csim::Variable &Ib2 = dset.getDependentVar("current", analyzer->makeVarName("I",  node_indices[j]));
                for(size_t i=0;i<N;i++)
                {
                    double real = Ib2.at(i).real(); 
                    double imag = Ib2.at(i).imag();
                    data[i][j]= std::complex<double>(real, imag);
                }
            }
            j++;
        }
        saveToCSV(out_base + "_tran.csv", data, buildColumnNames(probe_map), &sweep, ComplexOutputMode::REAL);
        std::cout << "TRAN analysis done -> " << out_base << "_tran.csv" << std::endl;


}           

int main(int argc, char** argv) {
    std::string input_file = argv[1];
    CircuitParser parser;
    parser.parseFile(input_file);
    parser.expandAllSubckts();
    parser.genEdges();
    parser.process_();
    std::set<std::string> all_nodes;
    auto node_connection_map = buildNodeConnectionMap(parser.m_edges, all_nodes);
    ModelManager modelMgr;
    std::unique_ptr<csim::Circuit> circuit(new csim::Circuit());
    // Add components
    std::vector<std::pair<std::string, std::string>> ctrl_sources;
    addComponentsToCircuit(circuit.get(), parser.m_edges, parser.m_models, modelMgr, ctrl_sources);
    circuit->netlist()->prepare();
    
    // Wire components
    wireComponents(circuit.get(), all_nodes, node_connection_map);
    
    // Wire CCCS/CCVS control terminals (P/Q) to control current sources
    wireControlledSources(circuit.get(), ctrl_sources);
    
    // Setup probes
    auto probe_map = setupProbes(parser.m_probe_names, node_connection_map);
    
    circuit->netlist()->generateNodes();
    
    // Run analyses
    for (const auto& temp : parser.m_analysis) {
        auto analysis_name = temp.first;
        auto params =toUpperCase(temp.second);
        std::string analysis_type;
        if (analysis_name == "op") analysis_type = "OP";
        else if (analysis_name == "dc") analysis_type = "DC";
        else if (analysis_name == "ac") analysis_type = "AC";
        else if (analysis_name == "tran") analysis_type = "transient";
        else continue;
        std::unique_ptr<csim::AnalyzerBase> analyzer(
             csim::Analyzers::createInstance(analysis_type.c_str(), circuit.get())
        );

        if (analysis_name == "op") {
                runOPAnalysis(analyzer.get(), circuit.get(), probe_map, parser.m_savename);
        }
        else if (analysis_name == "dc") {
                runDCAnalysis(analyzer.get(), circuit.get(), params, probe_map, parser.m_savename);
        }
        else if (analysis_name == "ac") {
                runACAnalysis(analyzer.get(), circuit.get(), params, probe_map, parser.m_savename);
        }
        else if (analysis_name == "tran") {
            auto temp=node_connection_map["0"][0];
            int ret = circuit->netlist()->ground(temp.component_name.c_str(), temp.pin_index);
            runTRANAnalysis(analyzer.get(), circuit.get(), params, probe_map, parser.m_savename);
        }

    }
    return 0;
}