#include "Students_entry.h"

std::ostream& operator<<(std::ostream& os, const Students_entry& entry) {
	os << "���: " << entry.fio << ", "
		<< "����������: " << entry.instrument << ", "
		<< "�������������: " << entry.teacher;
	return os;
}