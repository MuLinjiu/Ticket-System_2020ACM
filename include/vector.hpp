#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"
#include <iostream>
#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */

    template<typename T>
    class vector {
    private:
        T **first;
        int last = 0;
        int endd = 0;
    public:
        /**
         * TODO
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        /**
         * you can see RandomAccessIterator at CppReference for help.
         */
        class const_iterator;
        class iterator {
            friend class vector;
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            vector<T>*vec;
            T *pointer = nullptr;
            int pos = 0;
        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            iterator() {};

            iterator(vector<T> *a,T * p,int pos1 = 0) : vec(a),pos(pos1),pointer(p){}

            iterator(const iterator &other):vec(other.vec), pos(other.pos),pointer(other.pointer){
            }
            ~iterator(){
                pointer = nullptr;
            }

            iterator operator+(const int &n) const {
                //TODO
                return iterator(this->vec,this->pointer,this->pos + n);
            }
            iterator operator-(const int &n) const {
                //TODO
                return iterator(this->vec,this->pointer,this->pos - n);
            }
            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                //TODO
                if(this->vec == rhs.vec){
                    return(this->pos - rhs.pos);
                }
                else throw invalid_iterator();
            }
            iterator& operator+=(const int &n) {
                //TODO
                for(int i = 1 ; i <= n ; i++){
                    ++(*this);
                }
                return *this;
            }
            iterator& operator-=(const int &n) {
                //TODO
                for(int i = 1 ; i <= n ; i++){
                    --(*this);
                }
                return *this;
            }
            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator a(*this);
                pos++;
                return a;
            }
            /**
             * TODO ++iter
             */
            iterator& operator++() {
                pos++;
                return *this;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator a(*this);
                pos--;
                return a;
            }
            /**
             * TODO --iter
             */
            iterator& operator--() {
                pos--;
                return *this;
            }
            /**
             * TODO *it
             */
            T& operator*() const{
                return *(vec->first[pos]);
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                if(this->vec == rhs.vec && this->pos == rhs.pos)return true;
                else return false;
            }
            bool operator==(const const_iterator &rhs) const {
                if(this->vec == rhs.vec && this->pos == rhs.pos)return true;
                else return false;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                if(this->vec == rhs.vec && this->pos == rhs.pos)return false;
                else return true;
            }
            bool operator!=(const const_iterator &rhs) const {
                if(this->vec == rhs.vec && this->pos == rhs.pos)return false;
                else return true;
            }
        };
        /**
         * TODO
         * has same function as iterator, just for a const object.
         */
        class const_iterator {
            friend class vector;
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            const vector<T>*vec;
            T *pointer = nullptr;
            int pos = 0;
        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            const_iterator() = default;

            const_iterator(const vector<T>* a,T * p,int pos1 = 0) : vec(a),pos(pos1),pointer(p){
            }

            ~const_iterator(){
                pointer == nullptr;
            }

            const_iterator operator+(const int &n) const {
                //TODO
                for(int i = 1 ; i <= n ; i++){
                    ++(*this);
                }
                const_iterator a(this->vec,this->pointer,this->pos + n);
                return a;
            }
            const_iterator operator-(const int &n) const {
                //TODO
                for(int i = 1 ; i <= n ; i++){
                    --(*this);
                }
                const_iterator a(this->vec,this->pointer,this->pos - n);
                return a;
            }
            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                //TODO
                if(this->vec == rhs.vec){
                    return(this->pos - rhs.pos);
                }
                else throw invalid_iterator();
            }
            const_iterator& operator+=(const int &n) {
                //TODO
                for(int i = 1 ; i <= n ; i++){
                    ++(*this);
                }
                return *this;
            }
            const_iterator& operator-=(const int &n) {
                //TODO
                for(int i = 1 ; i <= n ; i++){
                    --(*this);
                }
                return *this;
            }
            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                const_iterator a(*this);
                pos++;
                pointer = vec->first[pos];
                return a;
            }
            /**
             * TODO ++iter
             */
            const_iterator& operator++() {
                pos++;
                return *this;
            }
            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                const_iterator a(*this);
                pos--;
                pointer = vec->first[pos];
                return a;
            }
            /**
             * TODO --iter
             */
            const_iterator& operator--() {
                pos--;
                pointer = vec->first[pos];
                return *this;
            }
            /**
             * TODO *it
             */
            T& operator*() const{
                return *(vec->first[pos]);
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                if(*(this->vec) == *(rhs.vec) &&this->pos == rhs.pos)return true;
                else return false;
            }
            bool operator==(const const_iterator &rhs) const {
                if(*(this->vec) == *(rhs.vec) && this->pos == rhs.pos)return true;
                else return false;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                if(*(this->vec) == *(rhs.vec) && this->pos == rhs.pos)return false;
                else return true;
            }
            bool operator!=(const const_iterator &rhs) const {
                if((this->vec) == (rhs.vec) && this->pos == rhs.pos)return false;
                else return true;
            }
        };
        /**
         * TODO Constructs
         * Atleast two: default constructor, copy constructor
         */
        vector(int size = 20) {
            first = new T* [size];
            for(int i = 0 ; i < 20 ; i++)first[i] = nullptr;
            last = -1;
            endd = size;
        }
        vector(const vector<T> &other) {
            endd = other.endd;
            last = other.last;
            first = new T*[other.endd];
            for(int i = 0 ; i <= other.last ; i++){
                first[i] = new T (*(other.first[i]));
            }
        }
        /**
         * TODO Destructor
         */
        ~vector() {
            for(int i = 0 ; i <= last ; i++){
                if(first[i])delete first[i];
            }
            delete []first;
        }
        /**
         * TODO Assignment operator
         */
        vector &operator=(const vector &other) {
            if(this->first == other.first && this->last == other.last)return *this;
            else{
                int size = other.endd;
                int len = other.last;
                for(int i = 0 ; i <= last ; i++){
                    if (first[i]) delete first[i];
                }
                delete []first;
                first = new T *[size];
                for(int i = 0 ; i <= len ; i++){
                    first[i] = new T (*(other.first[i]));
                }
                last = other.last;
                endd = other.endd;
                return *this;
            }
        }
        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         */
        T & at(const size_t &pos) {
            if(pos < 0 || pos >= size())throw index_out_of_bound();
            else return *(first[pos]);
        }
        const T & at(const size_t &pos) const {
            if(pos < 0 || pos > last)throw index_out_of_bound();
            return *(first[pos]);
        }
        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T & operator[](const size_t &pos) {
            if(pos < 0 || pos > last)throw index_out_of_bound();
            else return *(first[pos]);
        }
        const T & operator[](const size_t &pos) const {
            if(pos < 0 || pos > last)throw index_out_of_bound();
            else return *(first[pos]);
        }
        /**
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T & front() const {
            if(last == -1)throw container_is_empty();
            else return *(first[0]);
        }
        /**
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T & back() const {
            if(last == -1)throw container_is_empty();
            else return *(first[last]);
        }
        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            iterator a(this,*first,0);
            return a;
        }
        const_iterator cbegin() const {
            const_iterator a(this,*first,0);
            return a;
        }
        /**
         * returns an iterator to the end.
         */
        iterator end() {
            iterator a(this,first[last],size());
            return a;
        }
        const_iterator cend() const {
            const_iterator a(this,first[last],size());
            return a;
        }
        /**
         * checks whether the container is empty
         */
        bool empty() const {
            return ( last == -1 );
        }
        /**
         * returns the number of elements
         */
        size_t size() const {
            return last + 1;
        }
        /**
         * clears the contents
         */
        void clear() {
            for(int i = 0 ; i <= last  ; i++){
                if(first[i])delete first[i];
            }
            delete []first;
            first = new T *[20];
        }

        void expand(){
            int size = endd;
            T ** first2;
            first2 =  new T*[size * 2];
            for(int i = 0 ; i < size ; i++){
                first2[i] = new T(*first[i]);
            }
            for(int i = 0 ; i <= last  ; i++){
                if(first[i])delete first[i];
            }
            delete []first;
            first = first2;
            endd = 2 * size;
        }
        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T &value) {
            if(pos.vec != this)throw"1";
            if(last == endd - 1)expand();
            for (int i = last; i >= pos.pos; i--)first[i + 1] = first[i];
            last++;
            first[pos.pos] = new T(value);
            pos++;
            return pos;
        }
        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            if(ind < 0 || ind >= size())throw index_out_of_bound();
            if(last == endd - 1)expand();
            for (int i = last; i >= ind; i--)first[i + 1] = first[i];
            last++;
            first[ind] = new T(value);
            iterator a(this,*(first + ind + 1) ,ind + 1);
        }
        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            delete (first[pos.pos]);
            for(int i = pos.pos ; i < last; i++){
                first[i] = first[i + 1];
            }
            last--;
            return pos;
        }
        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t &ind) {
            if(ind < 0 || ind >= size())throw index_out_of_bound();
            delete (first[ind]);
            for(int i = ind ; i < last; i++){
                first[i] = first[i + 1];
            }
            last--;
            return(*this,*(first + ind) , ind);
        }
        /**
         * adds an element to the end.
         */
        void push_back(const T &value) {
            if(last + 1 == endd) expand();
            first[last + 1] = new T(value);
            ++last;
        }
        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back() {
            if(size() == 0)throw container_is_empty();
            delete (first[last]);
            last--;
        }
    };


}

#endif
