#include <bits/stdc++.h>
#include "kdt.h"
using namespace std;
const int N = 4;
using dataset = vector<pair<Point<N>, string>>;
void readdata(ifstream &fin, dataset &wow)
{
    double s;
    char tmp;
    while (fin >> s >> tmp)
    {
        string id;
        vector<double> ar;
        ar.emplace_back(s);
        for (int i = 1; i < N; i++)
        {
            fin >> s >> tmp;
            ar.emplace_back(s);
        }
        fin >> id;
        wow.emplace_back(ar, id);
    }
}

int main()
{
    ifstream fin1,fin2;
    fin1.open("D:\\data.txt", ios::in);
    if (!fin1.is_open())
    {
        cout << "data not found\n";
        return 1;
    }
    dataset tmp;
    readdata(fin1, tmp);
    shuffle(tmp.begin(),tmp.end(), std::mt19937(std::random_device()()));
    dataset train(tmp.begin(),tmp.begin() + 74),test(tmp.begin()+75,tmp.end());
    /*
    ifstream fin1,fin2;
    fin1.open("D:\\train_emtion.txt");
    fin2.open("D:\\test_emtion.txt");
    dataset train,test;
    readdata(fin1,train);
    readdata(fin2,test);
     */
    KDTree<N, string> kd(train);
    cout << "build complete\n";
    for (int i = 1; i <= 45; i+= 2)
    {
        int res = 0;
        for (auto &[poi, name] : test)
            res += kd.KNNvalue(poi, i) == name;
        cout << "K = " << i << "  " << res * 1.0 / test.size() * 100 << "%\n";
    }
    system("pause");
    return 0;
}
