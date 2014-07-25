#include "paintarea.h"
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QPainterPath>
#include <QtGui/QPen>
#include <QtGui/QColor>
#include <QtGui/QMouseEvent>
#include <QtCore/QPointF>
#include <QGraphicsScene>

//************************************************************************************
//  Реализация класса Области Рисования
//  Описание:   представляет собой сцену, отрисовывающую перемещение мышки
//  Применение: для рисования цифр, очистки области, сохранения картинки
//************************************************************************************
PaintArea::PaintArea(QWidget *parent) : QGraphicsView(parent) {
    painter_path = new QPainterPath();
    if (!painter_path) {
        throw UninitializedData("Uninitialized painter_path "
                                "PaintArea::PaintArea(QWidget *parent)");
    }
    QGraphicsScene *scene = new QGraphicsScene(this);
    if (!scene) {
        throw UninitializedData("Uninitialized scene "
                                "PaintArea::PaintArea(QWidget *parent)");
    }
    scene->setSceneRect(0, 0, 100, 130);
    setScene(scene);
    // сглаживаем грани примитивов
    setRenderHint(QPainter::Antialiasing, true);
    path_item = scene->addPath(*painter_path, QPen(QColor::fromRgb(10, 170, 50), 5));
    if (!path_item) {
        throw UninitializedData("Uninitialized path_item "
                                "PaintArea::PaintArea(QWidget *parent)");
    }

}
//-------------------------------------------------------------------------------------
PaintArea::~PaintArea() {
    delete path_item;
    delete painter_path;
}
//-------------------------------------------------------------------------------------
void PaintArea::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        painter_path->lineTo(event->posF());
        // сохраняем координаты
        path_item->setPath(mapToScene(*painter_path));
    }
}
//-------------------------------------------------------------------------------------
void PaintArea::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {            // создаем линию
        painter_path->moveTo(event->posF());
    } else if (event->buttons() & Qt::RightButton) {    // очищаем
        delete painter_path;
        painter_path = new QPainterPath();
        if (!painter_path) {
            throw UninitializedData("Uninitialized painter_path "
                                    "PaintArea::mousePressEvent(QMouseEvent *event)");
        }
        path_item->setPath(mapToScene(*painter_path));
    }
}
//-------------------------------------------------------------------------------------
void PaintArea::area_clear() {
    delete painter_path;
    painter_path = new QPainterPath();
    if (!painter_path) {
        throw UninitializedData("Uninitialized painter_path "
                                "PaintArea::area_clear()");
    }
    path_item->setPath(mapToScene(*painter_path));
}
