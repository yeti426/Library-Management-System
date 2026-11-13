// InputUtils.cpp —— 输入工具实现
#include "InputUtils.h"
#include <iostream>
#include <limits>

using namespace std;

namespace InputUtils {

string readLine(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 吃掉行尾换行
            return value;
        }
        cout << "输入无效，请输入一个整数！" << endl;
        cin.clear();                                            // 清除错误标志
        cin.ignore(numeric_limits<streamsize>::max(), '\n');    // 丢弃非法输入
    }
}

int readStock(const string& prompt) {
    int stock;
    while (true) {
        stock = readInt(prompt);
        if (stock >= 0) return stock;
        cout << "库存不能为负数，请重新输入！" << endl;
    }
}

}
