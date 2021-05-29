#ifndef TICKET_SYSTEM_2020ACM_TRAIN_HPP
#define TICKET_SYSTEM_2020ACM_TRAIN_HPP

#include <iostream>
#include "String.hpp"
#include "BpTree.hpp"
#include "Date.hpp"
#include "Order.hpp"
#include "Algorithms.hpp"
using namespace std;

class TRAIN_ALL {
private:
    static const int Num = 101;
    static const int Max_Seats = 100000;
    struct train {
        String trainID;
        int stationNum;
        String stations[Num];
        int prices[Num], seats[Num][Num]; // from 1 to stationNum - 1
        Date startTime;
        int travelTimes[Num]; // from 1 to stationNum - 1
        int stopoverTimes[Num]; // from 2 to stationNum - 1
        pair<Date, Date> saleDate;
        char type;
        bool released = false;
    };

    struct ticket {
        String trainID;
        Date startTime, endTime;
        String startStation, endStation;
        int seats, cost_time, cost_price;

        friend ostream &operator<<(ostream &os, const ticket &x) {
            os << x.trainID << ' ' << x.startStation << ' ' << x.startTime << " -> " << x.endStation << ' ' << x.endTime
               << ' ' << x.cost_price << ' ' << x.seats;
            return os;
        }
    };

    BpTree<pair<String, String>, train> train_stations;
    BpTree<String, train> train_id;
    BpTree<String, int> train_num_order;
    BpTree<pair<String, int>, Order> train_order;
    vector<ticket> valid;
    static Date standard_start;

    template<class T, class U>
    static bool equal(const pair<T, U> &lhs, const pair<T, U> &rhs) {
        return lhs.first == rhs.first;
    }

    static bool cmp_by_time(const ticket &lhs, const ticket &rhs) {
        if (lhs.cost_time == rhs.cost_time)
            return lhs.trainID < rhs.trainID;
        else return lhs.cost_time < rhs.cost_time;
    }

    static bool cmp_by_standard_time(const ticket &lhs, const ticket &rhs) {
        if (lhs.endTime - TRAIN_ALL::standard_start == rhs.endTime - TRAIN_ALL::standard_start)
            return lhs.trainID < rhs.trainID;
        else return lhs.endTime - TRAIN_ALL::standard_start < rhs.endTime - TRAIN_ALL::standard_start;
    }

    static bool cmp_by_cost(const ticket &lhs, const ticket &rhs) {
        if (lhs.cost_price == rhs.cost_price)
            return lhs.trainID < rhs.trainID;
        else return lhs.cost_price < rhs.cost_price;
    }

    vector<Order> recover(const String &id){
        vector<Order> candidates = train_order.search(make_pair(id, 0), equal);
        vector<Order> ret;
        for (auto &x : candidates){
            if (x.status != "pending") continue;
            Order res = query_ticket(x.trainID, x.startTime.getdate(), x.startStation, x.endStation);
            if (res.num >= x.num) {
                x.status = "success";
                buy_ticket(x.trainID, x.startTime.getdate(), x.num, x.startStation, x.endStation);
                train_order.modify(make_pair(x.trainID, x.pending_num), x);
                ret.push_back(x);
            }
        }
        return ret;
    }

public:
    TRAIN_ALL() : train_stations("train_stations"), train_id("train_id"), train_num_order("train_num_order"), train_order("train_order") {}

    void add_train(const String &id, int n, int m, const String *s, int *p, const Date &x, int *t, int *o, const pair<Date, Date> &d, char y){
        train tmp;
        tmp.trainID = id, tmp.stationNum = n, tmp.startTime = x, tmp.saleDate = d, tmp.type = y, tmp.released = false;
        for (int i = 1; i <= n; ++i) tmp.stations[i] = s[i];
        for (int i = 1; i < n; ++i) tmp.prices[i] = p[i], tmp.travelTimes[i] = t[i];
        for (int i = 0; i <= (d.second - d.first) / 1440; ++i)
            for (int j = 1; j < n; ++j)
                tmp.seats[i][j] = m;
        for (int i = 2; i < n; ++i) tmp.stopoverTimes[i] = o[i];
        if (!train_id.insert(id, tmp)){
            throw("invalid_insert");
        }
        for (int i = 1; i <= n; ++i)
            train_stations.insert(make_pair(s[i], id), tmp);
        train_num_order.insert(id, 0);
    }

    void release_train(const String &id){
        vector<train> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        if (res[0].released) throw("already released");
        res[0].released = true;
        for (int i = 1; i <= res[0].stationNum; ++i)
            train_stations.modify(make_pair(res[0].stations[i], id), res[0]);
        train_id.modify(id, res[0]);
    }

