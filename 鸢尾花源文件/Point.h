#ifndef KNN_POINT_H_
#define KNN_POINT_H_
#include <bits/stdc++.h>
using std::size_t;
template<size_t N>
class Point
{
 private:
    double data[N];
 public:
    typedef double* iterator;
    typedef  const double* const_iterator;
    size_t size() const;
    double& operator[](size_t i);
    double operator[](size_t i) const;
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    Point(){};
    Point(const std::vector<double>& ar);
};
template<size_t N>
Point<N>::Point(const std::vector<double> &ar)
{
    for(int i=0;i<N;i++)
        data[i] = ar[i];
}
template<size_t N>
double dis(const Point<N>&lhs,const Point<N>& rhs);
template <size_t N>
bool operator==(const Point<N>& lhs, const Point<N>& rhs);
template <size_t N>
bool operator!=(const Point<N>& lhs, const Point<N>& rhs);


template<size_t N>
double dis(const Point<N>&lhs,const Point<N>& rhs)
{
    double res = 0;
    for(auto i =0;i<N;i++)
        res += (lhs[i] - rhs[i]) * (lhs[i] - rhs[i]);
    return sqrt(res);
}

template <std::size_t N>
bool operator==(const Point<N>& lhs, const Point<N>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <std::size_t N>
bool operator!=(const Point<N>& lhs, const Point<N>& rhs)
{
    return !(lhs == rhs);
}
template<size_t N>
typename Point<N>::const_iterator Point<N>::end() const
{
    return data + size();
}
template<size_t N>
typename Point<N>::iterator Point<N>::end()
{
    return data + size();
}
template<size_t N>
typename Point<N>::const_iterator Point<N>::begin() const
{
    return data;
}
template<size_t N>
typename Point<N>::iterator Point<N>::begin()
{
    return data;
}
template<size_t N>
double Point<N>::operator[](size_t i) const
{
    return data[i];
}
template<size_t N>
double &Point<N>::operator[](size_t i)
{
    return data[i];
}
template<size_t N>
size_t Point<N>::size() const
{
    return N;
}

#endif //KNN__POINT_H_
