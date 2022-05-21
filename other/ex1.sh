#!/bin/bash


if ! awk -W version > /dev/null; then
   echo -e "awk not found! Install? (y/n) \c"
   read
   if "$REPLY" = "y"; then
      sudo apt-get install command
   fi
fi

if ! wget -V > /dev/null; then
   echo -e "wget not found! Install? (y/n) \c"
   read
   if "$REPLY" = "y"; then
      sudo apt-get install command
   fi
fi

if ! g++ -v > /dev/null; then
   echo -e "g++ not found! Install? (y/n) \c"
   read
   if "$REPLY" = "y"; then
      sudo apt-get install command
   fi
fi

# facebook - FB
# amazon - AMZN
# Netflix - NFLX
# Goolge - GOOG
# apple - AAPL
declare -a stockArray=("GOOG" "AMZN" "NFLX" "AAPL" "FB") 
#declare -a stockArray=("FB")

for str in "${stockArray[@]}"
do
   wget --output-document=${str}.json "https://www.alphavantage.co/query?function=TIME_SERIES_MONTHLY&outputsize=full&symbol=${str}&apikey=HIJ2CDXZD8CZ790Z&datatype=json"
   sed -i '1,8d' ${str}.json                          # remove the start lines
   fileName=${str}.stock
   awk 'ORS=NR%7?FS:RS' ${str}.json > ${fileName}     # orgnize the to one line for each date
   sed -i 's/}//g' ${fileName}                       # remove all the } charcter
   sed -i 's/{//g' ${fileName}                       # remove all the } charcter
   sed -i 's/\"//g' ${fileName}                       # remove all the " charcter
   sed -i 's/,//g' ${fileName}                       # remove all the , charcter
   sed -i  's/\s\s*/ /g' ${fileName}                  # multi space to one space
   rm ${str}.json

###########################################################################################################


   wget --output-document=${str}_EPS.json "https://www.alphavantage.co/query?function=EARNINGS&symbol=${str}&apikey=HIJ2CDXZD8CZ790Z"
   sed -i '1,2d' ${str}_EPS.json                          # remove the start lines
   fileName=${str}.esp
   sed -i 's/}//g' ${str}_EPS.json                       # remove all the } charcter
   sed -i 's/{//g' ${str}_EPS.json                        # remove all the } charcter
   sed -i 's/\"//g' ${str}_EPS.json                       # remove all the " charcter
   sed -i 's/,//g' ${str}_EPS.json                        # remove all the , charcter
   sed -i 's/[//g' ${str}_EPS.json                       # remove all the [ charcter
   sed -i  's/\s\s*/ /g' ${str}_EPS.json                   # multi space to one space
   awk 'NF' ${str}_EPS.json > temp_file && mv temp_file ${str}_EPS.json

# annualEarnings
   awk '/annualEarnings/,/]/' ${str}_EPS.json > file1.esp
   sed -i '1d' file1.esp
   awk 'ORS=NR%4?FS:RS' file1.esp > temp_file && mv temp_file file1.esp
   sed -i 's/]//g' file1.esp   # remove all the ] charcter
   sed -i '1 i\annualEarnings:' file1.esp

# quarterlyEarnings
   awk '/quarterlyEarnings/,/]/' ${str}_EPS.json > file2.esp
   sed -i '1d' file2.esp
   awk 'ORS=NR%6?FS:RS' file2.esp > temp_file && mv temp_file file2.esp
   sed -i 's/]//g' file1.esp   # remove all the ] charcter
   sed -i '1 i\quarterlyEarnings:' file2.esp

   cat file1.esp file2.esp > ${fileName}
   rm file1.esp file2.esp ${str}_EPS.json
done

