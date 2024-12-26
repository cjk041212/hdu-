import pandas as pd
from sklearn.model_selection import train_test_split, cross_val_score, cross_validate
from sklearn.preprocessing import LabelEncoder
from sklearn.metrics import classification_report, roc_auc_score, roc_curve, make_scorer
from sklearn.model_selection import StratifiedKFold

# 数据加载
df = pd.read_csv('iris.csv')
X = df.iloc[:, 1:5]
y = LabelEncoder().fit_transform(df['Species'])  # 编码目标变量

# 数据集分割
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3, random_state=42)

from sklearn.metrics import precision_score, recall_score, f1_score

# 自定义AUC计算
def calculate_auc(model, X, y):
    proba = model.predict_proba(X)
    if proba.shape[1] > 1:  # 多分类时取所有类别平均AUC
        auc = roc_auc_score(y, proba, multi_class="ovr")
    else:
        auc = roc_auc_score(y, proba[:, 1])
    return auc

# 交叉验证评估
def cross_validate_model(model, X, y, scoring, cv=5):
    skf = StratifiedKFold(n_splits=cv, shuffle=True, random_state=42)
    results = cross_validate(model, X, y, scoring=scoring, cv=skf, return_train_score=False)
    return {metric: results['test_' + metric].mean() for metric in scoring}

from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC

# 模型定义
knn = KNeighborsClassifier()
svm = SVC(probability=True, random_state=42)

# 评估指标
scoring = {'precision': make_scorer(precision_score, average='macro'),
           'recall': make_scorer(recall_score, average='macro'),
           'f1': make_scorer(f1_score, average='macro'),
           'roc_auc': 'roc_auc_ovr'}

# KNN 交叉验证
knn_scores = cross_validate_model(knn, X, y, scoring=scoring)

# SVM 交叉验证
svm_scores = cross_validate_model(svm, X, y, scoring=scoring)

print("KNN交叉验证结果：", knn_scores)
print("SVM交叉验证结果：", svm_scores)

from sklearn.decomposition import PCA

# PCA降维（保留95%的数据方差）
pca = PCA(n_components=2)
X_pca = pca.fit_transform(X)

# KNN和SVM在PCA后的特征上进行交叉验证
knn_scores_pca = cross_validate_model(knn, X_pca, y, scoring=scoring)
svm_scores_pca = cross_validate_model(svm, X_pca, y, scoring=scoring)

print("PCA + KNN交叉验证结果：", knn_scores_pca)
print("PCA + SVM交叉验证结果：", svm_scores_pca)

import numpy as np

# 计算相关系数并筛选K个特征（如K=2）
correlations = [np.corrcoef(X[col], y)[0, 1] for col in X.columns]
selected_features = X.columns[np.argsort(np.abs(correlations))[-2:]]  # 选择两个相关性最大的特征
X_corr = X[selected_features]

# KNN和SVM在相关系数筛选后的特征上进行交叉验证
knn_scores_corr = cross_validate_model(knn, X_corr, y, scoring=scoring)
svm_scores_corr = cross_validate_model(svm, X_corr, y, scoring=scoring)

print("相关系数法 + KNN交叉验证结果：", knn_scores_corr)
print("相关系数法 + SVM交叉验证结果：", svm_scores_corr)

# 汇总结果
results = {
    "模型": ["KNN", "SVM", "PCA + KNN", "PCA + SVM", "相关系数法 + KNN", "相关系数法 + SVM"],
    "Precision": [knn_scores['precision'], svm_scores['precision'],
                  knn_scores_pca['precision'], svm_scores_pca['precision'],
                  knn_scores_corr['precision'], svm_scores_corr['precision']],
    "Recall": [knn_scores['recall'], svm_scores['recall'],
               knn_scores_pca['recall'], svm_scores_pca['recall'],
               knn_scores_corr['recall'], svm_scores_corr['recall']],
    "F1": [knn_scores['f1'], svm_scores['f1'],
           knn_scores_pca['f1'], svm_scores_pca['f1'],
           knn_scores_corr['f1'], svm_scores_corr['f1']],
    "AUC": [knn_scores['roc_auc'], svm_scores['roc_auc'],
            knn_scores_pca['roc_auc'], svm_scores_pca['roc_auc'],
            knn_scores_corr['roc_auc'], svm_scores_corr['roc_auc']],
}

import pandas as pd
results_df = pd.DataFrame(results)
print(results_df)
