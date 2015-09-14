#include "recognition.h"
#include "genetic.h"
#include <QtDebug>
#include <cmath>
#include <algorithm>

// deprecated
void printPopulations(QVector<Population> populations) {
    for (int i = 0; i < 1; ++i) {
        qDebug() << "Symbol: " + QString::number(i);
        for (int j = 0; j < 1; ++j) {
            qDebug() << "Population: " + QString::number(j);
            QString s = "";
            for (int k = 0; k < chromosome_size; ++k) {
                s += QString::number(populations[i].getChromosome(j).getGene(k)) + " ";
            }
            qDebug() << s;
        }
    }
}

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

QVector<Matrix> GA::getWeightMatrix() {
    for (int i = 0; i < 10000; ++i) {
        if (i % 100 == 0)
            qDebug() << i;
        QVector<Population> newPopulations = crossing();
        selection(newPopulations);

    }
    return findBestWeightMatrix();
}

QVector<Matrix> GA::findBestWeightMatrix() {
    QVector<Matrix> result;
    for (int i = 0; i < symbols; ++i) {
        Chromosome best_ch = populations[i].getChromosome(0);
        for (int j = 1; j < population_size; ++j) {
            Chromosome cur_ch = populations[i].getChromosome(j);
            if (fitnessFunction(cur_ch, i) > fitnessFunction(best_ch, i)) {
                best_ch = cur_ch;
            }
        }
        Matrix matrix;
        for (int k = 0; k < row; ++k) {
            for (int m = 0; m < col; ++m) {
                matrix[k][m] = best_ch.getGene(k*col + m);
            }
        }
        result.append(matrix);
    }
    return result;

}

int GA::fitnessFunction(Chromosome ch, int index_symbol) {
    int sum = 0;

        for (int j = 0; j < test_data[index_symbol].size(); ++j) {
            for (int k = 0; k < test_data[index_symbol][j].size(); ++k) {

                    sum += test_data[index_symbol][j][k] * ch.getGene(k);

            }
        }

    return sum;
//    int sum = 0;
//    for (int i = 0; i < test_data.size(); ++i) {
//        for (int j = 0; j < test_data[i].size(); ++j) {
//            for (int k = 0; k < test_data[i][j].size(); ++k) {
//                if (i == index_symbol) {
//                    sum += test_data[i][j][k] * ch.getGene(k);
//                } else {
//                    sum -= test_data[i][j][k] * ch.getGene(k);
//                }
//            }
//        }
//    }
//    return sum;
}

void GA::selection(QVector<Population> newPopulations) {
    for (int i = 0; i < symbols; ++i) {
        for (int j = 0; j < population_size; ++j) {
            Chromosome ch = populations[i].getChromosome(j);
            Chromosome newCh = newPopulations[i].getChromosome(j);
            if (  fitnessFunction(ch, i) < fitnessFunction(newCh, i)) {
                populations[i].setChromosome(newCh, j);
            }
        }
    }
}

QVector<Population> GA::crossing() {
    QVector<Population> newPopulations;
    for (int i = 0; i < symbols; ++ i) {
        newPopulations.append(Population());
        for (int j = 0; j < population_size - 1; j+=2) {
            Chromosome par1 = populations[i].getChromosome(j);
            Chromosome par2 = populations[i].getChromosome(j + 1);
            int pos1 = par1.getRandNumber(0, chromosome_size -1);
            int pos2 = par2.getRandNumber(0, chromosome_size -1);
            if (pos1 > pos2) {
                int temp = pos1; pos1 = pos2; pos2 = temp;
            }
            for (int k = pos1; k < pos2; ++k) {
                int temp = par1.getGene(k);
                par1.setGene(k, par2.getGene(k));
                par2.setGene(k, temp);
            }
            mutation(par1);
            mutation(par2);
            newPopulations[i].addChromosome(par1);
            newPopulations[i].addChromosome(par2);

        }
    }
    return newPopulations;
}

void GA::mutation(Chromosome ch) {
    int probability = ch.getRandNumber(0, 100);
    if (probability < mutation_probability) {
        int index = ch.getRandNumber(0, chromosome_size -1);
        int weight = ch.getRandNumber(-10, 10);
        ch.setGene(index, weight);
    }

}

GA::GA(int size, QVector< QVector<Matrix> > training_matrix) {
    for (int i = 0; i < size; ++i) {
        populations.append(Population(size));
    }

    for (int i = 0; i < training_matrix.size(); ++i) {
        test_data.append(QVector< QVector<int> >());
        for (int j = 0; j < 1;/*training_matrix[i].size();*/ ++j) {
            test_data[i].append(QVector<int>());
            for (int k = 0; k < row; ++k) {
                for (int m = 0; m < col; ++m) {
                    test_data[i][j].append(training_matrix[i][j][k][m]);
                }
            }
        }
    }
}








