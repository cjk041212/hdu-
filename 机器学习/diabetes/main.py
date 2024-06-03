import pandas as pd
import numpy as np
from sklearn.metrics import roc_auc_score, roc_curve, recall_score, precision_score, accuracy_score, f1_score
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split, cross_val_score, GridSearchCV
from sklearn.decomposition import PCA
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.linear_model import LogisticRegression
from sklearn.naive_bayes import GaussianNB
from sklearn.tree import DecisionTreeClassifier

# 读取数据
df = pd.read_csv('diabetes.csv')

# 查看数据基本信息
print(df.info())
print(df.head())

# 查看是否有缺失值
print(df.isnull().sum())

# 将0值替换为NaN
columns_to_replace = ['Glucose', 'BloodPressure', 'SkinThickness', 'Insulin', 'BMI']
df[columns_to_replace] = df[columns_to_replace].replace(0, np.nan)

# 使用均值填充NaN
df.fillna(df.mean(), inplace=True)

# 标准化处理
scaler = StandardScaler()
df_scaled = pd.DataFrame(scaler.fit_transform(df.iloc[:, :-1]), columns=df.columns[:-1])
df_scaled['Outcome'] = df['Outcome']

# 划分数据集
X = df_scaled.drop('Outcome', axis=1)
y = df_scaled['Outcome']
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# PCA降维可视化
pca = PCA(n_components=2)
X_pca = pca.fit_transform(X)
plt.figure(figsize=(10, 8))
plt.scatter(X_pca[:, 0], X_pca[:, 1], c=y, cmap='viridis', edgecolor='k', s=100)
plt.xlabel('PCA Component 1')
plt.ylabel('PCA Component 2')
plt.title('PCA of Diabetes Dataset')
plt.colorbar()
plt.show()

# 相关性分析和相关性热图
corr_matrix = df.corr()
plt.figure(figsize=(14, 12))  # 调整图形的大小
sns.heatmap(corr_matrix, annot=True)
plt.title('Correlation Heatmap of Diabetes Dataset')
plt.tight_layout()  # 自动调整布局以确保标签不会被裁剪
plt.show()

# 定义模型
log_reg = LogisticRegression(random_state=42)
naive_bayes = GaussianNB()
decision_tree = DecisionTreeClassifier(random_state=42)

# 交叉验证评分
log_reg_scores = cross_val_score(log_reg, X, y, cv=10, scoring='accuracy')
naive_bayes_scores = cross_val_score(naive_bayes, X, y, cv=10, scoring='accuracy')
decision_tree_scores = cross_val_score(decision_tree, X, y, cv=10, scoring='accuracy')

# 输出各模型预测平均得分
print(f'Logistic Regression Average Score: {log_reg_scores.mean()}')
print(f'Naive Bayes Average Score: {naive_bayes_scores.mean()}')
print(f'Decision Tree Average Score: {decision_tree_scores.mean()}')

# Logistic Regression参数搜索
param_grid_lr = {'C': [0.1, 1, 10, 100], 'solver': ['liblinear', 'saga']}
grid_search_lr = GridSearchCV(log_reg, param_grid_lr, cv=10, scoring='accuracy')
grid_search_lr.fit(X_train, y_train)
best_params_lr = grid_search_lr.best_params_

# Decision Tree参数搜索
param_grid_dt = {'max_depth': [3, 5, 7, 10], 'min_samples_split': [2, 5, 10]}
grid_search_dt = GridSearchCV(decision_tree, param_grid_dt, cv=10, scoring='accuracy')
grid_search_dt.fit(X_train, y_train)
best_params_dt = grid_search_dt.best_params_

# 输出最佳参数
print(f'Best Logistic Regression Parameters: {best_params_lr}')
print(f'Best Decision Tree Parameters: {best_params_dt}')

# 使用最佳参数的逻辑回归模型
best_log_reg = LogisticRegression(**best_params_lr, random_state=42)
best_log_reg.fit(X_train, y_train)
log_reg_test_score = best_log_reg.score(X_test, y_test)

# 使用最佳参数的决策树模型
best_decision_tree = DecisionTreeClassifier(**best_params_dt, random_state=42)
best_decision_tree.fit(X_train, y_train)
decision_tree_test_score = best_decision_tree.score(X_test, y_test)

# 朴素贝叶斯模型
naive_bayes.fit(X_train, y_train)
naive_bayes_test_score = naive_bayes.score(X_test, y_test)

# 输出测试集得分
print(f'Logistic Regression Test Score: {log_reg_test_score}')
print(f'Naive Bayes Test Score: {naive_bayes_test_score}')
print(f'Decision Tree Test Score: {decision_tree_test_score}')

# 使用最佳参数的逻辑回归模型进行预测
y_pred_lr = best_log_reg.predict(X_test)
y_proba_lr = best_log_reg.predict_proba(X_test)[:, 1]

# 使用朴素贝叶斯模型进行预测
y_pred_nb = naive_bayes.predict(X_test)
y_proba_nb = naive_bayes.predict_proba(X_test)[:, 1]

# 使用最佳参数的决策树模型进行预测
y_pred_dt = best_decision_tree.predict(X_test)
y_proba_dt = best_decision_tree.predict_proba(X_test)[:, 1]


# 计算评价指标
def print_evaluation_metrics(y_test, y_pred, model_name):
    accuracy = accuracy_score(y_test, y_pred)
    precision = precision_score(y_test, y_pred)
    recall = recall_score(y_test, y_pred)
    f1 = f1_score(y_test, y_pred)

    print(f'{model_name} Evaluation Metrics:')
    print(f'Accuracy: {accuracy}')
    print(f'Precision: {precision}')
    print(f'Recall: {recall}')
    print(f'F1 Score: {f1}')
    print('-' * 30)


print_evaluation_metrics(y_test, y_pred_lr, 'Logistic Regression')
print_evaluation_metrics(y_test, y_pred_nb, 'Naive Bayes')
print_evaluation_metrics(y_test, y_pred_dt, 'Decision Tree')


# 计算并绘制ROC曲线
def plot_roc_curve_and_auc(y_test, y_proba, model_name):
    fpr, tpr, _ = roc_curve(y_test, y_proba)
    auc = roc_auc_score(y_test, y_proba)

    plt.plot(fpr, tpr, label=f'{model_name} (AUC = {auc:.2f})')

    return auc


plt.figure(figsize=(10, 8))
auc_lr = plot_roc_curve_and_auc(y_test, y_proba_lr, 'Logistic Regression')
auc_nb = plot_roc_curve_and_auc(y_test, y_proba_nb, 'Naive Bayes')
auc_dt = plot_roc_curve_and_auc(y_test, y_proba_dt, 'Decision Tree')

plt.plot([0, 1], [0, 1], 'k--', lw=2)
plt.xlabel('False Positive Rate')
plt.ylabel('True Positive Rate')
plt.title('ROC Curve')
plt.legend(loc='lower right')
plt.show()

print(f'Logistic Regression AUC: {auc_lr}')
print(f'Naive Bayes AUC: {auc_nb}')
print(f'Decision Tree AUC: {auc_dt}')