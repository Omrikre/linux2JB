#include <stdio.h>
#include <json-c/json.h>
#include <string.h>
#include <zip.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#define MAXSIZE 2048


struct stock{
    char time[11];
    double open;
    double high;
    double low;
    double close;
    double volume;
    double eps;
    char stockName[100];
	int size;
};typedef struct stock stock;
stock** Stocks;
int size = 0;

void readFileFromZip(stock** Stocks,int numFiles);
int getZipSize();
void printStocks(stock** Stocks, int size);
int myGlobalStaticContinueVariable;
void endWork();
void makeCSV(stock* Stock);
stock* readStockJson(char* filename);
struct json_object* getParsed(char* fileName);
size_t getSize(struct json_object* parsed);
void getItforStock(struct json_object_iterator* it, struct json_object_iterator* itEnd, struct json_object* parsed);
void getStockData(stock* Stock, struct json_object* parsed);
char* fetchStock();
void readEPSjson(stock* Stock, char* stockname);
void saveDataInZip(stock** Stocks, int size);
void readBuffer(char* buff, stock* stcok);
void stockDataResponse(char response[MAXSIZE], stock Stock);



int main(){

	
	

	int cpid;

	int choicePipe[2];
	int responsePipe[2];
	pipe(responsePipe);
	pipe(choicePipe);

    cpid = fork();
	
	
	//Parent
    if (cpid > 0) {
		
		
		
		char response[MAXSIZE];
		
		int size = 0;
		int choice;
		while(1){

			printf("----------------------------\n");
			printf("Please choose one of the following:\n1) Fetch stock.\n2) List stocks in database.\n3) Print stock data.\n4) Zip all data.\n");
			
			scanf("%d", &choice);
			write(choicePipe[1], &choice, sizeof(int));
			if (choice == 1)
			{
				read(responsePipe[0], response, MAXSIZE);
				printf("%s", response);
			}
			else if(choice == 2){
				printf("Stocks in database:\n");
				read(responsePipe[0], response, MAXSIZE);
				printf("%s", response);
				
			}
			else if(choice == 3){
				read(responsePipe[0], &size, sizeof(int));
				for (size_t i = 0; i < size; i++)
				{
					read(responsePipe[0], response, MAXSIZE);
					printf("%s\n", response);
				}
			}
			else if(choice == 4){
				read(responsePipe[0], response, MAXSIZE);
				printf("%s", response);
			}
		}
    }

	//Child
    else if(cpid == 0){ 
		signal(SIGINT, endWork);
		char stockName[100], response[MAXSIZE];
		int numFiles=getZipSize();
		Stocks = (stock**)malloc(sizeof(stock*)*100);
		if (numFiles > 0)
		{
			size=numFiles;
			readFileFromZip(Stocks,numFiles);
		}
		int choice;
		while (1)
		{
			read(choicePipe[0], &choice, sizeof(int));
			switch (choice)
			{
			case 1:
				strcpy(stockName, fetchStock());
				Stocks[size]=readStockJson(stockName);
				readEPSjson(Stocks[size], stockName);
				size+=1;
				write(responsePipe[1], "Stock has been feched\n", strlen("Stock has been feched\n")+ 1);
				break;
			case 2:
				response[0] = '\0';
				for (int i = 0; i < size; i++){
					
					strcat(response, Stocks[i][0].stockName);
					if(size-1 != i)
						strcat(response, ", ");
				}
				strcat(response, "\n");
				write(responsePipe[1], response, strlen(response)+ 1);
				break;
			case 3:
			printf("Enter stock name: \n");
				scanf("%s", stockName);
					for (size_t i = 0; i < size; i++)
					{
						if(strstr(Stocks[i][0].stockName, stockName)){
							write(responsePipe[1], &(Stocks[i][0].size), sizeof(int));
							for (size_t k = 0; k < Stocks[i][0].size; k++){
								stockDataResponse(response, Stocks[i][k]);
								usleep(200);
								write(responsePipe[1], response, strlen(response)+1);
							}
						}
					}
				break;
			case 4:
				saveDataInZip(Stocks,size);
				write(responsePipe[1], "Data has been zipped\n", strlen("Data has been zipped\n") + 1);
				break;
			default:
				break;
			}
		} 0;
		

	}
}

void stockDataResponse(char response[MAXSIZE], stock Stock){
	char temp[MAXSIZE];
	strcpy(response, "Time: ");
	strcat(response, Stock.time);
								
	strcat(response, ", Open: ");
	sprintf(temp, "%lf", Stock.open);
	strcat(response, temp);
	
	strcat(response, ", High: ");
	sprintf(temp, "%lf", Stock.high);
	strcat(response, temp);

	strcat(response, ", Low: ");
	sprintf(temp, "%lf", Stock.low);
	strcat(response, temp);

	strcat(response, ", Close: ");
	sprintf(temp, "%lf", Stock.close);
	strcat(response, temp);

	strcat(response, ", Volume: ");
	sprintf(temp, "%lf", Stock.volume);
	strcat(response, temp);
	
	strcat(response, ", EPS: ");
	sprintf(temp, "%lf", Stock.eps);
	strcat(response, temp);
}

