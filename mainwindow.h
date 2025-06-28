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

private:
    Ui::MainWindow *ui;
    HashTable* students;
    AVLTree* concerts;

    bool studentDialog(Students_entry& out, const Students_entry* initial = nullptr);
    bool concertDialog(Concerts_entry& out, const Concerts_entry* initial = nullptr);
};

#endif
