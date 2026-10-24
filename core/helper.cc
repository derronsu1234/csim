#include "helper.h"
#include <string>  
#include <algorithm>  
#ifdef _WIN32  
#include <direct.h>  
#define GETCWD _getcwd  
#else  
#include <unistd.h>  
#define GETCWD getcwd  
#include <libgen.h> 
#include <cstring>  
#endif  


bool isValid(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    // 检查是否以 'v' 或 'i' 开头
    if (str[0] != 'v' && str[0] != 'i') {
        return false;
    }

    return true;
}


bool isNumeric(const std::string& str) {
    try {
        stoi(str);
        return true;
    }
    catch (const std::invalid_argument&) {
        return false;
    }
    catch (const std::out_of_range&) {
        return false;
    }
}

std::pair<bool, std::string> check_path(const std::string& input_path) {
    bool is_absolute = false;
    std::string dir_path;
    // 判断是否为绝对路径（平台相关逻辑）
#ifdef _WIN32
    // Windows: 盘符路径（如 C:\）或 UNC 路径（\\ 开头）
    if (input_path.size() >= 3 && input_path[1] == ':' &&
        (input_path[2] == '\\' || input_path[2] == '/')) {
        is_absolute = true;
    }
    else if (input_path.size() >= 2 &&
        input_path[0] == '\\' && input_path[1] == '\\') {
        is_absolute = true;  // UNC 路径
    }
#else
    // Linux/macOS: 以 / 开头
    if (!input_path.empty() && input_path[0] == '/') {
        is_absolute = true;
    }
#endif
    size_t last_slash = input_path.find_last_of("/\\");
    if (last_slash != std::string::npos) {
        dir_path = input_path.substr(0, last_slash);
        // 处理根目录情况（如 "/" 或 "C:\"）
        if (dir_path.empty() && !input_path.empty()) {
            dir_path = input_path[0] == '/' ? "/" : input_path.substr(0, last_slash + 1);
        }
    }
    else {
#ifdef _WIN32
        dir_path = ".";
#else
        dir_path = "";
#endif
    }

    return { is_absolute, dir_path };
}


std::vector<std::string> split_space(const std::string& text) {
    std::vector<std::string> result;
    std::istringstream iss(text);
    std::string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}


