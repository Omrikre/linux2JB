#!/bin/sh


# shellcheck disable=SC2068
for var in $@
do
	 wget -q --output-document=${var}.json "https://www.alphavantage.co/query?function=TIME_SERIES_MONTHLY&outputsize=full&symbol=${var}&apikey=HIJ2CDXZD8CZ790Z&datatype=json"
   sed -i '1,8d' ${var}.json                          # remove the start lines
   fileName=${var}.stock
   awk 'ORS=NR%7?FS:RS' ${var}.json > ${fileName}     # organize the to one line for each date
   sed -i 's/}//g' ${fileName}                       # remove all the } character
   sed -i 's/{//g' ${fileName}                       # remove all the } character
   sed -i 's/\"//g' ${fileName}                       # remove all the " character
   sed -i 's/,//g' ${fileName}                       # remove all the , character
   sed -i  's/\s\s*/ /g' ${fileName}                  # multi space to one space
   rm ${var}.json

###########################################################################################################


   wget -q --output-document=${var}_EPS.json "https://www.alphavantage.co/query?function=EARNINGS&symbol=${var}&apikey=HIJ2CDXZD8CZ790Z"
   sed -i '1,2d' ${var}_EPS.json                          # remove the start lines
   fileName=${var}.esp
   sed -i 's/}//g' ${var}_EPS.json                       # remove all the } character
   sed -i 's/{//g' ${var}_EPS.json                        # remove all the } character
   sed -i 's/\"//g' ${var}_EPS.json                       # remove all the " character
   sed -i 's/,//g' ${var}_EPS.json                        # remove all the , character
   sed -i 's/[//g' ${var}_EPS.json                       # remove all the [ character
   sed -i 's/\s\s*/ /g' ${var}_EPS.json                   # multi space to one space
   awk 'NF' ${var}_EPS.json > temp_file && mv temp_file ${var}_EPS.json

# annualEarnings
   awk '/annualEarnings/,/]/' ${var}_EPS.json > file1.esp
   sed -i '1d' file1.esp
   awk 'ORS=NR%4?FS:RS' file1.esp > temp_file && mv temp_file file1.esp
   sed -i 's/]//g' file1.esp   # remove all the ] character
   sed -i '1 i\annualEarnings:' file1.esp

# quarterlyEarnings
   awk '/quarterlyEarnings/,/]/' ${var}_EPS.json > file2.esp
   sed -i '1d' file2.esp
   awk 'ORS=NR%6?FS:RS' file2.esp > temp_file && mv temp_file file2.esp
   sed -i 's/]//g' file1.esp   # remove all the ] character
   sed -i '1 i\quarterlyEarnings:' file2.esp

   cat file1.esp file2.esp > ${fileName}
   rm file1.esp file2.esp ${var}_EPS.json
   #rm wget-log wget-log.1
   #sleep 20s                									# sleep because the API user
done
