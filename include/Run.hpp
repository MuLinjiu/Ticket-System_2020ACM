//
// Created by 18382 on 2021/5/14.
//

#ifndef MAP_HPP_RUN_HPP
#define MAP_HPP_RUN_HPP
extern USER_ALL user_all;
bool firstuser = true;
void run_program(){
    string command;
    cin >> command;
    if(command == "add_user"){
        fstream f;
        f.open("data_user.dat",ios::in | ios::binary);
        f.seekg(0,ios::end);
        int x = f.tellg();
        f.close();
        try{
            if(x != sizeof (int)) {
                cout << firstuser << endl;
                string a;
                cin >> a;
                if (a != "-c")throw ("e");
                string cur_username;
                cin >> cur_username;
                cin >> a;
                if (a != "-u")throw ("e");
                string username;
                cin >> username;
                cin >> a;
                if (a != "-p")throw ("e");
                string password;
                cin >> password;
                cin >> a;
                if (a != "-n")throw ("e");
                string name;
                cin >> name;
                cin >> a;
                if (a != "-m")throw ("e");
                string add;
                cin >> add;
                cin >> a;
                if (a != "-g")throw ("e");
                int p;
                cin >> p;
                if (!user_all.checkadduser(cur_username, username)) {
                    throw "e";
                }
                user_all.add_user(username, password, name, add, p);
            }else{
                string a;
                cin >> a;
                if (a != "-c")throw ("e");
                string cur_username;
                cin >> cur_username;
                cin >> a;
                if (a != "-u")throw ("e");
                string username;
                cin >> username;
                cin >> a;
                if (a != "-p")throw ("e");
                string password;
                cin >> password;
                cin >> a;
                if (a != "-n")throw ("e");
                string name;
                cin >> name;
                cin >> a;
                if (a != "-m")throw ("e");
                string add;
                cin >> add;
                cin >> a;
                if (a != "-g")throw ("e");
                int p;
                cin >> p;
                user_all.add_user(username, password, name, add, 10);
                firstuser = false;
            }
        }catch (...){
            cout << -1;
            return;
        }
        cout << 0;
        return;
    }
    if(command == "login"){
        string a;
        cin >> a;
        string username;
        cin >> username;
        cin >> a;
        string pass;
        cin >> pass;
        try{
            user_all.login(username,pass);
        }catch (...){
            cout << -1;
            return;
        }
        cout << 0;
        return;
    }
    if(command == "logout"){
        string a,username;
        cin >> a >> username;
        try{
            user_all.logout(username);
        }catch (...){
            cout << -1;
            return;
        }cout << 0;
        return;
    }
    if(command == "query_profile"){
        string a,username,cur;
        cin >> a >> cur >> a >> username;
        try{
            if(user_all.checkquerypofile(cur,username)){
                user_all.query_pofile(username);
            }
        }catch (...){
            cout << -1;
            return;
        }
        return;
    }
    if(command == "modify_profile"){

    }
    if(command == "exit")exit(0);
}
#endif //MAP_HPP_RUN_HPP