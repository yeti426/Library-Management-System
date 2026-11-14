// ============================================================
//  Book.cpp  --  图书实体类的实现
// ============================================================
#include "Book.h"
#include <sstream>

// ------------------------------------------------------------
//  匿名命名空间：只在本文件内可见的辅助函数
//  作用：按“显示宽度”对齐中文（UTF-8 下一个汉字占 3 字节、但只占 2 列）
// ------------------------------------------------------------
namespace {

const int kIsbnWidth   = 14;   // 编号列宽（显示列）
const int kTitleWidth  = 28;   // 书名列宽
const int kAuthorWidth = 20;   // 作者列宽
const int kStockWidth  = 6;    // 库存列宽

// 判断一个 UTF-8 首字节对应的字符长度
int utf8CharLen(unsigned char c)
{
    if (c < 0x80)          return 1;
    if ((c >> 5) == 0x06)  return 2;
    if ((c >> 4) == 0x0E)  return 3;
    if ((c >> 3) == 0x1E)  return 4;
    return 1;              // 非法字节或 GBK 双字节，按 1 处理
}

// 解码一个 UTF-8 字符的 Unicode 码点
unsigned int utf8Decode(const std::string& s, std::size_t pos, int len)
{
    unsigned int cp = 0;
    unsigned char c = static_cast<unsigned char>(s[pos]);
    switch (len) {
        case 2:  cp = c & 0x1F; break;
        case 3:  cp = c & 0x0F; break;
        case 4:  cp = c & 0x07; break;
        default: cp = c;        break;
    }
    for (int k = 1; k < len; ++k) {
        cp = (cp << 6) | (static_cast<unsigned char>(s[pos + k]) & 0x3F);
    }
    return cp;
}

// 码点占几个显示列：中日韩文字与全角符号占 2 列，其余占 1 列
int cpWidth(unsigned int cp)
{
    if (cp >= 0x1100 &&
        (cp <= 0x115F || cp == 0x2329 || cp == 0x232A ||
         (cp >= 0x2E80 && cp <= 0xA4CF && cp != 0x303F) ||
         (cp >= 0xAC00 && cp <= 0xD7A3) ||
         (cp >= 0xF900 && cp <= 0xFAFF) ||
         (cp >= 0xFE30 && cp <= 0xFE6F) ||
         (cp >= 0xFF00 && cp <= 0xFF60) ||
         (cp >= 0xFFE0 && cp <= 0xFFE6))) {
        return 2;
    }
    return 1;
}

// 字符串的显示宽度
int strWidth(const std::string& s)
{
    int w = 0;
    std::size_t i = 0;
    while (i < s.size()) {
        int len = utf8CharLen(static_cast<unsigned char>(s[i]));
        if (i + static_cast<std::size_t>(len) > s.size()) { w += 1; break; }
        w += cpWidth(utf8Decode(s, i, len));
        i += static_cast<std::size_t>(len);
    }
    return w;
}

// 把 s 补齐/截断到恰好 width 个显示列（左侧对齐）
std::string fit(const std::string& s, int width)
{
    const int w = strWidth(s);
    if (w <= width) {
        return s + std::string(static_cast<std::size_t>(width - w), ' ');
    }
    // 太长：按显示宽度截断，末尾补上 "..."
    std::string out;
    int used = 0;
    std::size_t i = 0;
    while (i < s.size()) {
        int len = utf8CharLen(static_cast<unsigned char>(s[i]));
        if (i + static_cast<std::size_t>(len) > s.size()) break;
        int cw = cpWidth(utf8Decode(s, i, len));
        if (used + cw > width - 3) break;
        out.append(s, i, static_cast<std::size_t>(len));
        used += cw;
        i += static_cast<std::size_t>(len);
    }
    out += "...";
    used += 3;
    if (used < width) out.append(static_cast<std::size_t>(width - used), ' ');
    return out;
}

} // namespace

// ------------------------------------------------------------
//  构造
// ------------------------------------------------------------
Book::Book(std::string i, std::string t, std::string a, int s)
    : isbn(std::move(i)), title(std::move(t)), author(std::move(a)),
      stock(s < 0 ? 0 : s), totalCopies(s < 0 ? 0 : s), isBorrowed(s <= 0)
{
}

Book::Book(std::string i, std::string t, std::string a, int s, int total)
    : isbn(std::move(i)), title(std::move(t)), author(std::move(a)),
      stock(s < 0 ? 0 : s), totalCopies(total < 0 ? 0 : total), isBorrowed(s <= 0)
{
    // 数据自检：总册数不可能少于当前库存
    if (totalCopies < stock) totalCopies = stock;
}

// ------------------------------------------------------------
//  Getter
// ------------------------------------------------------------
std::string Book::getIsbn()        const { return isbn; }
std::string Book::getTitle()       const { return title; }
std::string Book::getAuthor()      const { return author; }
int         Book::getStock()       const { return stock; }
int         Book::getTotalCopies() const { return totalCopies; }
bool        Book::isBorrowedOut()  const { return isBorrowed; }
bool        Book::canBorrow()      const { return stock > 0; }

// ------------------------------------------------------------
//  Setter：库存为负时自动归零，并同步“是否全部借出”标记
// ------------------------------------------------------------
void Book::setStock(int s)
{
    stock = (s < 0) ? 0 : s;
    if (stock > totalCopies) totalCopies = stock;   // 保持数据自洽
    isBorrowed = (stock == 0);
}

// ------------------------------------------------------------
//  显示：一行对齐的文本
// ------------------------------------------------------------
std::string Book::toString() const
{
    std::string out;
    out += fit(isbn,   kIsbnWidth);
    out += fit(title,  kTitleWidth);
    out += fit(author, kAuthorWidth);

    // 库存列右对齐
    std::string num = std::to_string(stock);
    if (static_cast<int>(num.size()) < kStockWidth) {
        out.append(static_cast<std::size_t>(kStockWidth - static_cast<int>(num.size())), ' ');
    }
    out += num;

    out += "   ";
    out += fit(stock > 0 ? "可借" : "已借完", 6);
    return out;
}

std::string Book::tableHeader()
{
    std::string h;
    h += fit("ISBN/编号", kIsbnWidth);
    h += fit("书名",      kTitleWidth);
    h += fit("作者",      kAuthorWidth);
    h += fit("",          kStockWidth - 4);   // 让“库存”二字右对齐到数字列
    h += "库存";
    h += "   状态";
    return h;
}

// ------------------------------------------------------------
//  运算符重载
// ------------------------------------------------------------
bool Book::operator==(const Book& rhs) const
{
    return isbn == rhs.isbn;
}

bool Book::operator==(const std::string& rhs) const
{
    return isbn == rhs;
}

bool Book::operator<(const Book& rhs) const
{
    return isbn < rhs.isbn;     // 按编号字典序，可用于 std::sort
}

std::ostream& operator<<(std::ostream& os, const Book& b)
{
    os << b.toString();
    return os;
}