// 按指定字符分割字符串
std::vector<std::string> splitByChar(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;
    while ((end = str.find(delimiter, start)) != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    result.push_back(str.substr(start));
    return result;
}


std::string toLowerCase(const std::string& str) {
    std::string result;
    for (char c : str) {
        result += std::tolower(c);
    }

    return result;
}

std::vector<std::string> toLowerCase(const std::vector<std::string>& str) {
    std::vector<std::string> result(str.size());
    for (size_t ii = 0; ii < str.size(); ii++) {
        result[ii] = toLowerCase(str[ii]);
    }

    return result;
}


std::string vectorToString(const std::vector<int>& vec) {
    std::stringstream ss;
    for (int num : vec) {
        ss << num << ' ';
    }
    std::string result = ss.str();
    if (!result.empty()) {
        result.pop_back(); // 移除末尾的空格
    }
    return result;
}


std::string vectorToString(const std::vector<std::string>& vec) {
    std::stringstream ss;
    for (auto num : vec) {
        ss << num << ' ';
    }
    std::string result = ss.str();
    if (!result.empty()) {
        result.pop_back(); // 移除末尾的空格
    }
    return result;
}

double getValueAfterKey(const std::string& str, const  std::string& key, double default_value) {
    double result = default_value;
    if (str.empty() || key.empty()) {
        return result;
    }

    // 不区分大小写的子串查找
    auto it = std::search(
        str.begin(), str.end(),
        key.begin(), key.end(),
        [](char ch1, char ch2) {
            return std::tolower(static_cast<unsigned char>(ch1)) ==
                std::tolower(static_cast<unsigned char>(ch2));
        }
    );

    // 如果找到key
    if (it != str.end()) {
        // 计算值起始位置（跳过key）
        size_t value_start = std::distance(str.begin(), it) + key.size();

        // 查找值结束位置（空格或字符串结尾）
        size_t value_end = value_start;
        while (value_end < str.size() && !std::isspace(static_cast<unsigned char>(str[value_end]))) {
            ++value_end;
        }

        auto temp = str.substr(value_start, value_end - value_start);
        if (!isNumeric(temp) || temp.size() == 0)
            result = default_value;
        else
            result = std::stod(temp);
        return result;
    }

    return result;
}

std::string getValueAfterKey(const std::string& str, const std::string& key, const  std::string& default_value) {
    if (str.empty() || key.empty()) {
        return default_value;
    }

    // 不区分大小写的子串查找
    auto it = std::search(
        str.begin(), str.end(),
        key.begin(), key.end(),
        [](char ch1, char ch2) {
            return std::tolower(static_cast<unsigned char>(ch1)) ==
                std::tolower(static_cast<unsigned char>(ch2));
        }
    );

    // 如果找到key
    if (it != str.end()) {
        // 计算值起始位置（跳过key）
        size_t value_start = std::distance(str.begin(), it) + key.size();

        // 查找值结束位置（空格或字符串结尾）
        size_t value_end = value_start;
        while (value_end < str.size() && !std::isspace(static_cast<unsigned char>(str[value_end]))) {
            ++value_end;
        }

        // 返回key后的子字符串
        return str.substr(value_start, value_end - value_start);
    }

    // 未找到key返回默认值
    return default_value;
}

bool getValueAfterKey(const std::vector<std::string>& str, const  std::string& key, std::string& result) {
    for (auto ii : str) {
        std::string str1 = toLowerCase(ii);
        size_t found = str1.find(toLowerCase(key));
        if (found != std::string::npos) {
            size_t tstone_start = found + key.size();
            size_t tstone_end = str1.find(" ", tstone_start);
            result = str1.substr(tstone_start, tstone_end - tstone_start);
            return true;
        }
    }
    return false;
}

std::string findValue(const std::string& str, const std::string& key, const std::string& fuhao, const std::string& default_value) {
    // 如果输入字符串或关键词为空，直接返回默认值
    if (str.empty() || key.empty()) {
        return default_value;
    }

    // 将关键词转换为小写用于查找
    std::string key_lower;
    std::transform(key.begin(), key.end(), std::back_inserter(key_lower), ::tolower);

    // 创建小写版本的输入字符串用于查找
    std::string str_lower;
    std::transform(str.begin(), str.end(), std::back_inserter(str_lower), ::tolower);

    size_t pos = 0;
    while (pos < str_lower.length()) {
        // 查找关键词出现位置（不区分大小写）
        size_t found = str_lower.find(key_lower, pos);
        if (found == std::string::npos) {
            break; // 未找到关键词
        }

        // 检查关键词前是否有边界（字符串起始或非字母数字字符）
        bool left_boundary = (found == 0) || !std::isalnum(static_cast<unsigned char>(str_lower[found - 1]));

        // 检查关键词后是否有边界
        size_t after_key = found + key_lower.length();
        bool right_boundary = (after_key == str_lower.length()) || !std::isalnum(static_cast<unsigned char>(str_lower[after_key]));

        if (left_boundary && right_boundary) {
            // 跳过关键词
            size_t value_start = after_key;

            // 跳过关键词后的空格
            while (value_start < str.length() && std::isspace(static_cast<unsigned char>(str[value_start]))) {
                value_start++;
            }

            // 处理符号部分
            if (!fuhao.empty()) {
                // 检查是否有符号
                if (str.substr(value_start, fuhao.length()) != fuhao) {
                    pos = value_start; // 继续查找
                    continue;
                }
                // 跳过符号
                value_start += fuhao.length();
                // 跳过符号后的空格
                while (value_start < str.length() && std::isspace(static_cast<unsigned char>(str[value_start]))) {
                    value_start++;
                }
            }

            // 提取值（直到遇到空格或字符串结束）
            if (value_start < str.length()) {
                size_t value_end = value_start;
                while (value_end < str.length() && !std::isspace(static_cast<unsigned char>(str[value_end]))) {
                    value_end++;
                }
                return str.substr(value_start, value_end - value_start);
            }
            return default_value; // 关键词后无值
        }

        // 继续搜索剩余字符串
        pos = after_key;
    }

    return default_value;
}


double getValue(const std::string& str, const std::string& key) {
    std::string s1;
    std::string s2;

    s1 = findValue(str, key, "=", "not found");
    s2 = findValue(str, key, "", "not found");
    if (s1.size() != 0 && isNumeric(s1)) {
        return std::stod(s1);
    }
    else if (s2.size() != 0 && isNumeric(s2)) {
        return std::stod(s2);
    }
    else {
        return 0;
    }

}

int generate_5digit_number(const std::string& input_str) {

    std::hash<std::string> string_hasher;
    size_t seed = string_hasher(input_str);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(10000, 99999);

    return dist(rng);
}

template <typename T>
T interp1(std::vector<T> t1, std::vector<T> v1, T t2) {
    T data = 0;
    T k1 = 0;
    if (t2 <= t1[0]) {
        data = v1[0];
    }
    else if (t2 >= t1[t1.size() - 1]) {
        data = v1[t1.size() - 1];
    }
    else {
        int j = 0;
        for (auto v : t1) {
            if (v == t2) {
                data = v1[j];
                break;
            }
            else {
                if ((t2 > t1[j]) && (t2 < t1[j + 1])) {
                    k1 = (v1[j + 1] - v1[j]) / (t1[j + 1] - t1[j]);
                    data = k1 * (t2 - t1[j]) + v1[j];
                    break;
                }
            }
            j++;
        }
    }
    return data;
}

// implement template function in the header or define the specialization
template double
interp1<double>(std::vector<double> t1, std::vector<double> v1, double t2);

std::complex<double> Cominterp1(
    std::vector<double> Ti, double To, std::vector<std::complex<double>> Y1) {
    std::complex<double> Y2 = std::complex<double>(1e-8, 0);
    std::vector<double> I_Id(Y1.size(), 0);
    std::vector<double> Q_Id(Y1.size(), 0);

    for (size_t ii = 0; ii < Ti.size(); ii++) {
        I_Id[ii] = Y1[ii].real();
        Q_Id[ii] = Y1[ii].imag();

    }
    double real = 0;
    double imag = 0;
    real = interp1(Ti, I_Id, To);
    imag = interp1(Ti, Q_Id, To);
    Y2 = std::complex<double>(real, imag);

    return Y2;
}



bool fileExists(const std::string& filename) {
    std::ifstream f(filename);
    return f.good();  // 检查文件是否可打开
}

bool safeDeleteFile(const std::string& filename) {
    if (!fileExists(filename)) {
        std::cout << "File does not exist: " << filename << std::endl;
        return true;
    }

    if (std::remove(filename.c_str()) == 0) {
        std::cout << "Successfully deleted file: " << filename << std::endl;
        return true;
    }
    else {
        std::cerr << "Failed to delete file: " << filename << std::endl;
        return false;
    }
}

double clamp(double value, double min_val, double max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}


int checkSign(double value) {
    if (value > 0.0) {
        return 1;
    }
    else if (value < 0.0) {
        return -1;
    }
    else {
        return 0;
    }
}


// 获取文件所在目录  
std::string getDirectoryFromPath(const std::string& filePath) {  
#ifdef _WIN32  
    // Windows 路径处理  
    size_t lastSlashPos = filePath.find_last_of("\\/");  
    if (lastSlashPos == std::string::npos) return ".";  
    return filePath.substr(0, lastSlashPos);  
#else  
    // POSIX 路径处理  
    char *filePathCopy = new char[filePath.length() + 1];  
    strcpy(filePathCopy, filePath.c_str());  
    char *dirName = dirname(filePathCopy);  
    std::string directory(dirName);  
    delete[] filePathCopy;  
    return directory;  
#endif  
}  

std::string doubleToString(double value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

bool extractEutP(const std::string& str, std::string& res) {
    // 定义不区分大小写的正则表达式，匹配 v(eut_p) 或 i(eut_p)  
    std::regex pattern(R"(^[vi]\(([^)]+)\))", std::regex_constants::icase);
    std::smatch matches;
    
    if (std::regex_search(str, matches, pattern)) {
        res = matches[1];
        return true; // 返回括号中的内容  
    }

    return false; // 如果没有匹配，返回空字符串  
}

bool isEmptyOrWhitespace(const std::string& str) {
    return str.empty() || str.find_first_not_of(" \t") == std::string::npos;
}



double checkZero(double num) {
    if (num == 0) {
        return 1e-8;
    }
    else {
        return num;
    }
}


std::vector<double> string2double(std::vector<std::string>& str) {
    std::vector<double> result;
    for (int ii = 0; ii < str.size();ii++) {
        if(isNumeric(str[ii]))
            result.push_back(std::stod(str[ii]));
    }
    return result;
}

std::vector < std::vector<double>> string2double2(std::vector<std::string>& str) {
    std::vector < std::vector<double>> result(2);
    for (int ii = 0; ii < str.size(); ii++) {
        auto temp = split_space(str[ii]);
        // auto temp = splitByChar(str[ii], ' '); 
        //if(isNumeric(temp[0]))
            result[0].push_back( std::stod(temp[0]));
        //if (isNumeric(temp[1]))
            result[1].push_back(std::stod(temp[1]));
    }
    return result;
}

std::vector<int> parseStringToVector(const std::string& str) {
    std::vector<int> result;

    // 检查输入格式是否合法
    if (str.empty() || str.front() != '[' || str.back() != ']') {
        throw std::invalid_argument("Input string must be in format [a,b,c]");
    }

    // 去除首尾的方括号
    std::string content = str.substr(1, str.size() - 2);

    // 使用字符串流分割逗号
    std::istringstream iss(content);
    std::string token;

    while (std::getline(iss, token, ',')) {
        // 去除每个 token 的前后空白字符
        size_t first = token.find_first_not_of(" \t");
        size_t last = token.find_last_not_of(" \t");

        if (first == std::string::npos) {
            continue;  // 跳过空 token
        }

        std::string trimmed = token.substr(first, (last - first + 1));

        // 转换为整数
        result.push_back(std::stoi(trimmed));
    }

    return result;
}

std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::string result;
    for (size_t i = 0; i < strings.size(); ++i) {
        result += strings[i];
        if (i < strings.size() - 1) {
            result += delimiter;
        }
    }
    return result;
}



std::string extractContentInsideParentheses(const std::string& str, const std::string& keyword) {
    /*
    std::string result="";
    std::string keyword2 = keyword + '(';
    size_t start = str.find(keyword2)+ keyword.size();
    size_t end = str.find(')', start + 1);
    result = str.substr(start + 1, end - start - 1);

    return result;*/
    size_t pos = str.find(keyword);
    if (pos != std::string::npos) {
        size_t start = str.find('(', pos);
        if (start != std::string::npos) {
            size_t end = str.find(')', start);
            if (end != std::string::npos) {
                return str.substr(start + 1, end - start - 1);
            }
        }
    }
    return ""; // 如果没有找到括号内容，返回空字符串
}






void display_progress_bar(int total, int current) {
	const int bar_width = 70; // 进度条宽度  
	float progress = static_cast<float>(current) / total; // 计算进度  
	int pos = static_cast<int>(bar_width * progress); // 计算当前进度的字符位置  

	std::cout << "[";
	for (int i = 0; i < bar_width; ++i) {
		if (i < pos) {
			std::cout << "#"; // 已完成部分  
		}
		else {
			std::cout << "-"; // 未完成部分  
		}
	}
	std::cout << "] " << int(progress * 100) << " %\r"; // 输出百分比并回到行首  
	std::cout.flush(); // 刷新输出  
    //std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟工作  
}
 




bool canConvertToDouble(const std::string& str) {
    try {
        std::stod(str);
        return true; // 转换成功 
    } catch (const std::invalid_argument&) {  
        return false; // 字符串未能投影成数字 
    } catch (const std::out_of_range&) {  
        return false; // 数值超出 double 能表示的范围 
    }  
}


double convert_to_hz(const std::string& value_str) {
    double value = std::stod(value_str.substr(0, value_str.size() - 1));
    std::string unit = value_str.substr(value_str.size() - 1);

    if (unit == "k") {
        return value * 1000;
    }
    else if (unit == "M") {
        return value * 1000000;
    }
    else if (unit == "G") {
        return value * 1000000000;
    }
    else {
        throw std::invalid_argument("Invalid unit");
    }
}


double step(double time, double vl, double vh, double risetime)
{
    double k1 = (vh - vl) / risetime;
    double result;
    if (time < risetime) {
        result = k1 * time;
    }
    else {
        result = vh - vl;
    }
    return result;
}

double pulse(double time,
    double vl,
    double vh,
    double td,
    double risetime,
    double falltime,
    double pw,
    double period)
{
    
    if (time < td)
        return vl;
    double remainder = 1;
    if (period != 0) {
        remainder = fmod(time - td, period);  // 计算余数  
    }

    double k1 = (vh - vl) / risetime;
    double k2 = (vh - vl) / falltime;
    double result;
    if (remainder < risetime) {
        result = vl+k1 * remainder;
    }
    else if (remainder < risetime + pw ) {
        result = vh ;
    }
    else if (remainder < risetime + pw + falltime ) {
        result = vh  - k2 * (remainder - risetime - pw);
    }
    else {
        result = vl;
    }

    return result; 
    /*
    // 如果周期<=0，返回低电平
    if (period <= 0) {
        return vl;
    }

    // 计算当前时间在周期内的相对位置
    double cycle_time = std::fmod(time, period);
    if (cycle_time < 0) {
        cycle_time += period; // 处理负时间
    }

    // 1. 延时阶段：在td之前输出低电平
    if (cycle_time < td) {
        return vl;
    }

    // 计算相对于延时结束的时间
    double t = cycle_time - td;
    double total_pulse_time = risetime + pw + falltime;

    // 2. 上升沿阶段
    if (t < risetime) {
        return vl + (vh - vl) * (t / risetime);
    }

    // 3. 高电平阶段
    if (t < risetime + pw) {
        return vh;
    }

    // 4. 下降沿阶段
    if (t < total_pulse_time) {
        double fall_position = t - (risetime + pw);
        return vh - (vh - vl) * (fall_position / falltime);
    }

    // 5. 低电平阶段（直到周期结束）
    return vl;*/
}

std::tuple< std::vector<double>, std::vector<double>> lfsr(
    double vl,
    double vh,
    double td,
    double risetime,
    double falltime,
    double rate,
    std::vector<int> bit_stream) {

    double bit_time = 1 / rate;
    int num_bits = bit_stream.size();

    std::vector<double> time;
    std::vector<double> result;

    time.push_back(0.00);
    if (bit_stream.at(0) == 1)
        result.push_back(vh);
    else
        result.push_back(vl);

    for (int ii = 1; ii < num_bits; ii++) {
        
        if (bit_stream.at(ii ) > bit_stream.at(ii-1)) {
            time.push_back((ii - 1) * bit_time + risetime);
            result.push_back(vh);
        }
        else if (bit_stream.at(ii) < bit_stream.at(ii - 1)) {
            time.push_back((ii - 1) * bit_time + falltime);
            result.push_back(vl);
        }

        
        time.push_back(ii * bit_time);
        if (bit_stream.at(ii) == 1)
            result.push_back(vh);
        else 
            result.push_back(vl);


    }


#ifdef DEBUG_PRINT_ENABLED
        std::ofstream outfile("lfsr.csv");
        for (int jj = 0; jj < time.size(); jj++) {
                outfile << time[jj] << "," << result[jj] << "\n";
        }
        outfile.close();
    
#endif
    return std::make_tuple(std::move(time), std::move(result));
}



std::complex<double>* convertVectorToArray(const std::vector<std::vector<std::complex<double>>>& vec) {
    size_t rows = vec.size();
    size_t cols = vec[0].size();
    std::complex<double>* arr1d = new std::complex<double>[rows * cols];
    for (int ii = 0; ii < rows; ii++) {
        std::memcpy(arr1d + ii * cols, &vec[ii][0], cols * sizeof(std::complex<double>));
    }
    //for (size_t i = 0; i < rows * cols; ++i) {
     //   std::cout << "arr1d[" << i << "] = " << arr1d[i] << "\n";
    //}

    //delete[] arr1d;
    return arr1d;
}

std::vector<std::vector<std::complex<double>>> convertArrayToVector(std::complex<double>* x, int rows, int cols) {
    std::vector<std::vector<std::complex<double>>> res(rows, std::vector<std::complex<double>>(cols, 0));
    for (int ii = 0; ii < rows; ii++) {
        std::memcpy(&res[ii][0], (x + ii * cols), cols * sizeof(std::complex<double>));
    }
    /*for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            std::cout << "res= " << res[i][j] << "\n";
        }
    }*/
    return res;
}



std::vector<double> generateValues(const std::string& type, double min, double max, int numPoints) {
    std::vector<double> Values;

    if (numPoints <= 0) {
        std::cerr << "Error: Number of points must be greater than zero." << std::endl;
        return Values;
    }

    if (type == "lin") {
        // Linearly spaced frequency values
        double step = (max - min) / (numPoints - 1);
        for (int i = 0; i < numPoints; ++i) {
            Values.push_back(min + i * step);
        }
    }
    else if (type == "log") {
        // Logarithmically spaced frequency values
        double logMin = log10(min);
        double logMax = log10(max);
        double logStep = (logMax - logMin) / (numPoints - 1);
        for (int i = 0; i < numPoints; ++i) {
            Values.push_back(pow(10, logMin + i * logStep));
        }
    }
    else {
        std::cerr << "Error: Invalid type. Use 'lin' for linear or 'log' for logarithmic spacing." << std::endl;
        return Values;
    }

    return Values;
}




std::vector<int> decimalToBinary(int decimal,int N) {

    std::vector<int> binary(N,0);
    int i = 0;
    while (decimal > 0) {
        binary[N-i-1]= decimal % 2;
        decimal = decimal / 2;
        i++;
    }
    return binary;
}

std::vector<int> LFSR(std::vector<int>& s, std::vector<int>& t, int N) {
    int n = s.size();
    int m = t.size();
    std::vector<int> seq(N, 0);
    seq[0] = s[n - 1];
    for (int k = 0; k < N - 1; k++) {
        int b = s[t[0] - 1] ^ s[t[1] - 1];
        if (m > 2) {
            for (int i = 2; i < t.size(); i++) {
                b = s[t[i ] - 1] ^ b;
            }
        }
        s.pop_back();
        s.insert(s.begin(), b);   
        seq[k + 1] = s[n - 1];
    }
    return seq;
}
std::vector<int> LFSR(int seed, std::vector<int> taps) {
    // seed: 种子数，初始状态
    // taps：抽头
    auto max_it = std::max_element(taps.begin(), taps.end());
    auto data2 = decimalToBinary(seed, *max_it);
    auto data3 = LFSR(data2, taps, 1000);
    return data3;
}

void saveComplexVector(const std::vector<std::vector<std::vector<std::complex<double>>>>& data, const std::string& filename) {
    std::ofstream file(filename+".csv");

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file");
    }

    for (const auto& third : data) {
        for (const auto& second : third) {
            for (const auto& complex : second) {
                file << std::abs(complex) << "," ;
            }
        }
        file  << std::endl;
    }

    file.close();
}

