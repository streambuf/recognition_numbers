#include "recognition.h"
#include "genetic.h"
#include <QtDebug>
#include <cmath>



Chromosome::Chromosome(int size) {
    for (int i = 0; i < size; ++i) {
        genes.append(getRandNumber(-10, 10));
    }
}

Population::Population(int size) {
    for (int i = 0; i < size; ++i) {
        chromosomes.append(Chromosome(chromosome_size));
    }
}

int GA::fitnessFuction(Chromosome ch, int index_symbol) {
    int sum = 0;
    for (int i = 0; i < test_data.size(); ++i) {
        for (int j = 0; j < test_data[i].size(); ++j) {
            for (int k = 0; k < test_data[i][j].size(); ++k) {
                if (i == index_symbol) {
                    sum += test_data[i][j][k] * ch.getGene(k);
                } else {
                    sum -= test_data[i][j][k] * ch.getGene(k);
                }
            }
        }
    }
    return std::abs(sum);
}

GA::GA(int size, QVector< QVector<Matrix> > training_matrix) {
    for (int i = 0; i < size; ++i) {
        populations.append(Population(size));
    }

    for (int i = 0; i < training_matrix.size(); ++i) {
        test_data.append(QVector< QVector<int> >());
        for (int j = 0; j < training_matrix[i].size(); ++j) {
            test_data[i].append(QVector<int>());
            for (int k = 0; k < row; ++k) {
                for (int m = 0; m < col; ++m) {
                    test_data[i][j].append(training_matrix[i][j][k][m]);
                }
            }
        }
    }

}




