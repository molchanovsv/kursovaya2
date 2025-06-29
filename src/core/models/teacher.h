#ifndef TEACHER_H
#define TEACHER_H

#include <string>
#include <iostream>

struct Teacher {
    std::string surname;
    std::string initials;
};

std::ostream& operator<<(std::ostream& os, const Teacher& teacher);

#endif
