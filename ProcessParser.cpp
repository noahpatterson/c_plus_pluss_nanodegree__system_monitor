#include <iostream>
#include "ProcessParser.h"
using namespace std;

int main(void) {
  string pid = "1";
  string cmdline = getCmd(pid);
  cout << "line from pid: " << cmdline << "\n";
  
  vector<string> list = getPidList();
  cout << "first pid: " << list[8] << "\n";
  
  string vmtotal = getVmSize(pid);
  cout << "vmsize: " << vmtotal << "\n";
  
  string cpu_usage = getCpuPercent("39");
  cout << "cpu_usage: " << cpu_usage << "%\n";
  
  string system_up_time = to_string(getSysUpTime());
  cout << "system_up_time: " << system_up_time << "\n";
  
  string ProcUpTime = getProcUpTime("39");
  cout << "ProcUpTime: " << ProcUpTime << "\n";
  
  
  string proc_user = getProcUser("22");
  cout << "proc_user: " << proc_user << "\n";
  
  vector<string> cpu_percent = getSysCpuPercent();
  cout << "cpu_percent: " << cpu_percent[1] << "\n";
  
  int num_cores = getNumberOfCores();
  cout << "num_cores: " << num_cores << "\n";
  
  int num_processes = getTotalNumberOfProcesses();
  cout << "num_processes: " << num_processes << "\n";
  
  int num_processes_running = getNumberOfRunningProcesses();
  cout << "num_processes_running: " << num_processes_running << "\n";
  
  int num_threads = getTotalThreads();
  cout << "num_threads: " << num_threads << "\n";
  
  string version = getSysKernelVersion();
  cout << "version: " << version << "\n";
  
  string os = getOSName();
  cout << "os: " << os << "\n";
  
  bool pid_exist = isPidExisting("1");
  cout << "pid_exist [1]: " << pid_exist << "\n";
  bool pid_exist2 = isPidExisting("1000");
  cout << "pid_exist [1000]: " << pid_exist2 << "\n";
  
  float ram_percent = getSysRamPercent();
  cout << "ram_percent: " << ram_percent << "\n";
}