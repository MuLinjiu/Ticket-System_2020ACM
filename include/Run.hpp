#ifndef MAP_HPP_RUN_HPP
#define MAP_HPP_RUN_HPP
#include "String.hpp"

extern USER_ALL user_all;
extern TRAIN_ALL trainAll;

void splitstations(string & str,String *s){
    int num = 1;
    String x = "";
    for(int i = 0;i < str.size();i++){
        if(str[i] == '|'){
            s[num++] = x;
            x.clear();
        }else{
            x += str[i];
        }
    }
    s[num++] = x;
}
void splitprices(string & str,int *s){
    int num = 1;
    int ans = 0;
    for(int i = 0;i < str.size();i++){
        if(str[i] == '|'){
            s[num++] = ans;
            ans = 0;
        }else{
            ans = ans * 10 + str[i] - '0';
        }
    }
    s[num++] = ans;
}
void splitstopover(string & str,int *s){
    int num = 2;
    int ans = 0;
    for(int i = 0;i < str.size();i++){//jiayige .size()
        if(str[i] == '|'){
            s[num++] = ans;
            ans = 0;
        }else{
            ans = ans * 10 + str[i] - '0';
        }
    }
    s[num++] = ans;
}
pair<Date,Date> splitdate(string & str){
    int i = 0 ;
    String l , r;
    for(; str[i] != '|' ;i++){
        l += str[i];
    }
    for(++i; i < str.size();i++){
        r += str[i];
    }
    return make_pair(Date(l),Date(r));
}

bool run_program(){
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
            return true;
        }
        cout << 0 << endl;
    }
    else if(command == "login"){
        String username, pass;
        while(getchar() == ' ') {
            String a;
            cin >> a;
            switch(a[1]){
                case 'u':{
                    cin >> username;
                    break;
                }
                case 'p':{
                    cin >> pass;
                    break;
                }
            }
        }
        try{
            user_all.login(username,pass);
        }catch (...){
            cout << -1 << endl;
            return true;
        }
        cout << 0 << endl;
        return true;
    }
    else if(command == "logout"){
        String a,username;
        cin >> a >> username;
        try{
            user_all.logout(username);
        }catch (...){
            cout << -1 << endl;
            return true;
        }
        cout << 0 << endl;
    }
    else if(command == "query_profile"){
        String username, cur;
        while(getchar() == ' ') {
            String a;
            cin >> a;
            switch(a[1]){
                case 'c':{
                    cin >> cur;
                    break;
                }
                case 'u':{
                    cin >> username;
                    break;
                }
            }
        }
        try{
            if(user_all.checkquerypofile(cur,username)){
                user_all.query_pofile(username);
            } else cout << -1 << endl;
        }catch (...){
            cout << -1 << endl;
            return true;
        }
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
            return true;
        }
    }
    else if(command == "add_train"){
        string a,train_id,starttime,stations,prices,traveltimes,stopovertimes,saledate;
        char type;
        int stationnum,seatnum;
        pair<Date,Date>x;
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
                    splitstations(stations,sta);
                    break;
                }
                case 'p':{
                    cin >> prices;
                    splitprices(prices,p);
                    break;
                }
                case 't':{
                    cin >> traveltimes;
                    splitprices(traveltimes,t);
                    break;
                }
                case 'o':{
                    cin >> stopovertimes;
                    splitstopover(stopovertimes,stopover);
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
        }
        try{
            trainAll.add_train(String(train_id),stationnum,seatnum,sta,p,Date(starttime),t,stopover,x,type);
        }catch (...){
            cout << -1 << endl;
            return true;
        }
        cout << 0 << endl;
    }
    else if(command == "release_train"){
        String opt,id;
        cin >> opt >> id;
        if(opt != "-i")throw("e");
        try {
            trainAll.release_train(id);
        }catch (...){
            cout << -1 << endl;
            return true;
        }
        cout << 0 << endl;
    }
    else if(command == "query_train"){
        String trainid;
        String f;
        while(getchar() == ' ') {
            String a;
            cin >> a;
            switch(a[1]){
                case 'i':{
                    cin >> trainid;
                    break;
                }
                case 'd':{
                    cin >> f;
                    break;
                }
            }
        }
        try {
            trainAll.query_train(trainid, Date(f));
        }catch (...){
            cout << -1 << endl;
            return true;
        }
    }
    else if(command == "delete_train"){
        String opt,id;
        cin >> opt >> id;
        try {
            trainAll.delete_train(id);
        }catch (...){
            cout << -1 << endl;
            return true;
        }
        cout << 0 << endl;
    }
    else if(command == "query_ticket"){
        String start,end,da,p = "time";
        while(getchar() == ' ') {
            String a;
            cin >> a;
            switch(a[1]){
                case 's':{
                    cin >> start;
                    break;
                }
                case 't':{
                    cin >> end;
                    break;
                }
                case 'd':{
                    cin >> da;
                    break;
                }
                case 'p':{
                    cin >> p;
                    break;
                }
            }
        }
        try {
            trainAll.query_ticket(start, end, Date(da), p);
        }catch (...){
            cout << -1 << endl;
            return true;
        }
    }
    else if(command == "query_transfer"){
        String start,end,da,p = "time";
        while(getchar() == ' ') {
            String a;
            cin >> a;
            switch(a[1]){
                case 's':{
                    cin >> start;
                    break;
                }
                case 't':{
                    cin >> end;
                    break;
                }
                case 'd':{
                    cin >> da;
                    break;
                }
                case 'p':{
                    cin >> p;
                    break;
                }
            }
        }
        trainAll.query_transfer(start, end, Date(da), p);
    }
    else if(command == "buy_ticket"){
        String username, trainID, date, start, end, q = "false";
        int num;
        while(getchar() == ' ') {
            String a;
            cin >> a;
            switch(a[1]){
                case 'u':{
                    cin >> username;
                    break;
                }
                case 'i':{
                    cin >> trainID;
                    break;
                }
                case 'd':{
                    cin >> date;
                    break;
                }
                case 'n':{
                    cin >> num;
                    break;
                }
                case 'f': {
                    cin >> start;
                    break;
                }
                case 't': {
                    cin >> end;
                    break;
                }
                case 'q': {
                    cin >> q;
                    break;
                }
            }
        }
        try{
            user_all.buy_ticket(trainAll, username, trainID, Date(date), num, start, end, q == "true");
        }catch (...){
            cout << -1 << endl;
            return true;
        }
    }
    else  if (command == "query_order") {
        String a, username;
        cin >> a >> username;
        try {
            user_all.query_order(username);
        }catch (...){
            cout << -1 << endl;
            return true;
        }
    }
    else  if (command == "refund_ticket") {
        String username;
        int num = 1;
        while(getchar() == ' ') {
            String a;
            cin >> a;
            switch(a[1]){
                case 'u':{
                    cin >> username;
                    break;
                }
                case 'n':{
                    cin >> num;
                    break;
                }
            }
        }
        try {
            user_all.refund_ticket(trainAll, username, num);
        }catch (...){
            cout << -1 << endl;
            return true;
        }
        cout << 0 << endl;
    }
    else  if (command == "clean") {
        user_all.clean();
        trainAll.clean();
        cout << "0" << endl;
    }
    else if(command == "exit"){
        cout << "bye" << endl;
        return false;
    }
    return true;
}
#endif //MAP_HPP_RUN_HPP
