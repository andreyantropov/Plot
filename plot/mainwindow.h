#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <qcustomplot.h>
#include <QStyleFactory>
#include <QTimer>
#include <QTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QCustomPlot *customPlot_1, *customPlot_2; // Объявляем графическое полотно
    QCPBars *bar;
    QCPGraph *graphic; // Объявляем график
    QTimer *tmr;

    void fillComboBox(QComboBox *box, QString query);
    double selectRange(QString query);

    void createPlot_1();
    void createPlot_2();

    void drawGraphic(QVector<double> time, QVector<double> income, int num);
    void drawGraphic_2(QVector<double> time, QVector<double> income, int num);

private slots:
    void slotRangeChanged (const QCPRange &newRange); //Процедура приближения графика

    void slotPlotChanged(int num);

    void slotUpdatePlot();

    void slotDateTimeChanged();
    void slotDateTime2Changed();
};

#endif // MAINWINDOW_H
