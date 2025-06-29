#include "fio.h"

std::ostream& operator<<(std::ostream& os, const FIO& fio) {
    os << fio.surname << " " << fio.name << " " << fio.patronymic;
    return os;
}
