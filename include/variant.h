// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef VARIANT_H
#define VARIANT_H
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

enum class TypeTag {
    INT,
    DOUBLE,
    STRING,
    VECTOR_INT,
    VECTOR_DOUBLE,
    VECTOR2D_DOUBLE,
    CUSTOM_CLASS,
    TUPLE_VECTOR2D, // 新增类型标签
    TUPLE_VECTOR3D // 新增类型标签
};

class CustomClass {
public:
    CustomClass(int data) : data(data) {}
    int getData() const { return data; }
private:
    int data;
} ;
class Variant {
private:
    TypeTag tag; 
    union {
        int intValue;
        double doubleValue;
        std::string stringValue;
        std::vector<int> vectorIntValue;
        std::vector<double> vectorDoubleValue;
        std::vector < std::vector<double>> vector2dDoubleValue;
        CustomClass customClassValue;
        // 新增联合体成员
        std::tuple<
            std::vector<double>,
            std::vector<double>
        > tupleVector2DValue;
        std::tuple<
            std::vector<double>,
            std::vector<double>,
            std::vector<double>
        > tupleVector3DValue;
    };



    void cleanup() {
        switch (tag) {
            case TypeTag::STRING:
                stringValue.~basic_string();
                break;
            case TypeTag::VECTOR_INT:
                vectorIntValue.~vector();
                break;
            case TypeTag::VECTOR_DOUBLE:
                vectorDoubleValue.~vector();
                break;
            case TypeTag::VECTOR2D_DOUBLE:
                vector2dDoubleValue.~vector();
                break;
            case TypeTag::CUSTOM_CLASS:
                customClassValue.~CustomClass();
                break;
            case TypeTag::TUPLE_VECTOR2D:
                tupleVector2DValue.~tuple();  // 显式析构元组
                break;
            case TypeTag::TUPLE_VECTOR3D:
                tupleVector3DValue.~tuple();  // 显式析构元组
                break;
            default:
                break;  // POD类型无需析构
        }

    }

public:
    Variant() : tag(TypeTag::INT), intValue(0) {}  // 初始化为某个安全的POD类型  
    //Variant()  {}
    Variant(int v) : tag(TypeTag::INT), intValue(v) {}
    Variant(double v) : tag(TypeTag::DOUBLE), doubleValue(v) {}
    Variant(const std::string& v) : tag(TypeTag::STRING) {
        new (&stringValue) std::string(v);  // Placement new 构造非POD类型
    }
    Variant(const std::vector<int>& v) : tag(TypeTag::VECTOR_INT) {
        new (&vectorIntValue) std::vector<int>(v);
    }
    Variant(const std::vector<double>& v) : tag(TypeTag::VECTOR_DOUBLE) {
        new (&vectorDoubleValue) std::vector<double>(v);  // Placement new
    }
    Variant(const std::vector < std::vector<double>>& v) : tag(TypeTag::VECTOR2D_DOUBLE) {
        new (&vector2dDoubleValue) std::vector < std::vector<double>>(v);  // Placement new
    }
    Variant(const CustomClass& obj) : tag(TypeTag::CUSTOM_CLASS) {
        new (&customClassValue) CustomClass(obj);
    }
    // 新增构造函数
    Variant(const std::tuple<
        std::vector<double>,
        std::vector<double>
    >& v) : tag(TypeTag::TUPLE_VECTOR2D)
    {
        new (&tupleVector2DValue)
            decltype(tupleVector2DValue)(v);  // Placement new
    }
    Variant(const std::tuple<
        std::vector<double>,
        std::vector<double>,
        std::vector<double>
    >& v) : tag(TypeTag::TUPLE_VECTOR3D)
    {
        new (&tupleVector3DValue)
            decltype(tupleVector3DValue)(v);  // Placement new
    }

    ~Variant() {
        cleanup();
    }


