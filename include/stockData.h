//
// Created by barsela on 5/13/22.
//

#ifndef UNTITLED_STOCKDATA_H
#define UNTITLED_STOCKDATA_H


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
using namespace std;


class stockData {
private:

    // inner class - stockNode
    class stockNode {
    public:
        stockNode(int year, int month, float openRate, float highRate, float lowRate, float closeRate, int volume);
        string toString();
        int getYear() const { return this->year; }
        int getMonth() const { return this->month; }
        bool isInYear(int year) const { return year == this->year; }
        float getHighRate() { return highRate; }
        float getOpenRate() const;
        float getLowRate() const;
        float getCloseRate() const;
        int getVolume() const;

    private:
        // members: // date:
        int year, month , volume;
        float openRate, highRate, lowRate, closeRate;
    };

private:
    // members:
    string stockName;
    string stockFilePath;
    string epsFilePath;
    stockNode* dataNodeLst[1000];
    map<string, float> epsMap;
    float epsAverage;
    int dataSize;

private:
    // methods - private
    void loadDataFromFile();
    float getEPS(int year, int month);
    float getHighAverage();


public:
    // methods all Q
    stockData(string stockName);
    stockData(string stockName,bool fromCSV);
    stockData();

    void loadEPSFromFile();
    // methods - Q1
    string printStockByYear(int year);
    // methods - Q2
    string compareByPriceToEarnings();
    // methods - Q3
    void exportDataToCSV();
    string getStockName(){return stockName;}

};


#endif //UNTITLED_STOCKDATA_H
