//
// Created by barsela on 5/13/22.
//

#ifndef UNTITLED_MAIN_H
#define UNTITLED_MAIN_H

#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include<vector>
#include "stockData.h"
#include <set>
#include "ZipUtilities.h"
#include "Mission.h"
using namespace std;

int parentPid= 0;
int workerPid= 0;
int READ = 0;
int WRITE = 1;
set <string> availableStockList;

/// We use two pipes
/// First pipe to send input from parent to the worker
/// Second pipe to send stack information from worker to parent
int fd1[2]; /// Used to store two ends of first pipe
int fd2[2]; /// Used to store two ends of second pipe

/// function
int main();
void printMenu();
int userInput();
void sigint_handler(int signum);
void worker();
void parent();
void loadDB();
void cleanup();
string userInputWhichStockAndYear();
void importDataFromCSV(string stockName);


#endif //UNTITLED_MAIN_H
