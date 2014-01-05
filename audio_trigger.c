#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <getopt.h>

#define BUF_SIZE 1024

void Usage(int asked, char *argv0)
{
   fprintf(asked ? stdout : stderr,
      "Usage: %s [OPTIONS]...\n"
           "\t -p pid - process ID to send signal to\n"
           "\t -d delay - post trigger delay (ms)\n"
           "\t -t threshold - Sound power threshold at which to send signal\n"
           "\t -o out_file - Output filename (default stdout)\n"
           "\t -i in_file - file to get input (default stdin)\n\n\n"
           "Example:\n"
           "   $ arecord -D hw:1,0 -f S16_LE | %s -p `pidof raspivid` -d 1000\n"
      ,argv0, argv0);

   exit(-1);
}

void get_options(
   int argc,
   char *argv[],
   int *pid,
   int *post_delay,
   int *threshold,
   FILE **in,
   FILE **out)
{
   int c;

   while((c = getopt(argc, argv, "hp:d:t:o:i:")) != -1)
   {
      switch(c)
      {
         case 'h':
            Usage(1, argv[0]);
         case 'p':
            *pid = atoi(optarg);
            break;
         case 'd':
            *post_delay = atoi(optarg);
            break;
         case 't':
            *threshold = atoi(optarg);
            break;
         case 'o':
            *out = fopen(optarg, "wb");
            if(*out == NULL)
            {
               fprintf(stderr, "Failed to open output file %s\n", optarg);
               exit(-1);
            }
           break;
        case 'i':
           *in = fopen(optarg, "rb");
           if(*in == NULL)
           {
              fprintf(stderr, "Failed to open input file %s\n", optarg);
              exit(-1);
           }
           break;
        default:
           Usage(0, argv[0]);
      }
   }
}

void main(int argc, char * argv[])
{
   FILE * in = stdin;
   FILE * out = stdout;
   int pid = 0;
   int post_delay = 0;
   int threshold = 0;
   short audio_data[BUF_SIZE];

   get_options(argc, argv, &pid, &post_delay, &threshold, &in, &out);

   fread(audio_data, sizeof(audio_data[0]), BUF_SIZE, in);

   while(1)
   {
      short a_max = 0;
      long long sum_squares = 0;
      int i;
      int power;
      int count;

      count = fread(audio_data, sizeof(audio_data[0]), BUF_SIZE, in);

      if(count <= 0)
        exit(0);

      for(i = 0; i < BUF_SIZE; i++)
      {
         a_max = audio_data[i] > a_max ? audio_data[i] : a_max;
         sum_squares += (long long) audio_data[i] * (long long) audio_data[i];
      }

      power = (int) sqrt(sum_squares / BUF_SIZE);

      fprintf(out, "%f\n", sqrt(sum_squares / BUF_SIZE));
      if(a_max == 32767)
      {
         printf("CLIPPING\n");
      }

      if(threshold && power > threshold)
      {
         if(post_delay)
         {
            struct timespec delay;
            delay.tv_sec = post_delay / 1000;
            delay.tv_nsec = ((long) post_delay - (delay.tv_sec * 1000)) * 1000000;
            nanosleep(&delay, &delay);
         }
         if(pid)
            kill(pid, SIGUSR1);
         exit(0);
      }
   }

}
