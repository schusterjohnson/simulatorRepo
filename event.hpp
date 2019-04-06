//
//  event.hpp
//  simulator
//
//  Created by Sarah Schuster-Johnson on 3/30/19.
//  Copyright © 2019 Sarah Schuster-Johnsobn. All rights reserved.
//
#include <iostream>
#include <queue>
#ifndef event_hpp
#define event_hpp
#include <vector>
#include <stdio.h>

struct Shopper {
    double totalServiceTime;
    double individualServiceTime;
    bool finishedService;
};
struct Cashier {
    std::vector <Shopper> customers;
    double totalProcessTime;
    double totalCustomersServiced;
    double CustomerProcessTime;
    bool ifAvailable;
};
struct Banker {
    Shopper customer;
    double totalProcessTime;
    double totalCustomersServiced;
    bool ifAvailable;
};
#endif /* event_hpp */
