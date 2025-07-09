#include "hashtable.h"

HashTable::HashTable(int initSize)
    : table(new Entry[initSize]), fullSize(initSize), size(0) {
    for (int i = 0; i < fullSize; ++i) {
        table[i].status = 0;
    }
}

HashTable::~HashTable() {
    delete[] table;
}

int HashTable::calculateKey(const FIO& fio, const std::string& instrument) const {
    std::string fullName = fio.surname + fio.name + fio.patronymic + instrument;
    int key = 0;
    for (unsigned char c : fullName) {
        key += static_cast<int>(c);
    }
    return key;
}

int HashTable::hash(int key, int j, int s) const {
    return (key + j) % s;
}

int HashTable::initialIndex(const FIO& fio, const std::string& instrument) const {
    int key = calculateKey(fio, instrument);
    return hash(key, 0, fullSize);
}

void HashTable::resize(int newSize) {
    Entry* newTable = new Entry[newSize];
    for (int i = 0; i < newSize; ++i) {
        newTable[i].status = 0;
    }
    int activeCount = 0;
    for (int i = 0; i < fullSize; ++i) {
        if (table[i].status == 1) {
            int key = calculateKey(table[i].record.fio, table[i].record.instrument);
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
    double loadFactor = fullSize > 0 ? static_cast<double>(size) / fullSize : 0.0;
    if (loadFactor >= 0.6) {
        resize(fullSize * 2);
    } else if (loadFactor <= 0.25 && fullSize > 1 && size > 0) {
        resize(fullSize / 2);
    }
}

bool HashTable::insert(const Students_entry& record) {
    Students_entry existing;
    if (find(record.fio, record.instrument, existing) &&
        existing.fio == record.fio && existing.instrument == record.instrument)
        return false;

    int key = calculateKey(record.fio, record.instrument);
    int j = 0;
    int index;
    do {
        index = hash(key, j, fullSize);
        if (table[index].status != 1) {
            table[index].record = record;
            table[index].status = 1;
            ++size;
            checkResize();
            return true;
        }
        ++j;
    } while (j < fullSize);
    return false;
}

bool HashTable::remove(const FIO& fio, const std::string& instrument) {
    int key = calculateKey(fio, instrument);
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
            table[index].record.fio.patronymic == fio.patronymic &&
            table[index].record.instrument == instrument) {
            table[index].status = 2;
            --size;
            checkResize();
            return true;
        }
        ++j;
    } while (j < fullSize);
    return false;
}


bool HashTable::find(const FIO& fio, const std::string& instrument, Students_entry& res) const {
    int dummy;
    return find(fio, instrument, res, dummy);
}

bool HashTable::find(const FIO& fio, const std::string& instrument, Students_entry& res, int& steps) const {
    int key = calculateKey(fio, instrument);
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
            table[index].record.fio.patronymic == fio.patronymic &&
            table[index].record.instrument == instrument) {
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

