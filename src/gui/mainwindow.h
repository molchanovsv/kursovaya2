#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "hashtable.h"
#include "avltree.h"
#include "theme.h"
#include "student_entry.h"
#include <vector>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(HashTable* students, AVLTree* concerts,
                        const std::vector<Students_entry>& rawStudents,
                        const QString& studFile, const QString& concFile,
                        QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void refreshTables();
    void addStudent();
    void removeStudent();
    void editStudent();
    void addConcert();
    void removeConcert();
    void editConcert();
    void searchStudent();
    void searchConcert();
    void clearStudentSearch();
    void clearConcertSearch();
    void updateConcertTree();
    void updateReport();
    void studentCellChanged(int row, int column);
    void concertCellChanged(int row, int column);
    void studentContextMenu(const QPoint& pos);
    void concertContextMenu(const QPoint& pos);
    void exportStudents();
    void exportConcerts();
    void exportReport();

private:
    Ui::MainWindow *ui;
    HashTable* students;
    AVLTree* concerts;
    std::vector<int> studentRowMap;
    std::vector<Students_entry> studentList;
    std::vector<Concerts_entry> concertList;
    QString studentFile;
    QString concertFile;

    // search filters
    bool studentFilterActive = false;
    QString sSurname, sName, sPatronymic, sInstr, sTeacher;
    bool concertFilterActive = false;
    QString cSurname, cName, cPatronymic, cInstr, cPlay, cHall;
    QString cDate;
    bool cDateEnabled = false;

    bool studentDialog(Students_entry& out, const Students_entry* initial = nullptr);
    bool concertDialog(Concerts_entry& out, const Concerts_entry* initial = nullptr);
    bool fioDialog(FIO& out, const FIO* initial = nullptr, const QString& title = "Enter FIO");
    bool fioInstrumentDialog(FIO& fio, std::string& instrument,
                             const FIO* initial = nullptr, const QString& instr = QString(),
                             const QString& title = "Enter FIO and instrument");
    bool validateFIO(const QString& surname, const QString& name, const QString& patronymic) const;
    bool validateWord(const QString& word) const;
    bool validateInstrument(const QString& instrument) const;
    bool validateTeacher(const QString& surname, const QString& initials) const;
    bool validateHall(const QString& hall) const;
    bool validateDate(const QString& date) const;
};

#endif
