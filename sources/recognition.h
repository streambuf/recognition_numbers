#include <mainwindow.h>
#ifndef RECOGNITION_H
#define RECOGNITION_H

#include <QtGui/QApplication>
#include <QCloseEvent>
#include <tr1/array>
#include <QVector>


typedef std::tr1::array<std::tr1::array<int, 20>, 33>  Matrix;


const int limit = 400;

const int row = 33;
const int col = 20;
const int symbols = 10;
const int population_size = 20;
const int chromosome_size = row * col;
const int mutation_probability = 1;


//************************************************************************************
//  Класс Нейрон
//  Описание:   представляет собой взвешенный сумматор
//  Применение: каждый объект класса отвечает за свой символ
//************************************************************************************
class Neyron {
private:
    Matrix weight_matrix;   // веса синапсов
    Matrix scaled_matrix;   // масштабированные входные сигналы
    int sum_scaled_signals; // взвешенная сумма входных сигналов

public:
    Neyron();
    ~Neyron() { }

    inline Matrix get_weight_matrix() { return weight_matrix; }
    inline int get_sum_scaled_signals() { return sum_scaled_signals; }
    int count_sum_scaled_signals(Matrix in_matrix);
    void set_weight_matrix(Matrix w_matrix);
    void dec_weight(Matrix input);    // уменьшить вес синапсов
    void inc_weight(Matrix input);    // увеличить вес синапсов
    bool recognition();
};

//************************************************************************************
//  Класс Перцептрон
//  Описание:   представляет собой сеть нейронов
//  Применение: объект класса способен распознавать символ
//************************************************************************************
class Perceptron {
protected:
    Neyron neyrons[symbols];    // значение symbols зависит от задачи
    Matrix input_matrix;        // матрица входных сигналов
public:
    Perceptron() {}
    Perceptron(Matrix i_matrix);
    Perceptron(Matrix i_matrix, QVector<Matrix> w_vmatrix);
    ~Perceptron() {}

    void set_input_matrix(Matrix in_matirx);
    void set_weight_matrix_at(Matrix w_matrix, int index_symbol);
    void set_weight_matrix(QVector<Matrix> w_vmatrix);
    Matrix get_weight_matrix_at(int index_symbol);
    QVector<Matrix> get_weight_matrix();
    // получение взвешенной суммы входных сигналов одного нейрона
    int get_sum_scaled_signals_at(int index_symbol);
    // получение взвешенных сумм входных сигналов всех нейронов
    QVector<int> get_sum_scaled_signals();
    // распознавание символа - возвращает индекс
    int recognition();
    // изменение весов синапсов у нейронов в зависимости от параметра
    QVector<Matrix> teach(int correct_index_symbol);
};

//************************************************************************************
//  Класс MultiPerceptron
//  Описание:   представляет собой сеть перцептронов
//  Применение: объект класса способен распознавать последовательность символов
//************************************************************************************
class MultiPerceptron {
private:
    QVector<Matrix> weight_matrix;
    QVector<Perceptron> perceptrons;
public:
    MultiPerceptron() {}
    MultiPerceptron(QVector<Matrix> in_matrix);
    ~MultiPerceptron() {}

    void set_weight_matrix(QVector<Matrix> w_matrix);
    QVector<Matrix> get_weight_matrix();
    // количество перцептронов (символов)
    int size();
    // удалить старые перцептроны и создать новые на основе матриц вх. сигналов
    void update(QVector<Matrix> in_matrix);
    // обнулить матрицу весов
    void clear_weight_matrix();
    // распознать поселдовательность символов и вернуть результат в виде строки
    QString recognition();
    // изменение весов синапсов у нейронов в зависимости от параметра
    void teach(QString);
    // получение матрицы взвешенных сумм входных сигналов всех нейронов для каждого перцептрона
    QVector< QVector<int> > get_statistics_scaled_signals();
};

//************************************************************************************
//  Класс Учитель
//  Описание:   представляет собой алгоритмы обучения
//  Применение: обучение перцептрона для получения весов синапсов для конкретной
//              выборки данных
//************************************************************************************
class Teacher {
private:
    Perceptron perceptron;
    QVector< QVector<Matrix> > training_matrix;
    QString dir;
public:
    Teacher() {}
    Teacher(QVector< QVector<Matrix> >);
    ~Teacher() {}
    void set_weight_matrix(QVector<Matrix> w_matrix);
    void set_training_matrix(QVector< QVector<Matrix> >);
    QVector<Matrix> get_weight_matrix();
    QString get_directory() { return dir; }
    inline Perceptron get_perceptron() { return perceptron; }
    QVector< QVector<Matrix> > get_training_matrix();

    void teach();
    void load_training_matrix() {}
    void set_directory(QString d) { dir = d; }
};



class TeacherBuilder {
protected:
    Teacher *t;
public:
    TeacherBuilder() {}

    void create_new_teacher() {
        t = new Teacher();
    }
    virtual void teach() = 0;
    virtual void load_training_matrix() = 0;
    virtual void set_directory(QString dir) = 0;

    inline Teacher *get_teacher() { return t; }
    inline QVector<Matrix> get_weight_matrix() {
        return t->get_weight_matrix();
    }
};

class SimpleTeacherBuilder : public TeacherBuilder {
public:
    void teach();
    void set_directory(QString dir) {
        t->set_directory(dir);
    }
    void load_training_matrix();
};

class GeneticTeacherBuilder : public TeacherBuilder {
    void teach();
    void set_directory(QString dir) {
        t->set_directory(dir);
    }
    void load_training_matrix();
};


class TeacherDirector {
private:
    TeacherBuilder *teacher;
public:
    inline void set_teacher(TeacherBuilder *tb) {
        teacher = tb;
    }
    inline void teach(QString dir) {
        teacher->create_new_teacher();
        teacher->set_directory(dir);
        teacher->load_training_matrix();
        teacher->teach();
    }
};


#endif // RECOGNITION_H
