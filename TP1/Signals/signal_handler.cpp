# include <iostream>
# include <csignal>


// Function to handle signals

void signalHandler( int signum ) {

   // Checks if the signal is SIGHUP
   if (signum == 1) {
      std::cout << "Received signal SIGHUP" << std::endl;
   }

   // Checks if the signal is SIGINT
   else if (signum == 2) {
      std::cout << "Received signal SIGINT" << std::endl;
   }

   // Checks if the signal is SIGKILL
   else if (signum == 3) {
      std::cout << "Received signal SIGKILL" << std::endl;
      raise(SIGKILL);
   }

   // Else, it is not a valid signal
   else {
      std::cout << "Not a valid signal" << std::endl;
   }
}

int main (int argc, char *argv[]) {

   // Shows the PID of the process
   std::cout << "PID: " << getpid() << std::endl;
   
   // Register signals to signal handler 
   signal(1, signalHandler);
   signal(2, signalHandler);
   signal(3, signalHandler);


   // Ignore all other signals
   for (int i = 1; i <= 64; i++) {
      if (i != 1 && i != 2 && i != 3) {
         signal(i, signalHandler);
      }
   }
   

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