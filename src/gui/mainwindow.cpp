#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <vector>

MainWindow::MainWindow(HashTable* studentsTable, AVLTree* concertTree,
                       const QString& studFile, const QString& concFile,
                       QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), students(studentsTable), concerts(concertTree),
      studentFile(studFile), concertFile(concFile)
{
    ui->setupUi(this);
    connect(ui->addStudentButton, &QPushButton::clicked, this, &MainWindow::addStudent);
    connect(ui->removeStudentButton, &QPushButton::clicked, this, &MainWindow::removeStudent);
    connect(ui->editStudentButton, &QPushButton::clicked, this, &MainWindow::editStudent);
    connect(ui->addConcertButton, &QPushButton::clicked, this, &MainWindow::addConcert);
    connect(ui->removeConcertButton, &QPushButton::clicked, this, &MainWindow::removeConcert);
    connect(ui->editConcertButton, &QPushButton::clicked, this, &MainWindow::editConcert);
    connect(ui->searchConcertButton, &QPushButton::clicked, this, &MainWindow::searchConcert);
    connect(ui->searchStudentButton, &QPushButton::clicked, this, &MainWindow::searchStudent);
    connect(ui->clearStudentSearchButton, &QPushButton::clicked, this, &MainWindow::clearStudentSearch);
    connect(ui->clearConcertSearchButton, &QPushButton::clicked, this, &MainWindow::clearConcertSearch);
    connect(ui->saveStudentsButton, &QPushButton::clicked, this, &MainWindow::exportStudents);
    connect(ui->saveConcertsButton, &QPushButton::clicked, this, &MainWindow::exportConcerts);
    connect(ui->saveReportButton, &QPushButton::clicked, this, &MainWindow::exportReport);
    connect(ui->concertsTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::updateConcertTree);
    connect(ui->studentsTable, &QTableWidget::cellChanged, this, &MainWindow::studentCellChanged);
    connect(ui->concertsTable, &QTableWidget::cellChanged, this, &MainWindow::concertCellChanged);
    ui->studentsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->concertsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->studentsTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::studentContextMenu);
    connect(ui->concertsTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::concertContextMenu);
    connect(ui->instrumentFilterEdit, &QLineEdit::textChanged, this, &MainWindow::updateReport);
    connect(ui->hallFilterEdit, &QLineEdit::textChanged, this, &MainWindow::updateReport);
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, &MainWindow::updateReport);
    connect(ui->instrumentFilterCheck, &QCheckBox::toggled, this, &MainWindow::updateReport);
    connect(ui->hallFilterCheck, &QCheckBox::toggled, this, &MainWindow::updateReport);
    connect(ui->dateFilterCheck, &QCheckBox::toggled, this, &MainWindow::updateReport);
    ui->concertTree->setHeaderHidden(true);
    ui->reportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->studentsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->concertsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->reportTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->mainSplitter->setStretchFactor(0, 1);
    ui->mainSplitter->setStretchFactor(1, 1);
    ui->concertsSplitter->setStretchFactor(0, 1);
    ui->concertsSplitter->setStretchFactor(1, 1);
    refreshTables();
    updateConcertTree();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::refreshTables()
{
    ui->clearStudentSearchButton->setVisible(studentFilterActive);
    ui->clearConcertSearchButton->setVisible(concertFilterActive);
    ui->studentsTable->blockSignals(true);
    ui->studentsTable->clearContents();
    ui->studentsTable->setRowCount(0);
    ui->studentsTable->setColumnCount(6);
    QStringList studentHeaders;
    studentHeaders << "Хэш" << "Фамилия" << "Имя" << "Отчество" << "Инструмент" << "Учитель";
    ui->studentsTable->setHorizontalHeaderLabels(studentHeaders);
    double loadFactor = static_cast<double>(students->getSize()) / students->getFullSize();
    ui->tableInfoLabel->setText(QString("Размер таблицы: %1, заполненность: %2%")
        .arg(students->getFullSize())
        .arg(loadFactor * 100.0, 0, 'f', 2));
    bool useSearch = studentFilterActive;
    studentRowMap.clear();
    int row = 0;
    for (int i = 0; i < students->getFullSize(); ++i) {
        if (students->isOccupied(i)) {
            const auto& st = students->getEntry(i);
            QString sur = QString::fromStdString(st.fio.surname);
            QString nam = QString::fromStdString(st.fio.name);
            QString pat = QString::fromStdString(st.fio.patronymic);
            QString instr = QString::fromStdString(st.instrument);
            QString teacher =
                QString::fromStdString(st.teacher.surname + " " + st.teacher.initials);
            if (useSearch) {
                if (!sSurname.isEmpty() && !sur.contains(sSurname, Qt::CaseInsensitive))
                    continue;
                if (!sName.isEmpty() && !nam.contains(sName, Qt::CaseInsensitive))
                    continue;
                if (!sPatronymic.isEmpty() && !pat.contains(sPatronymic, Qt::CaseInsensitive))
                    continue;
                if (!sInstr.isEmpty() && !instr.contains(sInstr, Qt::CaseInsensitive))
                    continue;
                if (!sTeacher.isEmpty() && !teacher.contains(sTeacher, Qt::CaseInsensitive))
                    continue;
            }
            ui->studentsTable->insertRow(row);
            QTableWidgetItem* vh = new QTableWidgetItem(QString::number(i));
            vh->setTextAlignment(Qt::AlignCenter);
            ui->studentsTable->setVerticalHeaderItem(row, vh);

            auto makeItem = [](const QString& t) {
                QTableWidgetItem* it = new QTableWidgetItem(t);
                it->setTextAlignment(Qt::AlignCenter);
                return it;
            };

            int h = students->initialIndex(st.fio, st.instrument);
            ui->studentsTable->setItem(row, 0, makeItem(QString::number(h)));
            ui->studentsTable->setItem(row, 1, makeItem(QString::fromStdString(st.fio.surname)));
            ui->studentsTable->setItem(row, 2, makeItem(QString::fromStdString(st.fio.name)));
            ui->studentsTable->setItem(row, 3, makeItem(QString::fromStdString(st.fio.patronymic)));
            ui->studentsTable->setItem(row, 4, makeItem(QString::fromStdString(st.instrument)));
            ui->studentsTable->setItem(row, 5, makeItem(QString::fromStdString(st.teacher.surname + " " + st.teacher.initials)));
            studentRowMap.push_back(i);
            ++row;
        }
    }
    ui->studentsTable->setRowCount(row);
    ui->studentsTable->blockSignals(false);

    std::vector<Concerts_entry> list;
    concerts->toVector(list);
    bool useSearchC = concertFilterActive;
    concertList.clear();
    for (const auto& e : list) {
        QString sur = QString::fromStdString(e.fio.surname);
        QString nam = QString::fromStdString(e.fio.name);
        QString pat = QString::fromStdString(e.fio.patronymic);
        QString instrC = QString::fromStdString(e.instrument);
        QString play = QString::fromStdString(e.play);
        QString hall = QString::fromStdString(e.hall);
        QString date = QString::fromStdString(e.date);
        if (useSearchC) {
            if (!cSurname.isEmpty() && !sur.contains(cSurname, Qt::CaseInsensitive))
                continue;
            if (!cName.isEmpty() && !nam.contains(cName, Qt::CaseInsensitive))
                continue;
            if (!cPatronymic.isEmpty() && !pat.contains(cPatronymic, Qt::CaseInsensitive))
                continue;
            if (!cPlay.isEmpty() && !play.contains(cPlay, Qt::CaseInsensitive))
                continue;
            if (!cHall.isEmpty() && !hall.contains(cHall, Qt::CaseInsensitive))
                continue;
            if (cDateEnabled && !cDate.isEmpty() && date != cDate)
                continue;
        }
        concertList.push_back(e);
    }
    ui->concertsTable->blockSignals(true);
    ui->concertsTable->clearContents();
    ui->concertsTable->setRowCount(0);
    ui->concertsTable->setColumnCount(7);
    QStringList headers;
    headers << "Фамилия" << "Имя" << "Отчество" << "Инструмент" << "Пьеса" << "Зал" << "Дата";
    ui->concertsTable->setHorizontalHeaderLabels(headers);
    int count = static_cast<int>(concertList.size());
    for (int i = 0; i < count; ++i) {
        ui->concertsTable->insertRow(i);
        const auto& e = concertList[i];
        auto makeItem = [](const QString& t) {
            QTableWidgetItem* it = new QTableWidgetItem(t);
            it->setTextAlignment(Qt::AlignCenter);
            return it;
        };

        ui->concertsTable->setItem(i, 0, makeItem(QString::fromStdString(e.fio.surname)));
        ui->concertsTable->setItem(i, 1, makeItem(QString::fromStdString(e.fio.name)));
        ui->concertsTable->setItem(i, 2, makeItem(QString::fromStdString(e.fio.patronymic)));
        ui->concertsTable->setItem(i, 3, makeItem(QString::fromStdString(e.instrument)));
        ui->concertsTable->setItem(i, 4, makeItem(QString::fromStdString(e.play)));
        ui->concertsTable->setItem(i, 5, makeItem(QString::fromStdString(e.hall)));
        ui->concertsTable->setItem(i, 6, makeItem(QString::fromStdString(e.date)));
    }
    ui->concertsTable->blockSignals(false);
    updateConcertTree();
    updateReport();
}
