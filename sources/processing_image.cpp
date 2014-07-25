#include <processing_image.h>
#include <iostream>
//************************************************************************************
//  Реализация класса обработки изображений
//  Описание:   обработчик изображений, использующий бибилотеку OpenCV
//  Применение: получение и выделение контуров на изображениях
//************************************************************************************
ProcessingImage::ProcessingImage(IplImage *im) {
    if (!im) {
        throw UninitializedData("Uninitialized im "
                                "ProcessingImage::ProcessingImage(IplImage *im)");
    }
    image = im;
    image_gray_smooth_threshold();
    find_contours();
}
//-------------------------------------------------------------------------------------
void ProcessingImage::set_image(IplImage* im) {
    if (!im) {
        throw UninitializedData("Uninitialized im "
                                "ProcessingImage::set_image(IplImage* im)");
    }
    image = im;
    image_gray_smooth_threshold();
    find_contours();
}
//-------------------------------------------------------------------------------------
// нарисовать контуры на изображении
void ProcessingImage::drow_contours() {

    CvSeq* copy_approx_contours = approx_contours;

    for(; copy_approx_contours != 0; copy_approx_contours = copy_approx_contours->h_next) {


        CvRect rect = cvBoundingRect(copy_approx_contours, 0);

        CvPoint point1, point2;
        point1.x = rect.x;
        point2.x = (rect.x+rect.width);
        point1.y = rect.y;
        point2.y = (rect.y+rect.height);

        CvScalar color = CV_RGB( rand()&200, rand()&200, rand()&200 );
        cvRectangle(image, point1, point2, color, 1, 8, 0);
    }


}
//-------------------------------------------------------------------------------------
void ProcessingImage::image_gray_smooth_threshold() {
    if (!image) {
        throw UninitializedData("Uninitialized image "
                                "ProcessingImage::image_gray_smooth_threshold()");
    }
    IplImage *gray_image = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
    if (!gray_image) {
        throw UninitializedData("Uninitialized gray_image "
                                "ProcessingImage::image_gray_smooth_threshold()");
    }
    cvCvtColor(image, gray_image, CV_BGR2GRAY);

    // размытие
    IplImage *smooth_gray_image = cvCloneImage(gray_image);
    if (!smooth_gray_image) {
        throw UninitializedData("Uninitialized smooth_gray_image "
                                "ProcessingImage::image_gray_smooth_threshold()");
    }
    cvSmooth(gray_image, smooth_gray_image, CV_GAUSSIAN, 3);

    // порог
    CvScalar avg;       // среднее значение для массива, независимо от канала
    CvScalar avgStd;    // среднее отклонение для массива, независимо от канала
    cvAvgSdv(smooth_gray_image, &avg, &avgStd, NULL);

    threshold_smooth_gray_image = cvCloneImage(gray_image);
    if (!threshold_smooth_gray_image) {
        throw UninitializedData("Uninitialized threshold_smooth_gray_image "
                                "ProcessingImage::image_gray_smooth_threshold()");
    }
    cvThreshold(smooth_gray_image, threshold_smooth_gray_image,
                (int)avg.val[0] - 7 * (int)(avgStd.val[0] / 8), 255, CV_THRESH_BINARY_INV);
                //128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
}
//-------------------------------------------------------------------------------------
void ProcessingImage::find_contours() {

    if (!threshold_smooth_gray_image) {
        throw UninitializedData("Uninitialized threshold_smooth_gray_image "
                                "ProcessingImage::find_contours()");
    }
    // морфологические преобразования
    // увеличение темных областей
    IplImage *erode_image = cvCloneImage(threshold_smooth_gray_image);
    if (!erode_image) {
        throw UninitializedData("Uninitialized erode_image "
                                "ProcessingImage::find_contours()");
    }
    cvErode(threshold_smooth_gray_image, erode_image, NULL,1);  // размывание (операция сужения)

    // увеличение светлых областей
    IplImage *dilate_image = cvCloneImage(erode_image);
    if (!dilate_image) {
        throw UninitializedData("Uninitialized dilate_image "
                                "ProcessingImage::find_contours()");
    }
    cvDilate(erode_image, dilate_image, NULL,1); // растягивание (операция расширения)

    IplImage *prepared_image = cvCloneImage(dilate_image);
    if (!prepared_image) {
        throw UninitializedData("Uninitialized prepared_image "
                                "ProcessingImage::find_contours()");
    }
    //Init variables for countours
    CvSeq* first_contour = 0; // указатель на первый элемент последовательности, содержащей
                              // данные найденных контуров

    // хранилище памяти для хранения данных найденных контуров
    CvMemStorage* storage = cvCreateMemStorage(0);
    if (!storage) {
        throw UninitializedData("Uninitialized storage "
                                "ProcessingImage::find_contours()");
    }

    // поиск контуров
    cvFindContours(prepared_image, storage, &first_contour, sizeof(CvContour),
                   CV_RETR_EXTERNAL,        // найти только крайние внешние контуры
                   CV_CHAIN_APPROX_SIMPLE,  // сжать горизонтальные, вертикальные
                   // и диагональные сегменты и оставить только их конечные точки
                   cvPoint(0, 0));
    // сглаживание контуров

    approx_contours = cvApproxPoly(first_contour, sizeof(CvContour), storage,
                              CV_POLY_APPROX_DP,    // метод аппроксимации Дугласа-Пейкера
                              1,                    // желательная точность приближения
                              1);                   // аппроксимация для всех последовательн.
    if (!approx_contours) {
        throw UninitializedData("Uninitialized approx_contours "
                                "ProcessingImage::find_contours()");
    }

}
//-------------------------------------------------------------------------------------
// получение матриц контуров (символов)
QVector<Matrix> ProcessingImage::get_contours_pixels() {
    if (!approx_contours) {
        throw UninitializedData("Uninitialized approx_contours "
                                "ProcessingImage::get_contours_pixels()");
    }
    CvSeq* copy_approx_contours = approx_contours;

    CvMat data;
    int N = 0;
    for(N = 0; copy_approx_contours != 0; copy_approx_contours = copy_approx_contours->h_next) {

        CvRect rect = cvBoundingRect(copy_approx_contours, 0);

        if (rect.width < 675)

            ++N;
    }

    CvRect detected_contour[N];

    for (int i = 0; approx_contours != 0; approx_contours = approx_contours->h_next) {

        CvRect rect = cvBoundingRect(approx_contours, 0);

        if (rect.width < 675) {

            detected_contour[i] = rect;
            ++i;
        }

    }

    //Сортируем наши данные (сортировка вставками)

    CvRect temp;
    for (int i = 0; i < N - 1; ++i) {
        for(int j = i; j >= 0 && detected_contour[j].x > detected_contour[j + 1].x; --j) {

            temp = detected_contour[j];
            detected_contour[j] = detected_contour[j + 1];
            detected_contour[j + 1] = temp;
        }
    }

    Matrix input[N];

    for(int t = 0; t < N; ++t)
    {
        // получаем прямоугольный контур

        cvGetSubRect(threshold_smooth_gray_image, &data, detected_contour[t]);
        cv::Mat convData = cv::Mat(&data);
        cv::Mat scaledConvData;
        // изменяем размер
        cv::resize(convData, scaledConvData, cv::Size(col, row), 0, 0, CV_INTER_NN);
        data = scaledConvData;

        Matrix countour_pixels;
        // получаем пиксели контура
        for (int i = 0; i < data.rows; ++i) {
            uchar* ptr = (uchar*) (data.data.ptr + i * data.step);
            if (!ptr) {
                throw UninitializedData("Uninitialized ptr"
                                        "ProcessingImage::get_contours_pixels()");
            }
            for (int j = 0; j < data.cols; ++j) {
                if (ptr[j] == 255)
                    countour_pixels[i][j] = 1;
                else
                    countour_pixels[i][j] = 0;
            }
        }
        input[t] = countour_pixels;

    }

    QVector<Matrix> input_list;

    for (int i = 0; i < N; ++i) {
        input_list.push_back(input[i]);
    }

    return input_list;
}



