//
// Created by barsela on 5/13/22.
//

#ifndef UNTITLED_MISSION_H
#define UNTITLED_MISSION_H
#include <vector>
#include "stockData.h"
#include <set>
#include "../include/ZipUtilities.h"
class Mission {
public:
    string list_fetched_stocks();
    string PrintStockData(string name, int year ); // option 3
    string exportAndCreateDBStocksData() ; // option 4
private:
    set <string> getSetOfAllTheFilesInDir();
    set<string> removeDupWord(string str);
};


#endif //UNTITLED_MISSION_H
