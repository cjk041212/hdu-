#include"main.h"

// 全局变量
Product* headProduct = nullptr; // 商品链表头指针
Order* headOrder = nullptr; // 订单链表头指针
Customer* headCustomer = nullptr; // 顾客链表头指针

void addProduct(int id, string name, double price, string description, string category, int stock) {
    Product* newProduct = new Product{ id, name, price, description, category, stock, nullptr };
    if (headProduct == nullptr) {
        headProduct = newProduct;
    }
    else {
        Product* temp = headProduct;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newProduct;
    }
    cout << "商品添加成功！" << endl;
}

void displayAllProducts() {
    Product* temp = headProduct;
    while (temp != nullptr) {
        cout << "商品编号: " << temp->id << endl;
        cout << "名称: " << temp->name << endl;
        cout << "价格: " << temp->price << endl;
        cout << "描述: " << temp->description << endl;
        cout << "分类: " << temp->category << endl;
        cout << "库存: " << temp->stock << endl;
        cout << "-----------------------------" << endl;
        temp = temp->next;
    }
}

void deleteProduct(int id) {
    Product* temp = headProduct;
    Product* prev = nullptr;
    while (temp != nullptr && temp->id != id) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "未找到商品！" << endl;
        return;
    }
    if (prev == nullptr) {
        headProduct = temp->next;
    }
    else {
        prev->next = temp->next;
    }
    delete temp;
    cout << "商品删除成功！" << endl;
}

void modifyProduct(int id) {
    Product* temp = headProduct;
    while (temp != nullptr && temp->id != id) {
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "未找到商品！" << endl;
        return;
    }
    cout << "输入新的商品名称: ";
    getline(cin, temp->name);
    cout << "输入新的商品价格: ";
    cin >> temp->price; getchar();
    cout << "输入新的商品描述: ";
    getline(cin, temp->description);
    cout << "输入新的商品分类: ";
    getline(cin, temp->category);
    cout << "输入新的商品库存: ";
    cin >> temp->stock; getchar();
    cout << "商品信息修改成功！" << endl;
}

void findProduct(int id) {
    Product* temp = headProduct;
    while (temp != nullptr && temp->id != id) {
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "未找到商品！" << endl;
        return;
    }
    cout << "商品编号: " << temp->id << endl;
    cout << "名称: " << temp->name << endl;
    cout << "价格: " << temp->price << endl;
    cout << "描述: " << temp->description << endl;
    cout << "分类: " << temp->category << endl;
    cout << "库存: " << temp->stock << endl;
}

// 订单信息管理功能
void addOrder(int orderId, string customerInfo, string orderTime, string status, OrderItem* items) {
    Order* newOrder = new Order{ orderId, customerInfo, orderTime, status, items, nullptr };
    if (headOrder == nullptr) {
        headOrder = newOrder;
    }
    else {
        Order* temp = headOrder;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newOrder;
    }
    cout << "订单创建成功！" << endl;
}

double applyDiscount(double amount, int level) {
    double discount = 0.0;
    switch (level) {
    case 1:
        discount = 0.95; // 5%折扣
        break;
    case 2:
        discount = 0.90; // 10%折扣
        break;
    case 3:
        discount = 0.85; // 15%折扣
        break;
    default:
        discount = 1.0; // 没有折扣
        break;
    }
    return amount * discount;
}

void createOrder() {
    int orderId;
    string customerName, orderTime;
    cout << "输入订单编号: ";
    cin >> orderId; getchar();
    cout << "输入顾客姓名: ";
    getline(cin, customerName);
    cout << "输入订单时间: ";
    getline(cin, orderTime);

    Customer* customerTemp = headCustomer;
    while (customerTemp != nullptr && customerTemp->name != customerName) {
        customerTemp = customerTemp->next;
    }
    if (customerTemp == nullptr) {
        cout << "未找到顾客！" << endl;
        return;
    }
    int customerLevel = customerTemp->level;

    Order* newOrder = new Order{ orderId, customerName, orderTime, "未完成", nullptr, nullptr };
    int productId, quantity;
    char choice;
    do {
        cout << "输入商品编号: ";
        cin >> productId; getchar();
        cout << "输入商品数量: ";
        cin >> quantity; getchar();
        OrderItem* newItem = new OrderItem{ productId, quantity, nullptr };
        if (newOrder->items == nullptr) {
            newOrder->items = newItem;
        }
        else {
            OrderItem* temp = newOrder->items;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newItem;
        }
        cout << "是否继续添加商品？(y/n): ";
        cin >> choice;
    } while (choice == 'y');

    double totalAmount = 0.0;
    OrderItem* temp = newOrder->items;
    while (temp != nullptr) {
        Product* productTemp = headProduct;
        while (productTemp != nullptr && productTemp->id != temp->productId) {
            productTemp = productTemp->next;
        }
        if (productTemp != nullptr) {
            totalAmount += productTemp->price * temp->quantity;
        }
        temp = temp->next;
    }

    double discountedAmount = applyDiscount(totalAmount, customerLevel);
    cout << "原价: " << totalAmount << "元，折后价: " << discountedAmount << "元" << endl;

    if (headOrder == nullptr) {
        headOrder = newOrder;
    }
    else {
        Order* orderTemp = headOrder;
        while (orderTemp->next != nullptr) {
            orderTemp = orderTemp->next;
        }
        orderTemp->next = newOrder;
    }
    cout << "订单创建成功！" << endl;
}

