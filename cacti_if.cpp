//including libraries:
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include<string>
#include<sstream>
using namespace std;

//Global vars:
int init_size = 512; //start exploring from this size in KB
int final_size = 1024; //explore up to this size in KB
int step = 4; //explore all possible sizes in Bytes

int tech = 90; //available technologies are: 32, 45, 68 and 90 nm
int type = 3; //memory types are: 1-cache-type 2-ram-type(SPM) 3-main-type


//Functions prototypes:
bool generate_cfgs(); //generates cfg files for different RAM configurations(reads from /cfgs directory).
bool run_cacti();	//runs cacti on all the generated cfgs. running results are saved in /outs directory.
bool extract_results();	//extracts energy, area & performance results from the files generated by cacti and saves these to one file for all RAM sizes

int main(int argc, char *argv[])
{
	 if (argc != 6) 
	{
		fprintf(stderr, "Incorrect usage of command cacti...\n");
		fprintf(stderr, "Usage:\n======\ncacti [start] [end] [step] [technology] [type]\n\n");
		fprintf(stderr, "Creates a file with energy, area and performance data for all memory sizes specified. ");
		fprintf(stderr, "Start, End and Step are in Bytes.\n");		
		fprintf(stderr, "Technologies: 32(default), 45, 68 and 90 (nm)\n");
		fprintf(stderr, "Types: 1-Cache\t2-SPM(default)\t3-Main Memory\n\n");
		fprintf(stderr, "Example:\n========\ncacti 512 1024 16 32 1\n\n");
		return -1;
	}
	
	init_size = atoi(argv[1]);
	final_size = atoi(argv[2]);
	step = atoi(argv[3]);
	tech = atoi(argv[4]);
	type = atoi(argv[5]);

	clock_t start_time,elapsed;

	double elapsed_time; 
	start_time = clock(); //reset clock

	if(generate_cfgs())
	{
		if(run_cacti())
			extract_results();
		else
		{
			cout << "Cannot run cacti" << endl;
			return -1;
		}
	}
	else
	{
		cout << "Cannot generate .cfg files" << endl;
		return -1;
	}

	elapsed = clock()-start_time;
	elapsed_time = elapsed / ((double) CLOCKS_PER_SEC); 
	cout << "\nTime elapsed: " << elapsed_time << " seconds.\n";
	
	return 0;
}

/*******************************************************************************************************************************
		***********************************************		GENERATE CFGS		***********************************************
*******************************************************************************************************************************/

bool generate_cfgs()
{
	bool tech_flag = false, type_flag = false;
	system("rm -rf cacti_cfgs"); //delete old cfgs dir and files
	system("mkdir cacti_cfgs"); //create an new dir for new cfgs

	//create destination files and copy the data from src file into them
	for (int i = init_size; i <= final_size; i+= step)
	{
		//reading from template file
		ifstream src;
		src.open("temp.cfg");
	
		if(src)
		{
				std::ostringstream s;
				s << "cacti_cfgs/" << i << ".cfg";
				string file = s.str();

				//writing to files
				ofstream dst;
				dst.open(file.c_str());

				if(dst)
				{
					string line;
					src.seekg(ios::beg);
					dst.seekp(ios::beg);
					
					if(getline(src, line)&&getline(src, line))
					{
						dst << "# Cache size\n";
						dst << "-size (bytes) " << i << endl;
					}

					while(getline(src, line))
					{
						if(tech_flag)
						{
							switch (tech)
							{
								case 32: line = "-technology (u) 0.032"; break;
								case 45: line = "-technology (u) 0.045"; break;
								case 68: line = "-technology (u) 0.068"; break;
								case 90: line = "-technology (u) 0.090"; break;
								default: line = "-technology (u) 0.032";
							}
							tech_flag = false;
						}
						if(type_flag)
						{
							switch (type)
							{
								case 1: line = "-cache type \"cache\""; break;
								case 2: line = "-cache type \"ram\""; break;
								case 3: line = "-cache type \"main memory\""; break;
								default: line = "-cache type \"ram\"";
							}
							type_flag = false;
						}
						dst << line << endl;
						if(line == "-UCA bank count 1")
							tech_flag = true;
						if(line == "//-cache type \"cache\"")
							type_flag = true;
					}//end while getline

					dst.close();
				}//end if dst
				else
				{
					cout << "Cannot open destination file" << file << endl;
					return false;
				}
			
		src.close();
		}//end if src
		else
		{
			cout << "Cannot open source file\n";
			return false;
		}
	
		
	}//end for i

	return true;
}

/*******************************************************************************************************************************
		***********************************************		RUN CACTI		***********************************************
*******************************************************************************************************************************/

