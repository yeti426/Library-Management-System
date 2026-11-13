// UI.cpp —— 交互层实现：收集用户输入，调用数据层（队友A）的 Library 接口
#include "UI.h"
#include "../utils/InputUtils.h"
#include <iostream>

using namespace std;
using InputUtils::readLine;
using InputUtils::readInt;
using InputUtils::readStock;

namespace UI {

// 1. 新增图书
void handleAddBook(Library& lib) {
    cout << "\n===== 新增图书 =====" << endl;
    string isbn   = readLine("请输入编号(ISBN)：");
    string title  = readLine("请输入书名：");
    string author = readLine("请输入作者：");
    int stock     = readStock("请输入库存数量：");

    Book book(isbn, title, author, stock);
    if (lib.addBook(book)) {
        cout << ">> 新增成功！" << endl;
    } else {
        cout << ">> 新增失败：该编号已存在！" << endl;
    }
}

// 2. 查找图书（按编号或按书名）
void handleSearch(Library& lib) {
    cout << "\n===== 查找图书 =====" << endl;
    cout << "1. 按编号查找" << endl;
    cout << "2. 按书名查找" << endl;
    int choice = readInt("请选择查找方式：");

    if (choice == 1) {
        string isbn = readLine("请输入编号：");
        Book* book = lib.searchByIsbn(isbn);
        if (book != nullptr) {
            cout << ">> 找到图书：" << book->toString() << endl;
        } else {
            cout << ">> 未找到该编号的图书！" << endl;
        }
    } else if (choice == 2) {
        string title = readLine("请输入书名：");
        vector<Book> results = lib.searchByTitle(title);
        if (results.empty()) {
            cout << ">> 未找到相关图书！" << endl;
        } else {
            cout << ">> 共找到 " << results.size() << " 本相关图书：" << endl;
            for (const Book& b : results) {
                cout << "   " << b.toString() << endl;
            }
        }
    } else {
        cout << ">> 无效的选择！" << endl;
    }
}

// 3. 借阅图书
void handleBorrow(Library& lib) {
    cout << "\n===== 借阅图书 =====" << endl;
    string isbn = readLine("请输入要借阅的图书编号：");
    if (lib.borrowBook(isbn)) {
        cout << ">> 借阅成功！" << endl;
    } else {
        cout << ">> 借阅失败：图书不存在或库存不足！" << endl;
    }
}

// 4. 归还图书
void handleReturn(Library& lib) {
    cout << "\n===== 归还图书 =====" << endl;
    string isbn = readLine("请输入要归还的图书编号：");
    if (lib.returnBook(isbn)) {
        cout << ">> 归还成功！" << endl;
    } else {
        cout << ">> 归还失败：未找到该编号的图书！" << endl;
    }
}

// 5. 显示所有图书
void handleDisplayAll(Library& lib) {
    cout << "\n===== 全部图书 =====" << endl;
    lib.displayAll();
}

// 主菜单
void showMenu() {
    cout << "\n=============================================" << endl;
    cout << "        图书信息管理系统" << endl;
    cout << "=============================================" << endl;
    cout << "  1. 新增图书" << endl;
    cout << "  2. 查找图书" << endl;
    cout << "  3. 借阅图书" << endl;
    cout << "  4. 归还图书" << endl;
    cout << "  5. 显示全部" << endl;
    cout << "  6. 保存并退出" << endl;
    cout << "=============================================" << endl;
}

}
