#ifndef PAINTAREA_H
#define PAINTAREA_H
#include <QtGui/QGraphicsView>

//************************************************************************************
//  Класс Область Рисования
//  Описание:   представляет собой сцену, отрисовывающую перемещение мышки
//  Применение: для рисования цифр, очистки области, сохранения картинки
//************************************************************************************
class PaintArea : public QGraphicsView
{
    Q_OBJECT
private:
    QGraphicsPathItem *path_item;
    QPainterPath *painter_path;
public:
    // класс исключений
    class UninitializedData {
    public:
        QString description;
        UninitializedData(QString d) : description(d) { }
    };
    PaintArea(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void area_clear();
    ~PaintArea();
};
#endif // PAINTAREA_H
