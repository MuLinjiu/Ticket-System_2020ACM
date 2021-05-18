#ifndef MAP_HPP_USER_HPP
#define MAP_HPP_USER_HPP
#include <cstring>
#include <iostream>
#include "BpTree.hpp"
#include "String.hpp"
using namespace std;
using namespace sjtu;
class USER_ALL{

    struct user{
        String username;
        String password;
        String name;
        String address;
        int privilege = -1;
        user(const String & u, const String & pa, const String & n,const String & add, int p) {
            username = u;
            password = pa;
            name = n;
            address = add;
            privilege = p;
        }
        user() = default;
        friend ostream &operator << (ostream &os, const user &x){
            os << x.username << " " << x.name << " " << x.address << " " << x.privilege << endl;
            return os;
        }
    };
    BpTree<String,user> user_tree;
    map<String,user>user_online;
public:

    USER_ALL() : user_tree("user") {}

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
        if(possibleoffset[0].privilege <= possibleoffset2[0].privilege && possibleoffset[0].privilege <= c)return false;
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
        if(user_online.find(usn) == user_online.end())throw("user haven't login");
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
            user_tree.erase(usern);
            if(!password.empty())possibleoffset[0].password = password;
            if(!name.empty())possibleoffset[0].name = name;
            if(!add.empty())possibleoffset[0].address = add;
            if(pr != 0)possibleoffset[0].privilege = pr;
            user_tree.insert(usern,possibleoffset[0]);
            cout << possibleoffset[0] << endl;
        }
    }

};
#endif //MAP_HPP_USER_HPP
