#ifndef TICKET_SYSTEM_2020ACM_BPTREE_HPP
#define TICKET_SYSTEM_2020ACM_BPTREE_HPP

#include <fstream>
#include <string>
#include "vector.hpp"
#include "Storage_IO.hpp"
#include "Algorithms.hpp"
using namespace std;
//using namespace sjtu;

template<class Key, class Value>
class BpTree{
private:
    static const int M = 200; // Maximum number of fans out
    struct node{
        int num_keys; // 1-based
        bool is_leaf;
        int prec, succ;	// if node is leaf
        Key keys[M + 1]; // 0-based
        struct birec{
            int child;
            Value val;
        } birec[M + 1]; // inner node: child; leaf: val

        node() : num_keys(0), is_leaf(false), prec(0), succ(0){
            memset(birec, 0, sizeof(birec));
        }
    };

    int root;
    bool adjusted, updated;
    string file_name;
    Storage_IO<node> bpt_node_file;
    fstream bpt_basic_file;

    static bool equal(const Key &lhs, const Key &rhs) { return lhs == rhs; }

    int split(int child, node &y){
        int l = y.num_keys / 2, r = y.num_keys - l;
        node z;
        int new_child = bpt_node_file.write(z);
        y.num_keys = l, z.num_keys = r;
        if (y.is_leaf){
            z.is_leaf = true;
            z.succ = y.succ, y.succ = new_child, z.prec = child;
            if (z.succ){
                node tmp; bpt_node_file.read(z.succ, tmp);
                tmp.prec = new_child;
                bpt_node_file.write(z.succ, tmp);
            }
            for (int i = 0; i < r; ++i)
                z.keys[i] = y.keys[l+i], z.birec[i].val = y.birec[l+i].val;
        } else {
            r--, z.num_keys--;
            for (int i = 0; i < r; ++i)
                z.keys[i] = y.keys[l+i+1];
            for (int i = 0; i <= r; ++i)
                z.birec[i].child = y.birec[l+i+1].child;
        }
        bpt_node_file.write(child, y), bpt_node_file.write(new_child, z);
        return new_child;
    }

    void merge(int current, node &x, int child, node &y, int neighbor, node &z, int ptr) {
        if (y.is_leaf) {
            for (int i = 0; i < z.num_keys; ++i, ++y.num_keys)
                y.keys[y.num_keys] = z.keys[i], y.birec[y.num_keys].val = z.birec[i].val;
            y.succ = z.succ;
            if (z.succ){
                node tmp; bpt_node_file.read(z.succ, tmp);
                tmp.prec = child;
                bpt_node_file.write(z.succ, tmp);
            }
        } else {
            y.keys[y.num_keys++] = x.keys[ptr];
            for (int i = 0; i < z.num_keys; ++i, ++y.num_keys)
                y.keys[y.num_keys] = z.keys[i], y.birec[y.num_keys].child = z.birec[i].child;
            y.birec[y.num_keys].child = z.birec[z.num_keys].child;
        }
        bpt_node_file.erase(neighbor);
        x.num_keys--;
        for (int i = ptr; i < x.num_keys; ++i)
            x.keys[i] = x.keys[i+1], x.birec[i+1].child = x.birec[i+2].child;
        bpt_node_file.write(current, x), bpt_node_file.write(child, y);
    }

    bool insert(int current, const Key &key, const Value &val){ // current-x, child-y, new_child-z
        node x, y;
        int ptr = 0, child;
        bpt_node_file.read(current, x);
        if (x.is_leaf){
            ptr = lower_bound(x.keys, x.keys + x.num_keys, key) - x.keys;
            if (x.keys[ptr] == key) return false;
            for (int i = x.num_keys; i > ptr; i--)
                x.keys[i] = x.keys[i-1], x.birec[i].val = x.birec[i-1].val;
            x.num_keys++;
            x.keys[ptr] = key, x.birec[ptr].val = val;
            bpt_node_file.write(current, x);
            adjusted = false;
            return true;
        }
        ptr = upper_bound(x.keys, x.keys + x.num_keys, key) - x.keys;
        child = x.birec[ptr].child;
        auto ret = insert(child, key, val);
        if (!ret) return false;
        if (adjusted) return ret;
        bpt_node_file.read(child, y);
        if (y.is_leaf && y.num_keys > M || !y.is_leaf && y.num_keys >= M){
            int new_child = split(child, y);
            for (int i = x.num_keys; i > ptr; i--)
                x.keys[i] = x.keys[i-1];
            for (int i = x.num_keys + 1; i > ptr; i--)
                x.birec[i].child = x.birec[i-1].child;
            x.num_keys++;
            x.keys[ptr] = y.keys[y.num_keys];
            x.birec[ptr].child = child, x.birec[ptr+1].child = new_child;
            bpt_node_file.write(current, x);
        } else adjusted = true;
        return ret;
    }

