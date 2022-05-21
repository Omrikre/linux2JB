//
// Created by barsela on 5/13/22.
//

#ifndef EX2LINUX_CLION_ZIPUTILITIES_H
#define EX2LINUX_CLION_ZIPUTILITIES_H
#include <iostream>
#include "stockData.h"
#include <zip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <zip.h>
#include <vector>
#include <set>
using namespace std;

class ZipUtilities {
public:
    const char* DB_NAME = "stocks_db.zip";
    const char *prg;
    static void safe_create_dir(const char *dir);
    void createDB(set <string> stocksList);
    set <string>  extractZIP();
};
#endif //EX2LINUX_CLION_ZIPUTILITIES_H
