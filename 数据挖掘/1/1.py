import pandas as pd
from itertools import combinations

# 数据准备
data = {
    'T1': ['牛奶', '面包', '黄油'],
    'T2': ['牛奶', '啤酒', '尿布'],
    'T3': ['面包', '黄油', '饼干'],
    'T4': ['牛奶', '尿布', '饼干'],
    'T5': ['啤酒', '尿布'],
    'T6': ['牛奶', '尿布', '面包', '黄油'],
    'T7': ['啤酒', '饼干'],
    'T8': ['啤酒', '尿布', '饼干'],
    'T9': ['牛奶', '尿布', '面包', '黄油'],
    'T10': ['尿布', '面包', '黄油']
}

transactions = pd.DataFrame(
    [{item: (item in data[tid]) for item in ['牛奶', '啤酒', '尿布', '面包', '黄油', '饼干']} for tid in data])


# 计算支持度
def calculate_support(itemset, transactions):
    count = sum(transactions[list(itemset)].all(axis=1))
    support = count / len(transactions)
    return support


# 修复 Apriori 算法中生成候选项集的部分
def apriori(transactions, min_support):
    itemsets = []
    support_data = {}
    single_items = [frozenset([item]) for item in transactions.columns]
    current_itemsets = single_items

    while current_itemsets:
        # 计算支持度
        current_supports = {item: calculate_support(item, transactions) for item in current_itemsets}
        # 筛选满足支持度的项集
        current_itemsets = [item for item, support in current_supports.items() if support >= min_support]
        support_data.update(current_supports)
        itemsets.extend(current_itemsets)
        # 生成候选项集
        if current_itemsets:
            # 将 frozenset 转换为 set
            merged_set = set().union(*[set(item) for item in current_itemsets])
            current_itemsets = list(map(frozenset, combinations(merged_set, len(current_itemsets[0]) + 1)))
        else:
            current_itemsets = []

    return itemsets, support_data


# 生成关联规则
def generate_rules(frequent_itemsets, support_data, min_confidence):
    rules = []
    for itemset in frequent_itemsets:
        if len(itemset) < 2:
            continue
        for consequence in map(frozenset, combinations(itemset, 1)):
            antecedent = itemset - consequence
            if antecedent:
                confidence = support_data[itemset] / support_data[antecedent]
                if confidence >= min_confidence:
                    rules.append((antecedent, consequence, confidence))
    return rules


# 执行 Apriori 算法
min_support = 0.3
min_confidence = 0.7
frequent_itemsets, support_data = apriori(transactions, min_support)

# 打印频繁项集
print("频繁项集:")
for itemset in frequent_itemsets:
    print(f"{set(itemset)}: {support_data[itemset]:.2f}")

# 生成关联规则
rules = generate_rules(frequent_itemsets, support_data, min_confidence)

# 打印关联规则
print("\n关联规则:")
for antecedent, consequence, confidence in rules:
    print(f"{set(antecedent)} => {set(consequence)}, 置信度: {confidence:.2f}")
