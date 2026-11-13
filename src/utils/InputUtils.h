// InputUtils.h —— 输入工具（处理非法输入，防止死循环）
#pragma once
#include <string>

namespace InputUtils {
    // 读取一行字符串
    std::string readLine(const std::string& prompt);

    // 读取一个整数（非法输入时清除错误状态并要求重输）
    int readInt(const std::string& prompt);

    // 读取库存数量，要求 >= 0
    int readStock(const std::string& prompt);
}
