#ifndef GENETIC_H
#define GENETIC_H

class Chromosome {
private:
    QVector<int> genes;

public:
    Chromosome() {}
    Chromosome(int size);
    int getGene(int index) { return genes[index]; }
    int getRandNumber(const int min, const int max) {
        return ((qrand() % ((max + 1) - min)) + min);
    }


};

class Population {
private:
    QVector<Chromosome> chromosomes;
public:
    Population() {}
    Population(int size);
};


class GA {
private:
    QVector<Population> populations;
    QVector< QVector< QVector<int> > > test_data;
    int fitnessFuction(Chromosome ch, int index_symbol);
public:
    GA(int size, QVector< QVector<Matrix> > training_matrix);
};

#endif // GENETIC_H
