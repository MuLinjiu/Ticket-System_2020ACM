#ifndef TICKET_SYSTEM_2020ACM_TRAIN_HPP
#define TICKET_SYSTEM_2020ACM_TRAIN_HPP

#include <iostream>
#include "String.hpp"
#include "BpTree.hpp"
#include "Date.hpp"
using namespace std;

class TRAIN_ALL{
private:
    static const int Num = 101;
    struct train{
        String trainID;
        int stationNum;
        String stations[Num];
        int prices[Num], seats[Num];
        Date startTime;
        int travelTimes[Num];
        int stopoverTimes[Num]; // start from second element
        pair<Date, Date> saleDate;
        char type;
        bool released;
    };

    BpTree<pair<String, String>, train> train_stations;
    BpTree<String, train> train_id;

public:
    TRAIN_ALL() : train_stations("train_stations"), train_id("train_id") {}

    void add_train(const String &id, int n, int m, const String *&s, int *p, const Date &x, int *t, int *o, const pair<Date, Date> &d, char y){
        train tmp;
        tmp.trainID = id, tmp.stationNum = n, tmp.startTime = x, tmp.saleDate = d, tmp.type = y, tmp.released = false;
        for (int i = 1; i <= n; ++i) tmp.stations[i] = s[i];
        for (int i = 1; i < n; ++i) tmp.prices[i] = p[i], tmp.seats[i] = m, tmp.travelTimes[i] = t[i];
        for (int i = 2; i < n; ++i) tmp.stopoverTimes[i] = o[i];
        if (!train_id.insert(id, tmp)){
            throw("invalid_insert");
        }
        for (int i = 1; i <= n; ++i){
            train_stations.insert(make_pair(s[i], id), tmp);
        }
    }

    void release_train(const String &id){
        vector<train> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        if (res[0].released) throw("already released");
        res[0].released = true;
        train_id.erase(id), train_id.insert(id, res[0]);
    }

    void query_train(const String &id, const Date &d){
        vector<train> res = train_id.search(id);
        if (res[0].saleDate.first <= d && d <= res[0].saleDate.second){
            cout << id << ' ' << res[0].type << endl;
            Date cur = d;
            for (int i = 1; i <= res[0].stationNum; ++i){
                cout << res[0].stations[i];
                if (i == 1){
                    cur += res[0].startTime - Date();
                    cout << " xx-xx xx:xx -> " << cur;
                } else {
                    cur += res[0].travelTimes[i-1];
                    cout << ' ' << cur;
                    if (i == res[0].stationNum)
                        cout << " -> xx-xx xx:xx";
                    else{
                        cur += res[0].stopoverTimes[i];
                        cout << " -> " << cur;
                    }
                }
                cout << ' ' << res[0].prices[i];
                if (i == res[0].stationNum) cout << " x" << endl;
                else cout << ' ' << res[0].seats[i] << endl;
            }
        } else throw("not found");
    }

    void delete_train(const String &id){
        vector<train> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        if (res[0].released) throw("train is released");
        for (int i = 1; i <= res[0].stationNum; ++i)
            train_stations.erase(make_pair(res[0].stations[i], res[0].trainID));
        train_id.erase(res[0].trainID);
    }

};

#endif //TICKET_SYSTEM_2020ACM_TRAIN_HPP
