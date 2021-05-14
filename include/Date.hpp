#ifndef TICKET_SYSTEM_2020ACM_DATE_HPP
#define TICKET_SYSTEM_2020ACM_DATE_HPP

#include <iostream>

class Date{
	friend ostream &operator <<(ostream &os, const Date &x){
		Date::print(os, x.mm), os << '-', Date::print(os, x.dd);
		os << ' ', Date::print(os, x.hr), os << '-', Date::print(os, x.mi);
		return os;
	}
private:
	int mm, dd, hr, mi;

	static void print(ostream &os, int x){
		if (x == 0){
			os << "00";
		} else {
			if (x < 10) os << '0';
			os << x;
		}
	}

public:

};

#endif //TICKET_SYSTEM_2020ACM_DATE_HPP