void saveVectorToFile(const std::string& filename, std::vector < std::vector<double>>& data) {
    std::ofstream outfile;
    outfile.open(filename);

    for (auto ii : data) {
        for (auto jj : ii) {
            outfile << jj << ",";
        }
        outfile << "\n";
    }

    outfile.close(); // 关闭文件

}

std::vector<std::complex<float>> convert_float_to_complex(const std::vector<float>& input) {
    std::vector<std::complex<float>> output;
    output.reserve(input.size()); // 预分配内存以提高效率
    for (const auto& c : input) {
        output.push_back(std::complex<float>(c,0)); // 提取实部
    }
    return output;
}

std::vector<float> convert_complex_to_float(const std::vector<std::complex<float>>& input) {
    std::vector<float> output;
    output.reserve(input.size()); // 预分配内存以提高效率
    for (const auto& c : input) {
        output.push_back(c.real()); // 提取实部
    }
    return output;
}

std::vector<double> convert_complex_to_float(const std::vector<std::complex<double>>& input) {
    std::vector<double> output;
    output.reserve(input.size()); // 预分配内存以提高效率
    for (const auto& c : input) {
        output.push_back(c.real()); // 提取实部
    }
    return output;
}



double find_max_magnitude(const std::vector<std::vector<std::vector<std::complex<double>>>>& vec3d) {
    double max_val = 0.0;
    for (const auto& vec2d : vec3d) {          // 遍历三维向量
        for (const auto& vec1d : vec2d) {      // 遍历二维向量
            for (const auto& z : vec1d) {      // 遍历一维向量
                double magnitude = std::abs(z); // 计算复数模
                if (magnitude > max_val) {
                    max_val = magnitude;       // 更新最大值
                }
            }
        }
    }
    return max_val;
}




