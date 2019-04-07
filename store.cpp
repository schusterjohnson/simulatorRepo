//
//  store.cpp
//  simulator
//
//  Created by Sarah Schuster-Johnson on 4/3/19.
//  Copyright © 2019 Sarah Schuster-Johnsobn. All rights reserved.
//

#include <iostream>
#include <random>
#include <fstream>
#include <stdio.h>

#include "event.hpp"
#include "bank.hpp"

#define TOTAL_MINUTES    720
#define SIZE_OF_QUEUE    6
#define NUM_LOOPS    100

int updateGroceryQueue(Cashier *cashier, std::vector<double> *customerQueue) {
    int availableCashiers = 0;
    if (cashier->customers.front().finishedService == true) {
        customerQueue->push_back(cashier->customers.front().totalServiceTime);
        cashier->customers.erase(cashier->customers.begin());
        cashier->CustomerProcessTime = cashier->customers.front().individualServiceTime;
        cashier->totalCustomersServiced += 1;
        availableCashiers = 1;
    }
    if (cashier->CustomerProcessTime == 1)
        cashier->customers.front().finishedService = true;
    for (int i = 0; i < cashier->customers.size(); i++)
        cashier->customers.at(i).totalServiceTime += 1;
    cashier->CustomerProcessTime -= 1;
    return availableCashiers;
}

void addShopper(Cashier *cashier, double serviceT) {
    if (cashier->customers.size() == 0) cashier->CustomerProcessTime = serviceT;
    cashier->customers.push_back(Shopper{0, serviceT, false});
    cashier->totalProcessTime += serviceT;
}

double startStore(int customersGenerated, double customersEachMinute[], double serviceTimeInit[],
                  std::vector<double> custData) {
    std::vector<Cashier> cashiers(SIZE_OF_QUEUE, {std::vector<Shopper>(), 0, 0, 0});
    int customersHelped = 0;
    int customerInArray = 0;
    for (int currentTime = 0; currentTime < TOTAL_MINUTES; currentTime++) {
        /* -- add customers loop ----- */
        for (int c = 0; c < customersEachMinute[currentTime]; c++, customerInArray++) {
            assert (customerInArray <= customersGenerated);

            /* -- find the shortest queue --- */
            int smallestQueue = 0;
            for (int eachQueue = 0; eachQueue < SIZE_OF_QUEUE; eachQueue++) {
                if (cashiers[eachQueue].totalProcessTime < cashiers[smallestQueue].totalProcessTime)
                    smallestQueue = eachQueue;
            }
            addShopper(&cashiers[smallestQueue], serviceTimeInit[customerInArray]);
        }
        /* -- update each queue ----- */
        for (int eachQueue = 0; eachQueue < SIZE_OF_QUEUE; eachQueue++) {
            if (cashiers[eachQueue].customers.size() > 0)
                customersHelped += updateGroceryQueue(&cashiers[eachQueue], &custData);
        }
    }
    double waitTime = 0;
    for (int i = 0; i < customersHelped; i++) {
        waitTime += custData.at(i);
    }
    return waitTime / customersHelped;
}

void handleData(double custArrivalRate, double maxCustomerServiceTime, double storeArray[], double bankArray[]) {
    std::ofstream market("marketData", std::ios::out | std::ios::app);
    std::ofstream bank("bankData", std::ios::out | std::ios::app);
    std::ofstream diff("difference", std::ios::out | std::ios::app);
    std::ofstream perc("percentage", std::ios::out | std::ios::app);
    std::sort(bankArray, bankArray + NUM_LOOPS);
    std::sort(storeArray, storeArray + NUM_LOOPS);
    std::vector<double> bankPercentiles(3);
    std::vector<double> groceryPercentiles(3);
    double bank90thPercentile = 0;
    double store90thPercentile = 0;
    double b50th = 0;
    double s50th = 0;
    double b10th = 0;
    double s10th = 0;
    for (int i = NUM_LOOPS * 0.1; i < NUM_LOOPS; i++) {
        b10th += bankArray[i];
        s10th += storeArray[i];
        if (i > NUM_LOOPS * 0.5) {
            b50th += bankArray[i];
            s50th += storeArray[i];
            if (i > NUM_LOOPS * 0.9) {
                bank90thPercentile += bankArray[i];
                store90thPercentile += storeArray[i];
            }
        }
    }
    store90thPercentile = store90thPercentile / (NUM_LOOPS * 0.9);
    bank90thPercentile = bank90thPercentile / (NUM_LOOPS * 0.9);
    std::cout << "10th percentile Grocery Store : " << s10th / (NUM_LOOPS * 0.1) << std::endl;
    std::cout << "10th percentile Bank Store    : " << b10th / (NUM_LOOPS * 0.1) << std::endl;
    std::cout << "  50th percentile Grocery Store : " << s50th / (NUM_LOOPS * 0.5) << std::endl;
    std::cout << "  50th percentile Bank Store    : " << b50th / (NUM_LOOPS * 0.5) << std::endl;
    std::cout << "    90th percentile Grocery Store : " << store90thPercentile << std::endl;
    std::cout << "    90th percentile Bank Store    : " << bank90thPercentile << std::endl;
    double aveSum = (store90thPercentile + bank90thPercentile) * .5;

    market << std::to_string(custArrivalRate) + "  " + std::to_string(maxCustomerServiceTime) + "  " +
              std::to_string(store90thPercentile) << std::endl;
    bank << std::to_string(custArrivalRate) + "  " + std::to_string(maxCustomerServiceTime) + "  " +
            std::to_string(bank90thPercentile) << std::endl;
    diff << std::to_string(custArrivalRate) + "  " + std::to_string(maxCustomerServiceTime) + "  " +
            std::to_string(store90thPercentile - bank90thPercentile) << std::endl;
    perc << std::to_string(custArrivalRate) + "  " + std::to_string(maxCustomerServiceTime) + "  " +
            std::to_string(((store90thPercentile - bank90thPercentile) / aveSum) * 100) << std::endl;
    bank.close();
    market.close();
    diff.close();
    perc.close();
}

void startSimulations(double custArrivalRate, double maxCustomerServiceTime, int seed) {
    srand(seed);

    int totalCustomers = custArrivalRate * TOTAL_MINUTES;
    double customersEachMinute[TOTAL_MINUTES];
    int customersGenerated = 0;
    for (int i = 0; i < TOTAL_MINUTES; i++) {
        int rate = std::rand() % ((int) TOTAL_MINUTES - 1) + 1;
        if (rate < totalCustomers) {
            customersEachMinute[i] = custArrivalRate;
            customersGenerated += custArrivalRate;
        } else
            customersEachMinute[i] = 0;
    }
    double serviceTimeInit[customersGenerated];
    std::vector<double> custData;
    for (int i = 0; i < customersGenerated; i++) {
        double timeR = std::rand() % (int) (maxCustomerServiceTime) + 1;
        serviceTimeInit[i] = timeR;
    }
    double bankArray[NUM_LOOPS];
    double storeArray[NUM_LOOPS];
    for (int i = 0; i < NUM_LOOPS; i++) {
        storeArray[i] = startStore(customersGenerated, customersEachMinute, serviceTimeInit, custData);
        bankArray[i] = startBank(customersEachMinute, serviceTimeInit, custData);
    }
    handleData(custArrivalRate, maxCustomerServiceTime, storeArray, bankArray);
}
