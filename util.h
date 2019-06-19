#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <fstream>
#include "constants.h"
#include <cstring>
using namespace std;
// Classic helper function
class Util {

public:

static std::string convertToTime ( long int input_seconds );
static std::string getProgressBar(std::string percent);
static void getStream(std::string path, std::ifstream& stream);
static std::string readFirstLine(std::string file_path);
static vector<std::string> split_str(std::string str, std::string delimiter);
static vector<std::string> getProcessStat(string pid);
static string findLineStartingWith(string str, ifstream &file, string readline);
static string grabNumbersFromString(string str, string label);
};

std::string Util::convertToTime (long int input_seconds){
long minutes = input_seconds / 60;
long hours = minutes / 60;
long seconds = int(input_seconds%60);
minutes = int(minutes%60);
std::string result = std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds);
return result;
}
// constructing string for given percentage
// 50 bars is uniformly streched 0 - 100 %
// meaning: every 2% is one bar(|)
std::string Util::getProgressBar(std::string percent){

    std::string result = "0%% ";
    int _size= 50;
    int  boundaries;
    try {
        boundaries = (stof(percent)/100)*_size;
    } catch (...){
    boundaries = 0;
    }

    for(int i=0;i<_size;i++){
        if(i<=boundaries){
        result +="|";
        }
        else{
        result +=" ";
        }
    }

    result +=" " + percent.substr(0,5) + " /100%%";
    return result;
}

// wrapper for creating streams
void Util::getStream(std::string path, std::ifstream& stream){
    stream.open (path, std::ifstream::in);
    if (!stream && !stream.is_open()){
        stream.close();
        throw std::runtime_error("Non - existing PID");
    }
    //return stream;
}

std::string Util::readFirstLine(std::string file_path) {
  	ifstream file;
  	string readLine;
  	Util::getStream(file_path, file);
  	//while(std::getline (file,readLine)){ 
    //  cout << "Confirmation of data reading: " << readLine << endl;
    //}
  	std::getline(file,readLine);
  	file.close();
    return readLine;
}

vector<std::string> Util::split_str(std::string str, std::string delimiter) {
  vector<string> parsed;
  size_t found = 0;
  size_t current_delim_pos = str.find(delimiter);
  while (current_delim_pos != std::string::npos) {
    parsed.push_back(str.substr(found, current_delim_pos - found));
    found = current_delim_pos + 1;
    current_delim_pos = str.find(delimiter, found);
  }
  parsed.push_back(str.substr(found, current_delim_pos - found));
  return parsed;
}

vector<std::string> Util::getProcessStat(string pid) {
  string stat_path = Path::basePath() + pid + "/" + Path::statPath();
  string stat = Util::readFirstLine(stat_path);
  return Util::split_str(stat, " ");
}

string Util::findLineStartingWith(string str, ifstream &file, string readline) {
  while(std::getline (file,readline)) { 
    size_t index = readline.find(str);
    if (index != std::string::npos) {
      return readline;
    }
  }
}

string Util::grabNumbersFromString(string str, string label) {
  string str_minus_label = str.substr(label.size());
  //cout << "str_minus_label: " << str_minus_label << "\n";
  char c_found_str[str_minus_label.size() + 1];
  strcpy(c_found_str, str_minus_label.c_str());
  string found = "";
  for (char c : c_found_str) {
    //cout << "c_string:-" << c << "-\n";
    if (isdigit(c)) {
      found += c;
    }
  }
  return found;
}
#endif