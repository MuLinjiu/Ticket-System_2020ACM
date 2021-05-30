#ifndef MAP_HPP_USER_HPP
#define MAP_HPP_USER_HPP
#include <cstring>
#include <iostream>
#include<vector>
#include "BpTree.hpp"
#include "String.hpp"
#include "Date.hpp"
#include "Train.hpp"
#include "Order.hpp"
using namespace std;
//using namespace sjtu;
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

    BpTree<String> user_tree;
    Storage_IO<user> user_data;
    BpTree<pair<String, int>> order_tree;
    Storage_IO<Order> order_data;
    sjtu::map<String, bool> user_online;

    template<class T, class U>
    static bool equal(const pair<T, U> &lhs, const pair<T, U> &rhs) {
        return lhs.first == rhs.first;
    }

public:
    USER_ALL() : user_tree("user"), user_data("data_user.dat"), order_tree("order"), order_data("data_order.dat") {}

    bool checkadduser(const String &cur,const String &us,int p){
        if(user_online.find(cur) == user_online.end())return false;
        vector<int> possibleoffset = user_tree.search(cur);
        vector<int> possibleoffset2 = user_tree.search(us);
        if(possibleoffset.empty() || !possibleoffset2.empty())return false;
        user cur_user;
        user_data.read(possibleoffset[0], cur_user);
        if(cur_user.privilege <= p)return false;
        else return true;
    }

    bool checkquerypofile(const String &cur,const String &us){
        if(user_online.find(cur) == user_online.end())return false;
        vector<int>possibleoffset = user_tree.search(cur);
        vector<int>possibleoffset2 = user_tree.search(us);
        if(possibleoffset.empty() || possibleoffset2.empty())return false;
        if(us == cur)return true;
        user cur_user,query_user;
        user_data.read(possibleoffset[0],cur_user),user_data.read(possibleoffset2[0],query_user);
        if(cur_user.privilege <= query_user.privilege)return false;
        else return true;
    }

    bool checkmodifypofile(const String &cur,const String &us,int c){
        if(user_online.find(cur) == user_online.end())return false;
        vector<int>possibleoffset = user_tree.search(cur);
        vector<int>possibleoffset2 = user_tree.search(us);
        if(possibleoffset.empty() || possibleoffset2.empty())return false;
        if(us == cur)return true;
        user cur_user,query_user;
        user_data.read(possibleoffset[0],cur_user),user_data.read(possibleoffset2[0],query_user);
        if(cur_user.privilege <= query_user.privilege || cur_user.privilege <= c)return false;
        else return true;
    }

    void add_user(const String & u, const String & pa, const String & n,const String & add, int p){
        user tmp(u,pa,n,add,p);
        int index = user_data.write(tmp);
        if(!user_tree.insert(u, index)){
            throw("invalid_insert");
        }
    }

    void login(const String & userna, const String & pas){
        vector<int>possibleoffset = user_tree.search(userna);
        if(possibleoffset.empty()){
            throw("cannot find the user");
        }else{
            if(user_online.find(userna) == user_online.end()) {
                user cur_user;
                user_data.read(possibleoffset[0], cur_user);
                if(cur_user.password == pas) {
                    user_online[userna] = true;
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
        vector<int>possibleoffset = user_tree.search(a);
        if(possibleoffset.empty())throw("cannot find the user");
        else {
            user cur_user;
            user_data.read(possibleoffset[0], cur_user);
            cout << cur_user << endl;
        }
    }

    void modify_profile(const String & usern,const String & password, const String &name, const String &add, int pr){
        vector<int>possibleoffset = user_tree.search(usern);
        if(possibleoffset.empty())throw("cannot find the user");
        else{
            user cur_user;
            user_data.read(possibleoffset[0], cur_user);
            if(!password.empty()) cur_user.password = password;
            if(!name.empty()) cur_user.name = name;
            if(!add.empty()) cur_user.address = add;
            if(pr != 0) cur_user.privilege = pr;
            user_data.write(possibleoffset[0], cur_user);
            cout << cur_user << endl;
        }
    }

    void buy_ticket(TRAIN_ALL &train, const String &u, const String &id, const Date &d, int n, const String &f, const String &t, bool q){
        vector<int> users = user_tree.search(u);
        if (users.empty()) throw("cannot find the user");
        if (user_online.find(u) == user_online.end()) throw("user haven't login");
        Order res = train.query_ticket(id, d, f, t, n);
        if (res.num < n && !q) throw("no ticket");
        user cur_user;
        user_data.read(users[0], cur_user);
        res.order_num = ++cur_user.num_order;
        user_data.write(users[0], cur_user);
        res.userID = u;
        if (res.num >= n){
            res.status = "success", res.num = n;
            train.buy_ticket(id, d, n, f, t);
            cout << (long long)res.price * n << endl;
        } else {
            res.status = "pending", res.num = n;
            train.queue(id, res);
            cout << "queue" << endl;
        }
        order_tree.insert(make_pair(u, res.order_num), order_data.write(res));
    }

    void query_order(const String &u){
        if (user_online.find(u) == user_online.end()) throw("user haven't login");
        vector<int> res = order_tree.search(make_pair(u, 0), equal);
        cout << res.size() << endl;
        for (int i = (int)res.size() - 1; i >= 0; --i) {
            Order cur_order;
            order_data.read(res[i], cur_order);
            cout << cur_order << endl;
        }
    }

    void refund_ticket(TRAIN_ALL &train, const String &u, int n = 1){
        vector<int> users = user_tree.search(u);
        if (users.empty()) throw("cannot find the user");
        if (user_online.find(u) == user_online.end()) throw("user haven't login");
        user cur_user;
        user_data.read(users[0], cur_user);
        n = cur_user.num_order - n + 1;
        vector<int> res = order_tree.search(make_pair(u, n));
        if (res.size() != 1) throw("cannot find the order");
        Order cur_order;
        order_data.read(res[0], cur_order);
        if (cur_order.status == "refunded") throw("already refunded");
        auto updated = train.refund(cur_order);
        for (const auto &x : updated) {
            vector<int> res = order_tree.search(make_pair(x.userID, x.order_num));
            order_data.write(res[0], x);
        }
        cur_order.status = "refunded";
        order_data.write(res[0], cur_order);
    }

    void clean(){
        user_online.clear(), user_tree.clear(), order_tree.clear(), user_data.clear(), order_data.clear();
    }

};
#endif //MAP_HPP_USER_HPP
