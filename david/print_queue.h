//
// Created by meow on 8/12/2023.
//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <list>
#include <list>
#include <vector>
using namespace std;

void print_queue(list<char> queue) {
    if (queue.empty()) cout << " [Q <empty>]" << endl;
    else {
        cout << " [Q";
        for (auto it = queue.begin(); it != queue.end(); it++) {
            cout << " " << *it;
        }
        cout << "]" << endl;
    }
}