std::tuple<std::string, std::vector<double>, std::vector<double>> parseFrequencyCommand(const std::string& str) {
    std::string tt;
    std::vector<double> result;
    std::vector<double> result_;
    std::istringstream iss(str);
    std::string command;
    iss >> command;
    tt = command;
    if (command == "lin" || command == "log") {
        int npoints;
        double fstart, fstop;
        if (!(iss >> npoints >> fstart >> fstop) || npoints < 1) {
            tt = "lin";
            result = { 1 };
            result_ = { 1 };
        }

        if (npoints == 1) {
            tt = "lin";
            result = { fstart };
            result_ = { fstart };
        }

        result.reserve(npoints);
        if (command == "lin") {
            double step = (fstop - fstart) / (npoints - 1);
            for (int i = 0; i < npoints; ++i) {
                result.push_back(fstart + i * step);
            }
        }
        else {
            tt = "log";
            if (fstart <= 0 || fstop <= 0)  {
                result = { fstart };
                result_ = { fstart };
            };
            double ratio = pow(fstop / fstart, 1.0 / (npoints - 1));
            for (int i = 0; i < npoints; ++i) {
                result.push_back(fstart * pow(ratio, i));
            }

            result_= generateValues("lin", fstart, fstop, npoints *2);
        }
    }
    else if (command == "poi") {
        tt = "poi";
        int npoints;
        std::string token;
        if (iss >> token) {
            npoints = std::stoi(token);
        }
        // 读取剩余部分
        std::string remaining;
        std::getline(iss, remaining); // 从当前位置读取到行尾
        // 将逗号替换为空格（兼容两种格式）
        std::replace(remaining.begin(), remaining.end(), ',', ' ');
        // 从处理后的字符串中提取数字
        std::istringstream iss_remaining(remaining);
        double value;
        while (iss_remaining >> value) {
            result.push_back(value);
        }

        if (result.size() != static_cast<size_t>(npoints)) {
            tt = "lin";
            result = { 1 };
            result_ = { 1 };
        }

        std::sort(result.begin(), result.end());
        auto fstart = *result.begin();
        auto fstop = result.at(result.size()-1);
        result_ = generateValues("lin", fstart, fstop, result.size() * 2);

    }
    else {
        tt = "lin";
        result = { 1 };
        result_ = { 1 };
    }


    return std::make_tuple(tt, result, result_);
}

