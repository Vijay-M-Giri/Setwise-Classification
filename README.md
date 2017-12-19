# Setwise-Classification

## Initial reading
Read 'Setwise classification problem' paper by Charu agarwal.

## Data

### Prepare the data set

- Get the data for website fingerprinting from [here](http://traces.cs.umass.edu/index.php/Network/Network). It is listed in 2006 [paper](http://forensics.umass.edu/pubs/liberatore.ccs2006.pdf). Also read about website fingerprinting from here
- Once you download the data you need to parse it. Run the python script 'pcap.py' in Data Generation folder. Set the 'base' variable to appropriate file location.
  - The directory structure should be as follows:
    - pcap-files
	  - pcap-logs-0
	    - 2006-02-10T13:12:13.301721
		- ...
	  - pcap-logs-1
		- ...
	  - ...
- Now the data will be stored in the file 'data.data'. This file can be very large. First try to run the script with limited data.

### Normalize the data

- Open 'normalize.cc' file in Data Generation folder.
- The code is specific for forest cover data set. 
- Change the value of N (total number of entries in the file) and D (number of columns in the file)
- Change the file names in the code accordingly
- Run the code `g++ -std=c++11 normalize.cc`

### Divide the data

- Open 'divide_data.cc' file in Data Generation folder
- The code is specific for forest cover data set.
- Change the value of N (total entries), M (number of entries in initial stream), D (Number of columns - 1)
- In line no. 15 : the number(300) represents number of entities for testing. Change this as per requirement
- In line no. 16 : the number(1500) represents total number of entities. Change this as per requirement
- In line no. 29 : the number(10) represents total number classes. Change this as per requirement
- Also change the file names accordingly
- Run the code `g++ -std=c++11 divide_data.cc`

### Inter arrival time

- Open 'generate_time.cc'
- Change the value of N for some large number that is greater the number of entries in the stream
- Run the program `g++ -std=c++11 -lgsl -lgslcblas generate_time.cc`

## Running main program

- Some of the inputs are to be hard coded. (If someone is interested change the code so that every input can be externally controlled. :P)
- In line no.6 of 'R-Tree/index.h' file the NUMDIMS has to be changed. It represents the number of anchor points (Q)
- In 'main.c' file change the file name strings. (line no.18 - 21)
- A makefile is already present. Run `make`
- While executing, -q flag represents the number of anchor points, -m flag represents min number of points in entities, -f represents fraction of updates (read the charu agarwal paper to understand this)
- Sample execution: `./a.out -q 50 -m 10 -f 0.1`

## Future work

- Consider only k best trees for each query.
- Get some statistic for optimum value of k.
- Compare it with ground truth and if results are still bad think of some modification in the algorithm.
