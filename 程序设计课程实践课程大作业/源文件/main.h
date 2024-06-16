#pragma once
#include <iostream>
#include <string>//名字
#include <fstream>//文件操作

using namespace std;

// ANSI颜色代码
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// 商品信息结构体
struct Product {
    int id;
    string name;
    double price;
    string description;
    string category;
    int stock;
    Product* next;
};

// 订单信息结构体
struct OrderItem {
    int productId;
    int quantity;
    OrderItem* next;
};

struct Order {
    int orderId;
    string customerInfo;
    string orderTime;
    string status; // 已完成、未完成、取消
    OrderItem* items;
    Order* next;
};

// 顾客信息结构体
struct Customer {
    string name;
    int points;
    int level;
    Customer* next;
};

void addProduct(int id, string name, double price, string description, string category, int stock);
void displayAllProducts();
void deleteProduct(int id);
void modifyProduct(int id);
void findProduct(int id);
void addOrder(int orderId, string customerInfo, string orderTime, string status, OrderItem* items);
double applyDiscount(double amount, int level);
void createOrder();
void cancelOrder(int orderId);
void completeOrder(int orderId);
void displayAllOrders();
void findOrder(int orderId);
double calculateTotalSales(string startTime, string endTime);
void calculateProductSales(string startTime, string endTime);
void saveStatisticsToFile(string filename, string startTime, string endTime);
bool login(string username, string password);
void logout();
void addCustomer(string name, int points);
void setCustomerDiscount(string name, int points);
void setCustomerLevel(string name, int level);

