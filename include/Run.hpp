#ifndef MAP_HPP_RUN_HPP
#define MAP_HPP_RUN_HPP
#include "String.hpp"
extern USER_ALL user_all;
void run_program(){
    String command;
    cin >> command;
    if(command == "add_user"){
        fstream f;
        f.open("data_user.dat",ios::in | ios::binary);
        f.seekg(0,ios::end);
        int x = f.tellg();
        f.close();
        try{
            if(x != sizeof (int)) {
                String a;
                String cur_username;
                String username;
                String password;
                String name;
                String add;
                int p;
                while(getchar() == ' '){
                    cin >> a;
                    switch(a[1]){
                        case 'c':{
                            cin >> cur_username;
                            break;
                        }
                        case 'u':{
                            cin >> username;
                            break;
                        }
                        case 'p':{
                            cin >> password;
                            break;
                        }
                        case 'n':{
                            cin >> name;
                            break;
                        }
                        case 'm':{
                            cin >> add;
                            break;
                        }
                        case 'g':{
                            cin >> p;
                            break;
                        }
                        default:throw("e");
                    }
                }
                if (!user_all.checkadduser(cur_username, username,p)) {
                    throw "e";
                }
                user_all.add_user(username, password, name, add, p);
            }else{
                String a;
                String cur_username;
                String username;
                String password;
                String name;
                String add;
                int p;
                while(getchar() == ' '){
                    cin >> a;
                    switch(a[1]){
                        case 'c':{
                            cin >> cur_username;
                            break;
                        }
                        case 'u':{
                            cin >> username;
                            break;
                        }
                        case 'p':{
                            cin >> password;
                            break;
                        }
                        case 'n':{
                            cin >> name;
                            break;
                        }
                        case 'm':{
                            cin >> add;
                            break;
                        }
                        case 'g':{
                            cin >> p;
                            break;
                        }
                        default:throw("e");
                    }
                }
                user_all.add_user(username, password, name, add, 10);
            }
        }catch (...){
            cout << -1 << endl;
            return;
        }
        cout << 0 << endl;
        return;
    }
    if(command == "login"){
        String a;
        cin >> a;
        String username;
        cin >> username;
        cin >> a;
        String pass;
        cin >> pass;
        try{
            user_all.login(username,pass);
        }catch (...){
            cout << -1 << endl;
            return;
        }
        cout << 0 << endl;
        return;
    }
    if(command == "logout"){
        String a,username;
        cin >> a >> username;
        try{
            user_all.logout(username);
        }catch (...){
            cout << -1 << endl;
            return;
        }cout << 0 << endl;
        return;
    }
    if(command == "query_profile"){
        String a,username,cur;
        cin >> a >> cur >> a >> username;
        try{
            if(user_all.checkquerypofile(cur,username)){
                user_all.query_pofile(username);
            } else cout << -1 << endl;
        }catch (...){
            cout << -1 << endl;
            return;
        }
        return;
    }
    if(command == "modify_profile"){
        String a,cur_username,username,password,name,add;
        int p = 0;
        while(getchar() == ' '){
            cin >> a;
            switch(a[1]){
                case 'c':{
                    cin >> cur_username;
                    break;
                }
                case 'u':{
                    cin >> username;
                    break;
                }
                case 'p':{
                    cin >> password;
                    break;
                }
                case 'n':{
                    cin >> name;
                    break;
                }
                case 'm':{
                    cin >> add;
                    break;
                }
                case 'g':{
                    cin >> p;
                    break;
                }
                default:throw("e");
            }
        }
        try {
            if (user_all.checkmodifypofile(cur_username, username, p)){
                user_all.modify_profile(username,password,name,add,p);
            }else throw("e");
        }catch (...){
            cout << -1 << endl;
            return;

        }
    }
    if(command == "exit"){
        cout << "bye" << endl;
        exit(0);
    }
}
#endif //MAP_HPP_RUN_HPP
