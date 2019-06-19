#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"
#include "util.h"
#include <dirent.h>


using namespace std;

enum CPUStates{
	S_USER = 1,
	S_NICE,
	S_SYSTEM,
	S_IDLE,
	S_IOWAIT,
	S_IRQ,
	S_SOFTIRQ,
	S_STEAL,
	S_GUEST,
	S_GUEST_NICE
};

class ProcessParser{
  private:
    std::ifstream stream;
  public:
  	static string getCmd(string pid); //
    static vector<string> getPidList(); //
    static std::string getVmSize(string pid); //
    static std::string getCpuPercent(string pid); //
    static long int getSysUpTime(); //
    static std::string getProcUpTime(string pid); //
    static string getProcUser(string pid); //
    static vector<string> getSysCpuPercent(string coreNumber = "");  //
    static float getSysRamPercent(); //
    static string getSysKernelVersion(); //
    static int getNumberOfCores(); //
    static int getTotalThreads(); //
    static int getTotalNumberOfProcesses(); //
    static int getNumberOfRunningProcesses(); //
    static string getOSName(); //
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid); //
    static float get_sys_idle_cpu_time(vector<string> values);
    static float get_sys_active_cpu_time(vector<string> values);
};

// TODO: Define all of the above functions below:
string ProcessParser::getCmd(string pid) {
  	string file_path = Path::basePath() + pid + Path::cmdPath();
  	return Util::readFirstLine(file_path);
}

vector<string> ProcessParser::getPidList() {
  vector<string> list = {};
  
  string base_path = Path::basePath();
  char c_base_path[base_path.size() + 1];
  strcpy(c_base_path, base_path.c_str());
  
  DIR* directory = opendir(c_base_path);
  if (!directory) {
  	 throw std::runtime_error("getPidList -- Directory doesn't exist");
  }
  while (dirent* dir_entry = readdir(directory)) {
    if (dir_entry->d_type == DT_DIR) {
      char* name = dir_entry->d_name;
      if (isdigit(name[0])) {
        list.push_back(name);
      }
    }
  }
  closedir(directory);
  return list;
}

std::string ProcessParser::getVmSize(string pid) {
  string file_path = Path::basePath() + pid + Path::statusPath();
  ifstream file;
  string readLine;
  Util::getStream(file_path, file);
  string vm_size = "";
  while(std::getline (file,readLine)){ 
    size_t index = readLine.find("VmData");
    if (index != std::string::npos) {
      char c_found_str[readLine.size() + 1];
  	  strcpy(c_found_str, readLine.c_str());
     
      for (char c : c_found_str) {
        if (isdigit(c)) {
          vm_size += c;
        }
      }
      break;
    }
  }
  file.close();
  return vm_size;
}

long int ProcessParser::getSysUpTime() {
  string uptime_path = Path::basePath() + Path::upTimePath();
  string uptime = Util::readFirstLine(uptime_path);
  vector<string> parsed = Util::split_str(uptime, " ");
  return stol(parsed[0]);
}

std::string ProcessParser::getProcUpTime(string pid) {
  vector<string> parsed = Util::getProcessStat(pid);
  float utime = stof(parsed[13]);
  float hertz = sysconf(_SC_CLK_TCK);
  
  return to_string(utime/hertz);
}

string ProcessParser::getProcUser(string pid) {
  string file_path = Path::basePath() + pid + Path::statusPath();
  ifstream file;
  string readLine;
  Util::getStream(file_path, file);
  string uid_str = Util::findLineStartingWith("Uid", file, readLine);
  string uid_minus_title = uid_str.substr(5);
  char c_found_str[uid_minus_title.size() + 1];
  strcpy(c_found_str, uid_minus_title.c_str());
  string uid = "";
  for (char c : c_found_str) {
    if (isdigit(c)) {
      uid += c;
    } else {
      break;
    }
  }
  file.close();
  
  string user_file_path = "/etc/passwd";
  ifstream user_file;
  string user_readline;
  Util::getStream(user_file_path, user_file);
  string user_line = Util::findLineStartingWith("x:" + uid, user_file, user_readline);
  user_file.close();
  return user_line.substr(0, user_line.find(":"));
}

