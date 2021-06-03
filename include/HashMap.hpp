#ifndef TICKET_SYSTEM_2020ACM_HASHMAP_HPP
#define TICKET_SYSTEM_2020ACM_HASHMAP_HPP

template<int MOD, class Key, class Value>
class HashMap{
private:
    struct node {
        Key key;
        Value val;
        node *nxt;

        node() : nxt(nullptr) {}
        node(const Key& key_, const Value &val_, node *nxt_ = nullptr)
            : key(key_), val(val_), nxt(nxt_) {}
    };

    node *head[MOD];
    std::hash<Key> hash;

    inline size_t gethash(const Key &key) const {
        return hash(key) % MOD;
    }

public:
    HashMap() { memset(head, 0, sizeof(head)); }

    ~HashMap(){
        for (int i = 0; i < MOD; ++i){
            for (node *p = head[i], *q; p; p = q){
                q = p->nxt;
                delete p;
            }
        }
    }

    void clear(){
        for (int i = 0; i < MOD; ++i){
            for (node *p = head[i], *q; p; p = q){
                q = p->nxt;
                delete p;
            }
            head[i] = nullptr;
        }
    }

    size_t count(const Key &key) const{
        size_t pos = gethash(key);
        for (node *p = head[pos]; p; p = p->nxt){
            if (p->key == key) return 1;
        }
        return 0;
    }

    Value &operator[](const Key &key){
        size_t pos = gethash(key);
        for (node *p = head[pos]; p; p = p->nxt){
            if (p->key == key) return p->val;
        }
        insert(key, Value());
        return pos[head]->val;
    }

    void insert(const Key &key, const Value &val){
        size_t pos = gethash(key);
        head[pos] = new node(key, val, head[pos]);
    }

    void erase(const Key &key){
        size_t pos = gethash(key);
        if (head[pos] && key == head[pos]->key){
            node *t = head[pos];
            head[pos] = head[pos]->nxt;
            delete t;
        }
        for (node *p = head[pos]; p; p = p->nxt){
            if (p->nxt && p->nxt->key == key){
                node *t = p->nxt;
                p->nxt = t->nxt;
                delete t; break;
            }
        }
    }

};

#endif //TICKET_SYSTEM_2020ACM_HASHMAP_HPP