void cancelOrder(int orderId) {
    Order* temp = headOrder;
    while (temp != nullptr && temp->orderId != orderId) {
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "未找到订单！" << endl;
        return;
    }
    temp->status = "取消";
    cout << "订单取消成功！" << endl;
}

void completeOrder(int orderId) {
    Order* temp = headOrder;
    while (temp != nullptr && temp->orderId != orderId) {
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "未找到订单！" << endl;
        return;
    }
    temp->status = "已完成";
    cout << "订单完成成功！" << endl;
}

void displayAllOrders() {
    Order* temp = headOrder;
    while (temp != nullptr) {
        cout << "订单编号: " << temp->orderId << endl;
        cout << "顾客信息: " << temp->customerInfo << endl;
        cout << "订单时间: " << temp->orderTime << endl;
        cout << "订单状态: " << temp->status << endl;
        OrderItem* itemTemp = temp->items;
        while (itemTemp != nullptr) {
            cout << "商品编号: " << itemTemp->productId << ", 数量: " << itemTemp->quantity << endl;
            itemTemp = itemTemp->next;
        }
        cout << "-----------------------------" << endl;
        temp = temp->next;
    }
}

void findOrder(int orderId) {
    Order* temp = headOrder;
    while (temp != nullptr && temp->orderId != orderId) {
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "未找到订单！" << endl;
        return;
    }
    cout << "订单编号: " << temp->orderId << endl;
    cout << "顾客信息: " << temp->customerInfo << endl;
    cout << "订单时间: " << temp->orderTime << endl;
    cout << "订单状态: " << temp->status << endl;
    OrderItem* itemTemp = temp->items;
    while (itemTemp != nullptr) {
        cout << "商品编号: " << itemTemp->productId << ", 数量: " << itemTemp->quantity << endl;
        itemTemp = itemTemp->next;
    }
}

// 财务统计功能
double calculateTotalSales(string startTime, string endTime) {
    Order* temp = headOrder;
    double totalSales = 0.0;
    while (temp != nullptr) {
        if (temp->orderTime >= startTime && temp->orderTime <= endTime && temp->status == "已完成") {
            OrderItem* itemTemp = temp->items;
            while (itemTemp != nullptr) {
                Product* productTemp = headProduct;
                while (productTemp != nullptr && productTemp->id != itemTemp->productId) {
                    productTemp = productTemp->next;
                }
                if (productTemp != nullptr) {
                    totalSales += productTemp->price * itemTemp->quantity;
                }
                itemTemp = itemTemp->next;
            }
        }
        temp = temp->next;
    }
    return totalSales;
}

