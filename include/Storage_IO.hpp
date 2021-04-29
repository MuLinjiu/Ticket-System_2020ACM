#ifndef TICKET_SYSTEM_2020ACM_STORAGE_IO_HPP
#define TICKET_SYSTEM_2020ACM_STORAGE_IO_HPP

#include <fstream>
#include <string>

template<class Value>
class Storage_IO{
private:
	fstream file;

	int get_pos(int num){
		return (sizeof(int) + sizeof(Value)) * (num - 1) + sizeof(int);
	}

	void get_num(int pos){
		return (pos - sizeof(int)) / (sizeof(int) + sizeof(Value)) + 1;
	}

public:
	Storage_IO() = default;

	Storage_IO(string file_name){
		file.open(file_name, ios::in | ios::out | ios::binary);
		if (!file){
			file.open(file_name, ios::out);
			int zero = 0;
			file.write(reinterpret_cast<char *> (&zero), sizeof(int));
			file.close();
			file.open(file_name, ios::in | ios::out | ios::binary);
		}
	}

	~Storage_IO(){ file.close(); }

	int write(const Value& val){
		file.seekg(0);
		int nxt = 0, pos, num;
		file.read(reinterpret_cast<char *> (&nxt), sizeof(int));
		if (!nxt){
			pos = ios::end, num = get_num(pos);
		} else {
			num = nxt, pos = get_pos(num);
			file.seekg(pos + sizeof(Value));
			file.read(reinterpret_cast<char *> (&nxt), sizeof(int));
			file.seekp(0);
			file.write(reinterpret_cast<char *> (&nxt), sizeof(int));
		}
		file.seekp(pos);
		file.write(reinterpret_cast<char *> (&val), sizeof(Value));
		return num;
	}

	void read(int num, Value &val){
		file.seekg(get_pos(num));
		file.read(reinterpret_cast<char *> (&val), sizeof(Value));
	}

	void delete(int num){
		int pos = get_pos(num), nxt;
		file.seekg(0);
		file.read(reinterpret_cast<char *> (&nxt), sizeof(int));
		file.seekp(pos + sizeof(Value));
		file.write(reinterpret_cast<char *> (&nxt), sizeof(int));
		file.seekp(0);
		file.write(reinterpret_cast<char *> (&num), sizeof(int));
	}

};

#endif //TICKET_SYSTEM_2020ACM_STORAGE_IO_HPP
