#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(HashTable* studentsTable, AVLTree* concertTree, QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), students(studentsTable), concerts(concertTree)
{
    ui->setupUi(this);
    connect(ui->addStudentButton, &QPushButton::clicked, this, &MainWindow::addStudent);
    connect(ui->removeStudentButton, &QPushButton::clicked, this, &MainWindow::removeStudent);
    connect(ui->editStudentButton, &QPushButton::clicked, this, &MainWindow::editStudent);
    connect(ui->addConcertButton, &QPushButton::clicked, this, &MainWindow::addConcert);
    connect(ui->removeConcertButton, &QPushButton::clicked, this, &MainWindow::removeConcert);
    connect(ui->editConcertButton, &QPushButton::clicked, this, &MainWindow::editConcert);
    connect(ui->searchStudentButton, &QPushButton::clicked, this, &MainWindow::searchStudent);
    connect(ui->searchConcertButton, &QPushButton::clicked, this, &MainWindow::searchConcert);
    refreshTables();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::refreshTables()
{
    ui->studentsTable->clear();
    ui->studentsTable->setColumnCount(1);
    ui->studentsTable->setRowCount(students->getFullSize());
    for (int i = 0; i < students->getFullSize(); ++i) {
        if (students->isOccupied(i)) {
            QString text = QString::fromStdString(students->entryToString(i));
            ui->studentsTable->setItem(i, 0, new QTableWidgetItem(text));
        }
    }

    std::vector<Concerts_entry> list;
    concerts->toVector(list);
    ui->concertsTable->clear();
    ui->concertsTable->setColumnCount(1);
    ui->concertsTable->setRowCount(static_cast<int>(list.size()));
    for (int i = 0; i < list.size(); ++i) {
        const auto& e = list[i];
        QString text = QString::fromStdString(e.fio.surname + " " + e.fio.name + " " +
                                               e.fio.patronymic + " - " + e.play +
                                               " - " + e.hall + " - " + e.date);
        ui->concertsTable->setItem(i, 0, new QTableWidgetItem(text));
    }
}

void MainWindow::addStudent()
{
    Students_entry se;
    se.fio.surname = QInputDialog::getText(this, "Surname", "Surname").toStdString();
    se.fio.name = QInputDialog::getText(this, "Name", "Name").toStdString();
    se.fio.patronymic = QInputDialog::getText(this, "Patronymic", "Patronymic").toStdString();
    se.instrument = QInputDialog::getText(this, "Instrument", "Instrument").toStdString();
    se.teacher.surname = QInputDialog::getText(this, "Teacher surname", "Teacher surname").toStdString();
    se.teacher.initials = QInputDialog::getText(this, "Teacher initials", "Teacher initials").toStdString();
    students->insert(se);
    refreshTables();
}

void MainWindow::removeStudent()
{
    FIO f;
    f.surname = QInputDialog::getText(this, "Surname", "Surname").toStdString();
    f.name = QInputDialog::getText(this, "Name", "Name").toStdString();
    f.patronymic = QInputDialog::getText(this, "Patronymic", "Patronymic").toStdString();
    students->remove(f);
    refreshTables();
}

void MainWindow::editStudent()
{
    FIO f;
    f.surname = QInputDialog::getText(this, "Surname", "Surname").toStdString();
    f.name = QInputDialog::getText(this, "Name", "Name").toStdString();
    f.patronymic = QInputDialog::getText(this, "Patronymic", "Patronymic").toStdString();
    students->remove(f);
    Students_entry se;
    se.fio = f;
    se.instrument = QInputDialog::getText(this, "Instrument", "Instrument").toStdString();
    se.teacher.surname = QInputDialog::getText(this, "Teacher surname", "Teacher surname").toStdString();
    se.teacher.initials = QInputDialog::getText(this, "Teacher initials", "Teacher initials").toStdString();
    students->insert(se);
    refreshTables();
}

void MainWindow::addConcert()
{
    Concerts_entry e;
    e.fio.surname = QInputDialog::getText(this, "Surname", "Surname").toStdString();
    e.fio.name = QInputDialog::getText(this, "Name", "Name").toStdString();
    e.fio.patronymic = QInputDialog::getText(this, "Patronymic", "Patronymic").toStdString();
    e.play = QInputDialog::getText(this, "Play", "Play").toStdString();
    e.hall = QInputDialog::getText(this, "Hall", "Hall").toStdString();
    e.date = QInputDialog::getText(this, "Date", "Date").toStdString();
    concerts->insert(e);
    refreshTables();
}

void MainWindow::removeConcert()
{
    FIO f;
    f.surname = QInputDialog::getText(this, "Surname", "Surname").toStdString();
    f.name = QInputDialog::getText(this, "Name", "Name").toStdString();
    f.patronymic = QInputDialog::getText(this, "Patronymic", "Patronymic").toStdString();
    concerts->remove(f);
    refreshTables();
}

void MainWindow::editConcert()
{
    FIO f;
    f.surname = QInputDialog::getText(this, "Surname", "Surname").toStdString();
    f.name = QInputDialog::getText(this, "Name", "Name").toStdString();
    f.patronymic = QInputDialog::getText(this, "Patronymic", "Patronymic").toStdString();
    concerts->remove(f);
    Concerts_entry e;
    e.fio = f;
    e.play = QInputDialog::getText(this, "Play", "Play").toStdString();
    e.hall = QInputDialog::getText(this, "Hall", "Hall").toStdString();
    e.date = QInputDialog::getText(this, "Date", "Date").toStdString();
    concerts->insert(e);
    refreshTables();
}

void MainWindow::searchStudent()
{
    QStringList types;
    types << "Name" << "Instrument";
    bool ok = false;
    QString type = QInputDialog::getItem(this, "Search Student", "Search by:", types, 0, false, &ok);
    if (!ok || type.isEmpty())
        return;
    QString query = QInputDialog::getText(this, "Search Student", "Value:", QLineEdit::Normal, QString(), &ok);
    if (!ok)
        return;
    std::vector<Students_entry> results;
    if (type == "Name")
        results = students->searchByName(query.toStdString());
    else
        results = students->searchByInstrument(query.toStdString());

    QString text;
    for (const auto& st : results)
        text += QString::fromStdString(st.fio.surname + " " + st.fio.name + " " + st.fio.patronymic +
                                       " - " + st.instrument + " (" + st.teacher.surname + " " + st.teacher.initials + ")\n");
    if (text.isEmpty())
        text = "Not found";
    QMessageBox::information(this, "Results", text);
}

void MainWindow::searchConcert()
{
    QStringList types;
    types << "Date" << "Hall";
    bool ok = false;
    QString type = QInputDialog::getItem(this, "Search Concert", "Search by:", types, 0, false, &ok);
    if (!ok || type.isEmpty())
        return;
    QString query = QInputDialog::getText(this, "Search Concert", "Value:", QLineEdit::Normal, QString(), &ok);
    if (!ok)
        return;
    std::vector<Concerts_entry> results;
    if (type == "Date")
        results = concerts->searchByDate(query.toStdString());
    else
        results = concerts->searchByHall(query.toStdString());

    QString text;
    for (const auto& e : results)
        text += QString::fromStdString(e.fio.surname + " " + e.fio.name + " " + e.fio.patronymic +
                                       " - " + e.play + " - " + e.hall + " - " + e.date + "\n");
    if (text.isEmpty())
        text = "Not found";
    QMessageBox::information(this, "Results", text);
}

