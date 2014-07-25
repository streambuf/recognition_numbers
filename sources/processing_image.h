#ifndef PROCESSING_IMAGE_H
#define PROCESSING_IMAGE_H
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QPixmap>
#include <recognition.h>

//************************************************************************************
//  Класс обработки изображений
//  Описание:   обработчик изображений, использующий бибилотеку OpenCV
//  Применение: получение и выделение контуров на изображениях
//************************************************************************************
class ProcessingImage {
private:
    IplImage* image;                        // исходное изображение
    IplImage* threshold_smooth_gray_image;  // изображение в градациях серого
    CvSeq* approx_contours;                 // контуры
    void image_gray_smooth_threshold();
    void find_contours();
public:
    ProcessingImage() {}
    ProcessingImage(IplImage* im);
    ~ProcessingImage() {}

    void set_image(IplImage* im);
    void drow_contours();
    QVector<Matrix> get_contours_pixels();

};


class FilterCommand {
public:
    virtual void apply_filter(IplImage* im) = 0;
};

class GrayFilterCommand : public FilterCommand {
public:
    void apply_filter(IplImage* im);
};

class SmoothFilterCommand : public FilterCommand {
public:
    void apply_filter(IplImage* im);
};

class ThresholdFilterCommand : public FilterCommand {
public:
    void apply_filter(IplImage* im);
};

class FiltersInvoker {
private:
    QVector<FilterCommand*> filters;
public:
    FiltersInvoker() : filters(0) {}

    FiltersInvoker(QVector<FilterCommand*> flt) : filters(flt) {
    }

    void add_filter(FilterCommand *fcd) {
        filters.push_back(fcd);
    }

    void apply_filters(IplImage *im) {
        QVector<FilterCommand*>::iterator it = filters.begin();
        while (it != filters.end()) {
            (*it++)->apply_filter(im);
        }
    }
};

class AbstractFilters {
public:
    virtual void apply_filter(IplImage* im) = 0;
};

class GraySmoothThresholdFilter : public AbstractFilters {
public:
    void apply_filter(IplImage *im) {
        FiltersInvoker fi;

        fi.add_filter(new GrayFilterCommand);
        fi.add_filter(new SmoothFilterCommand);
        fi.add_filter(new ThresholdFilterCommand);

        fi.apply_filters(im);
    }
};

class ProcessingImageAbstractFactory {
public:
    virtual AbstractFilters* create_filters() const = 0;
};

class ProcessingImageFactory : public ProcessingImageAbstractFactory{
public:
    virtual AbstractFilters* create_filters() const {
        return new GraySmoothThresholdFilter();
    }
};

class ProcessingImageClient {
private:
    AbstractFilters* filters;
public:
    ProcessingImageClient(const ProcessingImageAbstractFactory& f) {
        filters = f.create_filters();
    }

    void apply_filters(IplImage* im) {
        filters->apply_filter(im);
    }
};


// класс исключений
class UninitializedData {
public:
    QString description;
    UninitializedData(QString d) {
        description = d;
    }
};

#endif // PROCESSING_IMAGE_H
