#include"main.h"

// ������
int main() {
    string username, password;
    cout << YELLOW << "�������û���: " << RESET;
    getline(cin, username);
    cout << YELLOW<<"����������: " << RESET;
    getline(cin, password);

    if (!login(username, password)) {
        cout << RED << "�û������������" << endl << RESET;
        return 0;
    }

    int choice;
    do {
        cout << YELLOW << "-----------------------------" << endl;
        cout << "1. �����Ʒ" << endl;
        cout << "2. ��ʾ������Ʒ" << endl;
        cout << "3. ɾ����Ʒ" << endl;
        cout << "4. �޸���Ʒ" << endl;
        cout << "5. ������Ʒ" << endl;
        cout << "6. ��������" << endl;
        cout << "7. ȡ������" << endl;
        cout << "8. ��ɶ���" << endl;
        cout << "9. ��ʾ���ж���" << endl;
        cout << "10. ���Ҷ���" << endl;
        cout << "11. ͳ�����۶�" << endl;
        cout << "12. ͳ����Ʒ�������" << endl;
        cout << "13. ����ͳ����Ϣ���ļ�" << endl;
        cout << "14. ��ӹ˿�" << endl;
        cout << "15. ���ù˿ͻ���" << endl;
        cout << "16. ���ù˿��Ǽ�" << endl;
        cout << "17. �˳�" << endl;
        cout << YELLOW << "-----------------------------" << endl << RESET;

        cout << GREEN << "������ѡ��: " << RESET;
        cin >> choice; getchar();
        switch (choice) {
        case 1: {
            cout << BLUE;
            int id, stock;
            string name, description, category;
            double price;
            cout << "������Ʒ���: ";
            cin >> id; getchar();
            cout << "������Ʒ����: ";
            getline(cin, name);
            cout << "������Ʒ�۸�: ";
            cin >> price; getchar();
            cout << "������Ʒ����: ";
            getline(cin,description);
            cout << "������Ʒ����: ";
            getline(cin, category);
            cout << "������Ʒ���: ";
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
            cout << "����Ҫɾ������Ʒ���: ";
            cin >> id; getchar();
            deleteProduct(id);
            break;
        }
        case 4: {
            cout << BLUE;
            int id;
            cout << "����Ҫ�޸ĵ���Ʒ���: ";
            cin >> id; getchar();
            modifyProduct(id);
            break;
        }
        case 5: {
            cout << BLUE;
            int id;
            cout << "����Ҫ���ҵ���Ʒ���: ";
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
            cout << "����Ҫȡ���Ķ������: ";
            cin >> orderId; getchar();
            cancelOrder(orderId);
            break;
        }
        case 8: {
            cout << CYAN;
            int orderId;
            cout << "����Ҫ��ɵĶ������: ";
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
            cout << "����Ҫ���ҵĶ������: ";
            cin >> orderId; getchar();
            findOrder(orderId);
            break;
        }
        case 11: {
            cout << RED;
            string startTime, endTime;
            cout << "���뿪ʼʱ��: ";
            getline(cin, startTime);
            cout << "�������ʱ��: ";
            getline(cin, endTime);
            double totalSales = calculateTotalSales(startTime, endTime);
            cout << "�����۶�: " << totalSales << endl;
            break;
        }
        case 12: {
            cout << RED;
            string startTime, endTime;
            cout << "���뿪ʼʱ��: ";
            getline(cin, startTime);
            cout << "�������ʱ��: ";
            getline(cin, endTime);
            calculateProductSales(startTime, endTime);
            break;
        }
        case 13: {
            cout << RED;
            string filename, startTime, endTime;
            cout << "�����ļ���: ";
            getline(cin, filename);
            cout << "���뿪ʼʱ��: ";
            getline(cin, startTime);
            cout << "�������ʱ��: ";
            getline(cin, endTime);
            saveStatisticsToFile(filename, startTime, endTime);
            break;
        }
        case 14: {
            cout << MAGENTA;
            string name;
            int points;
            cout << "����˿�����: ";
            getline(cin, name);
            cout << "����˿ͻ���: ";
            cin >> points; getchar();
            addCustomer(name, points);
            break;
        }
        case 15: {
            cout << MAGENTA;
            string name;
            int points;
            cout << "����˿�����: ";
            getline(cin, name);
            cout << "����˿ͻ���: ";
            cin >> points; getchar();
            setCustomerDiscount(name, points);
            break;
        }
        case 16: {
            cout << MAGENTA;
            string name;
            int level;
            cout << "����˿�����: ";
            getline(cin, name);
            cout << "����˿��Ǽ� (1-3): ";
            cin >> level; getchar();
            setCustomerLevel(name, level);
            break;
        }
        case 17:
            logout();
            break;
        default:
            cout << "��Ч��ѡ��" << endl;
            break;
        }
        cout << RESET << endl;
    } while (choice != 17);

    return 0;
}
