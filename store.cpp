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
#include "event.hpp"
#define TOTALMINUTES    720
#define SIZEofQUEUE    6
#define numLOOPS    100

int updateGroceryQueue (Cashier *cashier, std::vector <double> *dpc)
{
    int ca = 0;
    if (cashier->customers.front().finishedService == true){
        dpc->push_back(cashier->customers.front().totalServiceTime);
        cashier->customers.erase(cashier->customers.begin());
        cashier->CustomerProcessTime = cashier->customers.front().individualServiceTime;
        cashier->totalCustomersServiced += 1;
        ca = 1;
    }
    if ( cashier->CustomerProcessTime == 1)
        cashier->customers.front().finishedService = true;
    for ( int i = 0 ; i < cashier->customers.size() ; i ++)
        cashier->customers.at(i).totalServiceTime += 1;
    cashier->CustomerProcessTime -= 1;
    return ca;
}
void addShopper (Cashier *cashier , int currTime, double serviceT)
{
    if (cashier->customers.size() == 0)      cashier->CustomerProcessTime = serviceT;
    cashier->customers.push_back(Shopper{0, serviceT, false});
    cashier->totalProcessTime += serviceT;
}
double startStore (int customersGenerated, double customersEachMinute [], double serviceTimeInit [], std::vector <double> custData ){
    std::vector <Cashier> cashiers  (SIZEofQUEUE ,{ std::vector <Shopper> (), 0, 0, 0} );
    int customersHelped = 0;
    int customerInArray = 0;
    for ( int currentTime = 0; currentTime <  TOTALMINUTES; currentTime++){
        /* -- add customers loop ----- */
        for ( int c = 0 ; c < customersEachMinute[currentTime] ; c++, customerInArray++){
            assert (customerInArray <= customersGenerated);
            
            /* -- find the shortest queue --- */
            int smallestQueue = 0 ;
            for (int eachQueue = 0 ; eachQueue < SIZEofQUEUE ;  eachQueue++ ){
                if (cashiers[eachQueue].totalProcessTime < cashiers[smallestQueue].totalProcessTime)
                    smallestQueue = eachQueue;
            }
            addShopper(&cashiers[smallestQueue], currentTime, serviceTimeInit[customerInArray]);
        }
        /* -- update each queue ----- */
        for (int eachQueue = 0 ; eachQueue < SIZEofQUEUE ;  eachQueue++ ){
            if (cashiers[eachQueue].customers.size() > 0)
                customersHelped += updateGroceryQueue(&cashiers[eachQueue], &custData);
        }
    }
    double wtt = 0;
    for (int i = 0 ; i < customersHelped ; i++){
        wtt += custData.at(i);
    }
    return (double)wtt/customersHelped;
}
void addCustomerBank(Banker *bnkr,  std::vector <double> *custData, std::vector <Shopper> *mainLine)
{
    bnkr->totalCustomersServiced += 1;
    custData->push_back(  bnkr->customer.totalServiceTime);
    mainLine->erase(mainLine->begin());
    bnkr->customer = mainLine->front();
    bnkr->totalProcessTime = bnkr->customer.individualServiceTime;
}
void updateCurrentBankCustomer (Banker *bnkr)
{
    bnkr->totalProcessTime -= 1;
    bnkr->customer.totalServiceTime += 1;
    if (bnkr->totalProcessTime == 0){
        bnkr->customer.finishedService = true;
    }
}

double startBank (int customersGenerated , double customersEachMinute [],double allServiceTimes [], std::vector <double> custData)
{
    std::vector <Banker> bankTellers  (SIZEofQUEUE ,{ Banker { Shopper { 0, 0 , true}, 0, 0 } } );
    int customersHelped = 0;
    int customerInArray = 0;
    std::vector <Shopper> mainLine;
    for ( int currentTime = 0; currentTime <  TOTALMINUTES; currentTime++){
        /* -- add customer -- */
        for ( int c = 0 ; c < customersEachMinute[currentTime] ; c++ , customerInArray++){
            mainLine.push_back(Shopper{0, allServiceTimes[customerInArray], false});
            for (int eachQueue = 0 ; eachQueue < SIZEofQUEUE ;  eachQueue++ ) {
                if (bankTellers[eachQueue].customer.finishedService == true && mainLine.size() > 0){
                    addCustomerBank(&bankTellers[eachQueue], &custData, &mainLine);
                    customersHelped += 1;
                }
            }
        }
        for (int eachQueue = 0 ; eachQueue < SIZEofQUEUE ;  eachQueue++ ){
            if (bankTellers[eachQueue].totalProcessTime > 0 )
                updateCurrentBankCustomer (&bankTellers[eachQueue]);
        }
        for ( int i = 0 ; i < mainLine.size() ; i ++){
            mainLine.at(i).totalServiceTime += 1;
        }
    }
    double wtt = 0;
    for (int i = 0 ; i < customersHelped ; i++){
        wtt += custData.at(i);
    }
    return (double)wtt/customersHelped;
}

