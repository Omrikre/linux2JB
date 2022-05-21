#include "../include/stockData.h"

stockData::stockData(string stockName) {
    this->stockName = stockName;
    this->stockFilePath = stockName + ".stock";
    this->epsFilePath = stockName + ".esp";
    loadDataFromFile();
    loadEPSFromFile();
}
stockData::stockData(string stockName,bool fromCSV) {
    this->stockName = stockName;
}
string stockData::printStockByYear(int year) {
    string result;
    result = this->stockName +"\n";
    int month;
    for (int i = 0; i < this->dataSize; ++i) {
        month = this->dataNodeLst[i]->getMonth();
        if(this->dataNodeLst[i]->isInYear(year)) {
            if(month >= 10) {
                result = result + to_string(month) + ": [ " + this->dataNodeLst[i]->toString();
                result =result + "6. reported EPS: " + to_string(getEPS(year, month)) + " ]"  +"\n";
            }
            else {
                result = result + to_string(month) + ":  [ " + this->dataNodeLst[i]->toString();
                result = result + "6. reported EPS: " + to_string(getEPS(year, month)) + " ]"  +"\n";
            }
        }
    }
    return result;
}

void stockData::loadDataFromFile() {
    this->dataSize = 0;
    if(!this->stockFilePath.empty()) {
        ifstream inFile;
        inFile.open(this->stockFilePath);

        if (!inFile) {
            cerr << "Unable to open file";
            exit(1);   // call system to stop
        }

        string line;

        string date; //temp
        float open, high, low, close, volume;
        int year, month, day;
        int lineNum = 0;

        while(!inFile.eof()) {

            inFile >> date; // date in string
            sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day);
            inFile >> line;
            inFile >> line;
            inFile >> open; // open
            inFile >> line;
            inFile >> line;
            inFile >> high; // high
            inFile >> line;
            inFile >> line;
            inFile >> low; // low
            inFile >> line;
            inFile >> line;
            inFile >> close; // close
            inFile >> line;
            inFile >> line;
            inFile >> volume; // volume

            dataNodeLst[lineNum] = new stockNode(year, month, open, high, low, close, volume);
            lineNum++;
        }

        this->dataSize = lineNum;
        inFile.close();
    }
}

void stockData::loadEPSFromFile() {
    if(!this->epsFilePath.empty()) {
        ifstream EPSinFile;
        EPSinFile.open(this->epsFilePath);

        if (!EPSinFile) {
            cerr << "Unable to open file";
            exit(1);   // call system to stop
        }

        int counter = 0, year, month, day;
        float tempEps, sumEPS = 0;
        string line, date, newDate, preDate = "", whileStopper = "quarterlyEarnings:";

        if (!EPSinFile.eof()) {
            EPSinFile >> line;
            while (whileStopper.compare(line) != 0 && !EPSinFile.eof()) {
                EPSinFile >> line;
            }
        }

        for (int i = 0; i < this->dataSize; i++) {
            EPSinFile >> line;
            EPSinFile >> line;
            EPSinFile >> line;
            EPSinFile >> date; // date in string
            sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day);
            EPSinFile >> line;
            EPSinFile >> tempEps;
            EPSinFile >> line;
            EPSinFile >> line;
            EPSinFile >> line;
            EPSinFile >> line;
            EPSinFile >> line;

            sumEPS += tempEps;
            counter++;

            if (month >= 1 && month <= 3)
                month = 1;
            else if (month >= 4 && month <= 6)
                month = 2;
            else if (month >= 7 && month <= 9)
                month = 3;
            else if (month >= 10 && month <= 12)
                month = 4;
            else
                month = 0;

            newDate = to_string(year) + to_string(month);
            this->epsMap.insert(pair<string, float>(newDate, tempEps));
            EPSinFile >> line;

            if (newDate == preDate) {
                i = this->dataSize;
            }
            preDate = newDate;
        }
        EPSinFile.close();
        epsAverage = (float)sumEPS/counter;
    }
}

float stockData::getEPS(int year, int month) {
    if (month >= 1 && month <= 3)
        month = 1;
    else if (month >= 4 && month <= 6)
        month = 2;
    else if (month >= 7 && month <= 9)
        month = 3;
    else if (month >= 10 && month <= 12)
        month = 4;
    string date = to_string(year) + to_string(month);
    auto itr = this->epsMap.find(date);
    return itr->second;
}

string stockData::compareByPriceToEarnings() {
    string st = "";
    int spaceCounter;
    float highAverage;
    string stringToPrint;
    spaceCounter = 15;
    highAverage = getHighAverage();
    stringToPrint = stockName;
    spaceCounter -= stringToPrint.length();

    st += stringToPrint;
    while (spaceCounter > 0) {
        cout << " ";
        spaceCounter--;
    }
    st += to_string((int)(highAverage / epsAverage)) + "\n";
}

float stockData::getHighAverage() {
    int i;
    float sum = 0;
    for (i = 0 ; i < dataSize ; i++) {
        sum += dataNodeLst[i]->getHighRate();
    }
    return sum/i;
}

void stockData::exportDataToCSV() {
    ofstream csvFile;
    string fileName = stockName + ".csv";
    csvFile.open(fileName); // Create file

    if (!csvFile) {
        cerr << "Unable to create the csv file: " << fileName << endl;
        exit(1);   // call system to stop
    }

    csvFile << stockName << endl << endl;
    csvFile << " " << "," << "Year" << "," << "Month" << "," << "Open Rate" << "," << "High Rate" << ","
            << "Low Rate" << "," << "Close Rate" << "," << "Volume" << "," << "Reported EPS" << endl;

    for (int j = 0 ; j < dataSize ; j++) {
        csvFile << j + 1 << ","
                << dataNodeLst[j]->getYear() << ","
                << dataNodeLst[j]->getMonth() << ","
                << dataNodeLst[j]->getOpenRate() << ","
                << dataNodeLst[j]->getHighRate() << ","
                << dataNodeLst[j]->getLowRate() << ","
                << dataNodeLst[j]->getCloseRate() << ","
                << dataNodeLst[j]->getVolume() << ","
                << getEPS(dataNodeLst[j]->getYear(), dataNodeLst[j]->getMonth())
                << endl;
    }
    csvFile.close();
}

string stockData::stockNode::toString() {
    string res;
    res = "1. open: " + to_string(this->openRate).substr(0,7) + ", ";
    res += "2. high: " + to_string(this->highRate).substr(0,7) + ", ";
    res += "3. low: " + to_string(this->lowRate).substr(0,7) + ", ";
    res += "4. close: " + to_string(this->closeRate).substr(0,7) + ", ";
    res += "5. volume: " + to_string(this->volume) + ", ";
    return res;
}

stockData::stockNode::stockNode(int year, int month, float openRate, float highRate, float lowRate, float closeRate, int volume = -1) {
    this->year = year;
    this->month = month;
    this->openRate = openRate;
    this->highRate = highRate;
    this->lowRate = lowRate;
    this->closeRate = closeRate;
    this->volume = volume;
}

float stockData::stockNode::getOpenRate() const {return openRate;}

float stockData::stockNode::getLowRate() const {return lowRate;}

float stockData::stockNode::getCloseRate() const {return closeRate;}

int stockData::stockNode::getVolume() const {return volume;}

