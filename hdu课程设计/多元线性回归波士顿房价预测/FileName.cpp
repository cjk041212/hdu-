#include <iostream>
#include<vector>
#include<algorithm>
using namespace std;

#define Dimension 14
#define learningRate 0.002
#define eps 0.001
#define testNum 50
#define trainNum 10000

typedef struct {
    double data[Dimension], price;
} Node;

int NUM;
Node* ptr;
double* temp, * w, * tempans;
double* maxx, * minn;

void myPrint(Node* ptr) {
    for (int i = 0; i < NUM; i++) {
        printf("%d:\n", i);
        for (int j = 0; j < Dimension; j++)
            printf("%.6lf  ", (ptr + i)->data[j]);
        printf("实际房价：%.6lf\n", (ptr + i)->price);
    }
}

int readLine(Node* ptr, int flag) {
    for (int i = 1; i < Dimension; i++) {
        if (scanf("%lf", &(ptr->data[i])) == EOF) return -1;
    }
    if (!flag && scanf("%lf", &(ptr->price)) == EOF) return -1;
    ptr->data[0] = 1;
    return 1;
}

Node* myRead(int* num, Node* ptr) {
    FILE* file = fopen("housing.txt", "r");
    if (!file) {
        perror("Unable to open file!");
        exit(EXIT_FAILURE);
    }

    *num = 0;
    ptr = (Node*)malloc(sizeof(Node));
    if (!ptr) exit(-1);

    char s[1000];
    fgets(s, 1000, file); // skip the header line

    while (true) {
        Node temp;
        if (fscanf(file, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &temp.data[1], &temp.data[2], &temp.data[3], &temp.data[4], &temp.data[5],
            &temp.data[6], &temp.data[7], &temp.data[8], &temp.data[9], &temp.data[10],
            &temp.data[11], &temp.data[12], &temp.data[13], &temp.price) == EOF) break;

        temp.data[0] = 1; // bias term

        ptr[*num] = temp;
        (*num)++;
        ptr = (Node*)realloc(ptr, (*num + 1) * sizeof(Node));
        if (!ptr) exit(-1);
    }

    fclose(file);
    myPrint(ptr);
    return ptr;
}

void del() {
    free(ptr);
    free(maxx);
    free(minn);
    free(temp);
    free(w);
    free(tempans);
}

void printmaxxminn(double* maxx, double* minn) {
    for (int i = 0; i <= Dimension; i++) {
        printf("maxx[%d]=%.6lf  ", i, maxx[i]);
        printf("minn[%d]=%.6lf\n", i, minn[i]);
    }
}

void getmaxxminn(Node* ptr) {
    maxx = (double*)malloc((Dimension + 1) * sizeof(double));
    if (!maxx) exit(-1);
    minn = (double*)malloc((Dimension + 1) * sizeof(double));
    if (!minn) exit(-1);
    maxx[0] = 1; minn[0] = 0;
    for (int i = 1; i <= Dimension; i++) {
        maxx[i] = 0; minn[i] = 1000;
        for (int j = 0; j < NUM; j++) {
            if (i != Dimension) {
                maxx[i] = maxx[i] > (ptr + j)->data[i] ? maxx[i] : (ptr + j)->data[i];
                minn[i] = minn[i] < (ptr + j)->data[i] ? minn[i] : (ptr + j)->data[i];
            }
            else {
                maxx[i] = maxx[i] > (ptr + j)->price ? maxx[i] : (ptr + j)->price;
                minn[i] = minn[i] < (ptr + j)->price ? minn[i] : (ptr + j)->price;
            }
        }
    }
}

double f(const Node* ptr, double* w, const vector<int>& features) {
    double result = w[0]; // bias term
    for (size_t i = 0; i < features.size(); i++) {
        result += w[i + 1] * ptr->data[features[i]];
    }
    return result;
}

void normalizeWay(Node* ptr) {
    for (int i = 1; i <= Dimension; i++) {
        double c = maxx[i] - minn[i];
        if (i != Dimension) {
            ptr->data[i] = (ptr->data[i] - minn[i]) / c;
        }
        else {
            ptr->price = (ptr->price - minn[i]) / c;
        }
    }
}

void normalize(Node* ptr, int start, int end) {
    for (int i = start; i < end; i++) {
        normalizeWay(ptr + i);
    }
}

void normalizeBackWay(Node* ptr) {
    for (int i = 1; i <= Dimension; i++) {
        double c = maxx[i] - minn[i];
        if (i != Dimension) {
            ptr->data[i] = ptr->data[i] * c + minn[i];
        }
        else {
            ptr->price = ptr->price * c + minn[i];
        }
    }
}

void normalizeBack(Node* ptr, int start, int end) {
    for (int i = start; i < end; i++) {
        normalizeBackWay(ptr + i);
    }
}

void normalizeBackPrice(Node* ptr, double* tempans, int start, int end) {
    normalizeBack(ptr, start, end);
    for (int i = start; i < end; i++) {
        *(tempans + i) = *(tempans + i) * (maxx[Dimension] - minn[Dimension]) + minn[Dimension];
    }
}

void normalizePrice(Node* ptr, double* tempans, int start, int end) {
    normalize(ptr, start, end);
    for (int i = start; i < end; i++) {
        *(tempans + i) = (*(tempans + i) - minn[Dimension]) / (maxx[Dimension] - minn[Dimension]);
    }
}

