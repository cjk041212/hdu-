
#ifndef KNN__KDT_H_
#define KNN__KDT_H_
#include <bits/stdc++.h>
#include "Point.h"
using std::size_t,std::vector,std::pair;
template<size_t N,typename T>
class KDTree
{
 private:
    struct Node
    {
        Point<N> poi;
        Node* lson,*rson;
        int depth;
        T val;
        Node(const Point<N>& poi,int dep,T w):
        poi(poi),lson(nullptr), rson(nullptr),depth(dep) ,val(w) {}
    };
    Node* root;
    size_t sz,maxsz;
    Node* build(typename vector<pair<Point<N>,T>>::iterator l,
                typename vector<pair<Point<N>,T>>::iterator r,int dep);
    Node* findNode(Node* cur, const Point<N>& poi) const;
    void NearestNeighbourdfs(const Node* cur, const Point<N>& key, std::priority_queue<pair<double,T>>& q) const;
    Node* deepcopyTree(Node* rt);
    void freeResource(Node* cur);
 public:
    KDTree();
    explicit KDTree(vector<pair<Point<N>,T>>& poi);
    ~KDTree();
    KDTree(const KDTree& rhs);
    KDTree& operator=(const KDTree& rhs);
    size_t dimension() const;
    size_t size() const;
    bool empty() const;
    bool contain(const Point<N>& poi) const;
    T& operator[](const Point<N>& poi);
    T KNNvalue(const Point<N>& key,size_t k);
    void insert(const Point<N>& poi,const T& w);
};
template<size_t N, typename T>
T KDTree<N, T>::KNNvalue(const Point<N> &key, size_t k)
{
    std::priority_queue<pair<double,T>> q;
    maxsz = k;
    if (empty())
        return T();
    NearestNeighbourdfs(root,key,q);
    std::unordered_map<T,int> cnt;
    while(!q.empty())
    {
        cnt[q.top().second]++;
        q.pop();
    }
    T res;
    int max = 0;
    for(const auto &it : cnt)
    {
        if (it.second > max)
        {
            res = it.first;
            max = it.second;
        }
    }
    return res;
}
template<size_t N, typename T>
void KDTree<N, T>::NearestNeighbourdfs(const KDTree::Node *cur, const Point<N> &key, std::priority_queue<pair<double,T>> &q) const
{
    if (cur == nullptr)
        return;
    const Point<N>& curpoi = cur->poi;
    q.push({dis(curpoi, key), cur->val});
    if (q.size() > maxsz)
        q.pop();
    int dep = cur->depth;
    bool left;
    if (key[dep % N] < curpoi[dep % N])
    {
        NearestNeighbourdfs(cur->lson,key,q);
        left = true;
    }
    else
    {
        NearestNeighbourdfs(cur->rson,key,q);
        left = false;
    }
    if (q.size() < maxsz || fabs(key[dep % N] - curpoi[dep % N]) < q.top().first)
    {
        if (left)
            NearestNeighbourdfs(cur->rson,key,q);
        else
            NearestNeighbourdfs(cur->lson,key,q);
    }

}
template<size_t N, typename T>
void KDTree<N, T>::insert(const Point<N> &poi, const T &w)
{
    auto node = findNode(root,poi);
    if (node == nullptr)
    {
        root = new Node(poi,0,w);
        sz = 1;
        return;
    }
    if (node->val == w)
    {
        node->val = w;
        return;
    }
    int dep = node->depth;
    auto Newnode = new Node(poi,dep + 1,w);
    if (poi[dep % N] < node->poi[dep % N])
        node->lson = Newnode;
    else
        node->rson = Newnode;
    sz++;
}
template<size_t N, typename T>
T &KDTree<N, T>::operator[](const Point<N> &poi)
{
    auto node = findNode(root,poi);
    if (node != nullptr && node->poi == poi)
        return node->val;
    else
    {
        insert(poi,T());
        node = findNode(node,poi);
        return node->val;

    }
}
template<size_t N, typename T>
typename KDTree<N,T>::Node *KDTree<N, T>::findNode(KDTree::Node *cur, const Point<N> &poi) const
{
    if (cur == nullptr || cur->poi == poi)
        return cur;
    const Point<N>& curpoi = cur->poi;
    int curdep = cur->depth;
    if (poi[curdep % N] < curpoi[curdep % N])
        return cur->lson == nullptr ? cur : findNode(cur->lson,poi);
    else
        return cur->rson == nullptr ? cur : findNode(cur->rson,poi);
}
template<size_t N, typename T>
bool KDTree<N, T>::contain(const Point<N> &poi) const
{
    auto node = findNode(root,poi);
    return node != nullptr && node->poi == poi;
}
template<size_t N, typename T>
bool KDTree<N, T>::empty() const
{
    return sz == 0;
}
template<size_t N, typename T>
size_t KDTree<N, T>::size() const
{
    return sz;
}
template<size_t N, typename T>
size_t KDTree<N, T>::dimension() const
{
    return N;
}
template<size_t N, typename T>
KDTree<N, T>::KDTree():
sz(0),root(nullptr) {}
template<size_t N, typename T>
typename KDTree<N,T>::Node *KDTree<N, T>::deepcopyTree(KDTree::Node *rt)
{
    if (rt == nullptr)
        return nullptr;
    auto nroot = new Node(*rt);
    nroot->lson = deepcopyTree(rt->lson);
    nroot->rson = deepcopyTree(rt->rson);
    return nroot;
}
template<size_t N, typename T>
void KDTree<N, T>::freeResource(Node *cur)
{
    if (cur == nullptr)
        return ;
    freeResource(cur->lson);
    freeResource(cur->rson);
    delete cur;
}
template<size_t N, typename T>
typename KDTree<N,T>::Node *KDTree<N, T>::build(typename vector<pair<Point<N>, T>>::iterator l,
                                                typename vector<pair<Point<N>, T>>::iterator r,int dep)
{
    if (l >= r)
        return nullptr;
    auto axis = dep % N;
    auto cmp = [&](const pair<Point<N>,T> &lhs,const pair<Point<N>,T>& rhs )
        { return lhs.first[axis] < rhs.first[axis];};
    auto mid = l + (r - l) / 2;
    std::nth_element(l,mid,r,cmp);
    while(mid > l && mid->first[axis] == (mid - 1)->first[axis])
        mid--;
    auto Newnode = new Node(mid->first,dep,mid->second);
    Newnode->lson = build(l,mid,dep + 1);
    Newnode->rson = build(mid + 1,r,dep + 1);
    return Newnode;
}
template<size_t N, typename T>
KDTree<N, T>::KDTree(vector<pair<Point<N>, T>> &poi)
{
    root = build(poi.begin(),poi.end(),0);
    sz = poi.size();
}
template<size_t N, typename T>
KDTree<N, T>::KDTree(const KDTree& rhs)
{
    root = deepcopyTree(rhs);
    sz = rhs.sz;
}
template<size_t N, typename T>
KDTree<N,T> &KDTree<N, T>::operator=(const KDTree &rhs)
{
    if (*this == rhs)
        return *this;
    freeResource(root);
    root = deepcopyTree(rhs.root);
    sz = rhs.sz;
    return *this;
}
template<size_t N,typename T>
KDTree<N,T>::~KDTree()
{
    freeResource(root);
}

#endif //KNN__KDT_H_
