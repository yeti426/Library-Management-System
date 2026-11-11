// Library.h
#pragma once
#include <vector>
#include "Book.h"

class Library {
private:
    std::vector<Book> books;
    const std::string filename = "books.txt"; // 保存数据的文件名

public:
    Library();
    
    // 核心业务逻辑接口 (由队友A实现)
    bool addBook(const Book& book);                 // 新增图书
    Book* searchByIsbn(const std::string& isbn);    // 按编号查找 (返回指针方便修改)
    std::vector<Book> searchByTitle(const std::string& title); // 按书名查找
    bool borrowBook(const std::string& isbn);       // 借阅 (库存-1)
    bool returnBook(const std::string& isbn);       // 归还 (库存+1)
    void displayAll() const;                        // 显示所有图书 (返回给UI层打印)
    
    // 文件操作
    void loadFromFile();
    void saveToFile() const;
};