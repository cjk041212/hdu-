import pandas as pd
from sklearn.preprocessing import StandardScaler
from sklearn.cluster import KMeans, AgglomerativeClustering
from sklearn.metrics import f1_score, silhouette_score

# 1. 读取数据
data = pd.read_csv("wine-data.txt", header=None)
data['Index'] = data.index  # 添加索引列
labels = data.iloc[:, 0]  # 提取真实标签
features = data.iloc[:, 1:-1]  # 提取特征
labels -= 1  # 调整标签值为 0 开始

# 2. 数据标准化
scaler = StandardScaler()
features_scaled = scaler.fit_transform(features)

# 3. K-Means 聚类
kmeans = KMeans(n_clusters=3, random_state=4200)
kmeans_labels = kmeans.fit_predict(features_scaled)
#print(kmeans_labels)
# 4. 层次聚类
hierarchical = AgglomerativeClustering(n_clusters=3,linkage='single')
hierarchical_labels = hierarchical.fit_predict(features_scaled)
#print(hierarchical_labels)
# 5. 计算评估指标
def evaluate_clustering(true_labels, predicted_labels, features):
    # F1 分数
    micro_f1 = f1_score(true_labels, predicted_labels, average='micro')
    macro_f1 = f1_score(true_labels, predicted_labels, average='macro')
    # 轮廓系数
    silhouette = silhouette_score(features, predicted_labels)
    return micro_f1, macro_f1, silhouette

# 评估 K-Means
kmeans_micro_f1, kmeans_macro_f1, kmeans_silhouette = evaluate_clustering(
    labels, kmeans_labels, features_scaled
)

# 评估层次聚类
hierarchical_micro_f1, hierarchical_macro_f1, hierarchical_silhouette = evaluate_clustering(
    labels, hierarchical_labels, features_scaled
)

# 6. 输出结果
print("K-Means 评估结果:")
print(f"Micro-F1: {kmeans_micro_f1:.4f}, Macro-F1: {kmeans_macro_f1:.4f}, 轮廓系数: {kmeans_silhouette:.4f}")

print("\n层次聚类评估结果:")
print(f"Micro-F1: {hierarchical_micro_f1:.4f}, Macro-F1: {hierarchical_macro_f1:.4f}, 轮廓系数: {hierarchical_silhouette:.4f}")

# 7. 可视化
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA

# PCA 降维
pca = PCA(n_components=2)
features_pca = pca.fit_transform(features_scaled)

# 绘制聚类结果（PCA）
def plot_clusters(features_2d, labels, title):
    plt.figure(figsize=(8, 6))
    for cluster in range(max(labels) + 1):
        plt.scatter(features_2d[labels == cluster, 0],
                    features_2d[labels == cluster, 1],
                    label=f'Cluster {cluster}')
    plt.title(title)
    plt.xlabel("Component 1")
    plt.ylabel("Component 2")
    plt.legend()
    plt.show()

# 绘制 K-Means 和层次聚类结果
plot_clusters(features_pca, kmeans_labels, "K-Means Clustering (PCA)")
plot_clusters(features_pca, hierarchical_labels, "Hierarchical Clustering (PCA)")
from sklearn.metrics import confusion_matrix
from scipy.optimize import linear_sum_assignment

# 定义标签映射函数
def match_labels(true_labels, predicted_labels):
    cm = confusion_matrix(true_labels, predicted_labels)
    row_ind, col_ind = linear_sum_assignment(-cm)  # 最大化匹配
    label_mapping = {old: new for old, new in zip(col_ind, row_ind)}
    matched_labels = [label_mapping[label] for label in predicted_labels]
    return matched_labels


# 对 K-Means 和层次聚类结果重新映射标签
kmeans_labels_mapped = match_labels(labels, kmeans_labels)
hierarchical_labels_mapped = match_labels(labels, hierarchical_labels)

# 评估重新映射后的结果
kmeans_micro_f1, kmeans_macro_f1, kmeans_silhouette = evaluate_clustering(
    labels, kmeans_labels_mapped, features_scaled
)

hierarchical_micro_f1, hierarchical_macro_f1, hierarchical_silhouette = evaluate_clustering(
    labels, hierarchical_labels_mapped, features_scaled
)

# 输出重新映射后的评估结果
print("K-Means (重新映射后) 评估结果:")
print(f"Micro-F1: {kmeans_micro_f1:.4f}, Macro-F1: {kmeans_macro_f1:.4f}, 轮廓系数: {kmeans_silhouette:.4f}")

print("\n层次聚类 (重新映射后) 评估结果:")
print(f"Micro-F1: {hierarchical_micro_f1:.4f}, Macro-F1: {hierarchical_macro_f1:.4f}, 轮廓系数: {hierarchical_silhouette:.4f}")

