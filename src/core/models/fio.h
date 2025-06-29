#ifndef FIO_H
#define FIO_H

#include <string>

struct FIO {
    std::string surname;
    std::string name;
    std::string patronymic;

	bool operator<(const FIO& other) const {
		return (surname + name + patronymic) < (other.surname + other.name + other.patronymic);
	}

	bool operator>(const FIO& other) const {
		return (surname + name + patronymic) > (other.surname + other.name + other.patronymic);
	}

	bool operator==(const FIO& other) const {
		return (surname == other.surname) && (name == other.name) && (patronymic == other.patronymic);
	}
};


#endif
