#ifndef HELPER_H
#define HELPER_H
#include <vector>
#include <string>
#include <complex>
#include <regex>  
#include <iostream>  
#include <fstream>
#include <cmath>
#include <utility> // std::pair
#include <tuple>
#include "variant.h"
#include "public.h"
#include <complex>
#include <regex>  
#include <random>  
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

bool isValid(const std::string& str);
bool isNumeric(const std::string& str);
std::vector<std::string> split_space(const std::string& text);;  // 按空格分割字符串
std::vector<std::string> splitByChar(const std::string& str, char delimiter);  // 按指定字符分割字符串
std::pair<bool, std::string> check_path(const std::string& input_path);
std::string toLowerCase(const std::string& str);//转成小写
std::vector<std::string> toLowerCase(const std::vector<std::string>& str);//转成小写
std::string vectorToString(const std::vector<int>& vec);
std::string vectorToString(const std::vector<std::string>& vec);
double getValueAfterKey(const std::string& str, const std::string& key, double default_value);
std::string getValueAfterKey(const std::string& str, const  std::string& key, const std::string& default_value); // 提取"mname=s_model"(keyword="mname=")的s_model
bool getValueAfterKey(const std::vector<std::string>& str, const  std::string& key, std::string& result);
double getValue(const std::string& str, const std::string& key);
int generate_5digit_number(const std::string& input_str);

template <typename T>
T interp1(std::vector<T> t1, std::vector<T> v1, T t2);
std::complex<double> Cominterp1(std::vector<double> Ti, double To, std::vector<std::complex<double>> Y1);
bool fileExists(const std::string& filename);
bool safeDeleteFile(const std::string& filename);
double clamp(double value, double min_val, double max_val);
int checkSign(double value);
std::string getDirectoryFromPath(const std::string& filePath);
std::string doubleToString(double value);
bool extractEutP(const std::string& str, std::string& res);
bool isEmptyOrWhitespace(const std::string& str);
double checkZero(double num);
std::vector<int> parseStringToVector(const std::string& str);
std::vector<double> string2double(std::vector<std::string>& str);
std::vector < std::vector<double>> string2double2(std::vector<std::string>& str);
std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
std::string findValue(const std::string& str, const std::string& key, const std::string& fuhao, const std::string& default_value);
std::string extractContentInsideParentheses(const std::string& str, const std::string& keyword); // 提取keyword()内的内容
void display_progress_bar(int total, int current);
bool canConvertToDouble(const std::string& str);
double convert_to_hz(const std::string& value_str);
double step(double time, double vl, double vh, double risetime);
double pulse(double time,
    double vl,
    double vh,
    double td,
    double risetime,
    double falltime,
    double pw,
    double period);
std::tuple< std::vector<double>, std::vector<double>> lfsr(
    double vl,
    double vh,
    double td,
    double risetime,
    double falltime,
    double rate,
    std::vector<int> bit_stream);

std::complex<double>* convertVectorToArray(const std::vector<std::vector<std::complex<double>>>& vec);
std::vector<std::vector<std::complex<double>>> convertArrayToVector(std::complex<double>* x, int rows, int cols);
std::vector<double> generateValues(const std::string& type, double min, double max, int numPoints);
std::vector<int> decimalToBinary(int decimal, int N);
std::vector<int> LFSR(std::vector<int>& s, std::vector<int>& t, int N);
std::vector<int> LFSR(int seed, std::vector<int> taps);
void saveComplexVector(const std::vector<std::vector<std::vector<std::complex<double>>>>& data, const std::string& filename);
void saveVectorToFile(const std::string& filename, std::vector < std::vector<double>>& data);
std::vector<std::complex<float>> convert_float_to_complex(const std::vector<float>& input);
std::vector<float> convert_complex_to_float(const std::vector<std::complex<float>>& input);
std::vector<double> convert_complex_to_float(const std::vector<std::complex<double>>& input);
double find_max_magnitude(const std::vector<std::vector<std::vector<std::complex<double>>>>& vec3d);

std::tuple<std::string, std::vector<double>, std::vector<double>> parseFrequencyCommand(const std::string& str);

Variant parseControlledSource(std::string line, std::string& type, std::vector<std::string>& ctrl_nodes);
Variant parseCapacitor(std::string line, std::string& type);
std::vector<int> findIntervalIndices(const std::vector<double>& x, double z);
void applySubcktSuffixToControlNodes(std::vector<std::string>& ctrl_nodes, const circuit::Edge& edge);
std::vector<std::vector<double>> convertCoeffsTo2DMatrix(const std::vector<double>& coeffs, int poly_dim);
void printPoly2Matrix(const std::vector<std::vector<double>>& matrix);
/**
 * 倍频程类型枚举
 */
enum class OctaveType {
    FULL = 1,      // 1倍频程
    ONE_THIRD = 3, // 1/3倍频程  
    ONE_SIXTH = 6, // 1/6倍频程
    ONE_TWELFTH = 12, // 1/12倍频程
    ONE_TWENTY_FOURTH = 24 // 1/24倍频程
};

std::vector<double> generateLogFrequency(double fmin, int steps_per_decade, double fmax);
std::vector<double> generateOctaveFrequency(double fmin, double fmax, OctaveType octave_type);
void addValue(std::vector<double>& data, double td);

std::vector<double> getVIAtVoltage(const std::vector<double>& x, const std::vector<double>& y, double target_voltage);

#endif // HELPER_H