    void query_train(const String &id, const Date &d){
        vector<train> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        if (res[0].saleDate.first <= d && d <= res[0].saleDate.second){
            cout << id << ' ' << res[0].type << endl;
            Date cur = d;
            int cost = 0;
            for (int i = 1; i <= res[0].stationNum; ++i){
                cout << res[0].stations[i];
                if (i == 1){
                    cur += res[0].startTime;
                    cout << " xx-xx xx:xx -> " << cur;
                } else {
                    cur += res[0].travelTimes[i-1], cost += res[0].prices[i-1];
                    cout << ' ' << cur;
                    if (i == res[0].stationNum)
                        cout << " -> xx-xx xx:xx";
                    else{
                        cur += res[0].stopoverTimes[i];
                        cout << " -> " << cur;
                    }
                }
                cout << ' ' << cost;
                if (i == res[0].stationNum) cout << " x" << endl;
                else cout << ' ' << res[0].seats[(d-res[0].saleDate.first)/1440][i] << endl;
            }
        } else throw("not found");
    }

    void delete_train(const String &id){
        vector<train> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        if (res[0].released) throw("train is released");
        for (int i = 1; i <= res[0].stationNum; ++i)
            train_stations.erase(make_pair(res[0].stations[i], res[0].trainID));
        vector<int> num = train_num_order.search(id);
        for (int i = 1; i <= num[0]; ++i)
            train_order.erase(make_pair(id, i));
        train_num_order.erase(id);
        train_id.erase(id);
    }

    void query_ticket(const String &s, const String &t, const Date &d, const String &p = "time"){
        vector<train> res = train_stations.search(make_pair(s, String()), equal);
        valid.clear();
        for (int i = 0; i < res.size(); i ++){
            const train &x = res[i] ;
            if (!x.released) continue;
            Date cur = x.startTime;
            Date start_time, end_time, duration;
            int cost_price = 0, seats = Max_Seats;
            bool aboard = false, ashore = false;
            for (int i = 1; i <= x.stationNum; ++i){
                if (x.stations[i] == t){
                    ashore = true; end_time = cur; break;
                }
                if (i > 1 && i < x.stationNum)   cur += x.stopoverTimes[i];
                if (x.stations[i] == s){
                    duration = cur;
                    if (!((x.saleDate.first + duration).getdate() <= d && (x.saleDate.second + duration).getdate() >= d)) break;
                    aboard = true;
                    cur.setdate(d), start_time = cur;
                }
                if (aboard){
                    cost_price += x.prices[i];
                    seats = min(seats, x.seats[(d-(duration.getdate()+x.saleDate.first))/1440][i]);
                }
                if (i < x.stationNum) cur += x.travelTimes[i];
            }
            if (aboard && ashore){
                int cost_time = end_time - start_time;
                valid.push_back((ticket){x.trainID, start_time, end_time, s, t, seats, cost_time, cost_price});
            }
        }
        sort(valid.begin(), valid.end(), p == "time" ? cmp_by_time : cmp_by_cost);
        cout << valid.size() << endl;
        for (const auto& x : valid) cout << x << endl;
    }

    void query_transfer(const String &s, const String &t, const Date &d, const String &p = "time"){
        vector<train> res = train_stations.search(make_pair(s, String()), equal);
        pair<ticket, ticket> optimal;
        bool got_answer = false;
        for (const auto& x : res){
            if (!x.released) continue;
            Date cur = x.startTime, duration;
            int i = 1;
            for (; i <= x.stationNum; ++i) {
                if (x.stations[i] == s) break;
                if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
                if (i < x.stationNum) cur += x.travelTimes[i];
            }
            if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
            duration = cur;
            if ((x.saleDate.first + duration).getdate() <= d && (x.saleDate.second + duration).getdate() >= d){
                cur.setdate(d);
                Date start_time = cur;
                if (i < x.stationNum) cur += x.travelTimes[i];
                int cost_price = x.prices[i], seats = x.seats[(d-(duration.getdate()+x.saleDate.first))/1440][i];
                for (i++; i <= x.stationNum; ++i){
                    vector<train> res2 = train_stations.search(make_pair(x.stations[i], String()), equal);
                    valid.clear();
                    for (const auto& y : res2){
                        if (!y.released) continue;
                        Date cur2 = y.startTime;
                        Date start_time2, end_time2, duration2, d2;
                        int cost_price2 = 0, seats2 = Max_Seats;
                        bool aboard = false, ashore = false;
                        for (int j = 1; j <= y.stationNum; ++j){
                            if (y.stations[j] == t){
                                ashore = true; end_time2 = cur2; break;
                            }
                            if (j > 1 && j < y.stationNum) cur2 += y.stopoverTimes[j];
                            if (y.stations[j] == x.stations[i]){
                                aboard = true, duration2 = cur2;
                                if (y.saleDate.second + cur2 < cur) break;
                                if (y.saleDate.first + cur2 >= cur)
                                    d2 = (y.saleDate.first + cur2).getdate();
                                else d2 = cur2.gettime() >= cur.gettime() ? cur.getdate() : cur.getdate() + 1440;
                                cur2.setdate(d2), start_time2 = cur2;
                            }
                            if (aboard){
                                cost_price2 += y.prices[j];
                                seats2 = min(seats2, y.seats[(d2-(duration2.getdate()+y.saleDate.first))/1440][j]);
                            }
                            if (j < y.stationNum) cur2 += y.travelTimes[j];
                        }
                        if (aboard && ashore){
                            int cost_time2 = end_time2 - start_time2;
                            valid.push_back((ticket){y.trainID, start_time2, end_time2, x.stations[i], t, seats2, cost_time2, cost_price2});
                        }
                    }
                    TRAIN_ALL::standard_start = start_time;
                    sort(valid.begin(), valid.end(), p == "time" ? cmp_by_standard_time : cmp_by_cost);
                    int j = 0;
                    while (j < valid.size() && valid[j].trainID == x.trainID) j++;
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
                    if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
                    if (i < x.stationNum){
                        cur += x.travelTimes[i];
                        seats = min(seats, x.seats[(d-(duration.getdate()+x.saleDate.first))/1440][i]);
                        cost_price += x.prices[i];
                    }
                }
            }
        }
        if (!got_answer) cout << '0' << endl;
        else cout << optimal.first << endl << optimal.second << endl;
    }

