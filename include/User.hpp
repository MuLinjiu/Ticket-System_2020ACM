//
// Created by 18382 on 2021/5/14.
//

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
        String<> username;
        String<> password;
        String<> name;
        String<> address;
        int privilege = -1;
        user(const String<> & u, const String<> & pa, const String<> & n,const String<> & add, int p) {
            username = u;
            privilege = p;
        }
        user() = default;
    };
    BpTree<String<>,user> user_tree;
    map<String<>,user>user_online;
public:
    USER_ALL() : user_tree("user") {}
    bool checkadduser(const String<> &cur,const String<> &us){
        vector<user>possibleoffset = user_tree.search(cur);
        vector<user>possibleoffset2 = user_tree.search(us);
        if(possibleoffset.empty() || possibleoffset2.empty())return false;
        if(user_online.find(cur) == user_online.end())return false;
        if(possibleoffset[0].privilege <= possibleoffset2[0].privilege)return false;
        else return true;
    }
    bool checkquerypofile(const String<> &cur,const String<> &us){
        vector<user>possibleoffset = user_tree.search(cur);
        vector<user>possibleoffset2 = user_tree.search(us);
        if(user_online.find(cur) == user_online.end())return false;
        if(possibleoffset.empty() || possibleoffset2.empty())return false;
        if(possibleoffset[0].privilege < possibleoffset2[0].privilege)return false;
        else return true;
    }
    void add_user(const String<> & u, const String<> & pa, const String<> & n,const String<> & add, int p){
        user tmp(u,pa,n,add,p);
        if(!user_tree.insert(u,tmp)){
            throw("invalid_insert");
        }
    }
    void login(const String<> & userna, const String<> & pas){
        vector<user>possibleoffset = user_tree.search(userna);
        if(possibleoffset.empty()){
            throw("cannot find the user");
        }else{
            if(user_online.find(userna) == user_online.end()) {
                user_online[userna] = possibleoffset[0];
            }else {
                throw("user is online");
            }
        }
    }
    void logout(const String<> & usn){
        if(user_online.find(usn) == user_online.end())throw("user haven't login");
        else user_online.erase(user_online.find(usn));
    }

    void query_pofile(const String<> & a){
        vector<user>possibleoffset = user_tree.search(a);
        if(possibleoffset.empty())throw("cannot find the user");
        else {
            cout << possibleoffset[0].username << " " << possibleoffset[0].name << " " << possibleoffset[0].address << " " << possibleoffset[0].privilege << endl;
        }
    }
    void modify_profile(const String<> &opt,const String<> & data){

    }
};
#endif //MAP_HPP_USER_HPP
