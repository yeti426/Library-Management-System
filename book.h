// Book.h
#pragma once
#include <string>

class Book {
private:
    std::string isbn;      // 编号
    std::string title;     // 书名
    std::string author;    // 作者
    int stock;             // 库存
    bool isBorrowed;       // 借阅状态 (可以简化，或者用库存判断)

public:
    Book() = default;
    Book(std::string i, std::string t, std::string a, int s);
    
    // 提供Getter和Setter供队友B使用
    std::string getIsbn() const;
    std::string getTitle() const;
    std::string getAuthor() const;
    int getStock() const;
    void setStock(int s);
    
    // 打印单本书的信息（为了方便，直接写成文本格式）
    std::string toString() const;
};