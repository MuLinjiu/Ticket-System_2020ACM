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
    struct train {
        String trainID;
        int stationNum, maxSeats;
        String stations[Num];
        int prices[Num], seats[Num][Num]; // from 1 to stationNum - 1
        Date startTime;
        int travelTimes[Num]; // from 1 to stationNum - 1
        int stopoverTimes[Num]; // from 2 to stationNum - 1
        pair<Date, Date> saleDate;
        char type;
        bool released = false;
        int num_pending = 0;
    };

    struct ticket {
        String trainID;
        Date startTime, endTime;
        String startStation, endStation;
        int seats, cost_time, cost_price;

        friend ostream &operator<<(ostream &os, const ticket &x) {
            os << x.trainID << ' ' << x.startStation << ' ' << x.startTime << " -> " << x.endStation << ' ' << x.endTime << ' ' << x.cost_price << ' ' << x.seats;
            return os;
        }
    };

    BpTree<pair<String, String>> train_stations;
    BpTree<String> train_id;
    Storage_IO<train> train_data;
    BpTree<pair<String, int>> train_order;
    Storage_IO<Order> order_data;
    vector<ticket> valid;

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
        if (lhs.endTime == rhs.endTime)
            return lhs.trainID < rhs.trainID;
        else return lhs.endTime < rhs.endTime;
    }

    static bool cmp_by_cost(const ticket &lhs, const ticket &rhs) {
        if (lhs.cost_price == rhs.cost_price)
            return lhs.trainID < rhs.trainID;
        else return lhs.cost_price < rhs.cost_price;
    }

    static bool less(const pair<ticket, ticket> &lhs, const pair<ticket, ticket> &rhs){
        if (lhs.first.cost_time != rhs.first.cost_time)
            return lhs.first.cost_time < rhs.first.cost_time;
        else if (lhs.first.trainID != rhs.first.trainID)
            return lhs.first.trainID < rhs.first.trainID;
        else return lhs.second.trainID < rhs.second.trainID;
    }

    vector<Order> recover(const String &id){
        vector<int> candidates = train_order.search(make_pair(id, 0), equal);
        vector<Order> ret;
        Order x;
        for (auto &index : candidates){
            order_data.read(index, x);
            if (x.status != "pending") continue;
            Order res = query_ticket(x.trainID, x.startTime.getdate(), x.startStation, x.endStation, x.num);
            if (res.num >= x.num) {
                x.status = "success";
                buy_ticket(x.trainID, x.startTime.getdate(), x.num, x.startStation, x.endStation);
                order_data.erase(train_order.erase(make_pair(id, x.pending_num)));
                ret.push_back(x);
            }
        }
        return ret;
    }

