// proc.c, 159
// all processes are coded here
// processes do not access kernel data, must use syscalls

#include "constants.h" // include only these 2 files
#include "syscalls.h"
#include "data.h"

void InitProc(void) {
   int i;
   car_sem = SemInit(3);
   while(1) {
      SetVideo(1, 1);         // pos video
      Write(STDOUT, ".");
      for(i=0; i<LOOP/2; i++)asm("inb $0x80"); // busy CPU

      SetVideo(1, 1);         // pos video
      Write(STDOUT, " ");
      for(i=0; i<LOOP/2; i++)asm("inb $0x80"); // busy CPU
   }
}

void UserProc(void) {
   int my_pid;
   char str[3];

   // get my PID and make a string from it (null-delimited)
   my_pid = GetPid();
   str[0]=my_pid / 10 + '0';
   str[1]=my_pid % 10 + '0';
   str[2]='\0';
   SetVideo(my_pid + 1, 1);            // set video cursor to beginning of my row
   Write(STDOUT, "Print this big ass sentence to ensure that this test line wraps around the screen and then check for erasure! :)");
   Sleep(2);                           // sleep for 2 seconds

   while(1) {
      SetVideo(my_pid + 1, 1);         //call service to set video cursor to beginning of my row
      Write(STDOUT, str);              //call service to write out my PID str
      Sleep(2);

      SetVideo(my_pid + 1, 1);         //call service to set video cursor to beginning of my row
      Write(STDOUT, "--");             //call service to erase my PID str (with "--")
      Sleep(2);
   }
}

void CarProc(void){
   // get my pid and build a str
   int my_pid;
   char str[3];
   my_pid = GetPid();
   str[0] = my_pid / 10 + '0';
   str[1] = my_pid % 10 + '0';
   str[2] = '\0';
   
   SetVideo(my_pid + 1, 1);           // show pid on beginning of 'my' row
   Write(STDOUT, str);

   while(1){
     SetVideo(my_pid + 1, 10);
     Write(STDOUT, "I'm off ...       "); // show: I'm off ... on my row (skip my PID, don't overwrite it)
     Sleep(2);                        // sleep for 2 seconds 

     SemWait(car_sem);                // semaphore-wait on the car semaphore
     
     SetVideo(my_pid + 1, 10);
     Write(STDOUT, "I'm on the bridge!"); // show: I'm on the bridge! (on the same location to overwrite above)
     Sleep(2);                        // sleep for 2 seconds
     SemPost(car_sem);                // semaphore-post on the car semaphore
   }
}

void TermProc(void){
  int my_pid, device;
  int line = 1;
  char str[3];
  my_pid = GetPid();
  str[0] = my_pid / 10 + '0';
  str[1] = my_pid % 10 + '0';
  str[2] = '\0';

  // determine what my 'device' should be (even PID TERM0, odd TERM1)
  if(my_pid % 2 == 0) device = TERM0;
  else device = TERM1;
  while(1){
    // every 5 lines Write() to my device some special symbols (help viewing)
    if(line % 5 == 0) Write(device, "--->>\n\r");
    // Write() 'str' to my device
    Write(device, str);
    // Write() a lengthier message (see demo) to my device
    Write(device, ": I would make a skeleton joke, but you wouldn't find it very humerus.\n\r");
    Sleep(3); // sleep for 3 seconds
    line++;
  }
}