std::string ProcessParser::getCpuPercent(string pid) {
  float uptime = ProcessParser::getSysUpTime();

  vector<string> parsed = Util::getProcessStat(pid);
  
  float utime = stof(parsed[13]);
  float stime = stof(parsed[14]);
  float cutime = stof(parsed[15]);
  float cstime = stof(parsed[16]);
  float starttime = stof(parsed[21]);
  float hertz = sysconf(_SC_CLK_TCK);
  
  float total_time = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime / hertz);
  float cpu_usage = 100.0 * ((total_time / hertz) / seconds);
  
  return to_string(cpu_usage);
}

int ProcessParser::getNumberOfCores() {
  ifstream file;
  string readline;
  Util::getStream((Path::basePath() + Path::cpuInfoPath()), file);
  string cpu_cores = Util::findLineStartingWith("cpu cores", file, readline);
  string cores = cpu_cores.substr(cpu_cores.find(":") + 1);
  file.close();
  return stoi(cores);
}

vector<string> ProcessParser::getSysCpuPercent(string coreNumber) {
  ifstream file;
  string readline;
  Util::getStream((Path::basePath() + Path::statPath()), file);
  if (coreNumber == "") {
    coreNumber = "0";
  }
  string cpu_line = Util::findLineStartingWith("cpu" + coreNumber, file, readline);
  vector<string> parsed = Util::split_str(cpu_line, " ");
  file.close();
  return parsed;
}

int ProcessParser::getTotalNumberOfProcesses() {
  vector<string> pids = ProcessParser::getPidList();
  return pids.size();
}

int ProcessParser::getNumberOfRunningProcesses() {
  ifstream file;
  string readline;
  Util::getStream((Path::basePath() + Path::statPath()), file);
  string procs = Util::findLineStartingWith("procs_running", file, readline);
  string num_procs = procs.substr(procs.find(" "));
  file.close();
  return stoi(num_procs);
}

int ProcessParser::getTotalThreads() {
  vector<string> pids = ProcessParser::getPidList();
  int total_threads = 0;
  for (string pid : pids) {
     ifstream file;
  	 string readline;
  	 Util::getStream((Path::basePath() + pid + Path::statusPath()), file);
  	 string thread_line = Util::findLineStartingWith("Threads", file, readline);
     total_threads += stoi(thread_line.substr(9));
     
     file.close();
  }
  return total_threads;
}

string ProcessParser::getSysKernelVersion() {
  return Util::readFirstLine(Path::basePath() + Path::versionPath());
}

string ProcessParser::getOSName() {
  	 ifstream file;
  	 string readline;
  	 Util::getStream("/etc/os-release", file);
  	 string os = Util::findLineStartingWith("PRETTY_NAME", file, readline);
  	 
  	 file.close();
     return os.substr(os.find("\""));
}

bool ProcessParser::isPidExisting(string pid) {
  vector<string> pids = ProcessParser::getPidList();
  vector<string>::iterator found = find(pids.begin(), pids.end(), pid);
  if (found != pids.end()) {
    return true;
  }
  return false;
}

float ProcessParser::getSysRamPercent() {
     ifstream file;
  	 string readline;
  	 Util::getStream((Path::basePath() + Path::memInfoPath()), file);
  	 string mem_avail = Util::findLineStartingWith("MemAvailable", file, readline);
     float mem_avail_num = stof(Util::grabNumbersFromString(mem_avail, "MemAvailable:"));
     file.clear();
	 file.seekg (0, ios::beg);
  	 string mem_free = Util::findLineStartingWith("MemFree:", file, readline);
     float mem_free_num = stof(Util::grabNumbersFromString(mem_free, "MemFree:"));
     file.clear();
	 file.seekg (0, ios::beg);
  	 string buffers = Util::findLineStartingWith("Buffers:", file, readline);
     float buffers_num = stof(Util::grabNumbersFromString(buffers, "Buffers:"));
  
  	file.close();
  
    return float(100.0*(1-(mem_free_num/(mem_avail_num-buffers_num))));
}

float ProcessParser::get_sys_active_cpu_time(vector<string> values)
{
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}

float ProcessParser::get_sys_idle_cpu_time(vector<string>values)
{
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

string ProcessParser::PrintCpuStats(vector<string> values1, vector<string> values2) {
    float activeTime = ProcessParser::get_sys_active_cpu_time(values2) - ProcessParser::get_sys_active_cpu_time(values1);
    float idleTime = ProcessParser::get_sys_idle_cpu_time(values2) - ProcessParser::get_sys_idle_cpu_time(values1);
    float totalTime = activeTime + idleTime;
    float result = 100.0*(activeTime / totalTime);
    return to_string(result);
}