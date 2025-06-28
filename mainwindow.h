#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "HashTable.h"
#include "AVLTree.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(HashTable* students, AVLTree* concerts, QWidget* parent = nullptr);
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
    void updateConcertTree();
    void studentCellChanged(int row, int column);
    void concertCellChanged(int row, int column);

private:
    Ui::MainWindow *ui;
    HashTable* students;
    AVLTree* concerts;

    bool studentDialog(Students_entry& out, const Students_entry* initial = nullptr);
    bool concertDialog(Concerts_entry& out, const Concerts_entry* initial = nullptr);
    bool fioDialog(FIO& out, const FIO* initial = nullptr, const QString& title = "Enter FIO");
    bool validateFIO(const QString& surname, const QString& name, const QString& patronymic) const;
    bool validateWord(const QString& word) const;
    bool validateInstrument(const QString& instrument) const;
    bool validateTeacher(const QString& surname, const QString& initials) const;
    bool validatePlay(const QString& play) const;
    bool validateHall(const QString& hall) const;
    bool validateDate(const QString& date) const;
};

#endif