void calculateProductSales(string startTime, string endTime) {
    struct ProductSales {
        int productId;
        int totalQuantity;
        double totalSales;
        ProductSales* next;
    };
    ProductSales* headProductSales = nullptr;

    Order* temp = headOrder;
    while (temp != nullptr) {
        if (temp->orderTime >= startTime && temp->orderTime <= endTime && temp->status == "已完成") {
            OrderItem* itemTemp = temp->items;
            while (itemTemp != nullptr) {
                Product* productTemp = headProduct;
                while (productTemp != nullptr && productTemp->id != itemTemp->productId) {
                    productTemp = productTemp->next;
                }
                if (productTemp != nullptr) {
                    ProductSales* salesTemp = headProductSales;
                    while (salesTemp != nullptr && salesTemp->productId != itemTemp->productId) {
                        salesTemp = salesTemp->next;
                    }
                    if (salesTemp == nullptr) {
                        ProductSales* newSales = new ProductSales{ itemTemp->productId, itemTemp->quantity, productTemp->price * itemTemp->quantity, nullptr };
                        if (headProductSales == nullptr) {
                            headProductSales = newSales;
                        }
                        else {
                            ProductSales* temp = headProductSales;
                            while (temp->next != nullptr) {
                                temp = temp->next;
                            }
                            temp->next = newSales;
                        }
                    }
                    else {
                        salesTemp->totalQuantity += itemTemp->quantity;
                        salesTemp->totalSales += productTemp->price * itemTemp->quantity;
                    }
                }
                itemTemp = itemTemp->next;
            }
        }
        temp = temp->next;
    }

    ProductSales* salesTemp = headProductSales;
    while (salesTemp != nullptr) {
        cout << "商品编号: " << salesTemp->productId << ", 总数量: " << salesTemp->totalQuantity << ", 总销售额: " << salesTemp->totalSales << endl;
        salesTemp = salesTemp->next;
    }
}

void saveStatisticsToFile(string filename, string startTime, string endTime) {
    ofstream file;
    file.open(filename);

    double totalSales = calculateTotalSales(startTime, endTime);
    file << "总销售额: " << totalSales << endl;

    file << "各商品销售情况:" << endl;
    struct ProductSales {
        int productId;
        int totalQuantity;
        double totalSales;
        ProductSales* next;
    };
    ProductSales* headProductSales = nullptr;

    Order* temp = headOrder;
    while (temp != nullptr) {
        if (temp->orderTime >= startTime && temp->orderTime <= endTime && temp->status == "已完成") {
            OrderItem* itemTemp = temp->items;
            while (itemTemp != nullptr) {
                Product* productTemp = headProduct;
                while (productTemp != nullptr && productTemp->id != itemTemp->productId) {
                    productTemp = productTemp->next;
                }
                if (productTemp != nullptr) {
                    ProductSales* salesTemp = headProductSales;
                    while (salesTemp != nullptr && salesTemp->productId != itemTemp->productId) {
                        salesTemp = salesTemp->next;
                    }
                    if (salesTemp == nullptr) {
                        ProductSales* newSales = new ProductSales{ itemTemp->productId, itemTemp->quantity, productTemp->price * itemTemp->quantity, nullptr };
                        if (headProductSales == nullptr) {
                            headProductSales = newSales;
                        }
                        else {
                            ProductSales* temp = headProductSales;
                            while (temp->next != nullptr) {
                                temp = temp->next;
                            }
                            temp->next = newSales;
                        }
                    }
                    else {
                        salesTemp->totalQuantity += itemTemp->quantity;
                        salesTemp->totalSales += productTemp->price * itemTemp->quantity;
                    }
                }
                itemTemp = itemTemp->next;
            }
        }
        temp = temp->next;
    }

    ProductSales* salesTemp = headProductSales;
    while (salesTemp != nullptr) {
        file << "商品编号: " << salesTemp->productId << ", 总数量: " << salesTemp->totalQuantity << ", 总销售额: " << salesTemp->totalSales << endl;
        salesTemp = salesTemp->next;
    }

    file.close();
}

// 其他功能
bool login(string username, string password) {
    // 假设存在用户名和密码的静态数据
    if (username == "admin" && password == "admin") {
        return true;
    }
    else {
        return false;
    }
}

void logout() {
    cout << "已退出系统。" << endl;
}

void addCustomer(string name, int points) {
    Customer* newCustomer = new Customer{ name, points, 0,nullptr };
    if (headCustomer == nullptr) {
        headCustomer = newCustomer;
    }
    else {
        Customer* temp = headCustomer;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newCustomer;
    }
}

void setCustomerDiscount(string name, int points) {
    Customer* temp = headCustomer;
    while (temp != nullptr && temp->name != name) {
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "未找到顾客！" << endl;
        return;
    }
    temp->points = points;
    cout << "顾客积分设置成功！" << endl;
}

void setCustomerLevel(string name, int level) {
    Customer* temp = headCustomer;
    while (temp != nullptr && temp->name != name) {
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "未找到顾客！" << endl;
        return;
    }
    temp->level = level;
    cout << "顾客星级设置成功！" << endl;
}
