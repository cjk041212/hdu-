#pragma once
#include <iostream>
#include <string>//����
#include <fstream>//�ļ�����

using namespace std;

// ANSI��ɫ����
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// ��Ʒ��Ϣ�ṹ��
struct Product {
    int id;
    string name;
    double price;
    string description;
    string category;
    int stock;
    Product* next;
};

// ������Ϣ�ṹ��
struct OrderItem {
    int productId;
    int quantity;
    OrderItem* next;
};

struct Order {
    int orderId;
    string customerInfo;
    string orderTime;
    string status; // ����ɡ�δ��ɡ�ȡ��
    OrderItem* items;
    Order* next;
};

// �˿���Ϣ�ṹ��
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

