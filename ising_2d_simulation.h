// ising_2d_simulation.h

#ifndef ISING_2D_SIMULATION_H
#define ISING_2D_SIMULATION_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <random>
#include <fstream>
#include <sstream> 

#define UP 0
#define RIGHT 1
#define LEFT 2
#define DOWN 3

//#define L 16
//#define SIZE L*L

#define DATA 9

#define MAG 0
#define MAG2 1
#define MAG4 2
#define MAGERR 3
#define SUSERR 4
#define ENE 5
#define ENE2 6
#define ENE4 7
#define ENERR 8
#define CHERR 9

using namespace std;


void initialize(bool* spins, int L, mt19937& gen, uniform_int_distribution<int>& brandom);
void get_neighbors(int** neighs, int L);
void do_step(bool* spins, int** neighs, int L, double tstar, int N, double h[5], double& energy, mt19937& gen, 
		uniform_real_distribution<double>& ran_u, uniform_int_distribution<int>& ran_pos, double m[DATA]);
void do_step_wolff(bool* spins, int** neighs, int L, double tstar, int N, double& energy, mt19937& gen, 
			uniform_real_distribution<double>& ran_u, uniform_int_distribution<int>& ran_pos, double m[DATA]);
double magnetization(bool* spins, int L);
void flip_spin(bool* spins, int** neighs, int L, double h[5], double& energy, mt19937& gen, uniform_real_distribution<double>& ran_u, uniform_int_distribution<int>& ran_pos);
void add_to_cluster(bool* spins, int** neighs, int L, int pos, double& energy, double p, mt19937& gen, uniform_real_distribution<double>& ran_u);
double get_energy(bool* spins, int** neighs, int L);
void write(bool* spins, int L, string filename);
void w_output(ofstream& file, double tstar, int N, double m[DATA]);


//void initialize(bool spins[SIZE], mt19937& gen, uniform_int_distribution<int>& brandom);
//void get_neighbors(int neighs[SIZE][4]);


//void do_step(bool spins[SIZE],  int neighs[SIZE][4], double tstar, int N, double h[5], 
	     //double& energy, mt19937& gen, uniform_real_distribution<double>& ran_u, 
	     //uniform_int_distribution<int>& ran_pos, double m[DATA]);
//void do_step_wolff(bool spins[SIZE],  int neighs[SIZE][4], double tstar, int N, double& energy, 
		  //mt19937& gen, uniform_real_distribution<double>& ran_u, 
		  //uniform_int_distribution<int>& ran_pos, double m[DATA]);
//double magnetization(bool spins[SIZE]);

//void flip_spin(bool spins[SIZE], int neighs[SIZE][4], double h[5], double& energy, mt19937& gen, 
		//uniform_real_distribution<double>& ran_u, uniform_int_distribution<int>& ran_pos);
//void add_to_cluster(bool spins[SIZE], int neighs[SIZE][4], int pos, double& energy, double p, 
		//mt19937& gen, uniform_real_distribution<double>& ran_u);
//double get_energy(bool spins[SIZE], int neighs[SIZE][4]);

//void write(bool spins[SIZE]);
//void w_output(ofstream& file, double tstar, int N, double m[DATA]);


#endif 
