#include "recognition.h"
#include "QDir"
#include "processing_image.h"

//************************************************************************************
//  Реализация класса Neyron
//  Описание:   представляет собой взвешенный сумматор
//  Применение: каждый объект класса отвечает за свой символ
//************************************************************************************
// конструктор по умолчанию
Neyron::Neyron() {
   sum_scaled_signals = 0;
   for (int i = 0; i < row; ++i) {
       for (int j = 0; j < col; ++j) {
           weight_matrix[i][j] = 0;
           scaled_matrix[i][j] = 0;
       }
   }
}
//-------------------------------------------------------------------------------------
// получение взвешенной суммы входных сигналов
int Neyron::count_sum_scaled_signals(Matrix input_matrix) {
    // взвешиваем входные сигналы
    for (int x = 0; x < row; x++)  {
        for (int y = 0; y < col; y++) {
            scaled_matrix[x][y] = input_matrix[x][y] * weight_matrix[x][y];
        }
    }
    sum_scaled_signals = 0;
    // суммируем взвешенные сигналы
    for (int x = 0; x < row; x++) {
        for (int y = 0; y < col; y++) {
            sum_scaled_signals += scaled_matrix[x][y];
        }
    }
    return sum_scaled_signals;
}
//-------------------------------------------------------------------------------------
// установить матрицу весов
void Neyron::set_weight_matrix(Matrix w) {
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            weight_matrix[i][j] = w[i][j];
        }
    }
}
//-------------------------------------------------------------------------------------
// уменьшить вес синапсов
void Neyron::dec_weight(Matrix input_matrix) {
    for (int x = 0; x < row; x++) {
        for (int y = 0; y < col; y++) {
            weight_matrix[x][y] -= input_matrix[x][y];
        }
    }
}
//-------------------------------------------------------------------------------------
// увеличить вес синапсов
void Neyron::inc_weight(Matrix input_matrix) {
    for (int x = 0; x < row; x++) {
        for (int y = 0; y < col; y++) {
            weight_matrix[x][y] += input_matrix[x][y];
        }
    }
}
//-------------------------------------------------------------------------------------
// распознавание символа - возвращает индекс
bool Neyron::recognition() {
    return (sum_scaled_signals >= limit);
}


//************************************************************************************
//  Реализация класса Perceptron
//  Описание:   представляет собой сеть нейронов
//  Применение: объект класса способен распознавать символ
//************************************************************************************
// конструктор с одним параметром (матрица входных сигналов)
Perceptron::Perceptron(Matrix in) {
    input_matrix = in;
}
//-------------------------------------------------------------------------------------
// конструктор с двумя параметрами (матрица входных сигналов и матрицы весов)
Perceptron::Perceptron(Matrix in, QVector<Matrix> weights) {
    input_matrix = in;
    for (int i = 0; i < symbols; ++i) {
        neyrons[i].set_weight_matrix(weights[i]);
    }
}
//-------------------------------------------------------------------------------------
// установить матрицу входных сигналов
void Perceptron::set_input_matrix(Matrix in) {
    input_matrix = in;
}
//-------------------------------------------------------------------------------------
// установить матрицу входных сигналов для нерона с индексом i
void Perceptron::set_weight_matrix_at(Matrix weight, int i) {
    neyrons[i].set_weight_matrix(weight);
}
//-------------------------------------------------------------------------------------
// установить матрицу весов
void Perceptron::set_weight_matrix(QVector<Matrix> weights) {
    for (int i = 0; i < symbols; ++i) {
        neyrons[i].set_weight_matrix(weights[i]);
    }
}
//-------------------------------------------------------------------------------------
// установить матрицу весов для нейрона с индексом i
Matrix Perceptron::get_weight_matrix_at(int i) {
    return neyrons[i].get_weight_matrix();
}
//-------------------------------------------------------------------------------------
// получить матрицы весов
QVector<Matrix> Perceptron::get_weight_matrix() {
    QVector<Matrix> weight_matrix;
    for (int i = 0; i < symbols; ++i) {
        weight_matrix.push_back(neyrons[i].get_weight_matrix());
    }
    return weight_matrix;
}
//-------------------------------------------------------------------------------------
// получение взвешенной суммы входных сигналов нейрона с индексом i
int Perceptron::get_sum_scaled_signals_at(int i) {
    return neyrons[i].get_sum_scaled_signals();
}
//-------------------------------------------------------------------------------------
// получение взвешенных сумм входных сигналов всех нейронов
QVector<int> Perceptron::get_sum_scaled_signals() {
    QVector<int> sum_scaled_signals;
    for (int i = 0; i < symbols; ++i) {
        sum_scaled_signals.push_back(neyrons[i].get_sum_scaled_signals());
    }
    return sum_scaled_signals;
}
//-------------------------------------------------------------------------------------
// распознавание символа - возвращает индекс
int Perceptron::recognition() {
    int index_neyron = 0;
    int max_sum_scaled_signals = neyrons[index_neyron].count_sum_scaled_signals(input_matrix);

    for (int i = 1; i < symbols; ++i) {
        int sum_scaled_signals = neyrons[i].count_sum_scaled_signals(input_matrix);
        if (sum_scaled_signals > max_sum_scaled_signals) {
            max_sum_scaled_signals = sum_scaled_signals;
            index_neyron = i;
        }
    }
    return index_neyron;
}
//-------------------------------------------------------------------------------------
// изменение весов синапсов у неqронов в зависимости от параметра
QVector<Matrix> Perceptron::teach(int correct_index) {
    for (int i = 0; i < symbols; ++i) {
        if (i != correct_index && neyrons[i].recognition()) {
            neyrons[i].dec_weight(input_matrix);
        }
    }
    neyrons[correct_index].inc_weight(input_matrix);
    return get_weight_matrix();
}