void readBuffer(char* buff, stock* Stock){
	int column=0, row=0;
	char* line;
	char* value;
	char* temp;
	
	line = strsep(&buff, "\n");
	line = strsep(&buff, "\n");
	row++;
	while(line != NULL){
		value = strtok(line, ", ");
        while (value) {
         // Column 1
       		if (column == 0) {
            	strcpy(Stock[row-1].time, value);
       		}
        // Column 2
        	if (column == 1) {
                Stock[row-1].open = strtod(value, &temp);
        	}
 
        // Column 3
       		if (column == 2) {
                Stock[row-1].high = strtod(value, &temp);
        	}
			if (column == 3){
				Stock[row-1].low = strtod(value, &temp);
			}
			if (column == 4){
				Stock[row].close = strtod(value, &temp);
			}
			if (column == 5){
				Stock[row-1].volume = strtod(value, &temp);
			}
			if (column == 6){
				Stock[row-1].eps = strtod(value, &temp);
			}
                
        	value = strtok(NULL, ", ");
        	column++;
     	}
		row++;
		column=0;
		line = strsep(&buff, "\n");
	}

}

void readFileFromZip(stock** Stocks,int numFiles){

	int error;
	int size=0;
	zip_file_t * file;
	char buf[1000000];
	int j=0;
	int len;

	zip_t* zipper= zip_open("stock_db.zip", ZIP_RDONLY, &error);
	for(int i=0;i<numFiles;++i){
		file=zip_fopen(zipper,zip_get_name(zipper,i,0),0);
		zip_fread(file,buf,100000);
		while(buf[j]!='\0'){
			if(buf[j]=='\n'){
				size++;
			}
			j++;
		}
		Stocks[i]=(stock*)malloc(sizeof(stock)*size+1);
		readBuffer(buf, Stocks[i]);
		strcpy(Stocks[i][0].stockName,zip_get_name(zipper,i,0));
		len= strlen(Stocks[i][0].stockName);
		for (size_t k = 0; k < len; k++)
		{
			if (Stocks[i][0].stockName[k] == '.')
			{
				Stocks[i][0].stockName[k] = '\0';
				
			}
			
		}
		
		Stocks[i][0].size = size;
		
		size=0;
		j=0;
	}
}

int getZipSize(){
	int res=0;
	int error;
	zip_t* zipper= zip_open("stock_db.zip", ZIP_RDONLY, &error);
	
	res=zip_get_num_entries(zipper,0);
	//printf("%d",res);
	zip_close(zipper);
	return res;

}

void printStocks(stock** Stocks,int size){
	for (int i = 0; i < size; i++)
	{
		printf("%d) %s\n", (i+1), Stocks[i][0].stockName);
	}
	
}

void endWork(int singnum){
	if(singnum == SIGINT){
		printf("Creating zip and exiting.");
		saveDataInZip(Stocks, size);
		exit(1);
		
	}
	
}
void saveDataInZip(stock** Stocks, int size){
	for(int i = 0; i<size; i++){
		makeCSV(Stocks[i]);
	}
	int error;
	char name[100];
	zip_t* zipper= zip_open("stock_db.zip", ZIP_CREATE, &error);
	struct zip_source* source;
	for(int i = 0; i<size; i++){
		strcpy(name, Stocks[i][0].stockName);
		strcat(name, ".csv");
		source = zip_source_file(zipper, name, 0,0);
		zip_file_add(zipper, name, source, 0);
	}
	zip_close(zipper);
	//printf("Zip was Created.");
	
} 

void makeCSV(stock* Stock){
	FILE* file;
	char name[100];
    int size=Stock[0].size;
    strcpy(name, Stock[0].stockName);
    strcat(name, ".csv");
    file = fopen(name, "w+");
    fprintf(file,"Time, Open, High, Low, Close, Volume, EPS\n");
    for(int i=0;i<size;i++){
        fprintf(file,"%s, %.3lf, %.3lf, %.3lf, %.3lf, %.3lf, %.3lf\n", Stock[i].time, Stock[i].open, Stock[i].high, Stock[i].low, Stock[i].close, Stock[i].volume, Stock[i].eps);
    }
    fclose(file);
}

