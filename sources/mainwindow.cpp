#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QPixmap>
#include <QString>
#include <QBuffer>
#include <recognition.h>
#include <processing_image.h>
#include <QTextStream>
#include <QList>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <QtAlgorithms>


#define row 33
#define col 20
#define symbols 10

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    setWindowIcon(QIcon("img/icon.png"));
    QVector<Matrix> weights;
    weights = loadWeightMatrix();
    try {
       ui->setupUi(this);

    }
    catch(ExError in) {
        QMessageBox::information(this, "Ошибка", in.info);
    }
    catch(PaintArea::UninitializedData pa) {
        QMessageBox::information(this, "Ошибка", pa.description);
    }

    number.set_weight_matrix(weights);
    ui->lcdNumber->setAutoFillBackground(true);
    QPalette pal = ui->lcdNumber->palette();
    pal.setColor(QPalette::Normal, QPalette::WindowText, QColor::fromRgb(10,170,50));
    pal.setColor(QPalette::Normal, QPalette::Window, QColor::fromRgb(240,250,240));
    ui->lcdNumber->setPalette(pal);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveWeightMatrix();
    event->accept();
}

//************************************************************************************
//  GroupBox2 "Изображение"
//************************************************************************************
// кнопка "Загрузить"
void MainWindow::on_pushButton_clicked()
{
    IplImage* image = NULL;

    try {
        image = openImage();
    }
    catch(UninitializedData ie) {
        QMessageBox::information(this, "Ошибка", ie.description);
    }
    if (!image) {
        QMessageBox::information(this, "Ошибка", "Не удалось загрузить изображение");
     } else {

        updateNumber(image);

        ui->groupBox->setTitle("Область изображения2");
        ui->label->setVisible(true);
        ui->widget->setVisible(false);
        ui->groupBox_4->setEnabled(false);
    }
}

// кнопка "Нарисовать"
void MainWindow::on_pushButton_4_clicked() {
    ui->groupBox->setTitle("Рисовалка");
    ui->label->setVisible(false);
    ui->widget->setVisible(true);
    ui->groupBox_4->setEnabled(true);
}

//************************************************************************************
//  GroupBox3 "Обучение"
//************************************************************************************
// кнопка "Не верно"
void MainWindow::on_pushButton_2_clicked()
{
    bool ok = 0;
    QString correct_number = QInputDialog::getText(this,
                                                   "Введите правильное число",
                                                   "Правильное число:",
                                                   QLineEdit::Normal, "", &ok);
    if (ok) {
        bool ok2 = false;
        correct_number.toInt(&ok2);
        if (number.size() != correct_number.size() || !ok2 ) {
            QMessageBox::information(this, "Ошибка", "Неправильное число");
            return;
        }
        number.teach(correct_number);
    }

    updateScreen();
}

// кнопка "Тренеровать"
void MainWindow::on_pushButton_3_clicked() {

    SimpleTeacherBuilder teacher;
    TeacherDirector director;
    director.set_teacher(&teacher);
    director.teach("img/");

    number.set_weight_matrix(teacher.get_weight_matrix());

    updateScreen();
}

// кнопка "Генетический алгоритм"
void MainWindow::on_pushButton_9_clicked() {

    GeneticTeacherBuilder teacher;
    TeacherDirector director;
    director.set_teacher(&teacher);
    director.teach("img/");

    number.set_weight_matrix(teacher.get_weight_matrix());

    updateScreen();
}

// кнопка "Стереть память"
void MainWindow::on_pushButton_5_clicked() {
    number.clear_weight_matrix();
    updateScreen();
}

//************************************************************************************
//  GroupBox4 "Рисовалка"
//************************************************************************************

// кнопка "Распознать"
void MainWindow::on_pushButton_6_clicked() {
    QString file_name = "img/temp.jpg";
    QPixmap pix_map = QPixmap::grabWidget(ui->widget);
    QPixmap pix_map2 = pix_map.copy(20, 5, 661, 141);
    IplImage* image = PixmapToIplImage(&pix_map2);
    if (image) {
        updateNumber(image);

        ui->label->setVisible(true);
        ui->widget->setVisible(false);
        ui->groupBox_4->setEnabled(false);
    } else {
        QMessageBox::information(this, "Ошибка",
                                 "Изображение не было инициализированно");
    }
}

// кнопка "Очистить"
void MainWindow::on_pushButton_7_clicked() {
    try {
       ui->widget->area_clear();
    }
    catch(PaintArea::UninitializedData pa) {
        QMessageBox::information(this, "Ошибка", pa.description);
    }
}

// кнопка "сохранить"
void MainWindow::on_pushButton_8_clicked() {
    QString file_name = QFileDialog::getSaveFileName(this, "Сохранить картинку",
                                                     QDir::currentPath(),
                                                     "Картинка (*.jpg)");
    file_name += ".jpg";
    QPixmap pix_map = QPixmap::grabWidget(ui->widget);
    QPixmap pix_map2 = pix_map.copy(20, 5, 661, 141);
    if (!pix_map2.save(file_name)) {
        QMessageBox::information(this, "Картинка не была сохранена",
                                 "Проверьте правильность названия и типа");
    }
}





