
#include "include/main.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <sstream>


int main()
{
    cout << endl << endl;
    loadDB();
    signal(SIGINT, sigint_handler);
    pid_t p;

/// ----- pipe -----
/// first pipe - parent to worker
/// the parent send data(which task to do and more info) to worker
/// the worker read this data
    if (pipe(fd1) == -1) {
        printf("Pipe Failed: %s", stderr);
        return 1;
    }
/// second pipe - worker to parent
/// the worker send the result of the task to worker
/// the parent read this data
    if (pipe(fd2) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
/// ----- fork -----
    p = fork();
    if (p < 0) {
        fprintf(stderr, "fork Failed");
        return 1;
    }
/// ----- parent process -----
    else if (p > 0) {
        parent();
    }
/// ----- child process -----
    else
        worker();
    return 0;
}

/// ----- child process -----
void worker() {
    close(fd1[WRITE]); /// Close writing end of first pipe
    close(fd2[READ]);/// Close reading end of second pipe
    while (true) { /// loop until we get ^C signal
        workerPid = getpid();
        int readSize = 2000;
        char stringFromWorker[readSize]; /// which task to do and task data
        char workerResponse[readSize]; /// the result of the task
        string runSymbol, symbol, symbolWithStock, result;
        read(fd1[READ], stringFromWorker, readSize); /// read the data
        /// convert the string - the first char is the number option
        string st(stringFromWorker);
        string choose_st = st.substr(0, st.find("-"));
        st = st.substr(st.find("-")+1,st.length()); /// delete the number option and -
        int choose = stoi(choose_st); /// convert the number option to int
        Mission m;


        switch (choose) /// task
        {
            case 1: {/// fetch stock data
                symbol = st.substr(0, st.find("-"));
                runSymbol = "./fetchStockData.sh " + symbol;
                system(runSymbol.c_str());
                symbolWithStock = symbol + ".stock";
                ifstream f(symbolWithStock.c_str());
                if(f.good() && f.peek() != std::ifstream::traits_type::eof()) {
                    availableStockList.insert(symbol);
                    result = "\nFetch " + symbol + " DONE\n";
                }
                else {
                    result = "\nError while trying Fetch " + symbol + "\n";
                }
                strcpy(workerResponse, &result[0]);
                break;
            }
            case 2: { /// make a list of all the fetched stocks in convert it to string with space between the stocks
                /*string result = m.list_fetched_stocks();
                strcpy(workerResponse, &result[0]);
                result = "echo \"" + result + "\"";
                system(result.c_str());
                */
                result = "Available stocks: ";
                for (auto it : availableStockList) {
                    result = result + it + " ";
                }
                strcpy(workerResponse, &result[0]);
                break;
            }

            case 3: { /// print stock data
                string stockName;
                int year;
                /// the stock name and the year separate with -
                stockName = st.substr(0, st.find("-"));
                st = st.substr(st.find("-")+1,st.length());
                year = stoi(st);
                string result = m.PrintStockData(stockName,year);
                strcpy(workerResponse, &result[0]);
                break;
            }

            case 4: { /// create and save all csv file
                string result = "\nList of fetched stocks: \n";
                result += m.exportAndCreateDBStocksData();
                strcpy(workerResponse, &result[0]);
                break;
            }
        }
        /// Write the result of the task
        write(fd2[WRITE], workerResponse, readSize);
    }
}

void parent(){
    parentPid = getpid();
    close(fd1[READ]); /// Close reading end of first pipe
    close(fd2[WRITE]); /// Close writing end of second pipe
    string symbol;

    while (true)
    {
        cout << endl;
        printMenu();
        int input = userInput();
        string st = to_string(input)+"-";  /// separate between the input to other data
        switch (input) {
            case 1:
                cout << "Enter stocks symbol to fetch: ";
                cin >> symbol;
                st += symbol;
                break;
            case 3:
                st += userInputWhichStockAndYear();
                break;
        }

        char input_str[100];
        strcpy(input_str, &st[0]);
        int readSize = 2000;
        char stringFromWorker[readSize];

        /// Write input string
        write(fd1[WRITE], input_str, strlen(input_str) + 1);

        /// Read string from child, print it
        read(fd2[READ], stringFromWorker, readSize);
        cout << stringFromWorker << endl;
    }
}

void printMenu(){
    std::cout << "MENU:" << endl;
    std::cout << "1 - Fetch stock data" << endl;
    std::cout << "2 - List fetched stocks" << endl;
    std::cout << "3 - print stocks data" << endl;
    std::cout << "4 - Save all stocks data" << endl;
    std::cout << "Make your choice (1/2/3/4):" << endl;
}
int userInput(){
    /// get input and check if it is valid input
    int input = 0, preInput = 0;
    bool printed = false;
    do
    {
        std::cin >> input;
        if (preInput != input)
            printed = false;
        if (input < 1 || input > 4) {
            if(!printed) {
                std::cout << "Invalid Input, must be a value between 1 - 4 " << endl;
                std::cout << "Please try again " << endl;
                printed = true;
            }
        }
    } while (input < 1 || input > 4);
    return input;
}
string userInputWhichStockAndYear() {///for print stock data task
    string stockSymbol;
    string year;
    cout << endl << "Enter one stock symbol and the year after"<<endl;
    cout << "Stock symbol: ";
    cin >> stockSymbol;
    cout << "Year: ";
    cin >> year;
    return stockSymbol+"-"+year;
}

void sigint_handler(int signum) /// signal handler - do task 4 when we get ^C
{
    Mission m;
    if (getpid() == workerPid) /// child process will do mission 4 and exit
    {
        m.exportAndCreateDBStocksData();
    }
    cleanup();
    exit(1);
}

void loadDB (){/// EXTRACT THE FILES FROM THE ZIP FILE
    FILE *file;
    ZipUtilities DB;
    if ((file = fopen(DB.DB_NAME, "r"))) {
        fclose(file);
        availableStockList = DB.extractZIP();
    }
    else{
		cout << "There isn't available DB"<<endl;
	}
}
void cleanup()///close all the pipe
{
    close(fd1[READ]);
    close(fd1[WRITE]);

    close(fd2[READ]);
    close(fd2[WRITE]);

    system("rm -f *.stock");
    system("rm -f *.esp");

}

void importDataFromCSV(string stockName) {
    fstream csvFile;
    string fileName = stockName + ".csv";
    csvFile.open(fileName); // open the csv file

    if (!csvFile) { // check if exist
        cerr << "Unable to open the csv file: " << fileName << endl;
        exit(1);   // call system to stop
    }

    // open the stock file for the data
    fstream stockFile;
    string stockFileName = stockName + ".stock";
    stockFile.open(stockFileName); // open the stock file
    if (!stockFile) { // check if exist
        cerr << "Unable to create the stock file: " << fileName << endl;
        exit(1);   // call system to stop
    }

    string tmpWord, year, month, openRate, highRate, lowRate, closeRate, volume, reportedEPS;
    string tmpLine;

    getline(csvFile, tmpLine); // get first row - name of the stock
    getline(csvFile, tmpLine); // get second row - headers

    // Read the input
    while(!csvFile.eof()) {
        getline(csvFile, tmpLine);

        stringstream lineStream(tmpLine);
        lineStream >> year;
        lineStream >> tmpWord; // read ","
        lineStream >> month;
        lineStream >> tmpWord; // read ","
        lineStream >> openRate;
        lineStream >> tmpWord; // read ","
        lineStream >> highRate;
        lineStream >> tmpWord; // read ","
        lineStream >> lowRate;
        lineStream >> tmpWord; // read ","
        lineStream >> closeRate;
        lineStream >> tmpWord; // read ","
        lineStream >> volume;
        lineStream >> tmpWord; // read ","
        lineStream >> reportedEPS;
        lineStream >> tmpWord; // read ","

        // print to destination file
        stockFile << year << "-" << month << ": 1. open: " << openRate
        << " 2. high: " << highRate << " 3. low: " << lowRate
        << " 4. close: " << closeRate << " 5. volume: "  << volume
        << " 6. reportedEPS: " << reportedEPS << endl;
    }

    csvFile.close();
    stockFile.close();
}