void init() {
    ptr = myRead(&NUM, ptr);
    getmaxxminn(ptr);
    normalize(ptr, 0, NUM);
    temp = (double*)malloc(Dimension * sizeof(double));
    if (!temp) exit(-1);
    w = (double*)malloc(Dimension * sizeof(double));
    if (!w) exit(-1);
    tempans = (double*)malloc(NUM * sizeof(double));
    if (!tempans) exit(-1);
    for (int i = 0; i < Dimension; i++)
        *(temp + i) = *(w + i) = 1.0;
}

void predictPrice(Node* ptr) {
    printf("please input data with 13 dimension:\n");
    if (readLine(ptr, 1) == -1) exit(-1);
    ptr->price = w[0];
    for (int i = 1; i < Dimension; i++) {
        ptr->data[i] = (ptr->data[i] - minn[i]) / (maxx[i] - minn[i]);
        ptr->price += w[i] * ptr->data[i];
    }
    ptr->price = ptr->price * (maxx[Dimension] - minn[Dimension]) + minn[Dimension];
    printf("price = %f\n", ptr->price);
}

void predict(Node* ptr, double* w, double* tempans, int start, int end, const vector<int>& features) {
    for (int i = start; i < end; i++) {
        *(tempans + i) = f(ptr + i, w, features);
    }
}

double getRmse(Node* ptr, double* tempans, const vector<int>& features) {
    double result = 0;
    predict(ptr, w, tempans, NUM - testNum, NUM, features);
    normalizeBackPrice(ptr, tempans, NUM - testNum, NUM);
    for (int i = NUM - testNum; i < NUM; i++) {
        printf("%d:\n", i);
        for (int j = 0; j < Dimension; j++) {
            printf("%.6lf  ", (ptr + i)->data[j]);
        }
        printf("实际房价:%.6lf  ", (ptr + i)->price);
        printf("预估房价:%.6lf\n", *(tempans + i));
        result += (*(tempans + i) - (ptr + i)->price) * (*(tempans + i) - (ptr + i)->price) / testNum;
    }
    result = sqrt(result);
    normalizePrice(ptr, tempans, NUM - testNum, NUM);
    return result;
}

void calGD(Node* ptr, double* temp, double* w, const vector<int>& features) {
    for (size_t i = 0; i <= features.size(); i++) {
        double pd = 0;
        for (int j = 0; j < NUM - testNum; j++) {
            double sum = w[0];
            for (size_t k = 0; k < features.size(); k++) {
                sum += w[k + 1] * (ptr + j)->data[features[k]];
            }
            if (i == 0) {
                pd += (sum - (ptr + j)->price);
            }
            else {
                pd += (sum - (ptr + j)->price) * (ptr + j)->data[features[i - 1]];
            }
        }
        temp[i] = (learningRate * pd) / NUM;
    }
    printf("-----------------\n");
    for (size_t i = 0; i <= features.size(); i++) {
        w[i] = w[i] - temp[i];
        printf("w[%zu]=%.6lf\n", i, w[i]);
    }
}

double calLoss(Node* ptr, double* tempans, const vector<int>& features) {
    predict(ptr, w, tempans, 0, NUM - testNum, features);
    double loss = 0;
    normalizeBackPrice(ptr, tempans, 0, NUM - testNum);
    for (int i = 0; i < NUM - testNum; i++) {
        loss += ((ptr + i)->price - *(tempans + i)) * ((ptr + i)->price - *(tempans + i));
    }
    normalizePrice(ptr, tempans, 0, NUM - testNum);
    loss = loss / 2 * (NUM - testNum);
    return loss;
}

void train(Node* ptr, double* temp, double* w, double* tempans, const vector<int>& features) {
    for (int i = 0; i < trainNum; i++) {
        calGD(ptr, temp, w, features);
        printf("Loss=%.6lf\n", calLoss(ptr, tempans, features));
    }
    printf("RMSE=%.6lf\n", getRmse(ptr, tempans, features));
}

double calculateCorrelation(double* x, double* y, int n) {
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0, sumY2 = 0;
    for (int i = 0; i < n; i++) {
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
        sumY2 += y[i] * y[i];
    }
    return (n * sumXY - sumX * sumY) / sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
}

vector<int> selectTopFeatures(Node* ptr, int n, int numFeatures) {
    vector<pair<double, int>> correlations;
    double* prices = (double*)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        prices[i] = ptr[i].price;
    }
    for (int i = 1; i < Dimension; i++) {
        double* feature = (double*)malloc(n * sizeof(double));
        for (int j = 0; j < n; j++) {
            feature[j] = ptr[j].data[i];
        }
        double corr = calculateCorrelation(feature, prices, n);
        correlations.push_back({ abs(corr), i });
        free(feature);
    }
    free(prices);
    sort(correlations.rbegin(), correlations.rend());
    vector<int> topFeatures;
    for (int i = 0; i < numFeatures; i++) {
        topFeatures.push_back(correlations[i].second);
    }
    return topFeatures;
}

int main() {
    init();
    vector<int> topFeatures = selectTopFeatures(ptr, NUM, 4);
    printf("Top 4 features:\n");
    for (int i = 0; i < 4; i++) {
        printf("Feature %d\n", topFeatures[i]);
    }
    free(temp);
    free(w);
    temp = (double*)malloc((topFeatures.size() + 1) * sizeof(double));
    w = (double*)malloc((topFeatures.size() + 1) * sizeof(double));
    for (size_t i = 0; i <= topFeatures.size(); i++)
        *(temp + i) = *(w + i) = 1.0;
    train(ptr, temp, w, tempans, topFeatures);
    predictPrice(ptr);
    del();
    return 0;
}
