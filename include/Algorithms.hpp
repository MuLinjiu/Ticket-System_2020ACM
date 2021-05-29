#ifndef TICKET_SYSTEM_2020ACM_ALGORITHMS_HPP
#define TICKET_SYSTEM_2020ACM_ALGORITHMS_HPP

#include "vector.hpp"
using namespace sjtu;

template<class T>
T *upper_bound(T *begin, T *end, T num){
    int l = -1, r = end - begin;
    while (l + 1 < r){
        int mid = (l + r) >> 1;
        if (num < *(begin + mid)) r = mid; else l = mid;
    }
    return begin + r;
}

template<class T>
T *lower_bound(T *begin, T *end, T num){
    int l = -1, r = end - begin;
    while (l + 1 < r){
        int mid = (l + r) >> 1;
        if (num <= *(begin + mid)) r = mid; else l = mid;
    }
    return begin + r;
}

template<class T>
void sort(typename vector<T>::iterator begin, typename vector<T>::iterator end, bool (*cmp)(const T &, const T &)){
    int len = end - begin;
    if (len <= 1) return ;
    typename vector<T>::iterator i = begin, j = end - 1;
    T pivot = *(begin + (len + 1) / 2 - 1);
    while (j - i >= 0){
        while (cmp(*i, pivot)) i++;
        while (cmp(pivot, *j)) j--;
        if (j - i >= 0){
            swap(*i, *j);
            i++, j--;
        }
    }
    if (j - begin > 0) sort(begin, i, cmp);
    if (end - i > 1) sort(i, end, cmp);
}

#endif //TICKET_SYSTEM_2020ACM_ALGORITHMS_HPP
