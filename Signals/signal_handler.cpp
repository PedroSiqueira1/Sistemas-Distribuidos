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

int main () {
   // register signal SIGINT and signal handler  
   signal(1, signalHandler);
   signal(2, signalHandler);
   signal(3, signalHandler);
   
     

   while(1){
      std::cout << "Going to sleep...." << std::endl;
      sleep(1);
   }
   
   return 0;
}