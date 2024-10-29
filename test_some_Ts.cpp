// main.cpp
#include "ising_2d_simulation.h"
#include <cmath>

int main(int argc, char* argv[])
{
	int L = std::atoi(argv[1]);
	int N = std::atoi(argv[2]);
	double T = std::atof(argv[3]);
	int SIZE = L * L;


        bool* spins = new bool[SIZE]; //Stores the spins
        int** neighs = new int*[SIZE]; //Stores the spins

    for (int i = 0; i < SIZE; ++i) {
         neighs[i] = new int[4];
     }



	double h[5]; //Values of the exp(-J/kT)
	double energy; //Value of the energy of the system
        double m[DATA]; //This array contains several moments of the magnetization and energy

        mt19937 gen(958431198); //Mersenne Twister RNG
	uniform_int_distribution<int> brandom(0, 1); //Get any random integer
	uniform_int_distribution<int> ran_pos(0, SIZE-1); //Get any random integer
	uniform_real_distribution<double> ran_u(0.0, 1.0); //Our uniform variable generator

        initialize(spins, L, gen, brandom); //Init randomly
	get_neighbors(neighs, L); //Get neighbour table
	energy = get_energy(spins, neighs, L); //Compute initial energy

		cout << "!" << endl;

	if ( (T > 2.1) && (T < 2.3)) {
		cout << "!" << endl;
		do_step_wolff(spins, neighs, L, T, N, energy, gen, ran_u, ran_pos, m);
		cout << "!" << endl;
	}
	else {
		cout << "!" << endl;
		do_step(spins, neighs, L, T, N, h, energy, gen, ran_u, ran_pos, m);
		cout << "!" << endl;
	}

    	std::ostringstream spins_filename;
    	spins_filename << "data/final_spin_config_L" << L << "_N" << N << "_T" << round(T*100) << ".txt";
	write(spins, L, spins_filename.str());




	return 0;

}

