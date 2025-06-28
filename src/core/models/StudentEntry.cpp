#include "StudentEntry.h"

std::ostream& operator<<(std::ostream& os, const Students_entry& entry) {
	os << "ÔÈÎ: " << entry.fio << ", "
		<< "Èíñòðóìåíò: " << entry.instrument << ", "
		<< "Ïðåïîäàâàòåëü: " << entry.teacher;
	return os;
}