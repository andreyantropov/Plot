//Добавить выбор времени обновления графика

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    //Подключаем базу данных с ТЕСТОВЫМИ ДАННЫМИ
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath() + "/plot/plot.db");

    //Выбираем параметр в БД
    fillComboBox(ui->comboBox, "SELECT t_name FROM bench");

    //Устанавливаем временной промежуток
    ui->dateTimeEdit->setDateTime(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() - 86400));
    ui->dateTimeEdit_2->setDateTime(QDateTime::currentDateTime());

    tmr = new QTimer();
    tmr->setInterval(1000*60*5);

    //Устанавливаем стили графиков
    createPlot_1();
    createPlot_2();

    //СИГНАЛЫ-СЛОТЫ
    //Отслеживаем изменения временного диапазона
    connect(customPlot_1->xAxis, SIGNAL(rangeChanged(QCPRange)),
            this, SLOT(slotRangeChanged(QCPRange)));
    //Отслеживаем выбранный параметр
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotPlotChanged(int)));
    //Обновляем график по истечении счетчика
    connect(tmr, SIGNAL(timeout()),
            this, SLOT(slotUpdatePlot()));
    //Проверяем границы первого DateEdit
    connect(ui->dateTimeEdit, SIGNAL(dateTimeChanged(QDateTime)),
            this, SLOT(slotDateTimeChanged()));
    //Проверяем границы второго DateEdit
    connect(ui->dateTimeEdit_2, SIGNAL(dateTimeChanged(QDateTime)),
            this, SLOT(slotDateTime2Changed()));

    tmr->start();
    slotPlotChanged(0);
}

/*Процедура, отрисовывающая первый график*/
void MainWindow::createPlot_1()
{
    customPlot_1 = new QCustomPlot(); //Инициализируем графическое полотно
    ui->plotGrid_1->addWidget(customPlot_1,0,0,1,1);  //Устанавливаем customPlot_1 в окно приложения

    //Настройки графика
    //Включаем взаимодействие удаления/приближения
    customPlot_1->setInteraction(QCP::iRangeZoom,true);
    //Включаем взаимодействие перетаскивания графика
    customPlot_1->setInteraction(QCP::iRangeDrag, true);
    //Включаем перетаскивание только по горизонтальной оси
    customPlot_1->axisRect()->setRangeDrag(Qt::Horizontal);
    //Включаем удаление/приближение только по горизонтальной оси
    customPlot_1->axisRect()->setRangeZoom(Qt::Horizontal);
    //Подпись координат по Оси X в качестве даты и времени
    customPlot_1->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    //Устанавливаем формат даты и времени
    customPlot_1->xAxis->setDateTimeFormat("hh:mm");

    //Настраиваем шрифт по осям координат
    customPlot_1->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    customPlot_1->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    //Автоматическое масштабирование тиков по Оси X
    customPlot_1->xAxis->setAutoTickStep(true);

    //Делаем видимыми оси X, Y, но отключаем тики
    customPlot_1->xAxis2->setVisible(true);
    customPlot_1->yAxis2->setVisible(true);
    customPlot_1->xAxis2->setTicks(false);
    customPlot_1->yAxis2->setTicks(false);
    customPlot_1->xAxis2->setTickLabels(false);
    customPlot_1->yAxis2->setTickLabels(false);

    //Красный цвет подписей тиков по Оси Y
    customPlot_1->yAxis->setTickLabelColor(QColor(Qt::red));
    //Включаем Легенду графика
    customPlot_1->legend->setVisible(true);

    //Устанавливаем Легенду в левый верхний угол графика
    customPlot_1->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

    //Инициализируем график и привязываем его к Осям
    graphic = new QCPGraph(customPlot_1->xAxis, customPlot_1->yAxis);
    //Устанавливаем график на полотно
    customPlot_1->addPlottable(graphic);
    //Устанавливаем наименование графика
    graphic->setName("Какое-то значение");
    //Устанавливаем цвет графика
    graphic->setPen(QPen(QColor(Qt::red)));
    //Отключаем сглаживание
    graphic->setAntialiased(false);
    //График в виде линии
    graphic->setLineStyle(QCPGraph::lsLine);
}