    Variant(const Variant& other) : tag(other.tag) {
        switch (tag) {
            case TypeTag::INT:
                intValue = other.intValue;
                break;
            case TypeTag::DOUBLE:
                doubleValue = other.doubleValue;
                break;
            case TypeTag::STRING:
                new (&stringValue) std::string(other.stringValue);
                break;
            case TypeTag::VECTOR_INT:
                new (&vectorIntValue) std::vector<int>(other.vectorIntValue);
                break;
            case TypeTag::VECTOR_DOUBLE:
                new (&vectorDoubleValue) std::vector<double>(other.vectorDoubleValue);
                break;
            case TypeTag::VECTOR2D_DOUBLE:
                new (&vector2dDoubleValue) std::vector < std::vector<double>>(other.vector2dDoubleValue);
                break;
            case TypeTag::CUSTOM_CLASS:
                new (&customClassValue) CustomClass(other.customClassValue);
                break;
            // ... 已有类型的拷贝 ...
            case TypeTag::TUPLE_VECTOR2D:
                new (&tupleVector2DValue)
                    decltype(tupleVector2DValue)(other.tupleVector2DValue);
                break;
            case TypeTag::TUPLE_VECTOR3D:
                new (&tupleVector3DValue)
                    decltype(tupleVector3DValue)(other.tupleVector3DValue);
                break;
        }
    }

 
    Variant& operator=(const Variant& other) {
        if (this != &other) {
            cleanup();  // 清理当前值
            tag = other.tag;
            switch (tag) {
                case TypeTag::INT:
                    intValue = other.intValue;
                    break;
                case TypeTag::DOUBLE:
                    doubleValue = other.doubleValue;
                    break;
                case TypeTag::STRING:
                    new (&stringValue) std::string(other.stringValue);
                    break;
                case TypeTag::VECTOR_INT:
                    new (&vectorIntValue) std::vector<int>(other.vectorIntValue);
                    break;
                case TypeTag::VECTOR_DOUBLE:
                    new (&vectorDoubleValue) std::vector<double>(other.vectorDoubleValue);
                    break;
                case TypeTag::VECTOR2D_DOUBLE:
                    new (&vector2dDoubleValue) std::vector < std::vector<double>>(other.vector2dDoubleValue);
                    break;
                case TypeTag::CUSTOM_CLASS:
                    new (&customClassValue) CustomClass(other.customClassValue);
                    break;
                case TypeTag::TUPLE_VECTOR2D:
                    new (&tupleVector2DValue)
                        decltype(tupleVector2DValue)(other.tupleVector2DValue);
                    break;
                case TypeTag::TUPLE_VECTOR3D:
                    new (&tupleVector3DValue)
                        decltype(tupleVector3DValue)(other.tupleVector3DValue);
                    break;
            }
        }
        return *this;
    }


    TypeTag getTag() const { return tag; }


    int getInt() const {
        if (tag != TypeTag::INT) throw std::runtime_error("Type mismatch: not int");
        return intValue;
    }

    double getDouble() const {
        if (tag != TypeTag::DOUBLE) throw std::runtime_error("Type mismatch: not double");
        return doubleValue;
    }

    const std::string& getString() const {
        if (tag != TypeTag::STRING) throw std::runtime_error("Type mismatch: not string");
        return stringValue;
    }

    const std::vector<int>& getVectorInt() const {
        if (tag != TypeTag::VECTOR_INT) throw std::runtime_error("Type mismatch: not vector<int>");
        return vectorIntValue;
    }
    const std::vector<double>& getVectorDouble() const {
        if (tag != TypeTag::VECTOR_DOUBLE) throw std::runtime_error("Type mismatch: not vector<double>");
        return vectorDoubleValue;
    }
    const std::vector < std::vector<double>>& getVector2dDouble() const {
        if (tag != TypeTag::VECTOR2D_DOUBLE) throw std::runtime_error("Type mismatch: not vector<double>");
        return vector2dDoubleValue;
    }
    const CustomClass& getCustomClass() const {
        if (tag != TypeTag::CUSTOM_CLASS) throw std::runtime_error("Type mismatch: not CustomClass");
        return customClassValue;
    }

    // 新增访问函数
    const std::tuple<
        std::vector<double>,
        std::vector<double>
    >& getTupleVector2D() const {
        if (tag != TypeTag::TUPLE_VECTOR2D)
            throw std::runtime_error("Type mismatch: not tuple<vector3d>");
        return tupleVector2DValue;
    }
    const std::tuple<
        std::vector<double>,
        std::vector<double>,
        std::vector<double>
    >& getTupleVector3D() const {
        if (tag != TypeTag::TUPLE_VECTOR3D)
            throw std::runtime_error("Type mismatch: not tuple<vector3d>");
        return tupleVector3DValue;
    }
};

#endif // VARIANT_H