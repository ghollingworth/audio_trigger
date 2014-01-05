#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>

#define BUF_SIZE 1024

void main(int argc, char * argv[])
{
   FILE * in = stdin;
   FILE * out = stdout;
   int pid = 0;
   short audio_data[BUF_SIZE];
   
   if(argc > 1)
   {
      pid = atoi(argv[1]);
   }
   
   fread(audio_data, sizeof(audio_data[0]), BUF_SIZE, in);
   
   while(1)
   {
      short a_max = 0;
      long long sum_squares = 0;
      int i;
      int power;
      
      fread(audio_data, sizeof(audio_data[0]), BUF_SIZE, in);
      
      for(i = 0; i < BUF_SIZE; i++)
      {
         a_max = audio_data[i] > a_max ? audio_data[i] : a_max;
         sum_squares += (long long) audio_data[i] * (long long) audio_data[i];
      }
      
      power = (int) sqrt(sum_squares / BUF_SIZE);
      
      printf("Power = %f\n", sqrt(sum_squares / BUF_SIZE));
      if(a_max == 32767)
      {
         printf("CLIPPING\n");
      }
      
      if(power > 300)
      {
         if(pid)
            kill(pid, SIGUSR1);
         exit(0);
      }
   }
   
}

         