std::vector<double> unwrapPhaseFrom0To2Pi(const std::vector<double>& wrappedPhase) {
    std::vector<double> unwrapped;
    if (wrappedPhase.empty()) return unwrapped;

    unwrapped.reserve(wrappedPhase.size());
    unwrapped.push_back(wrappedPhase[0]); // 初始相位
    unwrapped.push_back(wrappedPhase[1]); // 初始相位
    //auto dt = wrappedPhase[1] - wrappedPhase[0];
    //auto iii = checkSign(dt);
    for (size_t i = 2; i < wrappedPhase.size(); ++i) {
        double prev_unwrapped = unwrapped[i - 1];
        double current_wrapped = wrappedPhase[i];

        // 检测跳变：若相位差超过 π 或小于 -π，说明发生包裹
        while (current_wrapped < prev_unwrapped) {

            current_wrapped= current_wrapped+ 2 * M_PI;

        }
        // 更新解包裹相位
        unwrapped.push_back(current_wrapped);
    }

    return unwrapped;
}


std::pair<std::string, int> findPolyOrPwl(const std::string& str) {
    for (size_t i = 0; i < str.length(); ++i) {
        // 检查当前字符是否为'p'或'P'
        if (std::tolower(static_cast<unsigned char>(str[i])) == 'p') {
            // 检查"PWL("模式
            if (i + 4 <= str.length()) {
                char c1 = std::tolower(static_cast<unsigned char>(str[i + 1]));
                char c2 = std::tolower(static_cast<unsigned char>(str[i + 2]));
                char c3 = std::tolower(static_cast<unsigned char>(str[i + 3]));
                if (c1 == 'w' && c2 == 'l' && c3 == '(') {
                    size_t start = i + 4; // 数字起始位置
                    size_t j = start;
                    // 提取数字部分
                    while (j < str.length() && std::isdigit(static_cast<unsigned char>(str[j]))) {
                        ++j;
                    }
                    // 验证数字后必须有闭合括号
                    if (j > start && j < str.length() && str[j] == ')') {
                        std::string numStr = str.substr(start, j - start);
                        char* end;
                        long n = std::strtol(numStr.c_str(), &end, 10);
                        // 确保整个数字字符串被转换
                        if (end == numStr.c_str() + numStr.length()) {
                            return { "pwl", static_cast<int>(n) };
                        }
                    }
                }
            }
            // 检查"POLY("模式
            if (i + 5 <= str.length()) {
                char c1 = std::tolower(static_cast<unsigned char>(str[i + 1]));
                char c2 = std::tolower(static_cast<unsigned char>(str[i + 2]));
                char c3 = std::tolower(static_cast<unsigned char>(str[i + 3]));
                char c4 = std::tolower(static_cast<unsigned char>(str[i + 4]));
                if (c1 == 'o' && c2 == 'l' && c3 == 'y' && c4 == '(') {
                    size_t start = i + 5; // 数字起始位置
                    size_t j = start;
                    // 提取数字部分
                    while (j < str.length() && std::isdigit(static_cast<unsigned char>(str[j]))) {
                        ++j;
                    }
                    // 验证数字后必须有闭合括号
                    if (j > start && j < str.length() && str[j] == ')') {
                        std::string numStr = str.substr(start, j - start);
                        char* end;
                        long n = std::strtol(numStr.c_str(), &end, 10);
                        // 确保整个数字字符串被转换
                        if (end == numStr.c_str() + numStr.length()) {
                            return { "poly", static_cast<int>(n) };
                        }
                    }
                }
            }
        }
    }
    return { "", -1 }; // 未找到
}

