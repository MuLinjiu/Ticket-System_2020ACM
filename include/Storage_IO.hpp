#ifndef TICKET_SYSTEM_2020ACM_STORAGE_IO_HPP
#define TICKET_SYSTEM_2020ACM_STORAGE_IO_HPP

#include <fstream>
#include <string>
#include "HashMap.hpp"
using namespace std;

template<class Value>
class Storage_IO{
    friend class LRU;
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
                node *prev = nullptr, *next = nullptr;

                node(const int &index_, node *prev = nullptr, node *next = nullptr)
                    : index(index_), prev(prev), next(next) {}
            };

            node *head, *end;
            int num;
            HashMap<hashsize, int, node *> pos;

            void remove(node *no){
                if (no == end) {
                    end = end->prev;
                    if (end) end->next = nullptr;
                    else head = end;
                }
                else if (no == head) {
                    head = head->next;
                    if (head) head->prev = nullptr;
                    else end = head;
                }
                else {
                    no->prev->next = no->next;
                    no->next->prev = no->prev;
                }
                no->prev = no->next = nullptr;
            }

            void add(node *no){
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
            }

        public:

            Link_Map() : num(0), head(nullptr), end(nullptr) {}

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
                head = end = nullptr, num = 0;
                pos.clear();
            }

            virtual size_t size() { return num; }

            virtual int begin() { return head ? head->index : 0; }

            virtual int nxt(int key) {
                node *no = pos[key];
                return no->next ? no->next->index : 0;
            }

            virtual void remove(int key) {
                node *no = pos[key];
                remove(no), delete no;
                pos.erase(key);
                num--;
            }

            virtual int add(int key) {
                node *no = new node(key);
                add(no);
                pos.insert(key, no);
                if (++num > limit){
                    int tmp = head->index;
                    Link_Map::remove(head->index);
                    return tmp;
                }
                return 0;
            }

            virtual void refresh(int key) {
                node *no = pos[key];
                remove(no), add(no);
            }

            virtual bool find(int key) const {
                return pos.count(key) == 1;
            }
        } queue;

        class Cached_Link_Map : public Link_Map {
        private:
            Storage_IO<Value> *opt;
            HashMap<hashsize, int, Value> cache;

        public:
            explicit Cached_Link_Map(Storage_IO<Value> *opt_) : opt(opt_), Link_Map() {}

            ~Cached_Link_Map(){
                opt->file.open(opt->file_name, ios::in | ios::out | ios::binary);
                for (int p = Link_Map::begin(); p; p = Link_Map::nxt(p)){
                    opt->file.seekp(get_pos(p));
                    opt->file.write(reinterpret_cast<char *> (&cache[p]), sizeof(Value));
                }
                opt->file.close();
            }

            void clear() override {
                Link_Map::clear();
                cache.clear();
            }

            void remove(int key) override {
                Link_Map::remove(key);
                cache.erase(key);
            }

            void add(int key, const Value &val) {
                int res = Link_Map::add(key);
                cache.insert(key, val);
                if (res){
                    opt->file.seekp(get_pos(res));
                    opt->file.write(reinterpret_cast<char *> (&cache[res]), sizeof(Value));
                    cache.erase(res);
                }
            }

            const Value &get(int key) { return cache[key]; }

            void modify(int key, const Value &val) { cache[key] = val; }

        } cache;

        Storage_IO<Value> *opt;

    public:
	    explicit LRU(Storage_IO<Value> *opt_) : opt(opt_), cache(opt_) {}

	    void clear() { queue.clear(), cache.clear(); }

	    void create(int key) { queue.add(key); }

	    void push(int key, const Value &val){
	        if (cache.find(key)) cache.modify(key, val), cache.refresh(key);
	        else if (queue.find(key)){
	            queue.remove(key), cache.add(key, val);
	        } else {
	            queue.add(key);
                opt->file.seekp(get_pos(key));
                opt->file.write(reinterpret_cast<char *> (const_cast<Value *> (&val)), sizeof(Value));
	        }
	    }

	    void get(int key, Value &val){
	        if (cache.find(key)) val = cache.get(key), cache.refresh(key);
	        else if (queue.find(key)) {
	            queue.remove(key);
                opt->file.seekg(get_pos(key));
                opt->file.read(reinterpret_cast<char *> (&val), sizeof(Value));
                cache.add(key, val);
            } else {
	            queue.add(key);
                opt->file.seekg(get_pos(key));
                opt->file.read(reinterpret_cast<char *> (&val), sizeof(Value));
	        }
	    }

	    void erase(int key){
	        if (queue.find(key)) queue.remove(key);
	        if (cache.find(key)) cache.remove(key);
	    }

    } LRU;

public:
	Storage_IO() : LRU(this) {}

	explicit Storage_IO(string file_name) : file_name(file_name), LRU(this){
		file.open(file_name, ios::in | ios::out | ios::binary);
		if (!file){
			file.open(file_name, ios::out | ios::binary);
			int zero = 0;
			file.write(reinterpret_cast<char *> (&zero), sizeof(int));
			file.close();
			file.open(file_name, ios::in | ios::out | ios::binary);
		}
	}

	~Storage_IO(){
	    file.close();
	}

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
		}make
		//LRU.create(num);
		return num;
	}

	void write(int num, const Value &val){
		LRU.push(num, val);
        // file.seekp(get_pos(num));
		// file.write(reinterpret_cast<char *> (const_cast<Value *> (&val)), sizeof(Value));
	}

	void read(int num, Value &val){
		LRU.get(num, val);
        // file.seekg(get_pos(num));
		// file.read(reinterpret_cast<char *> (&val), sizeof(Value));
	}

	void erase(int num){
        LRU.erase(num);
		int pos = get_pos(num), nxt;
		file.seekg(0);
		file.read(reinterpret_cast<char *> (&nxt), sizeof(int));
		file.seekp(pos + sizeof(Value));
		file.write(reinterpret_cast<char *> (&nxt), sizeof(int));
		file.seekp(0);
		file.write(reinterpret_cast<char *> (&num), sizeof(int));
	}

};

#endif //TICKET_SYSTEM_2020ACM_STORAGE_IO_HPP
