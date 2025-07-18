#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QApplication>
#include <QMenu>
#include <QActionGroup>
#include <QSettings>
#include "theme.h"
#include <vector>
#include "student_entry.h"

MainWindow::MainWindow(HashTable* studentsTable, AVLTree* concertTree,
                       const std::vector<Students_entry>& rawStudents,
                       const QString& studFile, const QString& concFile,
                       QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), students(studentsTable), concerts(concertTree),
      studentList(rawStudents), studentFile(studFile), concertFile(concFile)
{
    ui->setupUi(this);

    QMenu* settingsMenu = menuBar()->addMenu("\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270");
    QActionGroup* themeGroup = new QActionGroup(this);
    QAction* darkAct = settingsMenu->addAction("\320\242\321\217\320\274\320\275\320\260\321\217");
    darkAct->setCheckable(true);
    darkAct->setActionGroup(themeGroup);
    QAction* madAct = settingsMenu->addAction("\320\234\320\260\320\264\320\260\320\263\320\260\321\201\320\272\320\260\321\200");
    madAct->setCheckable(true);
    madAct->setActionGroup(themeGroup);
    QAction* sonicAct = settingsMenu->addAction("\320\241\320\276\320\275\320\270\320\272");
    sonicAct->setCheckable(true);
    sonicAct->setActionGroup(themeGroup);
    QAction* gojoAct = settingsMenu->addAction("\320\223\320\276\320\264\320\266\320\276 \320\241\320\260\321\202\320\276\321\200\321\203");
    gojoAct->setCheckable(true);
    gojoAct->setActionGroup(themeGroup);
    QAction* sansAct = settingsMenu->addAction("\320\241\320\260\320\275\321\201");
    sansAct->setCheckable(true);
    sansAct->setActionGroup(themeGroup);
    QSettings set;
    Theme current = themeFromString(set.value("theme", "dark").toString());
    switch (current) {
    case Theme::Madagascar: madAct->setChecked(true); break;
    case Theme::Sonic: sonicAct->setChecked(true); break;
    case Theme::GojoSatoru: gojoAct->setChecked(true); break;
    case Theme::Sans: sansAct->setChecked(true); break;
    default: darkAct->setChecked(true); break;
    }
    auto apply = [](Theme t){
        applyTheme(t, *qApp);
        QSettings s; s.setValue("theme", themeToString(t));
    };
    connect(darkAct, &QAction::triggered, this, [=]{ apply(Theme::Dark); });
    connect(madAct, &QAction::triggered, this, [=]{ apply(Theme::Madagascar); });
    connect(sonicAct, &QAction::triggered, this, [=]{ apply(Theme::Sonic); });
    connect(gojoAct, &QAction::triggered, this, [=]{ apply(Theme::GojoSatoru); });
    connect(sansAct, &QAction::triggered, this, [=]{ apply(Theme::Sans); });
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
    ui->studentsVectorTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->studentsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->concertsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->reportTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->mainSplitter->setStretchFactor(0, 1);
    ui->mainSplitter->setStretchFactor(1, 1);
    ui->concertsSplitter->setStretchFactor(0, 1);
    ui->concertsSplitter->setStretchFactor(1, 1);
    ui->verticalLayout->setStretch(0, 0);
    ui->verticalLayout->setStretch(1, 1);
    refreshTables();
    updateConcertTree();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::refreshTables()
{
    ui->clearStudentSearchButton->setVisible(studentFilterActive);
    ui->clearConcertSearchButton->setVisible(concertFilterActive);
    ui->studentsVectorTable->clearContents();
    ui->studentsVectorTable->setRowCount(0);
    ui->studentsVectorTable->setColumnCount(5);
    QStringList vecHeaders;
    vecHeaders << "Фамилия" << "Имя" << "Отчество" << "Инструмент" << "Учитель";
    ui->studentsVectorTable->setHorizontalHeaderLabels(vecHeaders);

    auto makeItemRO = [](const QString& t) {
        QTableWidgetItem* it = new QTableWidgetItem(t);
        it->setTextAlignment(Qt::AlignCenter);
        it->setFlags(it->flags() & ~Qt::ItemIsEditable);
        return it;
    };

    bool useSearchVec = studentFilterActive;
    int vrow = 0;
    for (int i = 0; i < static_cast<int>(studentList.size()); ++i) {
        const auto& st = studentList[i];
        QString sur = QString::fromStdString(st.fio.surname);
        QString nam = QString::fromStdString(st.fio.name);
        QString pat = QString::fromStdString(st.fio.patronymic);
        QString instr = QString::fromStdString(st.instrument);
        QString teacher = QString::fromStdString(st.teacher.surname + " " + st.teacher.initials);
        if (useSearchVec) {
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
        ui->studentsVectorTable->insertRow(vrow);
        QTableWidgetItem* vh = new QTableWidgetItem(QString::number(i));
        vh->setTextAlignment(Qt::AlignCenter);
        ui->studentsVectorTable->setVerticalHeaderItem(vrow, vh);
        ui->studentsVectorTable->setItem(vrow, 0, makeItemRO(sur));
        ui->studentsVectorTable->setItem(vrow, 1, makeItemRO(nam));
        ui->studentsVectorTable->setItem(vrow, 2, makeItemRO(pat));
        ui->studentsVectorTable->setItem(vrow, 3, makeItemRO(instr));
        ui->studentsVectorTable->setItem(vrow, 4, makeItemRO(teacher));
        ++vrow;
    }
    ui->studentsVectorTable->setRowCount(vrow);

    ui->studentsTable->blockSignals(true);
    ui->studentsTable->clearContents();
    ui->studentsTable->setRowCount(students->getFullSize());
    ui->studentsTable->setColumnCount(7);
    QStringList studentHeaders;
    studentHeaders << "Хэш" << "Фамилия" << "Имя" << "Отчество" << "Инструмент" << "Учитель" << "Статус";
    ui->studentsTable->setHorizontalHeaderLabels(studentHeaders);
    double loadFactor = static_cast<double>(students->getSize()) / students->getFullSize();
    ui->tableInfoLabel->setText(QString("Размер таблицы: %1, заполненность: %2%")
        .arg(students->getFullSize())
        .arg(loadFactor * 100.0, 0, 'f', 2));
    studentRowMap.clear();
    int full = students->getFullSize();
    for (int i = 0; i < full; ++i) {
        QTableWidgetItem* vh = new QTableWidgetItem(QString::number(i));
        vh->setTextAlignment(Qt::AlignCenter);
        ui->studentsTable->setVerticalHeaderItem(i, vh);

        auto makeItem = [](const QString& t, bool editable) {
            QTableWidgetItem* it = new QTableWidgetItem(t);
            it->setTextAlignment(Qt::AlignCenter);
            if (!editable)
                it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            return it;
        };

        if (students->isOccupied(i)) {
            const auto& st = students->getEntry(i);
            int h = students->calculateKey(st.fio, st.instrument);
            ui->studentsTable->setItem(i, 0, makeItem(QString::number(h), false));
            ui->studentsTable->setItem(i, 1, makeItem(QString::fromStdString(st.fio.surname), true));
            ui->studentsTable->setItem(i, 2, makeItem(QString::fromStdString(st.fio.name), true));
            ui->studentsTable->setItem(i, 3, makeItem(QString::fromStdString(st.fio.patronymic), true));
            ui->studentsTable->setItem(i, 4, makeItem(QString::fromStdString(st.instrument), true));
            ui->studentsTable->setItem(i, 5, makeItem(QString::fromStdString(st.teacher.surname + " " + st.teacher.initials), true));
            ui->studentsTable->setItem(i, 6, makeItem(QString::number(students->statusAt(i)), false));
            studentRowMap.push_back(i);
        } else {
            for (int c = 0; c < 7; ++c)
                ui->studentsTable->setItem(i, c, makeItem("", false));
            ui->studentsTable->setItem(i, 6, makeItem(QString::number(students->statusAt(i)), false));
            studentRowMap.push_back(-1);
        }
    }
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
