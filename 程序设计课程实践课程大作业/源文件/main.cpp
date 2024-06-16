#include"main.h"

// ȫ�ֱ���
Product* headProduct = nullptr; // ��Ʒ����ͷָ��
Order* headOrder = nullptr; // ��������ͷָ��
Customer* headCustomer = nullptr; // �˿�����ͷָ��

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
    cout << "��Ʒ��ӳɹ���" << endl;
}

void displayAllProducts() {
    Product* temp = headProduct;
    while (temp != nullptr) {
        cout << "��Ʒ���: " << temp->id << endl;
        cout << "����: " << temp->name << endl;
        cout << "�۸�: " << temp->price << endl;
        cout << "����: " << temp->description << endl;
        cout << "����: " << temp->category << endl;
        cout << "���: " << temp->stock << endl;
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
        cout << "δ�ҵ���Ʒ��" << endl;
        return;
    }
    if (prev == nullptr) {
        headProduct = temp->next;
    }
    else {
        prev->next = temp->next;
    }
    delete temp;
    cout << "��Ʒɾ���ɹ���" << endl;
}

void modifyProduct(int id) {
    Product* temp = headProduct;
    while (temp != nullptr && temp->id != id) {
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "δ�ҵ���Ʒ��" << endl;
        return;
    }
    cout << "�����µ���Ʒ����: ";
    getline(cin, temp->name);
    cout << "�����µ���Ʒ�۸�: ";
    cin >> temp->price; getchar();
    cout << "�����µ���Ʒ����: ";
    getline(cin, temp->description);
    cout << "�����µ���Ʒ����: ";
    getline(cin, temp->category);
    cout << "�����µ���Ʒ���: ";
    cin >> temp->stock; getchar();
    cout << "��Ʒ��Ϣ�޸ĳɹ���" << endl;
}

void findProduct(int id) {
    Product* temp = headProduct;
    while (temp != nullptr && temp->id != id) {
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "δ�ҵ���Ʒ��" << endl;
        return;
    }
    cout << "��Ʒ���: " << temp->id << endl;
    cout << "����: " << temp->name << endl;
    cout << "�۸�: " << temp->price << endl;
    cout << "����: " << temp->description << endl;
    cout << "����: " << temp->category << endl;
    cout << "���: " << temp->stock << endl;
}

// ������Ϣ������
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
    cout << "���������ɹ���" << endl;
}

double applyDiscount(double amount, int level) {
    double discount = 0.0;
    switch (level) {
    case 1:
        discount = 0.95; // 5%�ۿ�
        break;
    case 2:
        discount = 0.90; // 10%�ۿ�
        break;
    case 3:
        discount = 0.85; // 15%�ۿ�
        break;
    default:
        discount = 1.0; // û���ۿ�
        break;
    }
    return amount * discount;
}

