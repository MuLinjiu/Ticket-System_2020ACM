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
    static const int Max_Seats = 100000;
    struct train{
        String trainID;
        int stationNum;
        String stations[Num];
        int prices[Num], seats[Num]; // from 1 to stationNum - 1
        Date startTime;
        int travelTimes[Num]; // from 1 to stationNum - 1
        int stopoverTimes[Num]; // from 2 to stationNum - 1
        pair<Date, Date> saleDate;
        char type;
        bool released;
    };

    struct ticket{
        String trainID;
        Date startTime, endTime;
        String startStation, endStation;
        int seats, cost_time, cost_price;

        friend ostream &operator<<(ostream &os, const ticket &x){
            os << x.trainID << ' ' << x.startStation << ' ' << x.startTime << " -> " << x.endStation << ' ' << x.endTime << ' ' << x.cost_price << ' ' << x.seats;
            return os;
        }
    };

    BpTree<pair<String, String>, train> train_stations;
    BpTree<String, train> train_id;
    vector<ticket> valid;

    static bool equal(const pair<String, String> &lhs, const pair<String, String> &rhs){
        return lhs.first == rhs.first;
    }

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
                    cur += res[0].startTime;
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

    void straight_query(const String &s, const String &t, const Date &d, const String &p){
        vector<train> res = train_stations.search(make_pair(s, String()), equal);
        valid.clear();
        for (auto x : res){
            if (!x.released) continue;
            Date cur = x.startTime;
            Date start_time, end_time;
            int cost_price = 0, seats = Max_Seats;
            bool aboard = false;
            for (int i = 1; i <= x.stationNum; ++i){
                if (x.stations[i] == s){
                    aboard = true, start_time = cur;
                    if (i > 1 && i < x.stationNum)
                        start_time += x.stopoverTimes[i];
                }
                if (x.stations[i] == t) aboard = false, end_time = cur;
                if (aboard){
                    cost_price += x.prices[i];
                    seats = min(seats, x.seats[i]);
                }
                if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
                if (i < x.stationNum) cur += x.travelTimes[i];
            }
            if (!aboard && x.saleDate.first + start_time.getdate() <= d && x.saleDate.second + start_time.getdate() >= d){
                int cost_time = end_time - start_time;
                start_time.setdate(d);
                end_time = start_time + cost_time;
                valid.push_back((ticket){x.trainID, start_time, end_time, s, t, seats, cost_time, cost_price});
            }
        }
        sort();
    }

    void query_ticket(const String &s, const String &t, const Date &d, const String &p){
        straight_query(s, t, d, p);
        cout << valid.size() << endl;
        for (auto x : valid) cout << x << endl;
    }

    void query_transfer(const String &s, const String &t, const Date &d, const String &p){
        vector<train> res = train_stations.search(make_pair(s, String()), equal);
        pair<ticket, ticket> optimal;
        bool got_answer = false;
        for (auto x : res){
            if (!x.released) continue;
            Date cur = x.startTime;
            int i = 1;
            for (; i <= x.stationNum; ++i) {
                if (x.stations[i] == s) break;
                if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
                if (i < x.stationNum) cur += x.travelTimes[i];
            }
            if (x.saleDate.first + cur.getdate() <= d && x.saleDate.second + cur.getdate() >= d){
                if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
                if (i < x.stationNum) cur += x.travelTimes[i];
                int cost_price = 0, seats = x.seats[i];
                cur.setdate(d);
                Date start_time = cur;
                for (i++; i <= x.stationNum; ++i){
                    for (int offset = 0; offset <= 3; ++offset) {
                        straight_query(x.stations[i], t, cur.getdate() + offset, p);
                        int j = 0;
                        while (j < valid.size()){
                            if (valid[j].startTime >= cur) break;
                        }
                        if (j < valid.size()){
                            ticket tmp{x.trainID, start_time, cur, s, x.stations[i], seats, cur - start_time, cost_price};
                            if (!got_answer) optimal = make_pair(tmp, valid[j]), got_answer = true;
                            else {
                                if (p == "time"){
                                    if (valid[j].endTime - start_time == optimal.second.endTime - optimal.first.startTime) {
                                        if (tmp.cost_time < optimal.first.cost_time) optimal = make_pair(tmp, valid[j]);
                                    } else if (valid[j].endTime - start_time < optimal.second.endTime - optimal.first.startTime)
                                        optimal = make_pair(tmp, valid[j]);
                                } else {
                                    if (valid[j].cost_price + cost_price == optimal.first.cost_price + optimal.second.cost_price) {
                                        if (tmp.cost_time < optimal.first.cost_time) optimal = make_pair(tmp, valid[j]);
                                    } else if (valid[j].cost_price + cost_price < optimal.first.cost_price + optimal.second.cost_price)
                                        optimal = make_pair(tmp, valid[j]);
                                }
                            }
                        }
                    }
                    if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
                    if (i < x.stationNum) cur += x.travelTimes[i], seats = min(seats, x.seats[i]), cost_price += x.prices[i];;
                }
                if (!got_answer) cout << '0' << endl;
                else cout << optimal.first << endl << optimal.second << endl;
            }
        }
    }

};

#endif //TICKET_SYSTEM_2020ACM_TRAIN_HPP
