#ifndef TICKET_SYSTEM_2020ACM_BPTREE_HPP
#define TICKET_SYSTEM_2020ACM_BPTREE_HPP

#include <fstream>
#include <string>
#include "vector.hpp"
#include "Storage_IO.hpp"
using namespace std;
using namespace sjtu;

template<class Key, class Value>
class BpTree{
private:
	static const int M = 228; // Maximum number of fans out
	struct node{
		int num_keys; // 1-based
		bool is_leaf;
		int prec, succ;	// if node is leaf
		Key keys[M + 10]; // 0-based
		union birec{
			int child, index;
		} birec[M + 10]; // inner node: child; leaf: index

		node() : num_keys(0), is_leaf(false), prec(0), succ(0){
			memset(birec, 0, sizeof(birec));
		}
	};
	int root;
	Storage_IO<node> bpt_node_file;
	Storage_IO<Value> data_file;
	fstream bpt_basic_file;

	void insert(int current, const Key &key, int index){ // current-x, child-y, new_child-z
		node x, y;
		int ptr = 0, child;
		bpt_node_file.read(current, x);
		if (x.is_leaf){
			for (; ptr < x.num_keys && x.keys[ptr] < key; ++ptr);
			for (int i = x.num_keys; i > ptr; i--)
				x.keys[i] = x.keys[i-1], x.birec[i].index = x.birec[i-1].index;
			x.num_keys++;
			x.keys[ptr] = key, x.birec[ptr].index = index;
			bpt_node_file.write(current, x);
			return ;
		}
		for (; ptr < x.num_keys && x.keys[ptr] < key; ++ptr);
		child = x.birec[ptr].child;
		insert(child, key, index);
		bpt_node_file.read(child, y);
		if (y.is_leaf && y.num_keys > M || !y.is_leaf && y.num_keys >= M){
			int l = y.num_keys / 2, r = y.num_keys - l;
			node z;
			int new_child = bpt_node_file.write(z);
			y.num_keys = l, z.num_keys = r;
			if (y.is_leaf){
				z.is_leaf = true;
				z.succ = y.succ, y.succ = new_child, z.prec = child;
				for (int i = 0; i < r; ++i)
					z.keys[i] = y.keys[l+i], z.birec[i].index[i] = y.birec[l+i].index;
			} else {
				r--, z.num_keys--;
				for (int i = 0; i < r; ++i)
					z.keys[i] = y.keys[l+i+1];
				for (int i = 0; i <= r; ++i)
					z.birec[i].child = y.birec[l+i+1].child;
			}
			bpt_node_file.write(child, y), bpt_node_file(new_child, z);
			for (int i = x.num_keys; i > ptr; i--)
				x.keys[i] = x.keys[i-1];
			for (int i = x.num_keys + 1; i > ptr; i--)
				x.birec[i].index = x.birec[i-1].index;
			x.num_keys++;
			x.keys[ptr] = y.keys[l];
			x.birec[ptr].child = child, x.birec[ptr+1].child = new_child;
			bpt_node_file.write(current, x);
		}
	}

public:
	BpTree() = default;

	BpTree(string file_name) : bpt_node_file("bpt_" + file_name + "_node.dat"), data_file("data_" + file_name + ".dat"){
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

};

#endif //TICKET_SYSTEM_2020ACM_BPTREE_HPP
