#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Students_entry.h"
#include <string>
#include <ostream>
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

    int calculateKey(const FIO& fio) const {
        std::string fullName = fio.surname + fio.name + fio.patronymic;
        int key = 0;
        for (unsigned char c : fullName) {
            key += static_cast<int>(c);
        }
        return key;
    }

    int hash(int key, int j, int s) const {
        return (key + j) % s;
    }

    void resize(int newSize) {
        Entry* newTable = new Entry[newSize];

        for (int i = 0; i < newSize; i++) {
            newTable[i].status = 0;
        }

        int activeCount = 0;
        for (int i = 0; i < fullSize; i++) {
            if (table[i].status == 1) {
                int key = calculateKey(table[i].record.fio);
                int j = 0;
                int index;

                do {
                    index = hash(key, j, newSize);
                    if (newTable[index].status != 1) {
                        newTable[index].record = table[i].record;
                        newTable[index].status = 1;
                        activeCount++;
                        break;
                    }
                    j++;
                } while (j < newSize);
            }
        }

        delete[] table;
        table = newTable;
        fullSize = newSize;
        size = activeCount;
    }

    void checkResize() {
        double loadFactor = static_cast<double>(size) / fullSize;

        if (loadFactor >= 0.6) {
            resize(fullSize * 2);
        }
    }

public:
    HashTable(int initSize)
        : table(nullptr), fullSize(initSize), size(0) {
        table = new Entry[fullSize];
        for (int i = 0; i < fullSize; i++) {
            table[i].status = 0;
        }
    }

    ~HashTable() {
        delete[] table;
    }

    bool insert(const Students_entry& record) {
        checkResize();
        int key = calculateKey(record.fio);
        int j = 0;
        int index;

        do {
            index = hash(key, j, fullSize);
            if (table[index].status != 1) {
                table[index].record = record;
                table[index].status = 1;
                size++;
                return true;
            }
            j++;
        } while (j < fullSize);

        return false;
    }

    bool remove(const FIO& fio) {
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
                size--;
                checkResize();
                return true;
            }
            j++;
        } while (j < fullSize);

        return false;
    }

    void search(const FIO& fio) {
        int key = calculateKey(fio);
        int j = 0;
        int index;
        int steps = 0;

        do {
            steps++;
            index = hash(key, j, fullSize);
            if (table[index].status == 0) {
                std::cout << "Çàïèñü ñ ÔÈÎ " << fio << " íå íàéäåíà\n";
                return;
            }
            if (table[index].status == 1 &&
                table[index].record.fio.surname == fio.surname &&
                table[index].record.fio.name == fio.name &&
                table[index].record.fio.patronymic == fio.patronymic) {
                std::cout << "Íàéäåíà çàïèñü:\n";
                std::cout << "Èíäåêñ " << index << ": " << table[index].record << "\n\n";
                return;
            }
            j++;
        } while (j < fullSize);

        std::cout << "Çàïèñü ñ ÔÈÎ " << fio << " íå íàéäåíà\n\n";
        return;
    }

    void print() const {
        std::cout << "Õýø-òàáëèöà (ðàçìåð: " << size
            << ", êîýôôèöèåíò çàãðóçêè: " << static_cast<double>(size) / fullSize << ")\n";
        for (int i = 0; i < fullSize; i++) {
            if (table[i].status == 1) {
                std::cout << "Èíäåêñ " << i << ": " << table[i].record << std::endl;
            }
            else if (table[i].status == 2) {
                std::cout << "Èíäåêñ " << i << ": ÓÄÀËÅÍÎ" << std::endl;
            }
            else if (table[i].status == 0) {
                std::cout << "Èíäåêñ " << i << ": ÏÓÑÒÎ" << std::endl;
            }
        }
        std::cout << std::endl;
    }

    // additional search helpers for GUI
    bool find(const FIO& fio, Students_entry& res) const {
        int dummy;
        return find(fio, res, dummy);
    }

    bool find(const FIO& fio, Students_entry& res, int& steps) const {
        int key = calculateKey(fio);
        int j = 0;
        int index;
        steps = 0;
        do {
            steps++;
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
            j++;
        } while (j < fullSize);
        return false;
    }

    std::vector<Students_entry> searchByInstrument(const std::string& instr) const {
        std::vector<Students_entry> res;
        for (int i = 0; i < fullSize; ++i) {
            if (table[i].status == 1 && table[i].record.instrument == instr) {
                res.push_back(table[i].record);
            }
        }
        return res;
    }

    std::vector<Students_entry> searchByName(const std::string& name) const {
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

    //GUI
    int getSize() const { return size; }
    int getFullSize() const { return fullSize; }
    bool isOccupied(int index) const { return table[index].status == 1; }
    const Students_entry& getEntry(int index) const { return table[index].record; }
    std::string entryToString(int index) const;
};

inline std::string HashTable::entryToString(int index) const {
    const Students_entry& st = table[index].record;
    return st.fio.surname + " " + st.fio.name + " " + st.fio.patronymic +
           " - " + st.instrument + " (" + st.teacher.surname + " " +
           st.teacher.initials + ")";
}

#endif
