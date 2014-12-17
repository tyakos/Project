#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/select.h>

double distance(int, int, double*, double*);
int kbhit();

int main(int argc, char *argv[])
{
  FILE* output;
  int validInput, answer_yn;
  long int i = 0, j, max_recursion_number = 500000, iterations; //adjust max_recursion_number depending on cpu available
  double t = 0.0001;  //works well for the current max_recursion_number, adjust if needed
  double collision = 0.1;
  double v[2], m[2], a[2];
  double *y,*x;
  y = (double*)malloc(max_recursion_number*sizeof(double));
  x = (double*)malloc(max_recursion_number*sizeof(double));
  if(x == NULL || y == NULL)
  {
    perror("malloc");
    return(EXIT_FAILURE);
  }
  
  x[0] = 1;
  y[0] = 0;
  x[1] = -1;
  y[1] = 0;
  validInput = (argc == 7);
  validInput = validInput && sscanf(argv[1],"%lf", &x[2]); 
  validInput = validInput && sscanf(argv[2],"%lf", &y[2]); 
  validInput = validInput && sscanf(argv[3],"%lf", &v[0]);
  validInput = validInput && sscanf(argv[4],"%lf", &v[1]);
  validInput = validInput && sscanf(argv[5],"%lf", &m[0]) && (m[0] >= 0);
  validInput = validInput && sscanf(argv[6],"%lf", &m[1]) && (m[1] >= 0);
  if(!validInput)
   {
    fputs("Input validation failure, please use the program with x(0), y(0), vx(0), vy(0), m1, m2 as the command line options, exiting.\n", stderr);
    return(EXIT_FAILURE);
   }
  x[3] = x[2] + t*v[0];
  y[3] = y[2] + t*v[1];
  
  for(i = 3; i < max_recursion_number-1; i++)
  {
    if(distance(0, i, x, y) < collision || distance(1, i, x, y) < collision)
   {
     printf("Collision occured in time %lf \n", i*t);
     break;
   }
   
   a[0] = -(m[1]*(x[i]-x[0])*pow(distance(0,i,x,y), (-3)) + m[0]*(x[i]-x[1])*pow(distance(1,i,x,y), (-3)));
   a[1] = -(m[1]*(y[i]-y[0])*pow(distance(0,i,x,y), (-3)) + m[0]*(y[i]-y[1])*pow(distance(1,i,x,y), (-3)));
   x[i+1] = t*t*a[0] + 2*x[i] - x[i-1];
   y[i+1] = t*t*a[1] + 2*y[i] - y[i-1];
  }

  if(i == max_recursion_number)
  {
    printf("All iterations successful, collision did not occur in given time \n");
  }
  iterations = i;
  
  // Now write the output into file
  
  output=fopen("output2.dat", "w");
  if(output != (FILE*)NULL) 
  {
   for(j=2; j<=i; j++)
   {
    fprintf(output, "%lf %lf %lf\n", x[j], y[j], (j-2)*t);
   }
   fclose(output);
   printf("Trajectory written to output2.dat \n");
  }
  else
  {
    printf("Error writing to file.");
  }  
  
// Plotting starts here
  while( (answer_yn != 'y')&&(answer_yn != 'n') ) 
  { 
   printf("Plot?(y/n)");
   answer_yn = getchar();  
  } 
  
  if(answer_yn == 'y')
  {
   double min_total, max_total, scale;
   int k, n, p, x_coord, y_coord, graph_size;
   long int number_of_steps, time_scaling;
   int graph[52][52], ans, ans2; //int ans intead of char for ans because of EOF trap
   graph_size = 50;  
   printf("Enter the number of steps in which you want to display the motion: ");
   scanf("%ld", &number_of_steps);
   time_scaling = iterations/number_of_steps;
   for(j = 0; j < graph_size+1; j++)
   {
    for(k = 0; k < graph_size+1; k++)
    {
      graph[j][k] = 1;
    }
   }
   
   for(j = 2; j<iterations; j++)
   {
     if((max_total - x[j]) < 0)
     {
       max_total = x[j];
     }
     if((min_total - x[j]) > 0)
     {
       min_total = x[j];
     }
   }
       
    for(j = 2; j<iterations; j++)
    {
     if((max_total - y[j]) < 0)
     {
       max_total = y[j];
     }
     if((min_total - y[j]) > 0)
     {
       min_total = y[j];
     }
    }
    
   scale = max_total - min_total;
  
   x_coord = graph_size*(1 - min_total)/scale;
   y_coord = graph_size*(0 - min_total)/scale;
   graph[x_coord][y_coord] = 8;
   x_coord = graph_size*(-1 - min_total)/scale;
   y_coord = graph_size*(0 - min_total)/scale;
   graph[x_coord][y_coord] = 8;
   j = 2;
   printf("Usage: a for automatic mode, m for manual mode,\n in manual: Enter to plot the next step; q to plot all the steps and quit. \n An 8 marks the fixed objects, 0 - current location, 9 - trajectory (previous locations). \n Hint: press and hold Enter in manual mode \n Hint: you can enter automatic mode at any time. \n Press enter in automatic mode to pause. \n So: ");
   getchar();
   ans = getchar();
   for(p = 0; p < number_of_steps; p++)
   {
    if(ans == 'a')
    {
     x_coord = graph_size*(x[j] - min_total)/scale;
     y_coord = graph_size*(y[j] - min_total)/scale;
     if(kbhit())
     {
       printf("Continue? (y to continue, n to stop and quit)");
       answer_yn = '0';
       while( (answer_yn != 'y')&&(answer_yn != 'n') ) 
       { 
        answer_yn = getchar();  
       } 
       if(answer_yn == 'n')
       {
       ans = 'q';
       }
     }
     printf("%d %d \n", x_coord, y_coord);
     printf("Iteration: %ld/%ld   ", j, iterations);
     printf("Step: %d/%d \n", p, number_of_steps);
     graph[x_coord][y_coord] = 0;
   
     for(n = 0; n < graph_size+1; n++)
     {
      for(k = 0; k < graph_size+1; k++)
      {
        printf("%d", graph[k][graph_size-n]);
      }
      printf("\n");
     }
          usleep(10000);
   
     j = j + time_scaling;
     graph[x_coord][y_coord] = 9;
    }
    
    else if(ans == 'm')
    {
     printf("Continue?:");
     ans2 = getchar();
     if(ans2 == 'a') ans = 'a';
     if(ans2 == 'q') ans = 'q';
     x_coord = graph_size*(x[j] - min_total)/scale;
     y_coord = graph_size*(y[j] - min_total)/scale;
     printf("%d %d \n", x_coord, y_coord);
     printf("Iteration: %ld/%ld   ", j, iterations);
     printf("Step: %d/%d \n", p, number_of_steps);
     graph[x_coord][y_coord] = 0;
   
     for(n = 0; n < graph_size+1; n++)
     {
      for(k = 0; k < graph_size+1; k++)
      {
        printf("%d", graph[k][graph_size-n]);
      }
      printf("\n");
     }   
   
     j = j + time_scaling;
     graph[x_coord][y_coord] = 9;
    }
    
    else if(ans == 'q')
    {
     x_coord = graph_size*(x[j] - min_total)/scale;
     y_coord = graph_size*(y[j] - min_total)/scale;
     graph[x_coord][y_coord] = 0;
     j = j + time_scaling;
     graph[x_coord][y_coord] = 9;
    }
    else
    {
      printf("enter a valit character next time, asshole \n");
      break;
    }
   }

   if(ans == 'q')
   {
   printf("Final result: \n");
   for(n = 0; n < graph_size+1; n++)
   {
    for(k = 0; k < graph_size+1; k++)
    {
     printf("%d", graph[k][graph_size-n]);
    }
    printf("\n");
    }
   }

  }

  
  
  free(x);
  free(y);
  
  return(0);
    
}

double distance(int a, int b, double* x, double* y)
{
  double e = 0;
  e = sqrt((x[a] - x[b])*(x[a] - x[b]) + (y[a] - y[b])*(y[a] - y[b]));
  return e;  
} 

int kbhit()
{
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
  select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);
}
