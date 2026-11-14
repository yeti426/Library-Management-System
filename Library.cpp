// ============================================================
//  Library.cpp  --  图书管理类的实现
//  说明：
//    1) 本层不含任何 cin，只有 displayAll() 负责输出（其它地方只写 lastError）；
//    2) 所有失败原因写入 lastError，由 UI 层决定怎么展示。
// ============================================================
#include "Library.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

namespace {

const char kSep = '\t';          // 数据文件字段分隔符（用制表符，书名含空格也不怕）
const char* kDefaultFile = "books.txt";

std::string trim(const std::string& s)
{
    std::size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

// 字段里若混入分隔符，会把文件格式搞乱，必须在入库前拦下来
bool hasSep(const std::string& s)
{
    return s.find(kSep) != std::string::npos;
}

bool toInt(const std::string& s, int& out)
{
    const std::string t = trim(s);
    if (t.empty()) return false;
    try {
        std::size_t pos = 0;
        out = std::stoi(t, &pos);
        return pos == t.size();
    } catch (...) {
        return false;
    }
}

} // namespace

// ------------------------------------------------------------
//  构造：对象一建立就把磁盘上的数据读进内存
// ------------------------------------------------------------
Library::Library() : filename(kDefaultFile)
{
    loadFromFile();
}

Library::Library(std::string file) : filename(std::move(file))
{
    if (filename.empty()) filename = kDefaultFile;
    loadFromFile();
}

// ------------------------------------------------------------
//  内部查找
// ------------------------------------------------------------
Book* Library::findByIsbn(const std::string& isbn)
{
    const std::string key = trim(isbn);
    for (std::size_t i = 0; i < books.size(); ++i) {
        if (books[i].getIsbn() == key) return &books[i];
    }
    return nullptr;
}

const Book* Library::findByIsbn(const std::string& isbn) const
{
    const std::string key = trim(isbn);
    for (std::size_t i = 0; i < books.size(); ++i) {
        if (books[i].getIsbn() == key) return &books[i];
    }
    return nullptr;
}

// ------------------------------------------------------------
//  新增图书
// ------------------------------------------------------------
bool Library::addBook(const Book& book)
{
    lastError.clear();

    if (trim(book.getIsbn()).empty())  { lastError = "图书编号不能为空"; return false; }
    if (trim(book.getTitle()).empty()) { lastError = "书名不能为空";     return false; }
    if (hasSep(book.getIsbn()) || hasSep(book.getTitle()) || hasSep(book.getAuthor())) {
        lastError = "编号/书名/作者中不能含有制表符";
        return false;
    }
    if (findByIsbn(book.getIsbn()) != nullptr) {
        lastError = "编号已存在：" + book.getIsbn();
        return false;
    }

    books.push_back(book);
    return true;
}

// ------------------------------------------------------------
//  查找
// ------------------------------------------------------------
Book* Library::searchByIsbn(const std::string& isbn)
{
    lastError.clear();
    Book* p = findByIsbn(isbn);
    if (p == nullptr) lastError = "查无此书（编号：" + trim(isbn) + "）";
    return p;
}

const Book* Library::searchByIsbn(const std::string& isbn) const
{
    const Book* p = findByIsbn(isbn);
    if (p == nullptr) lastError = "查无此书（编号：" + trim(isbn) + "）";
    return p;
}

// 按书名模糊查找：只要书名里“包含”关键字就算命中（方便用户记不全书名）
std::vector<Book> Library::searchByTitle(const std::string& title) const
{
    lastError.clear();
    std::vector<Book> result;
    const std::string key = trim(title);
    if (key.empty()) {
        lastError = "书名关键字不能为空";
        return result;
    }
    for (std::size_t i = 0; i < books.size(); ++i) {
        if (books[i].getTitle().find(key) != std::string::npos) {
            result.push_back(books[i]);
        }
    }
    if (result.empty()) lastError = "没有书名包含“" + key + "”的图书";
    return result;
}

// ------------------------------------------------------------
//  借阅 / 归还
// ------------------------------------------------------------
bool Library::borrowBook(const std::string& isbn)
{
    lastError.clear();

    Book* p = findByIsbn(isbn);
    if (p == nullptr) {
        lastError = "查无此书（编号：" + trim(isbn) + "）";
        return false;
    }
    if (p->getStock() <= 0) {
        lastError = "《" + p->getTitle() + "》已全部借出，库存不足";
        return false;
    }
    p->setStock(p->getStock() - 1);
    return true;
}

bool Library::returnBook(const std::string& isbn)
{
    lastError.clear();

    Book* p = findByIsbn(isbn);
    if (p == nullptr) {
        lastError = "查无此书（编号：" + trim(isbn) + "）";
        return false;
    }
    // 在当前库存已经等于馆藏总册数时，说明没有借出的副本，不能再“归还”
    if (p->getStock() >= p->getTotalCopies()) {
        lastError = "《" + p->getTitle() + "》库存已满（" +
                    std::to_string(p->getStock()) + "/" +
                    std::to_string(p->getTotalCopies()) + "），没有待归还的副本";
        return false;
    }
    p->setStock(p->getStock() + 1);
    return true;
}

// ------------------------------------------------------------
//  显示全部
// ------------------------------------------------------------
void Library::displayAll() const
{
    if (books.empty()) {
        std::cout << "  （书库中还没有图书）\n";
        return;
    }

    const std::string sep(77, '-');        // 表格总宽 = 14+28+20+6+3+6
    std::cout << sep << "\n";
    std::cout << Book::tableHeader() << "\n";
    std::cout << sep << "\n";
    for (std::size_t i = 0; i < books.size(); ++i) {
        std::cout << books[i].toString() << "\n";
    }
    std::cout << sep << "\n";
    std::cout << "共 " << books.size() << " 种图书，当前可借 " << totalStock() << " 本\n";
}

// ------------------------------------------------------------
//  文件：读
//  文件格式（每行一条，制表符分隔）：
//    编号 \t 书名 \t 作者 \t 库存 \t 馆藏总册数
//  为兼容旧数据，最后一项可以省略（此时总册数 = 库存）。
// ------------------------------------------------------------
void Library::loadFromFile()
{
    lastError.clear();
    books.clear();

    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        return;                     // 首次运行没有数据文件，属于正常情况
    }