    pair<pair<bool, Key>, Value> erase(int current, const Key &key){ // current-x, child-y
        node x, y, z;
        int ptr = 0, child, neighbor;
        bpt_node_file.read(current, x);
        if (x.is_leaf){
            ptr = lower_bound(x.keys, x.keys + x.num_keys, key) - x.keys;
            Value val = x.birec[ptr].val;
            x.num_keys--;
            for (int i = ptr; i < x.num_keys; i++)
                x.keys[i] = x.keys[i+1], x.birec[i].val = x.birec[i+1].val;
            bpt_node_file.write(current, x);
            adjusted = false, updated = ptr > 0;
            if (!ptr) return make_pair(make_pair(true, x.keys[0]), val);
            else return make_pair(make_pair(false, Key()), val);
        }
        ptr = upper_bound(x.keys, x.keys + x.num_keys, key) - x.keys;
        child = x.birec[ptr].child;
        auto ret = erase(child, key);
        if (adjusted && updated) return ret;
        bpt_node_file.read(child, y);
        if (ret.first.first && ptr){
            x.keys[ptr-1] = ret.first.second;
            bpt_node_file.write(current, x);
            ret = make_pair(make_pair(false, Key()), ret.second);
            updated = true;
        }
        if (y.is_leaf && y.num_keys <= (M - 1) / 2 || !y.is_leaf && y.num_keys < (M - 1) / 2){
            if (ptr == x.num_keys) neighbor = x.birec[ptr-1].child;
            else neighbor = x.birec[ptr+1].child;
            bpt_node_file.read(neighbor, z);
            if (z.is_leaf && z.num_keys - 1 <= (M - 1) / 2 || !z.is_leaf && z.num_keys - 1 < (M - 1) / 2) {
                if (ptr == x.num_keys) merge(current, x, neighbor, z, child, y, ptr - 1);
                else merge(current, x, child, y, neighbor, z, ptr);
            } else { // borrow
                if (ptr == x.num_keys){
                    if (y.is_leaf){
                        for (int i = y.num_keys; i > 0; --i)
                            y.keys[i] = y.keys[i-1], y.birec[i].val = y.birec[i-1].val;
                        y.num_keys++, z.num_keys--;
                        y.keys[0] = z.keys[z.num_keys], y.birec[0].val = z.birec[z.num_keys].val;
                        x.keys[ptr-1] = y.keys[0];
                    } else {
                        y.birec[y.num_keys+1].child = y.birec[y.num_keys].child;
                        for (int i = y.num_keys; i > 0; --i)
                            y.keys[i] = y.keys[i-1], y.birec[i].child = y.birec[i-1].child;
                        y.num_keys++, z.num_keys--;
                        y.keys[0] = x.keys[ptr-1], y.birec[0].child = z.birec[z.num_keys+1].child;
                        x.keys[ptr-1] = z.keys[z.num_keys];
                    }
                } else {
                    if (y.is_leaf){
                        y.num_keys++, z.num_keys--;
                        y.keys[y.num_keys-1] = z.keys[0], y.birec[y.num_keys-1].val = z.birec[0].val;
                        for (int i = 0; i < z.num_keys; ++i)
                            z.keys[i] = z.keys[i+1], z.birec[i].val = z.birec[i+1].val;
                        x.keys[ptr] = z.keys[0];
                    } else {
                        y.num_keys++, z.num_keys--;
                        y.keys[y.num_keys-1] = x.keys[ptr], y.birec[y.num_keys].child = z.birec[0].child;
                        x.keys[ptr] = z.keys[0];
                        for (int i = 0; i < z.num_keys; ++i)
                            z.keys[i] = z.keys[i+1], z.birec[i].child = z.birec[i+1].child;
                        z.birec[z.num_keys].child = z.birec[z.num_keys+1].child;
                    }
                }
                bpt_node_file.write(current, x), bpt_node_file.write(child, y), bpt_node_file.write(neighbor, z);
            }
        } else adjusted = true;
        return ret;
    }

