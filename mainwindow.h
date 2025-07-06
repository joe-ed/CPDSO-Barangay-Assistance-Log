#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QTimer>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_submitButton_clicked();
    void on_historyTable_doubleClicked(const QModelIndex &index);
    void on_refreshButton_clicked();
    void checkForExternalUpdates();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QString excelFilePath;
    QTimer *fileWatcherTimer;
    QDateTime lastModifiedTime;

    void saveToExcel();
    void loadFromExcel();
    void sortTableByDateDescending();
private slots:
    void onHistoryTableEdited(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

};

#endif // MAINWINDOW_H