bool run_cacti()
{
	int x = 0;
	system("rm -rf cacti_outs"); //delete old results
	system("mkdir cacti_outs"); //create new dir for new results

	//run the configuration files one by one and save their outputs
	for (int i = init_size; i <= final_size; i+= step)
	{
		std::ostringstream s;
		s << "./cacti -infile cacti_cfgs/" << i << ".cfg > cacti_outs/" << i << ".out" ; 
		string command = s.str();
		cout << "***********Executing the cacti command: \n" << command << endl;
		x = system(command.c_str());
		if(x != 0)
		{
			cout << "\nProblems running CACTI... Please make sure a working version of CACTI is installed\n";
			return false;
		}

	}
	return true;
}

/*******************************************************************************************************************************
		***********************************************		EXTRACT RESULTS		***********************************************
*******************************************************************************************************************************/

bool extract_results()
{
	//The result file's name contains details about the memory type, technology and sizes
	std::ostringstream s;
	s << ((type==1)?"cache":((type==2)?"SPM":"Main")) << tech << "nm" << init_size << ":" << final_size << ":" << step << ".txt";
	string file = s.str();

	cout << "\n" << file << "\n";

	//writing to the result file
	ofstream dst;
	dst.open(file.c_str());//this is where the extracted data goes
	
	if(dst)
	{
		//Writing header info to the file:
		dst << "Technology: " << tech <<" nm\n";
		dst << "Memory Type: " << ((type==1)?"cache (Type 1)":((type==2)?"SPM (Type 2)":"Main Memory (Type 3)")) << "\n\n";		
		dst << setfill('=') <<left << setw(100) << "";
		dst << endl;
		dst << setfill(' ') << setw(20) << "Size(B)" << setw(20) << "Dynamic Energy(nJ)" << setw(20) << "Static Power(mW)";
		dst << setw(20) << "Area(mm2)" << setw(20) << "Performance(ns)";
		dst << "\n";
		dst << setfill('=') <<setw(100) << "";
		dst << "\n";

		//read output files one by one 
		for (int i = init_size; i <= final_size; i+= step)
		{
			std::ostringstream s;
			s << "cacti_outs/" << i << ".out";
			string file = s.str();

			//reading from ith output file file
			ifstream src;
			src.open(file.c_str());
			
			if(src)
			{
				cout <<"extracting " << file <<"...\n";
				
				bool power_info_flag = false, area_info_flag = false, time_info_flag = false;
				double power_info=0, energy_info=0, area_info=0, time_info=0;
				string line;

				//read line by line from the ith file
				while(getline(src,line))
				{
					if(power_info_flag) //extract power info(static in mW and dynamic in nJ)
					{
						
						//extract dynamic energy info
						getline(src, line);//reading the next line 
						//Find the right para ')' after which comes the energy value
						energy_info = atof((line.substr(line.find(')')+3)).c_str()); //+3 to skip the colon and space b4 the value

						//extract static power info--the following line immediately
						getline(src, line);//reading the empty line first
						//Find the right para ')' after which comes the power value
						power_info = atof((line.substr(line.find(')')+3)).c_str()); //+3 to skip the colon and space b4 the value
						power_info_flag = false;
					}
					if(area_info_flag) //extract area info
					{
						getline(src, line);//reading the empty line first
						//Find the right para ')' after which comes the area value
						area_info = atof((line.substr(line.find(')')+3).c_str())); //+3 to skip the colon and space b4 the value
						area_info_flag = false;
					}
					if(time_info_flag) //extract time info
					{
						getline(src, line);//reading the empty line first

						//Find the colon ':' after which comes the time value
						time_info = atof((line.substr(line.find(':')+2).c_str())); //+2 to skip the space b4 the value
						
						time_info_flag = false;
					}


					if(line == "Power Components:") //extract power info from the next line to come
						power_info_flag = true;
					else
						if(line == "Area Components:") //extract area info from the next line to come
							area_info_flag = true;
						else 
							if(line == "Time Components:") //extract time info from the next line to come
								time_info_flag = true;
				}//end while(getline())

				src.close();
				//append the extracted info into the results file
				dst << setfill(' ') << setw(20) << i << setw(20) << energy_info;
				dst << setw(20) << power_info << setw(20) << area_info << setw(20) << time_info << "\n";
			}//end if src
			else
			{
				cout << "Cannot open source file " << file << " for extraction.\n";
				return false;
			}
		
		}//end for
	}//end if dst
	else
	{
		cout << "Cannot extract into the destination file." << endl;
		return false;
	}

	return true;

}
