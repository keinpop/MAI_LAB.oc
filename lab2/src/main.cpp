#include <iostream>
#include <thread>
#include <chrono>

typedef struct _thread_data {
    int numberOfPointsFirst;
    int numberOfPointsSecond;
    int plays;
    int tour;
    int experements;
    int winChanceFirst;
    int winChanceSecond;
} threadData;

void* threadFunc(void* arg)
{
    threadData* tData = (threadData*)arg;
    unsigned int seed;
    seed = pthread_self();

    int firstP, secondP;
    for (int i = 0; i < tData->experements; ++i) {
        firstP = tData->numberOfPointsFirst;
        secondP = tData->numberOfPointsSecond;
        for (int j = 0; j < tData->plays - tData->tour + 1; ++j) { // + 1, тк индексация с 0
            firstP += rand_r(&seed) % 6 + 1; 
            firstP += rand_r(&seed) % 6 + 1;
            secondP += rand_r(&seed) % 6 + 1;
            secondP += rand_r(&seed) % 6 + 1;
        }

        if (firstP > secondP) {
            tData->winChanceFirst++;
        }

        if (secondP > firstP) {
            tData->winChanceSecond++;
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "\tError! Program must have only 1 key\n EXAMPLE:\n \t"
                  << argv[0] << " <NUMBER_OF_THREADS>\n";
        exit(EXIT_FAILURE);
    }

    clock_t startTime, endTime;
    float timer;
    startTime = clock();

    int numberOfThreads = atoi(argv[1]);

    int numberOfPointsFirst, numberOfPointsSecond, 
    plays, tour, experements;
    float percentWinsFirst = 0, percentWinsSecond = 0;

    std::cout << "1) Enter number of points first player: ";
    std::cin >> numberOfPointsFirst;
    std::cout << "2) Enter number of points second player: ";
    std::cin >> numberOfPointsSecond;
    std::cout << "3) Enter the number of this tour: ";
    std::cin >> tour;
    std::cout << "4) Enter number of throws (K): ";
    std::cin >> plays;
    std::cout << "5) Enter number of experements: ";
    std::cin >> experements;

    int numbOfExperemForOneThread = experements / numberOfThreads; 

    threadData tData[numberOfThreads];

    for (int i = 0; i < numberOfThreads; ++i) {
        tData[i].numberOfPointsFirst = numberOfPointsFirst;
        tData[i].numberOfPointsSecond = numberOfPointsSecond;
        tData[i].plays = plays;
        tData[i].tour = tour;

        if (i == numberOfThreads - 1) {
            tData[i].experements = numbOfExperemForOneThread + experements % numberOfThreads;
        } else {
            tData[i].experements = numbOfExperemForOneThread;
        }

        tData[i].winChanceFirst = 0;
        tData[i].winChanceSecond = 0;
    }

    pthread_t th[numberOfThreads];

    for (int i = 0; i < numberOfThreads; ++i) {
        if (pthread_create(&th[i], NULL, threadFunc, &tData[i]) != 0) {
            std::cerr << "\tError! Can'not create thread # " << i << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < numberOfThreads; ++i) {
        if (pthread_join(th[i], NULL) != 0) {
            std::cerr << "Error! Can'not join thread # " << i << std::endl;
            break;
        }

        threadData* res = &tData[i];
        percentWinsFirst += res->winChanceFirst;
        percentWinsSecond += res->winChanceSecond;
    }

    std::cout << "Probability of the first player to win: " 
              << percentWinsFirst / experements << std::endl;
    std::cout << "Probability of the second player to win: " 
              << percentWinsSecond / experements << std::endl;

    endTime = clock();
    timer = endTime - startTime;
    std::cout << "Time: " << timer / CLOCKS_PER_SEC << std::endl;
    
    return 0;
}