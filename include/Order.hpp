#ifndef RUN_HPP_ORDER_HPP
#define RUN_HPP_ORDER_HPP

#include <iostream>
#include "String.hpp"
#include "Date.hpp"

struct Order{
    friend ostream &operator<<(ostream &os, const Order &x){
        os << '[' << x.status << "] " << x.trainID << ' ' << x.startStation << ' ' << x.startTime << " -> " << x.endStation << ' ' << x.endTime << ' ' << x.price << ' ' << x.num;
        return os;
    }

    String userID, trainID;
    Date startTime, endTime;
    String startStation, endStation;
    int price, num;
    String status; // success, pending, refunded
    int order_num, pending_num = 0; // if pending

    Order() = default;

    Order(const String &id, const Date &st, const Date &et, const String &ss, const String &es, int p, int n, const String &status_)
        : trainID(id), startTime(st), endTime(et), startStation(ss), endStation(es), price(p), num(n), status(status_) {}

};

#endif //RUN_HPP_ORDER_HPP