void readEPSjson(stock* Stock, char* stockname){
	char espfile[100];
	char date[20];
	char year[5];
	strcpy(espfile, stockname);
	strcat(espfile, ".EPS.json");
	struct json_object* parsed = getParsed(espfile);
	struct json_object* annualEarnings;
	struct json_object* reportedEPS;
	struct json_object* fiscalDateEnding;
	json_object_object_get_ex(parsed, "annualEarnings", &annualEarnings);
	size_t size = json_object_array_length(annualEarnings);
	for (size_t i = 0; i < size; i++)
	{
		parsed= json_object_array_get_idx(annualEarnings, i);
		json_object_object_get_ex(parsed, "fiscalDateEnding", &fiscalDateEnding);
		json_object_object_get_ex(parsed, "reportedEPS", &reportedEPS);
		strncpy(year, json_object_get_string(fiscalDateEnding), 4);
		for (size_t k = 0; k < Stock[0].size; k++)
		{
			if(strstr(Stock[k].time, year) != NULL){
				Stock[k].eps = json_object_get_double(reportedEPS);
			}
		}

		

	}
	//Print the stock data.
	/*
	for (size_t k = 0; k < Stock[0].size; k++){
		printf("Time: %s, Open: %.3lf, High: %.3lf, Low: %.3lf, Close: %.3lf, Volume: %.3lf, ReportedEPS: %.3lf\n", 
		Stock[k].time, Stock[k].open, Stock[k].high, Stock[k].low, Stock[k].close, Stock[k].volume, Stock[k].eps);
	}*/
	
	
}


char* fetchStock(){
	char *Stock=(char*)malloc(100);
	char name[100]= "./get_stock_data.sh ";
	printf("type stock name ->");
	scanf("%s",Stock);
	strcat(name,Stock);
	system(name);
	return Stock;
}

stock* readStockJson(char* stockname){
	char stockfile[100];
	
	
	strcpy(stockfile, stockname);
	strcat(stockfile, ".stock.json");

	struct json_object* parsed= getParsed(stockfile);
 	stock* Stock;
	size_t size=getSize(parsed);
	int i=0;
	Stock = (stock*)malloc(sizeof(stock)*size+1);
	Stock[0].size=size;
	getStockData(Stock, parsed);
	strcpy(Stock[0].stockName, stockname);
	

	
	return Stock;
}

struct json_object* getParsed(char* fileName){
	FILE* fp;
	char buffer[100000];
    struct json_object *parsed;
	fp=fopen(fileName, "r");
	if(!fp){
		exit(1);
	}
	fread(buffer, 100000, 1, fp);
	fclose(fp);
	parsed = json_tokener_parse(buffer);
	return parsed;
	
}

size_t getSize(struct json_object* parsed){
	size_t size = 0;
	struct json_object_iterator it;
 	struct json_object_iterator itEnd;
	
	getItforStock(&it, &itEnd, parsed);
	
 	while (!json_object_iter_equal(&it, &itEnd)) {
		size++;
     	json_object_iter_next(&it);
		
 	}
	return size;
}

void getItforESP(struct json_object_iterator* it, struct json_object_iterator* itEnd, struct json_object* parsed){
	struct json_object *annualEarnings;
	json_object_object_get_ex(parsed, "annualEarnings", &annualEarnings);
	*it = json_object_iter_begin(annualEarnings);
 	*itEnd = json_object_iter_end (annualEarnings);
}

void getItforStock(struct json_object_iterator* it, struct json_object_iterator* itEnd, struct json_object* parsed){
	struct json_object *AdjustedTimeSeries;
	json_object_object_get_ex(parsed, "Monthly Adjusted Time Series", &AdjustedTimeSeries);
	*it = json_object_iter_begin(AdjustedTimeSeries);
 	*itEnd = json_object_iter_end(AdjustedTimeSeries);
}

void getStockData(stock* Stock, struct json_object* parsed){
	struct json_object *date;
	struct json_object *AdjustedTimeSeries;
	struct json_object *high;
	struct json_object *low;
	struct json_object *open;
	struct json_object *closed;
	struct json_object *volume;
	struct json_object_iterator it;
 	struct json_object_iterator itEnd;
	getItforStock(&it, &itEnd, parsed);
	json_object_object_get_ex(parsed, "Monthly Adjusted Time Series", &AdjustedTimeSeries);
	int i=0;
	while (!json_object_iter_equal(&it, &itEnd)) {
		json_object_object_get_ex(AdjustedTimeSeries, json_object_iter_peek_name(&it), &date);
		json_object_object_get_ex(date, "1. open", &open);
		json_object_object_get_ex(date, "2. high", &high);
		json_object_object_get_ex(date, "3. low", &low);
		json_object_object_get_ex(date, "4. close", &closed);
		json_object_object_get_ex(date, "6. volume", &volume);
		strcpy(Stock[i].time, json_object_iter_peek_name(&it));
		Stock[i].open=json_object_get_double(open);
		Stock[i].high=json_object_get_double(high);
		Stock[i].low=json_object_get_double(low);
		Stock[i].close=json_object_get_double(closed);
		Stock[i].volume=json_object_get_double(volume);
     	json_object_iter_next(&it);
		i++;
 	}
}