public:
    TRAIN_ALL() : train_stations("train_stations"), train_id("train_id"), train_order("train_order"), train_data("data_train.dat"), order_data("data_train_order.dat") {}

    void add_train(const String &id, int n, int m, const String *s, int *p, const Date &x, int *t, int *o, const pair<Date, Date> &d, char y){
        train tmp;
        tmp.trainID = id, tmp.stationNum = n, tmp.maxSeats = m, tmp.startTime = x, tmp.saleDate = d, tmp.type = y, tmp.released = false;
        for (int i = 1; i <= n; ++i) tmp.stations[i] = s[i];
        for (int i = 1; i < n; ++i) tmp.prices[i] = p[i], tmp.travelTimes[i] = t[i];
        for (int i = 0; i <= (d.second - d.first) / 1440; ++i)
            for (int j = 1; j < n; ++j)
                tmp.seats[i][j] = m;
        for (int i = 2; i < n; ++i) tmp.stopoverTimes[i] = o[i];
        int index = train_data.write(tmp);
        if (!train_id.insert(id, index)){
            throw("invalid_insert");
        }
        for (int i = 1; i <= n; ++i)
            train_stations.insert(make_pair(s[i], id), index);
    }

    void release_train(const String &id){
        vector<int> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        train cur_train;
        train_data.read(res[0], cur_train);
        if (cur_train.released) throw("already released");
        cur_train.released = true;
        train_data.write(res[0], cur_train);
    }

    void query_train(const String &id, const Date &d){
        vector<int> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        train cur_train;
        train_data.read(res[0], cur_train);
        if (cur_train.saleDate.first <= d && d <= cur_train.saleDate.second){
            cout << id << ' ' << cur_train.type << endl;
            Date cur = d;
            int cost = 0;
            for (int i = 1; i <= cur_train.stationNum; ++i){
                cout << cur_train.stations[i];
                if (i == 1){
                    cur += cur_train.startTime;
                    cout << " xx-xx xx:xx -> " << cur;
                } else {
                    cur += cur_train.travelTimes[i-1], cost += cur_train.prices[i-1];
                    cout << ' ' << cur;
                    if (i == cur_train.stationNum)
                        cout << " -> xx-xx xx:xx";
                    else{
                        cur += cur_train.stopoverTimes[i];
                        cout << " -> " << cur;
                    }
                }
                cout << ' ' << cost;
                if (i == cur_train.stationNum) cout << " x" << endl;
                else cout << ' ' << cur_train.seats[(d-cur_train.saleDate.first)/1440][i] << endl;
            }
        } else throw("not found");
    }

    void delete_train(const String &id){
        vector<int> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        train cur_train;
        train_data.read(res[0], cur_train);
        if (cur_train.released) throw("train is released");
        int index = train_id.erase(id);
        for (int i = 1; i <= cur_train.stationNum; ++i)
            train_stations.erase(make_pair(cur_train.stations[i], id));
        train_data.erase(index);
        for (int i = 1; i <= cur_train.num_pending; ++i)
            order_data.erase(train_order.erase(make_pair(id, i)));
    }

    void query_ticket(const String &s, const String &t, const Date &d, const String &p = "time"){
        vector<int> res = train_stations.search(make_pair(s, String()), equal);
        valid.clear();
        for (const auto &index : res){
            train x; train_data.read(index, x);
            if (!x.released) continue;
            Date cur = x.startTime;
            Date start_time, end_time, duration;
            int cost_price = 0, seats = x.maxSeats;
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
        vector<int> res = train_stations.search(make_pair(s, String()), equal);
        pair<ticket, ticket> optimal;
        bool got_answer = false;
        for (const auto& index : res){
            train x; train_data.read(index, x);
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
                    vector<int> res2 = train_stations.search(make_pair(x.stations[i], String()), equal);
                    valid.clear();
                    for (const auto& index2 : res2){
                        train y; train_data.read(index2, y);
                        if (!y.released) continue;
                        Date cur2 = y.startTime;
                        Date start_time2, end_time2, duration2, d2;
                        int cost_price2 = 0, seats2 = y.maxSeats;
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
                    sort(valid.begin(), valid.end(), p == "time" ? cmp_by_standard_time : cmp_by_cost);
                    int j = 0;
                    while (j < valid.size() && valid[j].trainID == x.trainID) j++;
                    if (j < valid.size()){
                        ticket tmp{x.trainID, start_time, cur, s, x.stations[i], seats, cur - start_time, cost_price};
                        if (!got_answer) optimal = make_pair(tmp, valid[j]), got_answer = true;
                        else {
                            if (p == "time"){
                                if (valid[j].endTime - start_time == optimal.second.endTime - optimal.first.startTime) {
                                    auto other = make_pair(tmp, valid[j]);
                                    if (less(other, optimal)) optimal = other;
                                } else if (valid[j].endTime - start_time < optimal.second.endTime - optimal.first.startTime)
                                    optimal = make_pair(tmp, valid[j]);
                            } else {
                                if (valid[j].cost_price + cost_price == optimal.first.cost_price + optimal.second.cost_price) {
                                    auto other = make_pair(tmp, valid[j]);
                                    if (less(other, optimal)) optimal = other;
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

    Order query_ticket(const String &id, const Date &d, const String &f, const String &t, int n){
        vector<int> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        train x; train_data.read(res[0], x);
        if (!x.released) throw("have not released");
        if (n > x.maxSeats) throw("exceed maximum seats");
        Date cur = x.startTime;
        Date start_time, end_time, duration;
        int cost_price = 0, seats = x.maxSeats;
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
        vector<int> res = train_id.search(id);
        train x; train_data.read(res[0], x);
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
        train_data.write(res[0], x);
    }

    void queue(const String &id, Order &order){
        vector<int> res = train_id.search(id);
        train x; train_data.read(res[0], x);
        order.pending_num = ++x.num_pending;
        train_data.write(res[0], x);
        train_order.insert(make_pair(id, order.pending_num), order_data.write(order));
    }

    vector<Order> refund(const Order &order){
        if (order.pending_num)
            order_data.erase(train_order.erase(make_pair(order.trainID, order.pending_num)));
        if (order.status == "pending"){
            return vector<Order>();
        } else {
            vector<int> res = train_id.search(order.trainID);
            train x; train_data.read(res[0], x);
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
            train_data.write(res[0], x);
            return recover(order.trainID);
        }
    }

    void clean(){
        train_stations.clear(), train_id.clear(), train_order.clear(), train_data.clear(), order_data.clear();
    }

};

#endif //TICKET_SYSTEM_2020ACM_TRAIN_HPP
