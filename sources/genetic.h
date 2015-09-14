#ifndef GENETIC_H
#define GENETIC_H

class Chromosome {
private:
    QVector<int> genes;

public:
    Chromosome() {}
    Chromosome(int size);
    int getGene(int index) { return genes[index]; }
    int setGene(int index, int value) { genes[index] =  value; }
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
    Chromosome getChromosome(int index) { return chromosomes[index]; }
    void addChromosome(Chromosome ch) { chromosomes.append(ch); }
    void setChromosome(Chromosome ch, int index) { chromosomes[index] = ch; }
    QVector<Chromosome> getChromosomes() { return chromosomes; }
};


class GA {
private:
    QVector<Population> populations;
    QVector< QVector< QVector<int> > > test_data;
    int fitnessFunction(Chromosome ch, int index_symbol);
    void selection(QVector<Population> newPopulations);
    QVector<Population> crossing();
    QVector<Matrix> findBestWeightMatrix();
    void mutation(Chromosome ch);

public:
    GA(int size, QVector< QVector<Matrix> > training_matrix);
    QVector<Matrix> getWeightMatrix();
};

#endif // GENETIC_H
