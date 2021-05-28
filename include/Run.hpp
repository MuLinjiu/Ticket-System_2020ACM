#ifndef MAP_HPP_RUN_HPP
#define MAP_HPP_RUN_HPP
#include "String.hpp"
extern USER_ALL user_all;
extern TRAIN_ALL trainAll;
void splitstations(String & str,String *s,int size){
    int num = 0;
    String x = "";
    for(int i = 0;num < size;i++){
        if(str[i] == '|' || str[i] == '\0'){
            s[num++] = x;
            x.clear();
        }else{
            x += str[i];
        }
    }
}
void splitprices(String & str,int *s,int size){
    int num = 0;
    int ans = 0;
    for(int i = 0;num < size;i++){
        if(str[i] == '|' || str[i] == '\0'){
            s[num++] = ans;
            ans = 0;
        }else{
            ans = ans * 10 + str[i] - '0';
        }
    }
}
void splitstopover(String & str,int *s,int size){
    int num = 0;
    int ans = 0;
    for(int i = 0;num < size;i++){//jiayige .size()
        if(str[i] == '|' || str[i] == '\0'){
            s[num++] = ans;
            ans = 0;
        }else{
            ans = ans * 10 + str[i] - '0';
        }
    }
}
pair<String,String> splitdate(String & str){
    int i = 0 ;
    String l , r;
    for(; str[i] != '|' ;i++){
        l += str[i];
    }
    for(;i < str.size();i++){
        r += str[i];
    }
    return make_pair(l,r);
}
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
    else if(command == "login"){
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
    else if(command == "logout"){
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
    else if(command == "query_profile"){
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
    else if(command == "modify_profile"){
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
    else if(command == "add_train"){
        String a,train_id,starttime,stations,prices,traveltimes,stopovertimes,saledate,type;
        int stationnum,seatnum;
        pair<String,String>x;
        String sta[101];
        int p[101];
        int stopover[101];
        int t[101];
        while(getchar() == ' '){
            cin >> a;
            switch(a[1]){
                case 'i':{
                    cin >> train_id;
                    break;
                }
                case 'n':{
                    cin >> stationnum;
                    break;
                }
                case 'm':{
                    cin >> seatnum;
                    break;
                }
                case 'x':{
                    cin >> starttime;
                    break;
                }
                case 's':{
                    cin >> stations;
                    splitstations(stations,sta,stationnum);
                    break;
                }
                case 'p':{
                    cin >> prices;
                    splitprices(prices,p,stationnum);
                    break;
                }
                case 't':{
                    cin >> traveltimes;
                    splitprices(traveltimes,t,stationnum);
                    break;
                }
                case 'o':{
                    cin >> stopovertimes;
                    splitstopover(stopovertimes,stopover,stationnum);
                    break;
                }
                case 'd':{
                    cin >> saledate;
                    x = splitdate(saledate);
                    break;
                }
                case 'y':{
                    cin >> type;
                    break;
                }
                default:throw("e");
            }
            trainAll.add_train(train_id,stationnum,seatnum,sta,p,,t,stopover,x,type);
        }
    }
    else if(command == "release_train"){
        String opt,id;
        cin >> opt >> id;
        if(opt != "-i")throw("e");
        trainAll.release_train(id);
    }
    else if(command == "query_train"){

    }
    else if(command == "exit"){
        cout << "bye" << endl;
        exit(0);
    }
}
#endif //MAP_HPP_RUN_HPP
