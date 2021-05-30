#ifndef TICKET_SYSTEM_2020ACM_DATE_HPP
#define TICKET_SYSTEM_2020ACM_DATE_HPP

#include <iostream>
#include "String.hpp"
using namespace std;

class Date{
	friend ostream &operator <<(ostream &os, const Date &x){
		Date::print(os, x.mm), os << '-', Date::print(os, x.dd);
		os << ' ', Date::print(os, x.hr), os << ':', Date::print(os, x.mi);
		return os;
	}

private:
	int mm, dd, hr, mi;
	static const int days[12];
	static const int sum[13];

	static void print(ostream &os, int x){
		if (x == 0){
			os << "00";
		} else {
			if (x < 10) os << '0';
			os << x;
		}
	}

public:
	Date() : mm(1), dd(1), hr(0), mi(0) {}

	Date(const String &str) : mm(1), dd(1), hr(0), mi(0) {
	    int x = (str[0] - '0') * 10 + (str[1] - '0'), y = (str[3] - '0') * 10 + (str[4] - '0');
	    if (str[2] == '-') mm = x, dd = y;
	    else hr = x, mi = y;
	}

	Date(int mm_, int dd_, int hr_, int mi_) : mm(mm_), dd(dd_), hr(hr_), mi(mi_) {}

	void setdate(const Date &other){
	    mm = other.mm, dd = other.dd;
	}

	void settime(const Date &other){
	    hr = other.hr, mi = other.mi;
	}

	Date getdate() const{ return {mm, dd, 0, 0}; }

	Date gettime() const{ return {1, 1, hr, mi}; }

	Date operator+(int x) const{
		if (x < 0) return *this - (-x);
		Date ret = Date(mm, dd, hr, mi + x);
		ret.hr += ret.mi / 60, ret.mi %= 60;
		ret.dd += ret.hr / 24, ret.hr %= 24;
		while (ret.dd > days[ret.mm-1]){
			ret.dd -= days[ret.mm-1];
			if (++ret.mm > 12) ret.mm = 1;
		}
		return ret;
	}

	Date operator+(const Date &x) const{
	    return *this + int(x);
	}

	Date operator-(int x) const{
		if (x < 0) return *this + (-x);
		Date ret = Date(mm, dd, hr, mi - x);
		ret.hr += ret.mi / 60, ret.mi %= 60;
		if (ret.mi < 0) ret.hr--, ret.mi += 60;
		ret.dd += ret.hr / 24, ret.hr %= 24;
		if (ret.hr < 0) ret.dd--, ret.hr += 24;
		while (ret.dd < 0){
			if (--ret.mm <= 0) ret.mm += 12;
			ret.dd += days[ret.mm-1];
		}
		return ret;
	}

	Date &operator+=(int x){
		return *this = *this + x;
	}

    Date &operator+=(const Date &x){
        return *this = *this + x;
    }

	Date &operator++(){
		return *this = *this + 1;
	}

	Date operator++(int x){
		Date ret = *this;
		*this = *this + 1;
		return ret;
	}

	Date &operator-=(int x){
		return *this = *this - x;
	}

	Date &operator--(){
		return *this = *this - 1;
	}

	Date operator--(int x){
		Date ret = *this;
		*this = *this - 1;
		return ret;
	}

	bool operator==(const Date &other) const{
		return mm == other.mm && dd == other.dd && hr == other.hr && mi == other.mi;
	}

	bool operator!=(const Date &other) const{
		return !(*this == other);
	}

	bool operator<(const Date &other) const{
		if (mm != other.mm) return mm < other.mm;
		if (dd != other.dd) return dd < other.dd;
		if (hr != other.hr) return hr < other.hr;
		return mi < other.mi;
	}

	bool operator<=(const Date &other) const{
		return *this < other || *this == other;
	}

	bool operator>(const Date &other) const{
		return other < *this;
	}

	bool operator>=(const Date &other) const{
		return *this > other || *this == other;
	}

	int operator-(const Date &other) const{
	    return int(*this) - int(other);
	}

	explicit operator int() const{
        return (sum[mm-1] + dd - 1) * 1440 + hr * 60 + mi;
	}

};

const int Date::days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const int Date::sum[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

#endif //TICKET_SYSTEM_2020ACM_DATE_HPP
