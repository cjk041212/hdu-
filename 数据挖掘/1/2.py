import pandas as pd
from mlxtend.frequent_patterns import apriori, association_rules
from scipy.sparse import csr_matrix

pd.set_option('display.max_columns', None)
pd.set_option('display.expand_frame_repr', False)
pd.set_option('display.max_colwidth', None)

movies = pd.read_csv('movies.dat', sep='::', engine='python', encoding='latin1',
                     names=['MovieID', 'Title', 'Genres'])
ratings = pd.read_csv('ratings.dat', sep='::', engine='python', encoding='latin1',
                      names=['UserID', 'MovieID', 'Rating', 'Timestamp'])
users = pd.read_csv('users.dat', sep='::', engine='python', encoding='latin1',
                    names=['UserID', 'Gender', 'Age', 'Occupation', 'Zip-code'])

user_movie_matrix = ratings.pivot(index='UserID', columns='MovieID', values='Rating')
user_movie_matrix = user_movie_matrix.notnull().astype('int')
user_movie_sparse = csr_matrix(user_movie_matrix)
print(user_movie_matrix)

frequent_itemsets = apriori(user_movie_matrix, min_support=0.3, use_colnames=True)
print("频繁项集：")
print(frequent_itemsets)

rules = association_rules(frequent_itemsets, metric="confidence", min_threshold=0.7)
print("\n关联规则：")
print(rules)

def recommend_movies(user_id, rules, top_n=5):
    user_rated_movies = set(user_movie_matrix.loc[user_id][user_movie_matrix.loc[user_id] > 0].index)
    applicable_rules = rules[rules['antecedents'].apply(lambda x: len(x & user_rated_movies) > 0)]
    recommendations = set()
    for _, rule in applicable_rules.iterrows():
        recommendations |= rule['consequents']
    recommendations -= user_rated_movies
    return movies[movies['MovieID'].isin(recommendations)].head(top_n)

print("\n为用户1推荐的电影：")
print(recommend_movies(1, rules))
