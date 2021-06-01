#ifndef TICKET_SYSTEM_2020ACM_STORAGE_IO_HPP
#define TICKET_SYSTEM_2020ACM_STORAGE_IO_HPP

#include <fstream>
#include <string>
#include "HashMap.hpp"
using namespace std;

template<class Value>
class Storage_IO{
    class LRU;
    friend LRU;
private:
    string file_name;
	fstream file;

	static int get_pos(int num){
		return (sizeof(int) + sizeof(Value)) * (num - 1) + sizeof(int);
	}

	static int get_num(int pos){
		return (pos - sizeof(int)) / (sizeof(int) + sizeof(Value)) + 1;
	}

	class LRU {
	private:
        static const int limit = 293;
        static const int hashsize = 397;

        class Link_Map {
        private:

            struct node {
                int index;
                node *prev = nullptr;
                node *next = nullptr;

                node(const int &index_, node *prev = nullptr, node *next = nullptr)
                    : index(index_), prev(prev), next(next) {}
            };

            node *head, *end;
            int size = 0;
            HashMap<hashsize, int, node *> pos;

        public:

            Link_Map() : head(nullptr), end(nullptr) {}

            virtual ~Link_Map() {
                for (node *p = head, *q; p; p = q) {
                    q = p->next;
                    delete p;
                }
            }

            virtual void clear() {
                for (node *p = head, *q; p; p = q) {
                    q = p->next;
                    delete p;
                }
                head = end = nullptr, size = 0;
                pos.clear();
            }

            virtual void remove(int key) {
                node *no = pos[key];
                if (no == end) {
                    node *tmp = end;
                    end = end->prev;
                    delete tmp;
                    if (end) end->next = nullptr;
                    else head = end;
                }
                else if (no == head) {
                    node *tmp = head;
                    head = head->next;
                    delete tmp;
                    if (head) head->prev = nullptr;
                    else end = head;
                }
                else {
                    no->prev->next = no->next;
                    no->next->prev = no->prev;
                    delete no;
                }
                pos.erase(key);
                size--;
            }

            virtual void add(int key) {
                node *no = new node(key);
                if (end != nullptr) {
                    end->next = no;
                    end->next->prev = end;
                    end->next->next = nullptr;
                    end = end->next;
                }
                if (head == nullptr) {
                    end = head = no;
                    head->next = nullptr;
                    head->prev = nullptr;
                }
                pos.insert(key, no);
                size++;
                if (size > limit) remove(head->index);
            }

            virtual bool find(int key) const {
                return pos.count(key) == 1;
            }
        } queue;

        class Cached_Link_Map : public Link_Map {
        private:
            HashMap<hashsize, int, Value> cache;

        public:
            Cached_Link_Map() : Link_Map() {}

            void clear() override {
                Link_Map::clear();
                cache.clear();
            }

            void remove(int key) override {
                Link_Map::remove(key);
                cache.erase(key);
            }

            void add(int key, const Value &val) {
                Link_Map::add(key);
                cache.insert(key, val);
            }

            void refresh(int key, const Value &val) {
                remove(key), add(key, val);
            }

            Value get(int key) const { return cache[key]; }

        } cache;

    public:
	    LRU() = default;

	    void clear() { queue.clear(), cache.clear(); }

	    void push(int key, const Value &val){
	        if (cache.find(key)) cache.refresh(key, val);
	        else if (queue.find(key)){
	            queue.remove(key), cache.add(key, val);
	        } else queue.add(key);
	    }

	    void get(Storage_IO<Value> &opt, int key, Value &val){
	        if (cache.find(key)) val = cache.get(key), cache.refresh(key, val);
	        else if (queue.find(key)) {
	            queue.remove(key);
                opt.file.seekg(get_pos(key));
                opt.file.read(reinterpret_cast<char *> (&val), sizeof(Value));
                cache.add(key, val);
            } else {
	            queue.add(key);
                opt.file.seekg(get_pos(key));
                opt.file.read(reinterpret_cast<char *> (&val), sizeof(Value));
	        }
	    }

	    void erase(int key){
	        if (queue.find(key)) queue.remove(key);
	        if (cache.find(key)) cache.remove(key);
	    }

    } LRU;

public:
	Storage_IO() = default;

	Storage_IO(string file_name) : file_name(file_name){
		file.open(file_name, ios::in | ios::out | ios::binary);
		if (!file){
			file.open(file_name, ios::out | ios::binary);
			int zero = 0;
			file.write(reinterpret_cast<char *> (&zero), sizeof(int));
			file.close();
			file.open(file_name, ios::in | ios::out | ios::binary);
		}
	}

	~Storage_IO(){ file.close(); }

	void clear(){
	    LRU.clear();
	    file.close();
        file.open(file_name, ios::out | ios::binary);
        int zero = 0;
        file.write(reinterpret_cast<char *> (&zero), sizeof(int));
        file.close();
        file.open(file_name, ios::in | ios::out | ios::binary);
	}

	int write(const Value &val){
		file.seekg(0);
		int nxt = 0, pos, num;
		file.read(reinterpret_cast<char *> (&nxt), sizeof(int));
		if (!nxt){
			file.seekp(0, ios::end);
			pos = file.tellp(), num = get_num(pos);
			file.write(reinterpret_cast<char *> (const_cast<Value *> (&val)), sizeof(Value));
			file.write(reinterpret_cast<char *> (&nxt), sizeof(int));
		} else {
			num = nxt, pos = get_pos(num);
			file.seekg(pos + sizeof(Value));
			file.read(reinterpret_cast<char *> (&nxt), sizeof(int));
			file.seekp(0);
			file.write(reinterpret_cast<char *> (&nxt), sizeof(int));
			file.seekp(pos);
			file.write(reinterpret_cast<char *> (const_cast<Value *> (&val)), sizeof(Value));
		}
		LRU.push(num, val);
		return num;
	}

	void write(int num, const Value &val){
		file.seekp(get_pos(num));
		file.write(reinterpret_cast<char *> (const_cast<Value *> (&val)), sizeof(Value));
		LRU.push(num, val);
	}

	void read(int num, Value &val){
		LRU.get(*this, num, val);
	}

	void erase(int num){
		int pos = get_pos(num), nxt;
		file.seekg(0);
		file.read(reinterpret_cast<char *> (&nxt), sizeof(int));
		file.seekp(pos + sizeof(Value));
		file.write(reinterpret_cast<char *> (&nxt), sizeof(int));
		file.seekp(0);
		file.write(reinterpret_cast<char *> (&num), sizeof(int));
		LRU.erase(num);
	}

};

#endif //TICKET_SYSTEM_2020ACM_STORAGE_IO_HPP
