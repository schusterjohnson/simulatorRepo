//
//  event.hpp
//  simulator
//
//  Created by Sarah Schuster-Johnson on 3/30/19.
//  Copyright © 2019 Sarah Schuster-Johnsobn. All rights reserved.
//

#ifndef event_hpp
#define event_hpp

#include <iostream>
#include <queue>
#include <vector>
#include <stdio.h>

struct Shopper {
    double totalServiceTime;
    double individualServiceTime;
    bool finishedService;
};
struct Cashier {
    std::vector<Shopper> customers;
    double totalProcessTime;
    double totalCustomersServiced;
    double CustomerProcessTime;
};
struct Banker {
    Shopper customer;
    double totalProcessTime;
    double totalCustomersServiced;
};
#endif /* event_hpp */