void createOrder() {
    int orderId;
    string customerName, orderTime;
    cout << "���붩�����: ";
    cin >> orderId; getchar();
    cout << "����˿�����: ";
    getline(cin, customerName);
    cout << "���붩��ʱ��: ";
    getline(cin, orderTime);

    Customer* customerTemp = headCustomer;
    while (customerTemp != nullptr && customerTemp->name != customerName) {
        customerTemp = customerTemp->next;
    }
    if (customerTemp == nullptr) {
        cout << "δ�ҵ��˿ͣ�" << endl;
        return;
    }
    int customerLevel = customerTemp->level;

    Order* newOrder = new Order{ orderId, customerName, orderTime, "δ���", nullptr, nullptr };
    int productId, quantity;
    char choice;
    do {
        cout << "������Ʒ���: ";
        cin >> productId; getchar();
        cout << "������Ʒ����: ";
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
        cout << "�Ƿ���������Ʒ��(y/n): ";
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
    cout << "ԭ��: " << totalAmount << "Ԫ���ۺ��: " << discountedAmount << "Ԫ" << endl;

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
    cout << "���������ɹ���" << endl;
}

void cancelOrder(int orderId) {
    Order* temp = headOrder;
    while (temp != nullptr && temp->orderId != orderId) {
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "δ�ҵ�������" << endl;
        return;
    }
    temp->status = "ȡ��";
    cout << "����ȡ���ɹ���" << endl;
}

void completeOrder(int orderId) {
    Order* temp = headOrder;
    while (temp != nullptr && temp->orderId != orderId) {
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "δ�ҵ�������" << endl;
        return;
    }
    temp->status = "�����";
    cout << "������ɳɹ���" << endl;
}

void displayAllOrders() {
    Order* temp = headOrder;
    while (temp != nullptr) {
        cout << "�������: " << temp->orderId << endl;
        cout << "�˿���Ϣ: " << temp->customerInfo << endl;
        cout << "����ʱ��: " << temp->orderTime << endl;
        cout << "����״̬: " << temp->status << endl;
        OrderItem* itemTemp = temp->items;
        while (itemTemp != nullptr) {
            cout << "��Ʒ���: " << itemTemp->productId << ", ����: " << itemTemp->quantity << endl;
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
        cout << "δ�ҵ�������" << endl;
        return;
    }
    cout << "�������: " << temp->orderId << endl;
    cout << "�˿���Ϣ: " << temp->customerInfo << endl;
    cout << "����ʱ��: " << temp->orderTime << endl;
    cout << "����״̬: " << temp->status << endl;
    OrderItem* itemTemp = temp->items;
    while (itemTemp != nullptr) {
        cout << "��Ʒ���: " << itemTemp->productId << ", ����: " << itemTemp->quantity << endl;
        itemTemp = itemTemp->next;
    }
}

// ����ͳ�ƹ���
double calculateTotalSales(string startTime, string endTime) {
    Order* temp = headOrder;
    double totalSales = 0.0;
    while (temp != nullptr) {
        if (temp->orderTime >= startTime && temp->orderTime <= endTime && temp->status == "�����") {
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
        if (temp->orderTime >= startTime && temp->orderTime <= endTime && temp->status == "�����") {
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
        cout << "��Ʒ���: " << salesTemp->productId << ", ������: " << salesTemp->totalQuantity << ", �����۶�: " << salesTemp->totalSales << endl;
        salesTemp = salesTemp->next;
    }
}

void saveStatisticsToFile(string filename, string startTime, string endTime) {
    ofstream file;
    file.open(filename);

    double totalSales = calculateTotalSales(startTime, endTime);
    file << "�����۶�: " << totalSales << endl;

    file << "����Ʒ�������:" << endl;
    struct ProductSales {
        int productId;
        int totalQuantity;
        double totalSales;
        ProductSales* next;
    };
    ProductSales* headProductSales = nullptr;

    Order* temp = headOrder;
    while (temp != nullptr) {
        if (temp->orderTime >= startTime && temp->orderTime <= endTime && temp->status == "�����") {
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
        file << "��Ʒ���: " << salesTemp->productId << ", ������: " << salesTemp->totalQuantity << ", �����۶�: " << salesTemp->totalSales << endl;
        salesTemp = salesTemp->next;
    }

    file.close();
}

// ��������
bool login(string username, string password) {
    // ��������û���������ľ�̬����
    if (username == "admin" && password == "admin") {
        return true;
    }
    else {
        return false;
    }
}

void logout() {
    cout << "���˳�ϵͳ��" << endl;
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
        cout << "δ�ҵ��˿ͣ�" << endl;
        return;
    }
    temp->points = points;
    cout << "�˿ͻ������óɹ���" << endl;
}

void setCustomerLevel(string name, int level) {
    Customer* temp = headCustomer;
    while (temp != nullptr && temp->name != name) {
        temp = temp->next;
    }
    if (temp == nullptr) {
        cout << "δ�ҵ��˿ͣ�" << endl;
        return;
    }
    temp->level = level;
    cout << "�˿��Ǽ����óɹ���" << endl;
}
