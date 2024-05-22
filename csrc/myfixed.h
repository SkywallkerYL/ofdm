/*************************************************************************
	> File Name: myfixed.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月20日 星期一 16时02分36秒
 ************************************************************************/
#ifndef MYFIXED
#define MYFIXED
#include "common.h"
#include<iostream>
using namespace std;
//一个定点数的结构体

//定点数位宽
#define QuantiWidth 15
//定点数小数位宽  小数位宽给太多了好像会导致误差增大
#define QuantiPoint 8
//定点数结构体
class FixedPoint {
public:
    static const int SCALE = 1 << QuantiPoint;  // QuantiPoint

    FixedPoint() : value(0) {}
    FixedPoint(int integer) : value(integer * SCALE) {}
    // 从int构造FixedPoint，不进行缩放
    FixedPoint(int v, bool) : value(v) {}

    FixedPoint(double d) : value(static_cast<int>(d * SCALE)) {}
    // 从int构造FixedPoint
    // 类型转换
    operator int() const { return value / SCALE; }
    operator double() const { return static_cast<double>(value) / SCALE; }

    // 算术运算
    FixedPoint operator+(const FixedPoint& other) const {
        FixedPoint result;
        result.value = this->value + other.value;
        return result;
    }

    FixedPoint operator-(const FixedPoint& other) const {
        FixedPoint result;
        result.value = this->value - other.value;
        return result;
    }
    //这里乘除法画一下就能理解了，
    //乘法是将两个定点数相乘，因为原来的最低位是小数，所以乘完后要移位
    //除法是将两个定点数相除，因为得到数最低位是1，所以除后要移位回去
    FixedPoint operator*(const FixedPoint& other) const {
        FixedPoint result;
        result.value = (this->value * other.value) / SCALE;
        return result;
    }

    FixedPoint operator/(const FixedPoint& other) const {
        FixedPoint result;
        result.value = (this->value * SCALE) / other.value;
        return result;
    }
    FixedPoint& operator/=(const FixedPoint& other) {
        // 这里假设你的FixedPoint类有一个转换为double的函数toDouble()
        value = (this->value * SCALE) / other.value;
        return *this;
    }
    FixedPoint& operator+=(const FixedPoint& other) {
        value += other.value;
        return *this;
    }
    FixedPoint& operator-=(const FixedPoint& other) {
        value -= other.value;
        return *this;
    }
    // 一元负号运算符
    FixedPoint operator-() const {
        FixedPoint result;
        result.value = -this->value;
        return result;
    }
    bool operator<(const FixedPoint& other) const {
        return value < other.value;
    }
    // 计算绝对值
    //FixedPoint abs() const {
    //    FixedPoint result;
    //    result.value = value < 0 ? -value : value;
    //    return result;
    //}
    // 获取value的值
    int getValue() const {
        return value;
    }

private:
    int value;  // 定点数的内部表示
};

// 在FixedPoint类外部定义abs函数
FixedPoint abs(const FixedPoint& fp) {
    int v = fp.getValue() < 0 ? -fp.getValue() : fp.getValue();
    return FixedPoint(v, true);
}
// 在FixedPoint类外部定义operator<<()运算符
std::ostream& operator<<(std::ostream& os, const FixedPoint& fp) {
    os << static_cast<double>(fp.getValue()) / FixedPoint::SCALE;
    return os;
}

#endif