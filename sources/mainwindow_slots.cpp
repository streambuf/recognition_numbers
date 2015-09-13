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
#include <iostream>
#define row 33
#define col 20

//************************************************************************************
//  Реализация  слотов
//************************************************************************************

IplImage* MainWindow::openImage()
{
    QString file_name = QFileDialog::getOpenFileName(this,
                                    "Открыть фал", QDir::currentPath(), "Картинки (*.png *.jpg *.jpeg *.bmp)");
    if(!file_name.size()) {
        throw ExError("Изображение не было выбрано");
    }
    IplImage *image = cvLoadImage(file_name.toAscii().data());
    if (image->imageSize==0) {

        throw ExError("Не удалось загрузить изображение");
    }

    return image;
}
//-------------------------------------------------------------------------------------
// сохранение матрицы весов в файл
void MainWindow::saveWeightMatrix() {
    QFile file;
    QTextStream tstream;
    // сохранение матриц весов в файл
    file.setFileName("w.txt");

    if (!file.open(QIODevice::WriteOnly)) {
        throw ExError("Ошибка при сохранении матрицы весов в файл");
    }

    tstream.setDevice(&file);
    QVector<Matrix> weights = number.get_weight_matrix();

    for (int k = 0; k < 10; ++k) {
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                tstream << weights[k][i][j];
                tstream << ";";
            }
            tstream << "\n";
        }
        tstream << "*";
    }
    file.close();
}
//-------------------------------------------------------------------------------------
// загрузка матрицы весов из файла
QVector<Matrix> MainWindow::loadWeightMatrix() {
    QFile file;
    QString lines;
    QStringList fields;
    QTextStream tstream;

    // чтение матриц весов из файла
    file.setFileName("w.txt");

    if (!file.open(QIODevice::ReadOnly)) {
        throw ExError("Ошибка при открытии файла для загрузки матрицы весов");
    }

    tstream.setDevice(&file);
    lines = tstream.readAll();
    fields = lines.split("*");
    Matrix weight;
    QVector<Matrix> weights;

    for (int k = 0; k < 10; ++k) {
        int p = 0;  // для пропуска символов переноса строки
        QStringList field_matrix = fields[k].split(";");

        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                weight[i][j] = field_matrix[i * col + j].toInt();
             }
            ++p;
        }
        weights.push_back(weight);
    }
    file.close();
    return weights;
}
//-------------------------------------------------------------------------------------

void MainWindow::updateScreen() {
    // вывод распознанного числа
    ui->lcdNumber->display(number.recognition());

    // вывод таблицы взвешенных сумм входных сигналов
    // настройка колонок и строк
    ui->tableWidget->setRowCount(number.size());
    ui->tableWidget->setColumnCount(symbols);
    for (int i = 0; i < symbols; ++i) {
        ui->tableWidget->setColumnWidth(i, 60);
    }

    // настройка заголовков
    QStringList vheader, hheader;
    for (int i = 0; i < number.size(); ++i) {
        vheader << QString::number(i+1) + "-я цифра";
    }
    for (int i = 0; i < symbols; ++i) {
        hheader << QString::number(i);
    }
    ui->tableWidget->setVerticalHeaderLabels(vheader);
    ui->tableWidget->setHorizontalHeaderLabels(hheader);

    // заполнение данными
    QVector< QVector<int> > scaled_signals = number.get_statistics_scaled_signals();
    for (int i = 0; i < number.size(); ++i) {
        int max = 0;
        for (int j = 0; j < symbols; ++j) {
            if (scaled_signals[i][j] > scaled_signals[i][max]) {
                max = j;
            }
            ui->tableWidget->setItem(i, j, new QTableWidgetItem(
                                         QString::number(scaled_signals[i][j])));
        }
        // выделение максимальных сумм цветом
        QLinearGradient grad (0,0,0,ui->tableWidget->rowHeight(0));
        grad.setColorAt(0, QColor::fromRgb(200,250,200));
        grad.setColorAt(1, QColor::fromRgb(50,250,100));
        ui->tableWidget->item(i, max)->setBackground(grad);
    }
}
//-------------------------------------------------------------------------------------
IplImage* MainWindow::PixmapToIplImage(QPixmap *pix) {
    if (!pix) {
        return NULL;
    }
    int width = (pix->toImage()).width();
    int height = (pix->toImage()).height();

    IplImage *img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

    if (!img) {
        return NULL;
    }

    for(int y = 0; y < height; ++y) {
        for(int x=0; x < width; ++x) {
            QRgb color = pix->toImage().pixel(x, y);
            cvSet2D(img, y, x, cvScalar(qBlue(color), qGreen(color), qRed(color), 1));
        }
    }
    return img;
}
//-------------------------------------------------------------------------------------
// загружает контуры в мультиперцептрон и выводит картинку с контурами
void MainWindow::updateNumber(IplImage* image) {
    if (!image) {
        QMessageBox::information(this, "Ошибка",
                                 "Неинициализированно изображение");
        return;
    }
    try {


        ProcessingImageFactory f;
        ProcessingImageClient pic(f);
        pic.apply_filters(image);

        ProcessingImage proc_image(image);
        proc_image.drow_contours();
        QVector<Matrix> contours = proc_image.get_contours_pixels();


        /*
        QTextCursor cursor;
            cursor = ui->textEdit->textCursor();
            ui->textEdit->clear();
            cursor.atStart();
            for (int k = 0; k < contours.size(); ++k) {
            for (int i = 0; i < row; ++i) {
                for (int j = 0; j < col; ++j) {
                    cursor.insertText(QString::number(contours[k][i][j]));
                }
                cursor.insertText("\n");
            }
             cursor.insertText("\n");
            }

        */

        number.update(contours);
    }
    catch(UninitializedData ie) {
        QMessageBox::information(this, "Ошибка", ie.description);
    }
    catch(ExError in) {
        QMessageBox::information(this, "Ошибка", in.info);
        return;
    }

    // Преобразуем IplImage в QImage для вывода
    QImage *q_image = new QImage((uchar*)image->imageData,
                                 image->width,
                                 image->height,
                                QImage::Format_RGB888
                                 );
    if (q_image) {
        // Вывод изображения на экран
        ui->label->setPixmap(QPixmap::fromImage(q_image->rgbSwapped()));
    } else {
        QMessageBox::information(this, "Ошибка",
                                 "Не удалось вывести изображение");
    }
    updateScreen();
}

