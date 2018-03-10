
/*
 * Conway's Game of Life
 *
 * A. Mucherino
 *
 * PPAR, TP4
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

 #include <mpi.h>

int N = 32;
int itMax = 20;

// allocation only
unsigned int* allocate()
{
   return (unsigned int*)calloc(N*N,sizeof(unsigned int));
};

// conversion cell location : 2d --> 1d
// (row by row)
int code(int x,int y,int dx,int dy)
{
   int i = (x + dx)%N;
   int j = (y + dy)%N;
   if (i < 0)  i = N + i;
   if (j < 0)  j = N + j;
   return i*N + j;
};

// writing into a cell location 
void write_cell(int x,int y,unsigned int value,unsigned int *world)
{
   int k;
   k = code(x,y,0,0);
   world[k] = value;
};

// random generation
unsigned int* initialize_random()
{
   int x,y;
   unsigned int cell;
   unsigned int *world;

   world = allocate();
   for (x = 0; x < N; x++)
   {
      for (y = 0; y < N; y++)
      {
         if (rand()%5 != 0)
         {
            cell = 0;
         }
         else if (rand()%2 == 0)
         {
            cell = 1;
         }
         else
         {
            cell = 2;
         };
         write_cell(x,y,cell,world);
      };
   };
   return world;
};

// dummy generation
unsigned int* initialize_dummy()
{
   int x,y;
   unsigned int *world;

   world = allocate();
   for (x = 0; x < N; x++)
   {
      for (y = 0; y < N; y++)
      {
         write_cell(x,y,x%3,world);
      };
   };
   return world;
};

// "glider" generation
unsigned int* initialize_glider()
{
   int x,y,mx,my;
   unsigned int *world;

   world = allocate();
   for (x = 0; x < N; x++)
   {
      for (y = 0; y < N; y++)
      {
         write_cell(x,y,0,world);
      };
   };

   mx = N/2 - 1;  my = N/2 - 1;
   x = mx;      y = my + 1;  write_cell(x,y,1,world);
   x = mx + 1;  y = my + 2;  write_cell(x,y,1,world);
   x = mx + 2;  y = my;      write_cell(x,y,1,world);
                y = my + 1;  write_cell(x,y,1,world);
                y = my + 2;  write_cell(x,y,1,world);

   return world;
};

// "small exploder" generation
unsigned int* initialize_small_exploder()
{
   int x,y,mx,my;
   unsigned int *world;

   world = allocate();
   for (x = 0; x < N; x++)
   {
      for (y = 0; y < N; y++)
      {
         write_cell(x,y,0,world);
      };
   };

   mx = N/2 - 2;  my = N/2 - 2;
   x = mx;      y = my + 1;  write_cell(x,y,2,world);
   x = mx + 1;  y = my;      write_cell(x,y,2,world);
                y = my + 1;  write_cell(x,y,2,world);
                y = my + 2;  write_cell(x,y,2,world);
   x = mx + 2;  y = my;      write_cell(x,y,2,world);
                y = my + 2;  write_cell(x,y,2,world);
   x = mx + 3;  y = my + 1;  write_cell(x,y,2,world);

   return world;
};


// reading a cell
int read_cell(int x,int y,int dx,int dy,unsigned int *world)
{
   int k = code(x,y,dx,dy);
   return world[k];
};

// updating counters
void update(int x,int y,int dx,int dy,unsigned int *world,int *nn,int *n1,int *n2)
{
   unsigned int cell = read_cell(x,y,dx,dy,world);
   if (cell != 0)
   {
      (*nn)++;
      if (cell == 1)
      {
         (*n1)++;
      }
      else
      {
         (*n2)++;
      };
   };
};

// looking around the cell
void neighbors(int x,int y,unsigned int *world,int *nn,int *n1,int *n2)
{
   int dx,dy;

   (*nn) = 0;  (*n1) = 0;  (*n2) = 0;

   // same line
   dx = -1;  dy = 0;   update(x,y,dx,dy,world,nn,n1,n2);
   dx = +1;  dy = 0;   update(x,y,dx,dy,world,nn,n1,n2);

   // one line down
   dx = -1;  dy = +1;  update(x,y,dx,dy,world,nn,n1,n2);
   dx =  0;  dy = +1;  update(x,y,dx,dy,world,nn,n1,n2);
   dx = +1;  dy = +1;  update(x,y,dx,dy,world,nn,n1,n2);   

   // one line up
   dx = -1;  dy = -1;  update(x,y,dx,dy,world,nn,n1,n2);
   dx =  0;  dy = -1;  update(x,y,dx,dy,world,nn,n1,n2);
   dx = +1;  dy = -1;  update(x,y,dx,dy,world,nn,n1,n2);
};

// computing a new generation
short newgeneration(unsigned int *world1,unsigned int *world2,int xstart,int xend)
{
   int x,y;
   int nn,n1,n2;
   unsigned int cell;
   short change = 0;

   // cleaning destination world
   for (x = 0; x < N; x++)
   {
      
      for (y = 0; y < N; y++)
      {
         write_cell(x,y,0,world2);
      };
   };

   // generating the new world
   for (x = xstart; x < xend; x++)
   {

      for (y = 0; y < N; y++)
      {
         //printf("xstart = %d, xend = %d, x = %d, y = %d, N = %d\n", xstart, xend, x, y, N);
         neighbors(x, y, world1, &nn, &n1, &n2);
         cell = read_cell(x, y, 0, 0, world1);
         if(cell == 0) {
            if(nn == 3) {
               change = 1;
               if(n1 > n2) {
                  write_cell(x, y, 1, world2);
               } else {
                  write_cell(x, y, 2, world2);
               }
            }
         } else {
            if(nn < 2 || nn > 3) {
               change = 1;
               write_cell(x, y, 0, world2);
            } else {
               write_cell(x, y, cell, world2);
            }
         }
          

      };
   };
   return change;
};

// cleaning the screen
void cls()
{
    int i;
    for (i = 0; i < 1; i++)
    {
        fprintf(stdout,"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    }
}

// diplaying the world
void print(unsigned int *world)
{
   int i;
   cls();
   for (i = 0; i < N; i++)  fprintf(stdout,"-");  

      int lig = -1;
   for (i = 0; i < N*N; i++)
   {
      if (i%N == 0)  fprintf(stdout,"%d\n", lig++);
      if (world[i] == 0)  fprintf(stdout," ");
      if (world[i] == 1)  fprintf(stdout,"o");
      if (world[i] == 2)  fprintf(stdout,"x");
   };
   fprintf(stdout,"%d\n", lig++);

   for (i = 0; i < N; i++)  fprintf(stdout,"-");  
   fprintf(stdout,"\n");
   sleep(1);
};

// main
int main(int argc,char *argv[])
{
   int it,change;
   unsigned int *world1,*world2;
   unsigned int *worldaux;

   unsigned int *neigh_row;

   int my_rank, n;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &n);

   if(N % n == 0) {
    
      if(my_rank == 0) {
         //world1 = initialize_dummy();
         //world1 = initialize_random();
         //world1 = initialize_glider();
         world1 = initialize_small_exploder();
      } else {
         world1 = allocate();
      }

      MPI_Bcast(world1, N * N, MPI_INT, 0, MPI_COMM_WORLD);

      neigh_row = (unsigned int*) calloc(N ,sizeof(unsigned int));

      /*
         Imaginons une grille de 32 * 32
         MPI lance 4 process

         32 / 4 = 8, chaque process compute 8 lignes

         Le process 0, les lignes 0 à 7
         Le process 1, les lignes 8 à 15
         Le process 2, les lignes 16 à 23
         Le process 3, les lignes 24 à 31

         Si je suis le process 1, alors ma ligne du haut (ligneUp) est 1 * (N/n) soit 1 * (32/4) = 1 * 8 = 8
         Ma ligne du dessous est donc 15 (lineDown)
         Le rang au dessus de moi est le rang 0 (rankUp)
         Le rang d'en dessous est le rang 2 (rankDown)

         La lineDown du rankUp est donc la 7, L7 est ma ligne voisine du haut (neigh_LUp)
         la lineUp du rankDown est donc la 16, L16 est ma ligne voisine du bas (neigh_LDown)

         En tant que process 2 je dois récupérer ces deux dernière lignes,
         Et je dois envoyer ma lineUp à mon rankUp et ma lineDown à mon rankDown
      */

      int lineUp = my_rank * (N/n); // la ligne du début
      int lineDown = (my_rank + 1) * (N/n) - 1;

      int rankUp = (my_rank - 1 < 0) ? n - 1 : my_rank - 1;
      int rankDown = (my_rank + 1 >= n) ? 0 : my_rank + 1;

      int neigh_LDown = (lineDown + 1 >= N) ? 0 : lineDown + 1;
      int neigh_LUp = (lineUp - 1 < 0) ? N - 1 : lineUp - 1;

      // getting started  
      
      world2 = allocate();

      if(my_rank == 0) {
         print(world1);
      }

      // décommenter si on fait des print sur d'autres process plus bas
      //MPI_Barrier(MPI_COMM_WORLD);
      
      it = 0;  change = 1;
      while ( it < itMax)
      {
         change = newgeneration(world1, world2, my_rank * (N/n) , (my_rank + 1) * (N/n) );
         worldaux = world1;  world1 = world2;  world2 = worldaux;
         
         it++;
         if(n > 1) {
            // On prend la ligne voisine d'en haut
            for(int i = 0; i < N; i++) {
               neigh_row[i] = read_cell(lineUp, i, 0, 0, world1);
            }

            /*
               Obligatoire de s'attendre mutuellement ici
               Il y a un interblocage entre chaque process car 1 envois à 0, 0 reçois de 1, etc
            */

            // on envois la ligne voisine du haut, au process du dessus (Si je suis 1 j'envois à 0)
            MPI_Send(neigh_row, N, MPI_INT, rankUp, 1, MPI_COMM_WORLD);
            // On recois la ligne voisine du haut du process du dessous, (Si je suis 1, je reçois de 2)
            MPI_Recv(neigh_row, N, MPI_INT, rankDown, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // C'est donc ma ligne voisine du dessous, je l'écris en dessous de ma ligne du dessous (Si j'écris de 8 à 15, j'écris en ligne 16)
            for(int i = 0; i < N; i++) {
               write_cell(neigh_LDown, i, neigh_row[i], world1);
            }

            // Je récupère la ligne du bas
            for(int i = 0; i < N; i++) {
               neigh_row[i] = read_cell(lineDown, i, 0, 0, world1);
            }

            // je l'envois à mon process d'en dessous (my_rank + 1)
            MPI_Send(neigh_row, N, MPI_INT, rankDown, 2, MPI_COMM_WORLD);
            // je reçois de mon process d'au dessus (my_rank - 1)
            MPI_Recv(neigh_row, N, MPI_INT, rankUp, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // c'est donc ma ligne du dessus, je l'écris au dessus de ma ligne du dessus, (Si j'écris de 8 à à 15 je l'écris en 7)
            for(int i = 0; i < N; i++) {
               write_cell(neigh_LUp, i, neigh_row[i], world1);
            }
         }

         /*
          Si on veut print à chaque itération
          */

         /*MPI_Reduce(world1, world2, N*N, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
         // MPI_Gather((world1 + N * lineUp), N * (N/n), MPI_INT, world2, N * (N/n), MPI_INT, 0, MPI_COMM_WORLD);
         if(my_rank == 0) {
            print(world2);
         }*/

         // intéressant cas montre le côté asynchrone, mais ne peut avoir plus d'une étape d'écart car interblocage aux send/recv
         //printf("process %d, etape %d\n", my_rank, it);
      }

      /*
         Plusieurs façons de regrouper les données
      */

         // façon 1
      MPI_Gather((world1 + N * lineUp), N * (N/n), MPI_INT, world2, N * (N/n), MPI_INT, 0, MPI_COMM_WORLD);
         
         // façon 2
      //MPI_Reduce(world1, world2, N*N, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

      if(my_rank == 0) {
         print(world2);
      }

      free(world1);   
      free(world2);
   }

   MPI_Finalize();
   // ending
   return 0;
};