//************************************************************************************
//  Реализация класса MultiPerceptron
//  Описание:   представляет собой сеть перцептронов
//  Применение: объект класса способен распознавать последовательность символов
//************************************************************************************
// конструктор с одинм параметром
MultiPerceptron::MultiPerceptron(QVector<Matrix> input_matrix) {
    for (int i = 0; i < input_matrix.size(); ++i) {
        Perceptron temp(input_matrix[i], weight_matrix);
        perceptrons.push_back(temp);
    }
}
//-------------------------------------------------------------------------------------
// установить матрицу весов
void MultiPerceptron::set_weight_matrix(QVector<Matrix> weights) {
    weight_matrix = weights;
}
//-------------------------------------------------------------------------------------
// получить матрицу весов
QVector<Matrix> MultiPerceptron::get_weight_matrix() {
    return weight_matrix;
}
//-------------------------------------------------------------------------------------
// количество перцептронов (символов)
int MultiPerceptron::size() {
    return perceptrons.size();
}
//-------------------------------------------------------------------------------------
// удалить старые перцептроны и создать новые на основе матриц вх. сигналов
void MultiPerceptron::update(QVector<Matrix> input_matrix) {
    perceptrons.clear();
    for (int i = 0; i < input_matrix.size(); ++i) {
        Perceptron temp(input_matrix[i], weight_matrix);
        perceptrons.push_back(temp);
    }
}
//-------------------------------------------------------------------------------------
// обнулить матрицу весов
void MultiPerceptron::clear_weight_matrix() {
    for (int k = 0; k < symbols; ++k) {
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                weight_matrix[k][i][j] = 0;
            }
        }
    }
}

//-------------------------------------------------------------------------------------
// распознать поселдовательность символов и вернуть результат в виде строки
QString MultiPerceptron::recognition() {
    QString result = "";
    for (int i = 0; i < perceptrons.size(); ++i) {
        result += QString::number(perceptrons[i].recognition());
    }
    return result;
}
//-------------------------------------------------------------------------------------
// изменение весов синапсов у нейронов в зависимости от параметра
// алгоритм: поесле исправления матрицы весов у одно перцептрона
// устаналиваем новую матрицу весов для всех остальных перцептронов
// и вызываем распознавание заново (чтобы не исправлять, вожмно уже
// несуществующую ошибку или исправить возникшую).
void MultiPerceptron::teach(QString correct_number) {
    QString wrong_number = recognition();
    while(wrong_number!=correct_number) {
        for (int i = 0; i < size(); ++i) {
            if (wrong_number[i] != correct_number[i]) {
               weight_matrix = perceptrons[i].teach(correct_number[i].unicode()-48);
            }
            for (int j = 0; j < size(); ++j) {
                perceptrons[j].set_weight_matrix(weight_matrix);
            }
            wrong_number = recognition();
        }
    }
}
//-------------------------------------------------------------------------------------
// получение списка взвешенных сумм входных сигналов всех нейронов для каждого перцептрона
QVector< QVector<int> > MultiPerceptron::get_statistics_scaled_signals() {
    QVector< QVector<int> > result;
    for (int i = 0; i < size(); ++i) {
        result.push_back(perceptrons[i].get_sum_scaled_signals());
    }
    return result;
}


//************************************************************************************
//  Реализация класса Учитель
//  Описание:   представляет собой алгоритмы обучения
//  Применение: обучение перцептрона для получения весов синапсов для конкретной
//              выборки данных
//************************************************************************************
// конструктор с одним параметром (матрицы изображения для обучения)
Teacher::Teacher( QVector< QVector<Matrix> > t_matrix) {
    training_matrix = t_matrix;
}
//-------------------------------------------------------------------------------------
// установить матрицу весов
void Teacher::set_weight_matrix(QVector<Matrix> w_matrix) {
    perceptron.set_weight_matrix(w_matrix);
}
//-------------------------------------------------------------------------------------
// установить матрицы изображения для обучения
void Teacher::set_training_matrix( QVector< QVector<Matrix> > t_matrix) {
    training_matrix = t_matrix;
}
//-------------------------------------------------------------------------------------
// получить матрицу весов
QVector<Matrix> Teacher::get_weight_matrix() {
    return perceptron.get_weight_matrix();
}
//-------------------------------------------------------------------------------------
// получить матрицу для обучения
QVector< QVector<Matrix> > Teacher::get_training_matrix() {
    return training_matrix;
}
//-------------------------------------------------------------------------------------
// изменить матрицу весов в зависимости от данных для обучения
void Teacher::teach() {
    for (int j = 0; j < training_matrix[0].size(); ++j) {
        for (int i = 0; i < 10; ++i) {
            perceptron.set_input_matrix(training_matrix[i][j]);
            int correct_index = perceptron.recognition();
            if (correct_index != i) {
                perceptron.teach(i);
            }
        }
     }
}


void SimpleTeacherBuilder::teach() {
    QVector< QVector<Matrix> > training_matrix = t->get_training_matrix();
    Perceptron perceptron = t->get_perceptron();
    for (int j = 0; j < training_matrix[0].size(); ++j) {
        for (int i = 0; i < 10; ++i) {
            perceptron.set_input_matrix(training_matrix[i][j]);
            int correct_index = perceptron.recognition();
            if (correct_index != i) {
                perceptron.teach(i);
            }
        }
     }
    t->set_weight_matrix(perceptron.get_weight_matrix());
}

void SimpleTeacherBuilder::load_training_matrix() {
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


