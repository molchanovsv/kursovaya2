#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "student_entry.h"
#include <string>
#include <vector>

class HashTable {
private:
    struct Entry {
        Students_entry record;
        int status = 0;
    };

    Entry* table;
    int fullSize;
    int size;

    int calculateKey(const FIO& fio, const std::string& instrument) const;
    int hash(int key, int j, int s) const;
    void resize(int newSize);
    void checkResize();

public:
    explicit HashTable(int initSize);
    ~HashTable();

    bool insert(const Students_entry& record);
    bool remove(const FIO& fio, const std::string& instrument);
    bool find(const FIO& fio, const std::string& instrument, Students_entry& res) const;
    bool find(const FIO& fio, const std::string& instrument, Students_entry& res, int& steps) const;
    std::vector<Students_entry> searchByInstrument(const std::string& instr) const;
    std::vector<Students_entry> searchByName(const std::string& name) const;

    int getSize() const { return size; }
    int getFullSize() const { return fullSize; }
    bool isOccupied(int index) const { return table[index].status == 1; }
    const Students_entry& getEntry(int index) const { return table[index].record; }
};

#endif
