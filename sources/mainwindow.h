#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv/cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>
#include <QFileDialog>
#include <QCloseEvent>
#include <tr1/array>
#include <recognition.h>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    class ExError {
    public:
        QString info;
        ExError(QString in) {
            info = in;
        }
    };

protected:
    void closeEvent(QCloseEvent *e);

    MultiPerceptron number;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    IplImage* openImage();
    void saveWeightMatrix();
    QVector<Matrix> loadWeightMatrix();
    IplImage* PixmapToIplImage(QPixmap *pix);
    void updateScreen();
    void updateNumber(IplImage* image);
    
private:
    Ui::MainWindow *ui;
};



#endif // MAINWINDOW_H