void GrayFilterCommand::apply_filter(IplImage* image) {
    if (!image) {
        throw UninitializedData("Uninitialized image "
                                "GrayFilterCommand::apply_filter(IplImage*)");
    }
    IplImage *gray_image = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
    if (!gray_image) {
        throw UninitializedData("Uninitialized gray_image "
                                "GrayFilterCommand::apply_filter(IplImage*)");
    }
    cvCvtColor(image, gray_image, CV_BGR2GRAY);
    image = cvCloneImage(gray_image);
}

void SmoothFilterCommand::apply_filter(IplImage* gray_image) {
    // размытие
    IplImage *smooth_gray_image = cvCloneImage(gray_image);
    if (!smooth_gray_image) {
        throw UninitializedData("Uninitialized smooth_gray_image "
                                "SmoothFilterCommand::apply_filter(IplImage*)");
    }
    cvSmooth(gray_image, smooth_gray_image, CV_GAUSSIAN, 3);
    gray_image = cvCloneImage(smooth_gray_image);
}

void ThresholdFilterCommand::apply_filter(IplImage* smooth_gray_image) {
    // порог
    CvScalar avg;       // среднее значение для массива, независимо от канала
    CvScalar avgStd;    // среднее отклонение для массива, независимо от канала
    cvAvgSdv(smooth_gray_image, &avg, &avgStd, NULL);

    IplImage* threshold_smooth_gray_image = cvCloneImage(smooth_gray_image);
    if (!threshold_smooth_gray_image) {
        throw UninitializedData("Uninitialized threshold_smooth_gray_image "
                                "ThresholdFilterCommand::apply_filter(IplImage*)");
    }
    cvThreshold(smooth_gray_image, threshold_smooth_gray_image,
                (int)avg.val[0] - 7 * (int)(avgStd.val[0] / 8), 255, CV_THRESH_BINARY_INV);
                //128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    smooth_gray_image = cvCloneImage(threshold_smooth_gray_image);
}
