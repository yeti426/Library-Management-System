// main.cpp —— 程序入口：只负责启动与主循环调度
#include <iostream>
#include <string>
#include "Library.h"
#include "src/ui/UI.h"
#include "src/utils/InputUtils.h"

using namespace std;
using namespace UI;

int main() {
    Library lib;
    lib.loadFromFile();   // 启动时从 books.txt 加载数据

    while (true) {
        showMenu();
        int choice = InputUtils::readInt("请输入选项(1-6)：");

        switch (choice) {
            case 1: handleAddBook(lib);    break;
            case 2: handleSearch(lib);     break;
            case 3: handleBorrow(lib);     break;
            case 4: handleReturn(lib);     break;
            case 5: handleDisplayAll(lib); break;
            case 6:
                lib.saveToFile();
                cout << ">> 数据已保存，感谢使用，再见！" << endl;
                return 0;
            default:
                cout << ">> 无效选项，请输入 1-6 之间的数字！" << endl;
                break;
        }

        cout << "\n按回车键继续...";
        string tmp;
        getline(cin, tmp);
    }
}
