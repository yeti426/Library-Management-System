// ============================================================
//  Library.h  --  图书管理类（业务逻辑层 / 数据管理层）
//  说明：内部用 vector<Book> 存放全部图书，对外只提供成员函数接口；
//        数据私有，UI 层（队友 B）拿不到 vector，只能调用下面的方法。
// ============================================================
#ifndef LIBRARY_LIBRARY_H
#define LIBRARY_LIBRARY_H
#pragma once

#include <string>
#include <vector>
#include "Book.h"

class Library
{
private:
    // ---------------- 数据成员（私有） ----------------
    std::vector<Book> books;        // 全部图书
    std::string       filename;     // 数据文件名，默认 "books.txt"
    mutable std::string lastError;  // 最近一次失败的原因，供 UI 层打印

    // 内部查找：找不到返回 nullptr
    Book*       findByIsbn(const std::string& isbn);
    const Book* findByIsbn(const std::string& isbn) const;

public:
    // ---------------- 构造：构造时自动从文件加载 ----------------
    Library();                                // 使用默认文件 books.txt
    explicit Library(std::string file);       // 指定数据文件（测试用）

    // ---------------- 核心业务逻辑 ----------------
    bool addBook(const Book& book);                                   // 新增图书（编号重复则失败）
    Book* searchByIsbn(const std::string& isbn);                      // 按编号查找（指针便于修改）
    const Book* searchByIsbn(const std::string& isbn) const;          // const 版本
    std::vector<Book> searchByTitle(const std::string& title) const;  // 按书名模糊查找
    bool borrowBook(const std::string& isbn);                         // 借阅（库存 -1）
    bool returnBook(const std::string& isbn);                         // 归还（库存 +1）
    void displayAll() const;                                          // 打印全部图书

    // ---------------- 文件操作 ----------------
    void loadFromFile();          // 从 filename 读取；文件不存在视为空库
    bool saveToFile() const;      // 写入 filename，成功返回 true（返回值可忽略）

    // ---------------- 辅助接口（供 UI 层使用） ----------------
    std::size_t        bookCount()     const;   // 图书种类数
    int                totalStock()    const;   // 当前可借总册数
    const std::string& getLastError()  const;   // 上一次操作失败的原因
    const std::string& getFilename()   const;   // 数据文件名
    void               clearLastError();
};

#endif // LIBRARY_LIBRARY_H
