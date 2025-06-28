#include "Teacher.h"

std::ostream& operator<<(std::ostream& os, const Teacher& teacher) {
    os << teacher.surname << " " << teacher.initials;
    return os;
}