void handleData (double custArrivalRate, double maxCustomerServiceTime, double storeArray[] , double bankArray[])
{
    std::ofstream market("marketData",  std::ios::out | std::ios::app);
    std::ofstream bank("bankData",  std::ios::out | std::ios::app);
    std::ofstream diff("difference",  std::ios::out | std::ios::app);
    std::ofstream perc("percentage",  std::ios::out | std::ios::app);
    std::sort(bankArray, bankArray + numLOOPS);
    std::sort(storeArray, storeArray + numLOOPS);
    std::vector <double> bankPercentiles(3);
    std::vector <double> groceryPercentiles(3);
    double b90th = 0 ;
    double s90th = 0;
    double b50th = 0 ;
    double s50th = 0;
    double b10th = 0 ;
    double s10th = 0;
    for ( int i = numLOOPS * 0.1 ; i < numLOOPS ; i ++) {
        b10th += bankArray[i] ;
        s10th += storeArray[i] ;
        if ( i > numLOOPS * 0.5 ){
            b50th += bankArray[i] ;
            s50th += storeArray[i] ;
            if ( i > numLOOPS * 0.9 ){
                b90th += bankArray[i] ;
                s90th += storeArray[i] ;
            }
        }
    }
    s90th = s90th/(numLOOPS * 0.9);
    b90th = b90th/(numLOOPS * 0.9);
    std::cout<< "10th percentile Grocery Store : "<<  s10th/(numLOOPS * 0.1) << std::endl;
    std::cout<< "10th percentile Bank Store    : "<< b10th/(numLOOPS * 0.1) << std::endl;
    std::cout<< "  50th percentile Grocery Store : "<< s50th/(numLOOPS * 0.5) << std::endl;
    std::cout<< "  50th percentile Bank Store    : "<< b50th/(numLOOPS * 0.5) << std::endl;
    std::cout<< "    90th percentile Grocery Store : "<<  s90th << std::endl;
    std::cout<< "    90th percentile Bank Store    : "<< b90th << std::endl;
    double aveSum = (s90th+b90th) * .5;

    market << std::to_string(custArrivalRate)+"  "+std::to_string(maxCustomerServiceTime) + "  " + std::to_string(s90th) << std::endl;
    bank << std::to_string(custArrivalRate)+"  "+std::to_string(maxCustomerServiceTime) + "  " + std::to_string(b90th)<< std::endl;
    diff << std::to_string(custArrivalRate)+"  "+std::to_string(maxCustomerServiceTime) + "  " + std::to_string(s90th-b90th)<< std::endl;
    perc << std::to_string(custArrivalRate)+"  "+std::to_string(maxCustomerServiceTime) + "  " + std::to_string( ( (s90th-b90th) /aveSum ) * 100 )<< std::endl;
    bank.close();
    market.close();
    diff.close();
    perc.close();
}
void openStore(double custArrivalRate, double maxCustomerServiceTime, int seed)
{
    srand(seed);
    
    int totalCustomers = custArrivalRate * TOTALMINUTES ;
    double customersEachMinute [TOTALMINUTES];
    int customersGenerated = 0 ;
    for (int i = 0 ; i < TOTALMINUTES  ; i++){
        int rate = std::rand() % ((int)TOTALMINUTES  - 1) + 1;
        if (rate < totalCustomers ){
            customersEachMinute[i] = custArrivalRate;
            customersGenerated+= custArrivalRate;
        }
        else
            customersEachMinute[i] = 0;
    }
    double serviceTimeInit [customersGenerated];
    std::vector <double> custData ;
    for (int i = 0 ; i < customersGenerated ; i++){
        double timeR = std::rand() % (int)(maxCustomerServiceTime) + 1;
        serviceTimeInit[i] =timeR;
    }
    double bankArray [numLOOPS];
    double storeArray [numLOOPS];
    for ( int i = 0 ; i < numLOOPS ; i ++) {
        storeArray[i] = startStore (customersGenerated ,  customersEachMinute,serviceTimeInit, custData );
        bankArray[i] = startBank (customersGenerated ,  customersEachMinute,serviceTimeInit, custData);
    }
    handleData ( custArrivalRate,  maxCustomerServiceTime,  storeArray ,  bankArray);
}
