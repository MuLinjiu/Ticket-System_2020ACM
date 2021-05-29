#ifndef MAP_HPP_USER_HPP
#define MAP_HPP_USER_HPP
#include <cstring>
#include <iostream>
#include "BpTree.hpp"
#include "String.hpp"
#include "Date.hpp"
#include "Train.hpp"
#include "Order.hpp"
using namespace std;
using namespace sjtu;
class USER_ALL{
private:
    struct user{
        String username;
        String password;
        String name;
        String address;
        int privilege = -1;
        int num_order = 0;
        user(const String & u, const String & pa, const String & n,const String & add, int p) {
            username = u;
            password = pa;
            name = n;
            address = add;
            privilege = p;
        }
        user() = default;
        friend ostream &operator << (ostream &os, const user &x){
            os << x.username << " " << x.name << " " << x.address << " " << x.privilege;
            return os;
        }
    };

    BpTree<String, user> user_tree;
    BpTree<pair<String, int>, Order> order_tree;
    map<String, user> user_online;

    template<class T, class U>
    static bool equal(const pair<T, U> &lhs, const pair<T, U> &rhs) {
        return lhs.first == rhs.first;
    }

public:
    USER_ALL() : user_tree("user"), order_tree("order") {}

    bool checkadduser(const String &cur,const String &us,int p){
        vector<user>possibleoffset = user_tree.search(cur);
        vector<user>possibleoffset2 = user_tree.search(us);
        if(possibleoffset.empty() || !possibleoffset2.empty())return false;
        if(user_online.find(cur) == user_online.end())return false;
        if(possibleoffset[0].privilege <= p)return false;
        else return true;
    }

    bool checkquerypofile(const String &cur,const String &us){
        vector<user>possibleoffset = user_tree.search(cur);
        vector<user>possibleoffset2 = user_tree.search(us);
        if(user_online.find(cur) == user_online.end())return false;
        if(possibleoffset.empty() || possibleoffset2.empty())return false;
        if(us == cur)return true;
        if(possibleoffset[0].privilege <= possibleoffset2[0].privilege)return false;
        else return true;
    }

    bool checkmodifypofile(const String &cur,const String &us,int c){
        vector<user>possibleoffset = user_tree.search(cur);
        vector<user>possibleoffset2 = user_tree.search(us);
        if(user_online.find(cur) == user_online.end())return false;
        if(possibleoffset.empty() || possibleoffset2.empty())return false;
        if(us == cur)return true;
        if(possibleoffset[0].privilege <= possibleoffset2[0].privilege || possibleoffset[0].privilege <= c)return false;
        else return true;
    }

    void add_user(const String & u, const String & pa, const String & n,const String & add, int p){
        user tmp(u,pa,n,add,p);
        if(!user_tree.insert(u,tmp)){
            throw("invalid_insert");
        }
    }

    void login(const String & userna, const String & pas){
        vector<user>possibleoffset = user_tree.search(userna);
        if(possibleoffset.empty()){
            throw("cannot find the user");
        }else{
            if(user_online.find(userna) == user_online.end()) {
                if(possibleoffset[0].password == pas) {
                    user_online[userna] = possibleoffset[0];
                }else throw("wrong pass");
            }else {
                throw("user is online");
            }
        }
    }

    void logout(const String & usn){
        if(user_online.find(usn) == user_online.end()) throw("user haven't login");
        else user_online.erase(user_online.find(usn));
    }

    void query_pofile(const String & a){
        vector<user>possibleoffset = user_tree.search(a);
        if(possibleoffset.empty())throw("cannot find the user");
        else {
            cout << possibleoffset[0].username << " " << possibleoffset[0].name << " " << possibleoffset[0].address << " " << possibleoffset[0].privilege << endl;
        }
    }

    void modify_profile(const String & usern,const String & password, const String &name, const String &add, int pr){
        vector<user>possibleoffset = user_tree.search(usern);
        if(possibleoffset.empty())throw("cannot find the user");
        else{
            if(!password.empty())possibleoffset[0].password = password;
            if(!name.empty())possibleoffset[0].name = name;
            if(!add.empty())possibleoffset[0].address = add;
            if(pr != 0)possibleoffset[0].privilege = pr;
            user_tree.modify(usern,possibleoffset[0]);
            cout << possibleoffset[0] << endl;
        }
    }

    void buy_ticket(TRAIN_ALL &train, const String &u, const String &id, const Date &d, int n, const String &f, const String &t, bool q){
        vector<user> users = user_tree.search(u);
        if (users.empty()) throw("cannot find the user");
        if (user_online.find(u) == user_online.end()) throw("user haven't login");
        Order res = train.query_ticket(id, d, f, t);
        if (res.num < n && !q) throw("no ticket");
        res.order_num = ++users[0].num_order;
        user_tree.modify(u, users[0]);
        res.userID = u;
        if (res.num >= n){
            res.status = "success", res.num = n;
            train.buy_ticket(id, d, n, f, t);
            cout << res.price * n << endl;
        } else {
            res.status = "pending", res.num = n;
            train.queue(id, res);
            cout << "queue" << endl;
        }
        order_tree.insert(make_pair(u, res.order_num), res);
    }

    void query_order(const String &u){
        vector<user> users = user_tree.search(u);
        if (users.empty()) throw("cannot find the user");
        if (user_online.find(u) == user_online.end()) throw("user haven't login");
        vector<Order> res = order_tree.search(make_pair(u, 0), equal);
        cout << res.size() << endl;
        for (int i = (int)res.size() - 1; i >= 0; --i)
            cout << res[i] << endl;
    }

    void refund_ticket(TRAIN_ALL &train, const String &u, int n = 1){
        vector<user> users = user_tree.search(u);
        if (users.empty()) throw("cannot find the user");
        if (user_online.find(u) == user_online.end()) throw("user haven't login");
        n = users[0].num_order - n + 1;
        vector<Order> res = order_tree.search(make_pair(u, n));
        if (res.size() != 1) throw("cannot find the order");
        if (res[0].status == "refunded") throw("already refunded");
        auto updated = train.refund(res[0]);
        for (const auto &x : updated)
            order_tree.modify(make_pair(x.userID, x.order_num), x);
        res[0].status = "refunded";
        order_tree.modify(make_pair(u, n), res[0]);
    }

    void clean(){
        user_online.clear(), user_tree.clear(), order_tree.clear();
    }

};
#endif //MAP_HPP_USER_HPP