    Order query_ticket(const String &id, const Date &d, const String &f, const String &t){
        vector<train> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        train &x = res[0];
        if (!x.released) throw("have not released");
        Date cur = x.startTime;
        Date start_time, end_time, duration;
        int cost_price = 0, seats = Max_Seats;
        bool aboard = false, ashore = false;
        for (int i = 1; i <= x.stationNum; ++i){
            if (x.stations[i] == t){
                ashore = true, end_time = cur; break;
            }
            if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
            if (x.stations[i] == f){
                duration = cur;
                if (!((x.saleDate.first + duration).getdate() <= d && (x.saleDate.second + duration).getdate() >= d)) break;
                aboard = true;
                cur.setdate(d), start_time = cur;
            }
            if (aboard){
                cost_price += x.prices[i];
                seats = min(seats, x.seats[(d-(duration.getdate()+x.saleDate.first))/1440][i]);
            }
            if (i < x.stationNum) cur += x.travelTimes[i];
        }
        if (aboard && ashore)
            return Order(x.trainID, start_time, end_time, f, t, cost_price, seats, "None");
        throw("purchase failed");
    }

    void buy_ticket(const String &id, const Date &d, int n, const String &f, const String &t){
        vector<train> res = train_id.search(id);
        train &x = res[0];
        bool aboard = false;
        Date cur = x.startTime, days;
        for (int i = 1; i <= x.stationNum; ++i){
            if (x.stations[i] == t) break;
            if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
            if (x.stations[i] == f)
                aboard = true, days = cur.getdate();
            if (aboard) x.seats[(d-(days+x.saleDate.first))/1440][i] -= n;
            if (i < x.stationNum) cur += x.travelTimes[i];
        }
        for (int i = 1; i <= x.stationNum; ++i)
            train_stations.modify(make_pair(x.stations[i], x.trainID) , x);
        train_id.modify(x.trainID, x);
    }

    void queue(const String &id, Order &order){
        vector<train> res = train_id.search(id);
        train &x = res[0];
        vector<int> num = train_num_order.search(id);
        train_num_order.modify(id, num[0] + 1);
        order.pending_num = num[0] + 1;
        train_order.insert(make_pair(id, num[0] + 1), order);
    }

    vector<Order> refund(const Order &order){
        if (order.pending_num) {
            vector<Order> res = train_order.search(make_pair(order.trainID, order.pending_num));
            if (res.size() != 1) throw("cannot find pending num");
            res[0].status = "refunded";
            train_order.modify(make_pair(order.trainID, order.pending_num), res[0]);
        }
        if (order.status == "pending"){
            return vector<Order>();
        } else {
            vector<train> res = train_id.search(order.trainID);
            train &x = res[0];
            bool aboard = false;
            Date cur = x.startTime, days;
            for (int i = 1; i <= x.stationNum; ++i){
                if (x.stations[i] == order.endStation) break;
                if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
                if (x.stations[i] == order.startStation)
                    aboard = true, days = cur.getdate();
                if (aboard) x.seats[(order.startTime.getdate()-(days+x.saleDate.first))/1440][i] += order.num;
                if (i < x.stationNum) cur += x.travelTimes[i];
            }
            for (int i = 1; i <= x.stationNum; ++i)
                train_stations.modify(make_pair(x.stations[i], x.trainID), x);
            train_id.modify(x.trainID, x);
            return recover(order.trainID);
        }
    }

    void clean(){
        train_stations.clear(), train_id.clear(), train_num_order.clear(), train_order.clear();
    }

};

Date TRAIN_ALL::standard_start;

#endif //TICKET_SYSTEM_2020ACM_TRAIN_HPP