Variant parseControlledSource(std::string line, std::string& type, std::vector<std::string>& ctrl_nodes) {
    Variant res;
    type = "0";
    ctrl_nodes.clear(); // 清空控制节点数组
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    auto result = findPolyOrPwl(line);
    type = result.first;
    int poly_dimension = result.second;
    if (type == "poly") {
        std::cout << "Parsing POLY(" << poly_dimension << ") control source" << std::endl;

        // 验证poly格式
        std::regex poly_regex(R"(poly\((\d+)\))", std::regex_constants::icase);
        std::smatch match;
        if (!std::regex_search(tokens[0], match, poly_regex)) {
            throw std::invalid_argument("Invalid poly(n) format: " + tokens[0]);
        }

        int parsed_dim = std::stoi(match[1]);
        if (parsed_dim != poly_dimension) {
            std::cerr << "Warning: Dimension mismatch in poly parsing" << std::endl;
        }

        // 计算需要的控制节点数量：每个维度需要2个节点（+，-）
        size_t expected_ctrl_nodes = 2 * poly_dimension;
        size_t ctrl_start = 1;  // 第一个token是poly(n)
        size_t ctrl_end = ctrl_start + expected_ctrl_nodes;

        // 验证token数量足够
        if (tokens.size() < ctrl_end) {
            throw std::invalid_argument("Insufficient tokens for poly(" +
                std::to_string(poly_dimension) +
                ") control nodes");
        }

        // 提取控制节点
        for (size_t i = ctrl_start; i < ctrl_end; ++i) {
            ctrl_nodes.push_back(tokens[i]);
        }

        // 提取系数
        std::vector<double> coefficients;
        for (size_t i = ctrl_end; i < tokens.size(); ++i) {
            try {
                coefficients.push_back(std::stod(tokens[i]));
            }
            catch (const std::exception& e) {
                std::cerr << "Warning: Invalid coefficient '" << tokens[i]
                    << "', skipping. Error: " << e.what() << std::endl;
            }
        }

        if (coefficients.empty()) {
            throw std::invalid_argument("No valid coefficients found for poly");
        }

        // 根据维度处理系数
        if (poly_dimension == 1) {
            // poly(1): 返回一维系数数组
            res = Variant(coefficients);
            std::cout << "Parsed poly(1) with " << coefficients.size() << " coefficients" << std::endl;
        }
        else if (poly_dimension == 2) {
            // poly(2): 转换为二维系数矩阵
            auto coeff_matrix = convertCoeffsTo2DMatrix(coefficients, poly_dimension);
            res = Variant(coeff_matrix);
            std::cout << "Parsed poly(2) with " << coefficients.size()
                << " coefficients converted to " << coeff_matrix.size()
                << "x" << coeff_matrix[0].size() << " matrix" << std::endl;
            printPoly2Matrix(coeff_matrix);
        }
        else {
            throw std::invalid_argument("Unsupported poly dimension: " +
                std::to_string(poly_dimension) +
                ". Only poly(1) and poly(2) are supported.");
        }
        /*
        std::vector<double> coefficients;
        std::regex poly_regex(R"(poly\((\d+)\))");
        std::smatch match;
        if (!std::regex_match(tokens[0], match, poly_regex)) {
            throw std::invalid_argument("Invalid poly(n) format.");
        }
        int poly_dim = std::stoi(match[1]);
        size_t ctrl_start = 0 + 1;
        size_t ctrl_end = ctrl_start + 2 * poly_dim;
        for (size_t i = ctrl_start; i < ctrl_end; ++i) {
            ctrl_nodes.push_back(tokens[i]);
        }
        for (size_t i = ctrl_end; i < tokens.size(); ++i) {
            coefficients.push_back(std::stod(tokens[i]));
        }
        res = Variant(coefficients);*/
    }
    else if (type == "pwl") {
        size_t ctrl_start = 1;
        size_t ctrl_end = 3;
        for (size_t i = ctrl_start; i < ctrl_end; ++i) {
            ctrl_nodes.push_back(tokens[i]);
        }
        std::vector<double> x;
        std::vector<double> y;
        for (size_t i = ctrl_end; i < tokens.size(); ++i) {
            std::vector<std::string> tokens2 = splitByChar(tokens[i], ',');
            x.push_back(std::stod(tokens2[0]));
            y.push_back(std::stod(tokens2[1]));
        }
        auto myTuple = std::make_tuple(x, y);
        res = Variant(myTuple);
    }
    else {
        size_t ctrl_start = 0;
        size_t ctrl_end = 2;
        for (size_t i = ctrl_start; i < ctrl_end; ++i) {
            ctrl_nodes.push_back(tokens[i]);
        }
        auto temp = (tokens[ctrl_end]);
        res = Variant({ std::stod(temp) });
    }

    return res;
}

