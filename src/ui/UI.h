// UI.h —— 交互层：菜单展示与各功能的用户交互
#pragma once
#include "Library.h"

namespace UI {
    // 显示主菜单
    void showMenu();

    // 各菜单项对应的处理函数
    void handleAddBook(Library& lib);      // 1. 新增图书
    void handleSearch(Library& lib);       // 2. 查找图书
    void handleBorrow(Library& lib);       // 3. 借阅图书
    void handleReturn(Library& lib);       // 4. 归还图书
    void handleDisplayAll(Library& lib);   // 5. 显示全部
}
