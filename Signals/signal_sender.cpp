# include <iostream>
# include <csignal>
# include <unistd.h>

using namespace std;



int main (int argc, char *argv[]) {

   // Read the signal_number from the command line
   int signal_number = atoi(argv[1]);
   
   // Read the pid from the command line
   int pid = atoi(argv[2]);

   // Checks if the signal_number is valid
   if (signal_number < 0 || signal_number > 64) {
      cout << "Invalid signal number" << endl;
      return 1;
   }

   // Checks if process exists before sending the signal
   if (kill(pid, 0) == -1) {
      cout << "Process does not exist" << endl;
      return 1;
   }

   // Send the signal
   kill(pid, signal_number);
   cout << "Signal sent" << endl;
   return 0;
}