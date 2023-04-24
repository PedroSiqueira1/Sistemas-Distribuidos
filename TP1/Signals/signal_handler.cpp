# include <iostream>
# include <csignal>


// Receive signal from the system

void signalHandler( int signum ) {

   // Checks if the signal is SIGHUP
   if (signum == 1) {
      std::cout << "Received signal SIGHUP" << std::endl;
   }

   // Checks if the signal is SIGINT
   if (signum == 2) {
      std::cout << "Received signal SIGINT" << std::endl;
   }

   // Checks if the signal is SIGKILL
   if (signum == 3) {
      std::cout << "Received signal SIGKILL" << std::endl;
      raise(SIGKILL);
   }
}

int main (int argc, char *argv[]) {

   // Shows the PID of the process
   std::cout << "PID: " << getpid() << std::endl;
   
   // register signal SIGINT and signal handler  
   signal(1, signalHandler);
   signal(2, signalHandler);
   signal(3, signalHandler);
   
   // Busy wait for signal
   if (atoi(argv[1]) == 0) {
      while(1) {
         std::cout << "Going to sleep...." << std::endl;
         sleep(1);
      }
   }

   // Blocking wait for signal
   if (atoi(argv[1]) == 1) {
      while(1) {
         pause();
      }
      
   }
   
   return 0;
}