/*Процедура, отрисовывающая второй график*/
void MainWindow::createPlot_2()
{
    customPlot_2 = new QCustomPlot(this);
    ui->plotGrid_2->addWidget(customPlot_2);

    bar = new QCPBars(customPlot_2->xAxis, customPlot_2->yAxis);
    customPlot_2->addPlottable(bar);

    //Установка цвета
    QPen pen;
    //Толщина контура столбца
    pen.setWidthF(2.0);
    //Легенда
    bar->setName(QString::fromUtf8("Гистограмма"));
    //Цвет контура столбца
    pen.setColor(QColor(50, 50, 100));
    bar->setPen(pen);

    //Цвет столбца
    bar->setBrush(QColor(250, 0, 0));

    //Включаем взаимодействие удаления/приближения
    customPlot_2->setInteraction(QCP::iRangeZoom,true);
    //Включаем взаимодействие перетаскивания графика
    customPlot_2->setInteraction(QCP::iRangeDrag, true);
    //Включаем перетаскивание только по горизонтальной оси
    customPlot_2->axisRect()->setRangeDrag(Qt::Horizontal);
    //Включаем удаление/приближение только по горизонтальной оси
    customPlot_2->axisRect()->setRangeZoom(Qt::Horizontal);

    //Автоматическое масштабирование тиков по Оси X
    customPlot_2->xAxis->setAutoTickStep(true);

    //Делаем видимыми оси X, Y, но отключаем тики
    customPlot_2->xAxis2->setVisible(true);
    customPlot_2->yAxis2->setVisible(true);
    customPlot_2->xAxis2->setTicks(false);
    customPlot_2->yAxis2->setTicks(false);
    customPlot_2->xAxis2->setTickLabels(false);
    customPlot_2->yAxis2->setTickLabels(false);

    //Красный цвет подписей тиков по Оси Y
    customPlot_2->yAxis->setTickLabelColor(QColor(Qt::red));
    //Включаем Легенду графика
    customPlot_2->legend->setVisible(true);

    //Устанавливаем Легенду в левый верхний угол графика
    customPlot_2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

    //Устанавливаем график на полотно
    customPlot_2->addPlottable(bar);
}

/*Процедура, заполняющая комбо-бокс с отслеживаемым параметром
из базы данных*/
void MainWindow::fillComboBox(QComboBox *box, QString query)
{
    if(db.open())
    {
        QSqlQuery qr;
        qr.exec(query);
        while(qr.next())
            box->addItem(qr.value(0).toString());
        db.close();
    }
    else
        qDebug() << "Can't open database";
}

/*Процедура, извлекающая диапазон значений параметра для оси Y*/
double MainWindow::selectRange(QString query)
{
    if(db.open())
    {
        QSqlQuery qr;
        double result;
        qr.exec(query);
        while(qr.next())
            result = qr.value(0).toDouble();
        db.close();
        return result;
    }
    else
    {
        qDebug() << "Can't open database";
        return 0;
    }
}

/*Процедура, генерирующая случайные значения показателей датчика
 и вызывающая процедуру отрисовки по заданным параметрам.
Значения ON/OFF обозначают включение/выключение датчика.*/
void MainWindow::slotPlotChanged(int num)
{
    double now = QDateTime::currentDateTime().toTime_t();
    //Объявляем вектора времени и показаний
    QVector<double> time(800), income(800);
    QVector<QString> stopDrawing(800);
    stopDrawing[0] = "on";
    stopDrawing[400] = "off";
    stopDrawing[650] = "on";

    for(int i = 0; i < 800; i++)
        time[i] = now - 3600*i;

    //Заполняем график значениями
    for (int i = 0; i < 800; i++)
    {
        if(stopDrawing[i] != "off")
            income[i] = qrand() % 50;
        else
            while((i < 800) && (stopDrawing[i] != "on"))
                i++;
    }
    drawGraphic(time, income, num);
    drawGraphic_2(time, income, num);
}