    pair<int, node> search(int current, const Key &key){
        node x;
        bpt_node_file.read(current, x);
        if (x.is_leaf) return make_pair(current, x);
        int ptr = upper_bound(x.keys, x.keys + x.num_keys, key) - x.keys;
        return search(x.birec[ptr].child, key);
    }

public:
    BpTree() = default;

    BpTree(string file_name) : file_name(file_name), bpt_node_file("bpt_" + file_name + "_node.dat"){
        bpt_basic_file.open("bpt_" + file_name + "_basic.dat", ios::in | ios::out | ios::binary);
        if (!bpt_basic_file){
            node tmp;
            tmp.is_leaf = true;
            root = bpt_node_file.write(tmp);
            bpt_basic_file.open("bpt_" + file_name + "_basic.dat", ios::out | ios::binary);
            bpt_basic_file.write(reinterpret_cast<char *> (&root), sizeof(int));
            bpt_basic_file.close();
            bpt_basic_file.open("bpt_" + file_name + "_basic.dat", ios::in | ios::out | ios::binary);
        } else {
            bpt_basic_file.seekg(0);
            bpt_basic_file.read(reinterpret_cast<char *> (&root), sizeof(root));
        }
    }

    ~BpTree(){ bpt_basic_file.close(); }

    void clear(){
        bpt_node_file.clear();
        bpt_basic_file.close();
        bpt_basic_file.open("bpt_" + file_name + "_basic.dat", ios::out | ios::binary);
        node tmp;
        tmp.is_leaf = true;
        root = bpt_node_file.write(tmp);
        bpt_basic_file.write(reinterpret_cast<char *> (&root), sizeof(int));
        bpt_basic_file.close();
        bpt_basic_file.open("bpt_" + file_name + "_basic.dat", ios::in | ios::out | ios::binary);
    }

    bool insert(const Key &key, const Value &val){
        if (!insert(root, key, val)) return false;
        node x, y;
        bpt_node_file.read(root, x);
            if (x.is_leaf && x.num_keys > M || !x.is_leaf && x.num_keys >= M){
            int new_child = split(root, x);
            y.num_keys = 1;
            y.keys[0] = x.keys[x.num_keys];
            y.birec[0].child = root, y.birec[1].child = new_child;
            root = bpt_node_file.write(y);
            bpt_basic_file.seekp(0);
            bpt_basic_file.write(reinterpret_cast<char *> (&root), sizeof(int));
        }
        return true;
    }

    bool modify(const Key &key, const Value &val) {
        pair<int, node> res = search(root, key);
        node x = res.second;
        int ptr = lower_bound(x.keys, x.keys + x.num_keys, key) - x.keys;
        if (ptr < x.num_keys && x.keys[ptr] == key){
            x.birec[ptr].val = val;
            bpt_node_file.write(res.first, x);
            return true;
        } else return false;
    }

    Value erase(const Key &key){
        auto ret = erase(root, key);
        node x; bpt_node_file.read(root, x);
        if (!x.num_keys && !x.is_leaf){
            bpt_node_file.erase(root);
            root = x.birec[0].child;
            bpt_basic_file.seekp(0);
            bpt_basic_file.write(reinterpret_cast<char *> (&root), sizeof(int));
        }
        return ret.second;
    }

    vector<Value> search(const Key &key, bool (*equ)(const Key &, const Key &) = equal){
        node x = search(root, key).second;
        int ptr = lower_bound(x.keys, x.keys + x.num_keys, key) - x.keys;
        if (ptr == x.num_keys){
            if (!x.succ) return vector<Value>();
            else {
                bpt_node_file.read(x.succ, x);
                ptr = 0;
            }
        }
        static vector<Value> ret;
        ret.clear();
        while(equ(x.keys[ptr], key)){
            ret.push_back(x.birec[ptr].val);
            if (++ptr >= x.num_keys){
                if (!x.succ) break;
                bpt_node_file.read(x.succ, x);
                ptr = 0;
            }
        }
        return ret;
    }

};

#endif //TICKET_SYSTEM_2020ACM_BPTREE_HPP
