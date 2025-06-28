#include "Students_entry.h"

std::ostream& operator<<(std::ostream& os, const Students_entry& entry) {
	os << "ФИО: " << entry.fio << ", "
		<< "Инструмент: " << entry.instrument << ", "
		<< "Преподаватель: " << entry.teacher;
	return os;
}