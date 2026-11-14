// ============================================================
//  Book.h  --  图书实体类（数据层）
//  说明：只描述“一本书”应有的数据和操作，不含任何界面/IO 交互代码。
//        外部只能通过 public 接口访问数据，体现封装。
// ============================================================
#ifndef LIBRARY_BOOK_H
#define LIBRARY_BOOK_H
#pragma once

#include <string>
#include <iostream>

class Book
{
private:
    // ---------------- 数据成员（私有） ----------------
    std::string isbn;         // 图书编号（唯一，主键）
    std::string title;        // 书名
    std::string author;       // 作者
    int         stock;        // 当前可借数量
    int         totalCopies;  // 馆藏总册数（借出后 stock 减少，totalCopies 不变）
    bool        isBorrowed;   // 是否已全部借出（由 stock 自动派生，避免数据不一致）

public:
    // ---------------- 构造与析构 ----------------
    Book() = default;                                       // 默认构造（vector 需要）
    Book(std::string i, std::string t, std::string a, int s);
    Book(std::string i, std::string t, std::string a, int s, int total);

    // ---------------- Getter（只读访问） ----------------
    std::string getIsbn()        const;
    std::string getTitle()       const;
    std::string getAuthor()      const;
    int         getStock()       const;
    int         getTotalCopies() const;
    bool        isBorrowedOut()  const;   // 是否已全部借出
    bool        canBorrow()      const;   // 是否还有可借副本

    // ---------------- Setter（只开放必要的一个） ----------------
    void setStock(int s);                 // 修改库存，同时同步 isBorrowed

    // ---------------- 显示 ----------------
    std::string        toString() const;  // 格式化成一行对齐的文本
    static std::string tableHeader();     // 与 toString 严格对齐的表头

    // ---------------- 运算符重载（OOP 特性，报告里可写） ----------------
    bool operator==(const Book& rhs) const;         // 编号相同即视为同一本书
    bool operator==(const std::string& rhs) const;  // 支持 book == "ISBN"
    bool operator<(const Book& rhs) const;          // 按编号排序，可用于 std::sort
    friend std::ostream& operator<<(std::ostream& os, const Book& b);
};

#endif // LIBRARY_BOOK_H
