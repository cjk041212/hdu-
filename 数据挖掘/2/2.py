import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.preprocessing import LabelEncoder, StandardScaler
from sklearn.tree import DecisionTreeClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.ensemble import RandomForestClassifier, GradientBoostingClassifier
from sklearn.metrics import f1_score, roc_auc_score, accuracy_score
from sklearn.impute import SimpleImputer

# 1. 数据加载
train_df = pd.read_csv('titanic-train.csv')

# 2. 初步探索数据
print(train_df.info())
print(train_df.head())
print(train_df.isnull().sum())

# 3. 数据预处理
# 填充Age的缺失值
imputer = SimpleImputer(strategy='median')
train_df['Age'] = imputer.fit_transform(train_df[['Age']])

# 填充Embarked缺失值
train_df['Embarked'].fillna(train_df['Embarked'].mode()[0], inplace=True)

# 删除Cabin列，因为缺失值太多
train_df.drop(columns=['Cabin'], inplace=True)

# 将Sex列转换为数值型
le = LabelEncoder()
train_df['Sex'] = le.fit_transform(train_df['Sex'])

# 对Embarked进行One-Hot Encoding
train_df = pd.get_dummies(train_df, columns=['Embarked'], drop_first=True)

# 创建FamilySize和IsAlone特征
train_df['FamilySize'] = train_df['SibSp'] + train_df['Parch']
train_df['IsAlone'] = (train_df['FamilySize'] == 0).astype(int)

# 删除Name, Ticket, SibSp, Parch列
train_df.drop(columns=['Name', 'Ticket', 'SibSp', 'Parch'], inplace=True)

# 4. 数据划分为训练集、验证集和测试集
X = train_df.drop(columns=['Survived'])
y = train_df['Survived']

# 将数据集划分为训练集、验证集和测试集
X_train, X_temp, y_train, y_temp = train_test_split(X, y, test_size=0.3, random_state=42)  # 70%训练集，30%暂时保留为验证集和测试集
X_val, X_test, y_val, y_test = train_test_split(X_temp, y_temp, test_size=0.5, random_state=42)  # 剩下的30%分为验证集和测试集（各50%）

# 5. 特征缩放
scaler = StandardScaler()
X_train = scaler.fit_transform(X_train)
X_val = scaler.transform(X_val)
X_test = scaler.transform(X_test)

# 6. 初始化模型
dt_model = DecisionTreeClassifier(random_state=42)
nb_model = GaussianNB()
rf_model = RandomForestClassifier(random_state=42)
gb_model = GradientBoostingClassifier(random_state=42)

# 7. 5折交叉验证评估模型
models = {
    "Decision Tree": dt_model,
    "Naive Bayes": nb_model,
    "Random Forest": rf_model,
    "Gradient Boosting": gb_model
}

# 8. 输出每个模型的交叉验证结果
for model_name, model in models.items():
    # 进行5折交叉验证
    cv_scores = cross_val_score(model, X_train, y_train, cv=5, scoring='accuracy')  # 使用准确率评估
    print(f"{model_name} - Cross Validation Accuracy: {np.mean(cv_scores):.4f} ± {np.std(cv_scores):.4f}")

# 9. 训练模型并输出测试集上的评估结果
for model_name, model in models.items():
    model.fit(X_train, y_train)
    test_pred = model.predict(X_test)

    # 计算F1-Score、AUC和准确率（在测试集上）
    test_f1 = f1_score(y_test, test_pred)
    test_auc = roc_auc_score(y_test, test_pred)
    test_accuracy = accuracy_score(y_test, test_pred)

    # 输出测试集上的评估结果
    print(f"{model_name} - Test F1: {test_f1:.4f}, AUC: {test_auc:.4f}, Accuracy: {test_accuracy:.4f}")
