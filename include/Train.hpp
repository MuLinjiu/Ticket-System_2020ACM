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
        int prices[Num]; // from 1 to stationNum - 1
        Date departTimes[Num]; // for stationNum, it's arrive time
        int travelTimes[Num]; // from 1 to stationNum - 1
        int stopoverTimes[Num]; // from 2 to stationNum - 1
        pair<Date, Date> saleDate;
        char type;
        bool released = false;
        int num_pending = 0;
    };

    struct daily {
        int seats[Num]; // from 1 to stationNum - 1
    };

    struct queryInfo{
        String trainID;
        int index, stationID;
        Date l, r;
    };

    struct ticket {
        String trainID;
        Date startTime, endTime;
        String startStation, endStation;
        int seats, cost_time, cost_price, day;

        friend ostream &operator<<(ostream &os, const ticket &x) {
            os << x.trainID << ' ' << x.startStation << ' ' << x.startTime << " -> " << x.endStation << ' ' << x.endTime << ' ' << x.cost_price << ' ' << x.seats;
            return os;
        }
    };

    BpTree<pair<String, String>, queryInfo> train_stations;
    BpTree<String, int> train_id;
    Storage_IO<train> train_data;
    BpTree<pair<String, int>, int> train_order;
    Storage_IO<Order> order_data;
    BpTree<pair<int, String>, int> train_daily;
    Storage_IO<daily> daily_data;
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

    void getSeats(ticket &t){
        train x;
        auto res = train_id.search(t.trainID);
        train_data.read(res[0], x);
        auto ret = train_daily.search(make_pair(t.day, t.trainID));
        daily s; daily_data.read(ret[0], s);
        bool aboard = false;
        for (int i = 1; i <= x.stationNum; ++i){
            if (x.stations[i] == t.endStation) break;
            if (x.stations[i] == t.startStation){
                aboard = true, t.seats = s.seats[i];
                continue;
            }
            if (aboard) t.seats = min(t.seats, s.seats[i]);
        }
    }

