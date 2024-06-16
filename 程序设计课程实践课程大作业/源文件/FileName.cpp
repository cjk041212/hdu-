#include"main.h"

// 主函数
int main() {
    string username, password;
    cout << YELLOW << "请输入用户名: " << RESET;
    getline(cin, username);
    cout << YELLOW<<"请输入密码: " << RESET;
    getline(cin, password);

    if (!login(username, password)) {
        cout << RED << "用户名或密码错误！" << endl << RESET;
        return 0;
    }

    int choice;
    do {
        cout << YELLOW << "-----------------------------" << endl;
        cout << "1. 添加商品" << endl;
        cout << "2. 显示所有商品" << endl;
        cout << "3. 删除商品" << endl;
        cout << "4. 修改商品" << endl;
        cout << "5. 查找商品" << endl;
        cout << "6. 创建订单" << endl;
        cout << "7. 取消订单" << endl;
        cout << "8. 完成订单" << endl;
        cout << "9. 显示所有订单" << endl;
        cout << "10. 查找订单" << endl;
        cout << "11. 统计销售额" << endl;
        cout << "12. 统计商品销售情况" << endl;
        cout << "13. 保存统计信息到文件" << endl;
        cout << "14. 添加顾客" << endl;
        cout << "15. 设置顾客积分" << endl;
        cout << "16. 设置顾客星级" << endl;
        cout << "17. 退出" << endl;
        cout << YELLOW << "-----------------------------" << endl << RESET;

        cout << GREEN << "请输入选择: " << RESET;
        cin >> choice; getchar();
        switch (choice) {
        case 1: {
            cout << BLUE;
            int id, stock;
            string name, description, category;
            double price;
            cout << "输入商品编号: ";
            cin >> id; getchar();
            cout << "输入商品名称: ";
            getline(cin, name);
            cout << "输入商品价格: ";
            cin >> price; getchar();
            cout << "输入商品描述: ";
            getline(cin,description);
            cout << "输入商品分类: ";
            getline(cin, category);
            cout << "输入商品库存: ";
            cin >> stock; getchar();
            addProduct(id, name, price, description, category, stock);
            break;
        }
        case 2:
            cout << BLUE;
            displayAllProducts();
            break;
        case 3: {
            cout << BLUE;
            int id;
            cout << "输入要删除的商品编号: ";
            cin >> id; getchar();
            deleteProduct(id);
            break;
        }
        case 4: {
            cout << BLUE;
            int id;
            cout << "输入要修改的商品编号: ";
            cin >> id; getchar();
            modifyProduct(id);
            break;
        }
        case 5: {
            cout << BLUE;
            int id;
            cout << "输入要查找的商品编号: ";
            cin >> id; getchar();
            findProduct(id);
            break;
        }
        case 6:
            cout << CYAN;
            createOrder();
            break;
        case 7: {
            cout << CYAN;
            int orderId;
            cout << "输入要取消的订单编号: ";
            cin >> orderId; getchar();
            cancelOrder(orderId);
            break;
        }
        case 8: {
            cout << CYAN;
            int orderId;
            cout << "输入要完成的订单编号: ";
            cin >> orderId; getchar();
            completeOrder(orderId);
            break;
        }
        case 9:
            cout << CYAN;
            displayAllOrders();
            break;
        case 10: {
            cout << CYAN;
            int orderId;
            cout << "输入要查找的订单编号: ";
            cin >> orderId; getchar();
            findOrder(orderId);
            break;
        }
        case 11: {
            cout << RED;
            string startTime, endTime;
            cout << "输入开始时间: ";
            getline(cin, startTime);
            cout << "输入结束时间: ";
            getline(cin, endTime);
            double totalSales = calculateTotalSales(startTime, endTime);
            cout << "总销售额: " << totalSales << endl;
            break;
        }
        case 12: {
            cout << RED;
            string startTime, endTime;
            cout << "输入开始时间: ";
            getline(cin, startTime);
            cout << "输入结束时间: ";
            getline(cin, endTime);
            calculateProductSales(startTime, endTime);
            break;
        }
        case 13: {
            cout << RED;
            string filename, startTime, endTime;
            cout << "输入文件名: ";
            getline(cin, filename);
            cout << "输入开始时间: ";
            getline(cin, startTime);
            cout << "输入结束时间: ";
            getline(cin, endTime);
            saveStatisticsToFile(filename, startTime, endTime);
            break;
        }
        case 14: {
            cout << MAGENTA;
            string name;
            int points;
            cout << "输入顾客名称: ";
            getline(cin, name);
            cout << "输入顾客积分: ";
            cin >> points; getchar();
            addCustomer(name, points);
            break;
        }
        case 15: {
            cout << MAGENTA;
            string name;
            int points;
            cout << "输入顾客名称: ";
            getline(cin, name);
            cout << "输入顾客积分: ";
            cin >> points; getchar();
            setCustomerDiscount(name, points);
            break;
        }
        case 16: {
            cout << MAGENTA;
            string name;
            int level;
            cout << "输入顾客名称: ";
            getline(cin, name);
            cout << "输入顾客星级 (1-3): ";
            cin >> level; getchar();
            setCustomerLevel(name, level);
            break;
        }
        case 17:
            logout();
            break;
        default:
            cout << "无效的选择！" << endl;
            break;
        }
        cout << RESET << endl;
    } while (choice != 17);

    return 0;
}
