#include "hashtable.h"
#include <iostream>

HashTable::HashTable(int initSize)
    : table(new Entry[initSize]), fullSize(initSize), size(0) {
    for (int i = 0; i < fullSize; ++i) {
        table[i].status = 0;
    }
}

HashTable::~HashTable() {
    delete[] table;
}

int HashTable::calculateKey(const FIO& fio) const {
    std::string fullName = fio.surname + fio.name + fio.patronymic;
    int key = 0;
    for (unsigned char c : fullName) {
        key += static_cast<int>(c);
    }
    return key;
}

int HashTable::hash(int key, int j, int s) const {
    return (key + j) % s;
}

void HashTable::resize(int newSize) {
    Entry* newTable = new Entry[newSize];
    for (int i = 0; i < newSize; ++i) {
        newTable[i].status = 0;
    }
    int activeCount = 0;
    for (int i = 0; i < fullSize; ++i) {
        if (table[i].status == 1) {
            int key = calculateKey(table[i].record.fio);
            int j = 0;
            int index;
            do {
                index = hash(key, j, newSize);
                if (newTable[index].status != 1) {
                    newTable[index].record = table[i].record;
                    newTable[index].status = 1;
                    ++activeCount;
                    break;
                }
                ++j;
            } while (j < newSize);
        }
    }
    delete[] table;
    table = newTable;
    fullSize = newSize;
    size = activeCount;
}

void HashTable::checkResize() {
    double loadFactor = static_cast<double>(size) / fullSize;
    if (loadFactor >= 0.6) {
        resize(fullSize * 2);
    }
}

bool HashTable::insert(const Students_entry& record) {
    checkResize();
    int key = calculateKey(record.fio);
    int j = 0;
    int index;
    do {
        index = hash(key, j, fullSize);
        if (table[index].status != 1) {
            table[index].record = record;
            table[index].status = 1;
            ++size;
            return true;
        }
        ++j;
    } while (j < fullSize);
    return false;
}

bool HashTable::remove(const FIO& fio) {
    int key = calculateKey(fio);
    int j = 0;
    int index;
    do {
        index = hash(key, j, fullSize);
        if (table[index].status == 0) {
            return false;
        }
        if (table[index].status == 1 &&
            table[index].record.fio.surname == fio.surname &&
            table[index].record.fio.name == fio.name &&
            table[index].record.fio.patronymic == fio.patronymic) {
            table[index].status = 2;
            --size;
            checkResize();
            return true;
        }
        ++j;
    } while (j < fullSize);
    return false;
}

void HashTable::search(const FIO& fio) {
    int key = calculateKey(fio);
    int j = 0;
    int index;
    int steps = 0;
    do {
        ++steps;
        index = hash(key, j, fullSize);
        if (table[index].status == 0) {
            std::cout << "\u0417\u0430\u043f\u0438\u0441\u044c \u0441 \u0424\u0418\u041e " << fio << " \u043d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d\u0430\n";
            return;
        }
        if (table[index].status == 1 &&
            table[index].record.fio.surname == fio.surname &&
            table[index].record.fio.name == fio.name &&
            table[index].record.fio.patronymic == fio.patronymic) {
            std::cout << "\u041d\u0430\u0439\u0434\u0435\u043d\u0430 \u0437\u0430\u043f\u0438\u0441\u044c:\n";
            std::cout << "\u0418\u043d\u0434\u0435\u043a\u0441 " << index << ": " << table[index].record << "\n\n";
            return;
        }
        ++j;
    } while (j < fullSize);
    std::cout << "\u0417\u0430\u043f\u0438\u0441\u044c \u0441 \u0424\u0418\u041e " << fio << " \u043d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d\u0430\n\n";
}

bool HashTable::find(const FIO& fio, Students_entry& res) const {
    int dummy;
    return find(fio, res, dummy);
}

bool HashTable::find(const FIO& fio, Students_entry& res, int& steps) const {
    int key = calculateKey(fio);
    int j = 0;
    int index;
    steps = 0;
    do {
        ++steps;
        index = hash(key, j, fullSize);
        if (table[index].status == 0) {
            return false;
        }
        if (table[index].status == 1 &&
            table[index].record.fio.surname == fio.surname &&
            table[index].record.fio.name == fio.name &&
            table[index].record.fio.patronymic == fio.patronymic) {
            res = table[index].record;
            return true;
        }
        ++j;
    } while (j < fullSize);
    return false;
}

std::vector<Students_entry> HashTable::searchByInstrument(const std::string& instr) const {
    std::vector<Students_entry> res;
    for (int i = 0; i < fullSize; ++i) {
        if (table[i].status == 1 && table[i].record.instrument == instr) {
            res.push_back(table[i].record);
        }
    }
    return res;
}

std::vector<Students_entry> HashTable::searchByName(const std::string& name) const {
    std::vector<Students_entry> res;
    for (int i = 0; i < fullSize; ++i) {
        if (table[i].status == 1) {
            const auto& r = table[i].record;
            if (r.fio.surname == name || r.fio.name == name || r.fio.patronymic == name)
                res.push_back(r);
        }
    }
    return res;
}

void HashTable::print() const {
    std::cout << "\u0425\u044d\u0448-\u0442\u0430\u0431\u043b\u0438\u0446\u0430 (\u0440\u0430\u0437\u043c\u0435\u0440: " << size
              << ", \u043a\u043e\u044d\u0444\u0444\u0438\u0446\u0438\u0435\u043d\u0442 \u0437\u0430\u0433\u0440\u0443\u0437\u043a\u0438: "
              << static_cast<double>(size) / fullSize << ")\n";
    for (int i = 0; i < fullSize; ++i) {
        if (table[i].status == 1) {
            std::cout << "\u0418\u043d\u0434\u0435\u043a\u0441 " << i << ": " << table[i].record << std::endl;
        } else if (table[i].status == 2) {
            std::cout << "\u0418\u043d\u0434\u0435\u043a\u0441 " << i << ": \u0423\u0414\u0410\u041b\u0415\u041d\u041e" << std::endl;
        } else if (table[i].status == 0) {
            std::cout << "\u0418\u043d\u0434\u0435\u043a\u0441 " << i << ": \u041f\u0423\u0421\u0422\u041e" << std::endl;
        }
    }
    std::cout << std::endl;
}

std::string HashTable::entryToString(int index) const {
    const Students_entry& st = table[index].record;
    return st.fio.surname + " " + st.fio.name + " " + st.fio.patronymic +
           " - " + st.instrument + " (" + st.teacher.surname + " " +
           st.teacher.initials + ")";
}

