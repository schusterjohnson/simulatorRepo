//
// Created by Will Dunn on 2019-04-07.
//
#pragma once

#include "event.hpp"

#define TOTAL_MINUTES    720
#define SIZE_OF_QUEUE    6
#define NUM_LOOPS    100

void addCustomerBank(Banker *bnkr, std::vector<double> *custData, std::vector<Shopper> *mainLine) {
    bnkr->totalCustomersServiced += 1;
    custData->push_back(bnkr->customer.totalServiceTime);
    mainLine->erase(mainLine->begin());
    bnkr->customer = mainLine->front();
    bnkr->totalProcessTime = bnkr->customer.individualServiceTime;
}

void updateCurrentBankCustomer(Banker *bnkr) {
    bnkr->totalProcessTime -= 1;
    bnkr->customer.totalServiceTime += 1;
    if (bnkr->totalProcessTime == 0) {
        bnkr->customer.finishedService = true;
    }
}

double startBank(double customersEachMinute[], double allServiceTimes[],
                 std::vector<double> custData) {
    std::vector<Banker> bankTellers(SIZE_OF_QUEUE, {Banker{Shopper{0, 0, true}, 0, 0}});
    int customersHelped = 0;
    int customerInArray = 0;
    std::vector<Shopper> mainLine;
    for (int currentTime = 0; currentTime < TOTAL_MINUTES; currentTime++) {
        /* -- add customer -- */
        for (int c = 0; c < customersEachMinute[currentTime]; c++, customerInArray++) {
            mainLine.push_back(Shopper{0, allServiceTimes[customerInArray], false});
            for (int eachQueue = 0; eachQueue < SIZE_OF_QUEUE; eachQueue++) {
                if (bankTellers[eachQueue].customer.finishedService == true && mainLine.size() > 0) {
                    addCustomerBank(&bankTellers[eachQueue], &custData, &mainLine);
                    customersHelped += 1;
                }
            }
        }
        for (int eachQueue = 0; eachQueue < SIZE_OF_QUEUE; eachQueue++) {
            if (bankTellers[eachQueue].totalProcessTime > 0)
                updateCurrentBankCustomer(&bankTellers[eachQueue]);
        }
        for (int i = 0; i < mainLine.size(); i++) {
            mainLine.at(i).totalServiceTime += 1;
        }
    }
    double waitTime = 0;
    for (int i = 0; i < customersHelped; i++) {
        waitTime += custData.at(i);
    }
    return waitTime / customersHelped;
}
