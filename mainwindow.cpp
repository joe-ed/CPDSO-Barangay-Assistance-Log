#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "xlsxdocument.h"
#include <QDate>
#include <QStandardPaths>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(new QStandardItemModel(this))
{
    ui->setupUi(this);

    // Setup table
    QStringList headers = {"Date", "Name", "Gender", "Barangay", "Purpose", "Action"};
    model->setHorizontalHeaderLabels(headers);
    ui->historyTable->setModel(model);
    ui->historyTable->horizontalHeader()->setStretchLastSection(true);
    ui->historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Set Excel file path
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    excelFilePath = docPath + "/BarangayAssistanceLog.xlsx";

    loadFromExcel();
    sortTableByDateDescending();

    // Connect signals
    connect(ui->historyTable, &QTableView::doubleClicked, this, &MainWindow::on_historyTable_doubleClicked);
    connect(model, &QStandardItemModel::dataChanged, this, &MainWindow::onHistoryTableEdited);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::on_refreshButton_clicked);

    // Setup file watcher timer
    fileWatcherTimer = new QTimer(this);
    connect(fileWatcherTimer, &QTimer::timeout, this, &MainWindow::checkForExternalUpdates);
    fileWatcherTimer->start(5000); // Check every 5 seconds

    // Store initial modification time
    QFileInfo fileInfo(excelFilePath);
    if (fileInfo.exists()) {
        lastModifiedTime = fileInfo.lastModified();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_submitButton_clicked()
{
    QString name = ui->nameEdit->toPlainText().trimmed();
    QString barangay = ui->barangayEdit->toPlainText().trimmed();
    QString action = ui->actionEdit->toPlainText().trimmed();
    QString gender = ui->genderCombo->currentText();
    QString purpose = ui->purposeCombo->currentText();
    QString date = QDate::currentDate().toString("yyyy-MM-dd");

    // Guard: Do nothing if fields are empty
    if (name.isEmpty() && barangay.isEmpty() && action.isEmpty()) {
        ui->statusbar->showMessage("Nothing to submit — all fields empty.", 3000);
        return;
    }

    QList<QStandardItem*> rowItems;
    rowItems << new QStandardItem(date)
             << new QStandardItem(name)
             << new QStandardItem(gender)
             << new QStandardItem(barangay)
             << new QStandardItem(purpose)
             << new QStandardItem(action);

    model->insertRow(0, rowItems); // Insert at top to maintain newest-first order
    saveToExcel();

    ui->statusbar->showMessage(QString("Record saved. Excel file updated at:\n%1").arg(excelFilePath), 7000);

    ui->nameEdit->clear();
    ui->barangayEdit->clear();
    ui->actionEdit->clear();
    ui->genderCombo->setCurrentIndex(0);
    ui->purposeCombo->setCurrentIndex(0);
}

void MainWindow::on_historyTable_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    ui->historyTable->edit(index);
    connect(model, &QStandardItemModel::itemChanged, this, [=](QStandardItem*){
        saveToExcel();
    });
}

void MainWindow::saveToExcel()
{
    QXlsx::Document xlsx;

    xlsx.write("A1", "Date");
    xlsx.write("B1", "Name");
    xlsx.write("C1", "Gender");
    xlsx.write("D1", "Barangay");
    xlsx.write("E1", "Purpose");
    xlsx.write("F1", "Action");

    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            QStandardItem *item = model->item(row, col);
            if (item)
                xlsx.write(row + 2, col + 1, item->text());
        }
    }

    if (xlsx.saveAs(excelFilePath)) {
        // Update last modified time after saving
        QFileInfo fileInfo(excelFilePath);
        lastModifiedTime = fileInfo.lastModified();
    }
}

void MainWindow::loadFromExcel()
{
    if (!QFile::exists(excelFilePath))
        return;

    QXlsx::Document xlsx(excelFilePath);
    model->removeRows(0, model->rowCount()); // Clear existing data

    int row = 2;
    while (!xlsx.read(row, 1).toString().isEmpty()) {
        QList<QStandardItem*> items;
        for (int col = 1; col <= 6; ++col) {
            items << new QStandardItem(xlsx.read(row, col).toString());
        }
        model->appendRow(items);
        row++;
    }

    // Store last modified time after loading
    QFileInfo fileInfo(excelFilePath);
    lastModifiedTime = fileInfo.lastModified();
}

void MainWindow::onHistoryTableEdited(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    Q_UNUSED(roles)

    ui->statusbar->showMessage(QString("Table updated by user. Excel file updated at:\n%1").arg(excelFilePath), 7000);

    // also update Excel file to keep it in sync
    saveToExcel();
}

void MainWindow::on_refreshButton_clicked()
{
    loadFromExcel();
    sortTableByDateDescending();
    ui->statusbar->showMessage("Data refreshed from Excel file.", 3000);
}

void MainWindow::checkForExternalUpdates()
{
    if (!QFile::exists(excelFilePath))
        return;

    QFileInfo fileInfo(excelFilePath);
    QDateTime currentModifiedTime = fileInfo.lastModified();

    if (currentModifiedTime > lastModifiedTime) {
        loadFromExcel();
        sortTableByDateDescending();
        lastModifiedTime = currentModifiedTime;
        ui->statusbar->showMessage("Data automatically refreshed - external changes detected.", 3000);
    }
}

void MainWindow::sortTableByDateDescending()
{
    model->sort(0, Qt::DescendingOrder); // Sort by date (column 0) in descending order
}