    std::string line;
    int lineNo = 0;
    while (std::getline(fin, line)) {
        ++lineNo;

        // Windows 换行残留的 '\r'
        if (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1);
        if (line.empty() || line[0] == '#') continue;       // 空行、注释行

        std::string field[5];
        int n = 0;
        std::istringstream ss(line);
        while (n < 5 && std::getline(ss, field[n], kSep)) ++n;

        if (n < 4) {
            std::cerr << "[提示] " << filename << " 第 " << lineNo
                      << " 行字段不足，已跳过。\n";
            continue;
        }

        int stock = 0;
        if (!toInt(field[3], stock) || stock < 0) {
            std::cerr << "[提示] " << filename << " 第 " << lineNo
                      << " 行库存不是合法数字，已跳过。\n";
            continue;
        }

        int total = stock;
        if (n >= 5) {
            int t = 0;
            if (toInt(field[4], t) && t >= stock) total = t;
        }

        books.push_back(Book(field[0], field[1], field[2], stock, total));
    }
}

// ------------------------------------------------------------
//  文件：写（返回 true 表示成功，返回值可以忽略，不影响原有调用方式）
// ------------------------------------------------------------
bool Library::saveToFile() const
{
    lastError.clear();

    std::ofstream fout(filename.c_str(), std::ios::out | std::ios::trunc);
    if (!fout.is_open()) {
        lastError = "无法打开文件进行写入：" + filename;
        return false;
    }

    for (std::size_t i = 0; i < books.size(); ++i) {
        const Book& b = books[i];
        fout << b.getIsbn()        << kSep
             << b.getTitle()       << kSep
             << b.getAuthor()      << kSep
             << b.getStock()       << kSep
             << b.getTotalCopies() << '\n';
    }
    fout.flush();
    if (!fout.good()) {
        lastError = "写入文件过程中发生错误：" + filename;
        return false;
    }
    return true;
}

// ------------------------------------------------------------
//  辅助接口
// ------------------------------------------------------------
std::size_t Library::bookCount() const
{
    return books.size();
}

int Library::totalStock() const
{
    int sum = 0;
    for (std::size_t i = 0; i < books.size(); ++i) sum += books[i].getStock();
    return sum;
}

const std::string& Library::getLastError() const
{
    return lastError;
}

const std::string& Library::getFilename() const
{
    return filename;
}

void Library::clearLastError()
{
    lastError.clear();
}
