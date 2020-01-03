# CACTI-Batch-Generator
This projects implements an interface to CACTI 6.5 simulator. It allows automatically generating simulation results for a range of memory configurations with just one command. CACTI is a cache, scratchpad and RAM memory simulation tool. 

## Installation

1. To get the CACTI Batch Generator up and running, you first need a working version of CACTI set up on you machine. This repository contains the CACTI version 6.5. You can set it up on your machine by running the following commands from a Linux terminal:

` git pull https://github.com/abuaesh/CACTI-Batch-Generator
cd cacti65
make
`
Now you should have the executable file _cacti_. 

2. The next step is to compile _cacti_if_. 

## Usage

### Using CACTI 6.5

1. To run CACTI , just modify the parameters in the _.cfg_ file to specify the parameters of the memory to be simulated. Note: You can find examples of _.cfg_ files in the same directory, like _cache.cfg_.
2. 	After you have configured the sepcifications in the _.cfg_ file, type in your terminal: 
`./cacti –infile [file.cfg ]`

**Note:** Your input file can have any name, not just _cache_, as long as it has _.cfg_ extension and is of the correct format. The results will be displayed in the console.

### Using the Batch Generator

To run the batch generator program implemented on top of CACTI, you need to:
1.	Make sure the CACTI version you installed works correctly by testing it with any configuration file. See the previous section [Installation](https://github.com/abuaesh/CACTI-Batch-Generator/blob/master/README.md#installation) for more details about using CACTI for a single file.
2.	Save the interface file: _cacti_if.cpp_ and the configuration file: _temp.cfg_ together in the same directory.
3.	Update the path to the installed version of CACTI in the file: _cacti_if.cpp_, line 178. 
4.	Compile _cacti_if.cpp_ using the g++ command, as follows: 
`g++ cacti_if.cpp -o cacti_if`
5.	Run the compiled file with the following parameters: 
`./cacti_if [start] [end] [step] [technology] [type]`
where start, end and step represent memory sizes in bytes. The available technologies for the technology field are 32(default), 45, 68 and 90 nm. And, type can have any value from 1 to 3; such that 1 stands for cache, 2 for scratchpad (default) and 3 for main memory (or RAM).

#### Understanding the Results
The program gives the time elapsed for running. The final results are saved in a file named after the type, technology and sizes. For example, to simulate a cache memory module --Type 1 memory- with sizes of 512 bytes, 528 B, 544 B, up to 1024 bytes--that is a step of 16 bytes- in the 32 nm technology, run the command:

 `./cacti_if 512 1024 16 32 1`
 
 which generates the file: _cache32nm512:1024:16.txt_ containing the desired data. Upon wrong invocation, an error message that explains the command in detail appears.  
 
 # More Information
 
 You can find more information about this project in the description document.



