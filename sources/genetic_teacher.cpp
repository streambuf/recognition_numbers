#include "recognition.h"
#include "QDir"
#include "processing_image.h"
#include <QTime>
#include "genetic.h"

void GeneticTeacherBuilder::teach() {
    // for random
    qsrand(static_cast<uint>(QTime::currentTime().msec()));

    QVector< QVector<Matrix> > training_matrix = t->get_training_matrix();

    GA ga = GA(population_size, training_matrix);

    t->set_weight_matrix(ga.getWeightMatrix());
}


void GeneticTeacherBuilder::load_training_matrix() {
    QString dir_name = t->get_directory();
    QVector< QVector<Matrix> > training_matrix;

    for (int i = 0; i < symbols; ++i) {
        // выбираем папку с нужной цифрой
        QDir directory(dir_name + QString::number(i));
        if (directory.dirName()==0) {
            return;
        }
        // фильтр для открытия только изображений формата png/jpg/gif
        QStringList image_filter;
        image_filter << "*.png" << "*.jpg" << "*.gif";
        // список файлов
        QFileInfoList list_files = directory.entryInfoList( image_filter, QDir::Files );
        QVector<Matrix> images_matrix;

        // получение матриц входных сигналов с изображений
        for (int j = 0; j < list_files.size(); ++j) {
            QFileInfo file_info = list_files.at(j);
            QString file_name = file_info.fileName();
            QString path_image = dir_name + QString::number(i) + "/" + file_name;
            IplImage *image = cvLoadImage(path_image.toAscii().data());

            if (image->imageSize == 0) {
                return;
            }

            ProcessingImage proc_image(image);

            QVector<Matrix> input_matrix = proc_image.get_contours_pixels();

            images_matrix.push_back(input_matrix[0]);
        }
        training_matrix.push_back(images_matrix);
    }
    t->set_training_matrix(training_matrix);
}