/*Процедура, отрисовывающая первый график на полотне*/
void MainWindow::drawGraphic(QVector<double> time, QVector<double> income, int num)
{
    graphic->setData(time, income); //Устанавливаем данные
    customPlot_1->rescaleAxes(); //Масштабируем график по данным
    customPlot_1->yAxis->setRange(selectRange("SELECT n_min FROM params WHERE id = '"+QString::number(num + 1)+"'"), selectRange("SELECT n_max FROM params WHERE id = '"+QString::number(num + 1)+"'"));
    customPlot_1->xAxis->setRange(ui->dateTimeEdit->dateTime().toTime_t(), ui->dateTimeEdit_2->dateTime().toTime_t());
    customPlot_1->replot(); //Отрисовываем график
}

/*Процедура, отрисовывающая второй график на полотне*/
void MainWindow::drawGraphic_2(QVector<double> time, QVector<double> income, int num)
{
    //Установки значений оси X
    QVector<double> ticks;
    for(int i = 0; i < time.size()/10; i++)
        ticks << i;

    //Данные
    QVector<double> barData(time.size()/10);
    for(int i = 0; i < ticks.size(); i++)
        for(int j = i*10; j < i*10 + 10; j++)
            barData[i] += income[j];

    bar->setData(ticks, barData);
    //Масштабируем график по данным
    customPlot_2->rescaleAxes();
    customPlot_2->yAxis->setRange(selectRange("SELECT n_min FROM params WHERE id = '"+QString::number(num + 1)+"'"), 400);
    customPlot_2->xAxis->setRange(ticks.size() - 10, ticks.size());
    //Отрисовываем график
    customPlot_2->replot();
}

/*Процедура, отслеживающая масштабирование графика.
Изменяет значения на оси X на год, месяц, день, часы:минуты
в зависимости от масштаба*/
void MainWindow::slotRangeChanged(const QCPRange &newRange)
{
    if(newRange.size() < 3600)
        customPlot_1->xAxis->setDateTimeFormat("hh:mm:ss");
    else if(newRange.size() < 86400)
         customPlot_1->xAxis->setDateTimeFormat("hh:mm");
    else if((newRange.size() >= 86400) && (newRange.size() < 2592000))
        customPlot_1->xAxis->setDateTimeFormat("dd MMM");
    else if((newRange.size() >= 2592000) && (newRange.size() < 93312000))
         customPlot_1->xAxis->setDateTimeFormat("MMM yyyy");
    else
         customPlot_1->xAxis->setDateTimeFormat("yyyy");
}

/*Процедура, отслеживающая изменения комбо-бокса с параметрами*/
void MainWindow::slotUpdatePlot()
{
    slotPlotChanged(ui->comboBox->currentIndex());
}

/*Процедура, отслеживающая изменения первого поля QDateEdit.
Не позволяет установить значение, превышающее значение второго
QDateEdit*/
void MainWindow::slotDateTimeChanged()
{
    if(ui->dateTimeEdit_2->dateTime().toTime_t() <= ui->dateTimeEdit->dateTime().toTime_t())
        ui->dateTimeEdit->setDateTime(QDateTime::fromTime_t(ui->dateTimeEdit_2->dateTime().toTime_t() - 60));
    slotUpdatePlot();
}

/*Процедура, отслеживающая изменения второго поля QDateEdit
Не позволяет установить значение, превышающее значение первого
QDateEdit*/
void MainWindow::slotDateTime2Changed()
{
    if(ui->dateTimeEdit_2->dateTime().toTime_t() <= ui->dateTimeEdit->dateTime().toTime_t())
        ui->dateTimeEdit_2->setDateTime(QDateTime::fromTime_t(ui->dateTimeEdit->dateTime().toTime_t() + 60));
    slotUpdatePlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}