std::pair<std::string, std::vector<double>> extractPOLY(const std::string& str) {
    std::istringstream iss(str);
    std::string token;
    std::vector<double> numbers;

    // 提取第一个token并转换为大写进行不区分大小写比较
    if (!(iss >> token)) {
        return { "", {} };
    }

    // 将token转换为大写
    std::string upperToken = token;
    std::transform(upperToken.begin(), upperToken.end(), upperToken.begin(),
        [](unsigned char c) { return std::toupper(c); });

    // 检查是否是POLY（不区分大小写）
    if (upperToken != "POLY") {
        double num = std::stod(token);
        return { "", {num} };
    }

    // 提取后续的数字（支持科学计数法，e/E不区分大小写）
    while (iss >> token) {
        try {
            // 替换小写e为大写E，确保stod正确处理
            for (char& c : token) {
                if (c == 'e') c = 'E';
            }

            size_t pos;
            double num = std::stod(token, &pos);

            // 确保整个token都被转换
            if (pos == token.size()) {
                numbers.push_back(num);
            }
        }
        catch (...) {
            // 转换失败时忽略该token
        }
    }

    return { "poly", numbers };
}

Variant parseCapacitor(std::string line, std::string& type) {
    Variant res;
    type = "0";
    auto result = extractPOLY(line);
    if (result.first.empty()) {
        res = Variant({ result.second[0] });
    }
    else {
        res = Variant(result.second);
        type = result.first;
    }
    return res;
}

std::vector<int> findIntervalIndices(const std::vector<double>& x, double z) {
    // 处理空数组
    if (x.empty()) {
        return {};
    }

    // 使用二分查找找到第一个大于等于z的元素
    auto it = std::lower_bound(x.begin(), x.end(), z);

    // z大于所有元素
    if (it == x.end()) {
        return { -1 };
    }

    // z等于找到的元素
    if (*it == z) {
        int idx = std::distance(x.begin(), it);
        return { static_cast<int>(idx) };
    }

    // z小于所有元素
    if (it == x.begin()) {
        return { -1 };
    }

    // z位于两个元素之间
    int idx_prev = std::distance(x.begin(), it) - 1;
    int idx_next = idx_prev + 1;
    return { static_cast<int>(idx_prev), static_cast<int>(idx_next) };
}

void applySubcktSuffixToControlNodes(std::vector<std::string>& ctrl_nodes, 
                                   const circuit::Edge& edge) {
    if (!edge.is_in_subckt || edge.subckt_suffix.empty()) {
        return;
    }
    
    for (auto& node : ctrl_nodes) {
        if (node == "0") {
            continue; // 跳过接地节点
        }
        
        // 首先检查是否在节点映射中（外部节点）
        auto it = edge.node_mapping.find(node);
        if (it != edge.node_mapping.end()) {
            node = it->second;  // 使用映射后的外部节点名称
            continue;
        }
        
        // 如果不是外部节点，添加子电路后缀
        if (node.find(edge.subckt_suffix) == std::string::npos) {
            node += edge.subckt_suffix;
        }
    }
}

// 将一维系数数组转换为二维矩阵
std::vector<std::vector<double>> convertCoeffsTo2DMatrix(const std::vector<double>& coeffs, int poly_dim) {
    if (poly_dim != 2) {
        throw std::invalid_argument("convertCoeffsTo2DMatrix only supports poly(2)");
    }

    // 为poly(2)创建系数矩阵：最大支持2次项，所以是3x3矩阵
    // matrix[i][j] 对应 v1^i * v2^j 的系数
    std::vector<std::vector<double>> matrix(3, std::vector<double>(3, 0.0));
    // 系数顺序通常为：a00, a10, a01, a11, a20, a02, a21, a12, a22
    std::vector<std::pair<int, int>> coeff_order = {
        {0, 0},  // a00: 常数项
        {1, 0},  // a10: v1
        {0, 1},  // a01: v2  
		{2, 0},  // a20: v1^2
        {1, 1},  // a11: v1*v2
        {0, 2},  // a02: v2^2
        {2, 1},  // a21: v1^2*v2
        {1, 2},  // a12: v1*v2^2
        {2, 2}   // a22: v1^2*v2^2
    };

    // 填充矩阵
    for (size_t k = 0; k < coeffs.size() && k < coeff_order.size(); ++k) {
        int i = coeff_order[k].first;
        int j = coeff_order[k].second;
        matrix[i][j] = coeffs[k];

        std::cout << "  Coefficient[" << k << "] = " << coeffs[k]
            << " -> matrix[" << i << "][" << j << "] (v1^" << i << "*v2^" << j << ")" << std::endl;
    }

    // 如果系数不足，其余项为0（已在初始化时设置）
    if (coeffs.size() < coeff_order.size()) {
        std::cout << "Note: " << (coeff_order.size() - coeffs.size())
            << " higher-order terms set to zero" << std::endl;
    }

    return matrix;
}