public:
    TRAIN_ALL() : train_stations("train_stations"), train_id("train_id"), train_order("train_order"), train_data("data_train.dat"), order_data("data_train_order.dat"), train_daily("train_daily"), daily_data("data_train_daily.dat") {}

    void add_train(const String &id, int n, int m, const String *s, int *p, const Date &x, int *t, int *o, const pair<Date, Date> &d, char y) {
        train tmp;
        tmp.trainID = id, tmp.stationNum = n, tmp.maxSeats = m, tmp.saleDate = d, tmp.type = y, tmp.released = false;
        for (int i = 1; i <= n; ++i) tmp.stations[i] = s[i], tmp.travelTimes[i] = t[i];
        for (int i = 2; i < n; ++i) tmp.stopoverTimes[i] = o[i];
        tmp.prices[1] = 0, tmp.departTimes[1] = x;
        Date cur = x;
        int cost = 0;
        for (int i = 2; i < n; ++i) {
            cur += t[i - 1] + o[i], cost += p[i - 1];
            tmp.prices[i] = cost, tmp.departTimes[i] = cur;
        }
        cur += t[n - 1], cost += p[n - 1];
        tmp.prices[n] = cost, tmp.departTimes[n] = cur;
        int index = train_data.write(tmp);
        if (!train_id.insert(id, index)) {
            throw ("invalid_insert");
        }
    }

    void release_train(const String &id){
        vector<int> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        train cur_train;
        train_data.read(res[0], cur_train);
        if (cur_train.released) throw("already released");
        cur_train.released = true;
        train_data.write(res[0], cur_train);
        for (int i = 1; i <= cur_train.stationNum; ++i){
            queryInfo q = (queryInfo){id, res[0], i, cur_train.saleDate.first + cur_train.departTimes[i], cur_train.saleDate.second + cur_train.departTimes[i]};
            train_stations.insert(make_pair(cur_train.stations[i], id), q);
        }
        daily td;
        for (int i = 1; i < cur_train.stationNum; ++i) td.seats[i] = cur_train.maxSeats;
        for (int i = 0; i <= (cur_train.saleDate.second - cur_train.saleDate.first) / 1440; ++i)
            train_daily.insert(make_pair(i, id), daily_data.write(td));
    }

    void query_train(const String &id, const Date &d){
        vector<int> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        train cur_train;
        train_data.read(res[0], cur_train);
        if (cur_train.saleDate.first <= d && d <= cur_train.saleDate.second){
            cout << id << ' ' << cur_train.type << endl;
            Date cur = d;
            vector<int> ret; daily seats;
            if (cur_train.released){
                res = train_daily.search(make_pair((d-cur_train.saleDate.first)/1440, id));
                daily_data.read(res[0], seats);
            }
            daily_data.read(res[0], seats);
            for (int i = 1; i <= cur_train.stationNum; ++i){
                cout << cur_train.stations[i];
                if (i == 1){
                    cur += cur_train.departTimes[1];
                    cout << " xx-xx xx:xx -> " << cur;
                } else {
                    cur += cur_train.travelTimes[i-1];
                    cout << ' ' << cur;
                    if (i == cur_train.stationNum)
                        cout << " -> xx-xx xx:xx";
                    else{
                        cur += cur_train.stopoverTimes[i];
                        cout << " -> " << cur;
                    }
                }
                cout << ' ' << cur_train.prices[i];
                if (i == cur_train.stationNum) cout << " x" << endl;
                else cout << ' ' << (cur_train.released ? seats.seats[i] : cur_train.maxSeats) << endl;
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
        train_data.erase(index);
    }

    void query_ticket(const String &s, const String &t, const Date &d, const String &p = "time"){
        auto res1 = train_stations.search(make_pair(s, String()), equal);
        auto res2 = train_stations.search(make_pair(t, String()), equal);
        valid.clear();
        for (int i = 0, j = 0; i < res1.size(); ++i){
            while (j < res2.size() && res2[j].trainID < res1[i].trainID) ++j;
            if (j < res2.size() && res2[j].trainID == res1[i].trainID){
                if (res1[i].stationID < res2[j].stationID && res1[i].l.getdate() <= d && d <= res1[i].r.getdate()){
                    train x; train_data.read(res1[i].index, x);
                    auto res = train_daily.search(make_pair((d-res1[i].l.getdate())/1440, res1[i].trainID));
                    daily seats; daily_data.read(res[0], seats);
                    int num = x.maxSeats;
                    for (int k = res1[i].stationID; k < res2[j].stationID; ++k)
                        num = min(num, seats.seats[k]);
                    int cost_time = x.departTimes[res2[j].stationID-1] + x.travelTimes[res2[j].stationID-1] - x.departTimes[res1[i].stationID];
                    Date start = x.departTimes[res1[i].stationID].setdate(d), end = start + cost_time;
                    valid.push_back((ticket){x.trainID, start, end, s, t, num, cost_time, x.prices[res2[j].stationID] - x.prices[res1[i].stationID], 0});
                }
            }
        }
        sort(valid.begin(), valid.end(), p == "time" ? cmp_by_time : cmp_by_cost);
        cout << valid.size() << endl;
        for (const auto& x : valid) cout << x << endl;
    }

    void query_transfer(const String &f, const String &t, const Date &d, const String &p = "time"){
        auto res = train_stations.search(make_pair(f, String()), equal);
        pair<ticket, ticket> optimal;
        bool got_answer = false;
        for (const auto& index : res){
            train x; train_data.read(index.index, x);
            if (!x.released) continue;
            Date cur = x.departTimes[1], duration;
            int i = 1;
            for (; i <= x.stationNum; ++i) {
                if (x.stations[i] == f) break;
                if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
                if (i < x.stationNum) cur += x.travelTimes[i];
            }
            if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
            duration = cur;
            if ((x.saleDate.first + duration).getdate() <= d && (x.saleDate.second + duration).getdate() >= d){
                cur.setdate(d);
                Date start_time = cur;
                if (i < x.stationNum) cur += x.travelTimes[i];
                int start_num = i;
                for (i++; i <= x.stationNum; ++i){
                    auto res1 = train_stations.search(make_pair(x.stations[i], String()), equal);
                    auto res2 = train_stations.search(make_pair(t, String()), equal);
                    valid.clear();
                    Date d2;
                    for (int u = 0, v = 0; u < res1.size(); ++u){
                        while (v < res2.size() && res2[v].trainID < res1[u].trainID) ++v;
                        if (v < res2.size() && res2[v].trainID == res1[u].trainID){
                            if (res1[u].stationID < res2[v].stationID && res1[u].r >= cur){
                                if (res1[u].l >= cur)
                                    d2 = res1[u].l.getdate();
                                else d2 = res1[u].l.gettime() >= cur.gettime() ? cur.getdate() : cur.getdate() + 1440;
                                train y; train_data.read(res1[u].index, y);
                                int cost_time = y.departTimes[res2[v].stationID-1] + y.travelTimes[res2[v].stationID-1] - y.departTimes[res1[u].stationID];
                                Date start = y.departTimes[res1[u].stationID].setdate(d2), end = start + cost_time;
                                valid.push_back((ticket){y.trainID, start, end, x.stations[i], t, 0, cost_time, y.prices[res2[v].stationID] - y.prices[res1[u].stationID], (d2 - res1[u].l.getdate()) / 1440});
                            }
                        }
                    }
                    sort(valid.begin(), valid.end(), p == "time" ? cmp_by_standard_time : cmp_by_cost);
                    int j = 0;
                    while (j < valid.size() && valid[j].trainID == x.trainID) j++;
                    if (j < valid.size()){
                        ticket tmp{x.trainID, start_time, cur, f, x.stations[i], 0, cur - start_time, x.prices[i] - x.prices[start_num], (d - (duration.getdate() + x.saleDate.first)) / 1440};
                        if (!got_answer) optimal = make_pair(tmp, valid[j]), got_answer = true;
                        else {
                            if (p == "time"){
                                if (valid[j].endTime - start_time == optimal.second.endTime - optimal.first.startTime) {
                                    auto other = make_pair(tmp, valid[j]);
                                    if (less(other, optimal)) optimal = other;
                                } else if (valid[j].endTime - start_time < optimal.second.endTime - optimal.first.startTime)
                                    optimal = make_pair(tmp, valid[j]);
                            } else {
                                if (valid[j].cost_price + tmp.cost_price == optimal.first.cost_price + optimal.second.cost_price) {
                                    auto other = make_pair(tmp, valid[j]);
                                    if (less(other, optimal)) optimal = other;
                                } else if (valid[j].cost_price + tmp.cost_price < optimal.first.cost_price + optimal.second.cost_price)
                                    optimal = make_pair(tmp, valid[j]);
                            }
                        }
                    }
                    if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
                    if (i < x.stationNum) cur += x.travelTimes[i];
                }
            }
        }
        if (!got_answer) cout << '0' << endl;
        else{
            getSeats(optimal.first), getSeats(optimal.second);
            cout << optimal.first << endl << optimal.second << endl;
        }
    }

    Order query_ticket(const String &id, const Date &d, const String &f, const String &t, int n){
        vector<int> res = train_id.search(id);
        if (res.empty()) throw("cannot find the train");
        train x; train_data.read(res[0], x);
        if (!x.released) throw("have not released");
        if (n > x.maxSeats) throw("exceed maximum seats");
        Date cur = x.departTimes[1];
        Date start_time, end_time, duration;
        int seats = x.maxSeats, l, r;
        bool aboard = false, ashore = false;
        vector<int> ret; daily s;
        for (int i = 1; i <= x.stationNum; ++i){
            if (x.stations[i] == t){
                ashore = true, end_time = cur, r = i; break;
            }
            if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
            if (x.stations[i] == f){
                duration = cur;
                if (!((x.saleDate.first + duration).getdate() <= d && (x.saleDate.second + duration).getdate() >= d)) break;
                aboard = true;
                cur.setdate(d), start_time = cur, l = i;
                ret = train_daily.search(make_pair((d-(duration.getdate()+x.saleDate.first))/1440, x.trainID));
                daily_data.read(ret[0], s);
            }
            if (aboard) seats = min(seats, s.seats[i]);
            if (i < x.stationNum) cur += x.travelTimes[i];
        }
        if (aboard && ashore)
            return Order(x.trainID, start_time, end_time, f, t, x.prices[r] - x.prices[l], seats, "None");
        throw("purchase failed");
    }

    void buy_ticket(const String &id, const Date &d, int n, const String &f, const String &t){
        vector<int> res = train_id.search(id);
        train x; train_data.read(res[0], x);
        bool aboard = false;
        Date cur = x.departTimes[1], days;
        vector<int> ret; daily s;
        for (int i = 1; i <= x.stationNum; ++i){
            if (x.stations[i] == t) break;
            if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
            if (x.stations[i] == f) {
                aboard = true, days = cur.getdate();
                ret = train_daily.search(make_pair((d-(days+x.saleDate.first))/1440, x.trainID));
                daily_data.read(ret[0], s);
            }
            if (aboard) s.seats[i] -= n;
            if (i < x.stationNum) cur += x.travelTimes[i];
        }
        daily_data.write(ret[0], s);
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
            Date cur = x.departTimes[1], days;
            vector<int> ret; daily s;
            for (int i = 1; i <= x.stationNum; ++i){
                if (x.stations[i] == order.endStation) break;
                if (i > 1 && i < x.stationNum) cur += x.stopoverTimes[i];
                if (x.stations[i] == order.startStation) {
                    aboard = true, days = cur.getdate();
                    ret = train_daily.search(make_pair((order.startTime.getdate()-(days+x.saleDate.first))/1440, x.trainID));
                    daily_data.read(ret[0], s);
                }
                if (aboard) s.seats[i] += order.num;
                if (i < x.stationNum) cur += x.travelTimes[i];
            }
            daily_data.write(ret[0], s);
            return recover(order.trainID);
        }
    }

    void clean(){
        train_stations.clear(), train_id.clear(), train_order.clear(), train_data.clear(), order_data.clear(), train_daily.clear(), daily_data.clear();
    }

};

#endif //TICKET_SYSTEM_2020ACM_TRAIN_HPP
