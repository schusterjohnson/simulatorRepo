//
//  main.cpp
//  simulator
//
//  Created by Sarah Schuster-Johnson on 3/30/19.
//  Copyright © 2019 Sarah Schuster-Johnsobn. All rights reserved.
//
#include "store.hpp"
int main(int argc, const char * argv[]) {
    for ( int i = 1 ; i < 10 ; i ++ ){
        for ( double j = 1 ; j < 5 ; j += 0.5){
            openStore(i, j, i+j);
        }
    }
    
    return 0;
}