// 打印二维系数矩阵（用于调试）
void printPoly2Matrix(const std::vector<std::vector<double>>& matrix) {
    std::cout << "coefficient matrix:" << std::endl;
    for (size_t i = 0; i < matrix.size(); ++i) {
        std::cout << "matrix" << i << " ";
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            std::cout  << std::scientific<< matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}



std::vector<double> generateLogFrequency(double fmin, int steps_per_decade, double fmax) {
    if (fmin == 0.00)
        fmin = 1;
    double log_fmin = std::log10(fmin);
    double log_fmax = std::log10(fmax);

    // 计算步长（每十倍频率内的步长）
    double step_size = 1.0 / steps_per_decade;

    // 计算总的步数
    int total_steps = static_cast<int>((log_fmax - log_fmin) / step_size) + 1;

    // 创建结果向量
    std::vector<double> frequencies;
    frequencies.reserve(total_steps);

    // 生成频率序列
    for (int i = 0; i < total_steps; ++i) {
        double log_freq = log_fmin + i * step_size;
        double freq = std::pow(10.0, log_freq);

        // 确保不超过fmax
        if (freq > fmax) {
            break;
        }

        frequencies.push_back(freq);
    }

    // 如果最后一个频率不是fmax，添加fmax
    if (frequencies.back() < fmax) {
        frequencies.push_back(fmax);
    }

    return frequencies;
}
/**
 * 生成ISO标准倍频程频率序列
 * @param fmin 起始频率 (Hz)
 * @param fmax 停止频率 (Hz)
 * @param octave_type 倍频程类型
 * @return 倍频程频率向量
 */
std::vector<double> generateOctaveFrequency(double fmin, double fmax, OctaveType octave_type) {
    // 计算频率比值：2^(1/n)，其中n是倍频程分割数
    int n = static_cast<int>(octave_type);
    double ratio = std::pow(2.0, 1.0 / n);

    std::vector<double> result;

    // 根据不同倍频程类型，使用对应的ISO标准频率
    std::vector<double> base_frequencies;

    switch (octave_type) {
    case OctaveType::FULL: // 1倍频程
        base_frequencies = {
            1.0, 2.0, 4.0, 8.0, 16.0, 31.5, 63.0, 125.0, 250.0, 500.0,
            1000.0, 2000.0, 4000.0, 8000.0, 16000.0, 32000.0
        };
        break;

    case OctaveType::ONE_THIRD: // 1/3倍频程
        base_frequencies = {
            1.0, 1.25, 1.6, 2.0, 2.5, 3.15, 4.0, 5.0, 6.3, 8.0,
            10.0, 12.5, 16.0, 20.0, 25.0, 31.5, 40.0, 50.0, 63.0, 80.0,
            100.0, 125.0, 160.0, 200.0, 250.0, 315.0, 400.0, 500.0, 630.0, 800.0,
            1000.0, 1250.0, 1600.0, 2000.0, 2500.0, 3150.0, 4000.0, 5000.0, 6300.0, 8000.0,
            10000.0, 12500.0, 16000.0, 20000.0, 25000.0, 31500.0, 40000.0, 50000.0, 63000.0, 80000.0
        };
        break;

    case OctaveType::ONE_SIXTH: // 1/6倍频程
        // 基于1000Hz，按2^(1/6)比值生成
        base_frequencies.push_back(1000.0);
        break;

    case OctaveType::ONE_TWELFTH: // 1/12倍频程  
        // 基于1000Hz，按2^(1/12)比值生成（音乐半音阶）
        base_frequencies.push_back(1000.0);
        break;

    case OctaveType::ONE_TWENTY_FOURTH: // 1/24倍频程
        // 基于1000Hz，按2^(1/24)比值生成
        base_frequencies.push_back(1000.0);
        break;
    }

    // 如果有预定义的标准频率，筛选范围内的
    if (base_frequencies.size() > 1) {
        for (double freq : base_frequencies) {
            if (freq >= fmin && freq <= fmax) {
                result.push_back(freq);
            }
        }
    }

    // 从基准频率1000Hz开始，向上和向下扩展
    double base_freq = 1000.0;

    // 向下扩展到fmin
    double freq_down = base_freq;
    while (freq_down > fmin) {
        freq_down /= ratio;
        if (freq_down >= fmin) {
            result.push_back(freq_down);
        }
    }

    // 向上扩展到fmax
    double freq_up = base_freq;
    while (freq_up < fmax) {
        freq_up *= ratio;
        if (freq_up <= fmax) {
            result.push_back(freq_up);
        }
    }

    // 添加基准频率1000Hz（如果在范围内）
    if (base_freq >= fmin && base_freq <= fmax) {
        result.push_back(base_freq);
    }

    // 排序并去重
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;
}

void addValue(std::vector<double>& data, double td) {
    for (auto& value : data) {
        value += td;
    }
}


std::vector<double> getVIAtVoltage(const std::vector<double>& x, const std::vector<double>& y, double target_voltage)
{
    std::vector<double> result;
    double value;
    double derivative = 0;
    if (x.empty() || y.empty()) {
        return result;
    }


    // 找到目标电压的位置
    int exact_index = -1;
    int lower_index = -1;
    int upper_index = -1;

    // 查找精确匹配或插值位置
    for (size_t i = 0; i < x.size(); i++) {
        if (std::abs(x[i] - target_voltage) < 1e-10) {
            exact_index = i;
            break;
        }
        if (x[i] < target_voltage) {
            lower_index = i;
        }
        if (x[i] > target_voltage && upper_index == -1) {
            upper_index = i;
            break;
        }
    }

    // 情况1：找到精确匹配
    if (exact_index != -1) {
        value = y[exact_index];


        // 使用中心差分计算导数
        if (exact_index == 0) {
            // 前向差分
            if (x.size() > 1) {
                double dv = x[1] - x[0];
                double di = y[1] - y[0];
                derivative = di / dv;
            }
        }
        else if (exact_index == static_cast<int>(x.size()) - 1) {
            // 后向差分
            double dv = x[exact_index] - x[exact_index - 1];
            double di = y[exact_index] - y[exact_index - 1];
            derivative = di / dv;
        }
        else {
            // 中心差分
            double dv = x[exact_index + 1] - x[exact_index - 1];
            double di = y[exact_index + 1] - y[exact_index - 1];
            derivative = di / dv;
        }
        result = { value,derivative };
        return result;
    }

    // 情况2：在数据范围内插值
    if (lower_index != -1 && upper_index != -1) {
        bool is_interpolation = true;

        double v1 = x[lower_index];
        double i1 = y[lower_index];
        double v2 = x[upper_index];
        double i2 = y[upper_index];

        // 线性插值
        double t = (target_voltage - v1) / (v2 - v1);
        value = i1 + t * (i2 - i1);

        // 导数就是斜率
        derivative = (i2 - i1) / (v2 - v1);
        result = { value,derivative };
        return result;
    }

    // 情况3：外推
    bool is_interpolation = false;

    if (target_voltage < x[0]) {
        // 左侧外推 - 使用前两个点
        if (x.size() >= 2) {
            double v1 = x[0];
            double i1 = y[0];
            double v2 = x[1];
            double i2 = y[1];

            double slope = (i2 - i1) / (v2 - v1);
            value = i1 + slope * (target_voltage - v1);
            derivative = slope;
        }
        else {
            value = x[0];
            derivative = 0.0;
        }
    }
    else {
        // 右侧外推 - 使用后两个点
        if (x.size() >= 2) {
            size_t n = x.size();
            double v1 = x[n - 2];
            double i1 = y[n - 2];
            double v2 = x[n - 1];
            double i2 = y[n - 1];

            double slope = (i2 - i1) / (v2 - v1);
            value = i2 + slope * (target_voltage - v2);
            derivative = slope;
        }
        else {
            value = x[0];
            derivative = 0.0;
        }
    }
    result = { value,derivative };
    return result;

}


