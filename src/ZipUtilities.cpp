//
// Created by ZipUtilitiessela on 5/13/22.
//

#include "../include/ZipUtilities.h"

void ZipUtilities::safe_create_dir(const char *dir)
{
    if (mkdir(dir, 0755) < 0) {
        if (errno != EEXIST) {
            perror(dir);
            exit(1);
        }
    }
}
void ZipUtilities::createDB(set <string> stocksSet)
{
    vector <stockData> stocksList;
    set<string>::iterator itr;
    for (itr = stocksSet.begin(); itr != stocksSet.end(); itr++) {
        stocksList.push_back(stockData(*itr));
    }


    int err =0 ;
    struct zip *newDB = zip_open(DB_NAME,ZIP_CREATE,&err);

    for (int i = 0; i < stocksList.size(); i++)
    {
        string esp = stocksList[i].getStockName() + ".csv";
        //int size_esp = esp.length();
        char* espChar = &esp[0];
        zip_source* newDBsource = zip_source_file(newDB,espChar,0,0);
        zip_file_add(newDB,espChar,newDBsource,0);
    }
    zip_close(newDB);
}
set <string>  ZipUtilities::extractZIP() {
    set <string> stockNameSet;
    int err = 0;
    zip *z = zip_open(DB_NAME, 0, &err);
    struct zip_stat st;

    for (int i = 0; i < zip_get_num_entries(z, 0); i++) {
        zip_stat_index(z, i, 0, &st);
        char *contents = new char[st.size];
        zip_file *f = zip_fopen(z, st.name, 0);
        zip_fread(f, contents, st.size);
        zip_fclose(f);

        ofstream outfile(st.name);
        outfile << contents;
        outfile.close();

        delete[] contents;
        string s(st.name);
        stockNameSet.insert(s.substr(0,s.find(".")));
    }
    zip_close(z);
    return stockNameSet;
}



