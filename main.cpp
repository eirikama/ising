// main.cpp
#include "ising_2d_simulation.h"

int main(int argc, char* argv[])
{
	int L = std::atoi(argv[1]);
	int N = std::atoi(argv[2]);
	int SIZE = L * L;

    // Create an output filename that includes L
    std::ostringstream filename;
    filename << "data/results_L" << L << "_N" << N << ".txt";

        bool* spins = new bool[SIZE]; //Stores the spins
        int** neighs = new int*[SIZE]; //Stores the spins

    for (int i = 0; i < SIZE; ++i) {
         neighs[i] = new int[4];
     }



	double h[5]; //Values of the exp(-J/kT)
	double energy; //Value of the energy of the system
        double m[DATA]; //This array contains several moments of the magnetization and energy
	//int neighs[SIZE][4]; //To store nearest neighbours

        mt19937 gen(958431198); //Mersenne Twister RNG
	uniform_int_distribution<int> brandom(0, 1); //Get any random integer
	uniform_int_distribution<int> ran_pos(0, SIZE-1); //Get any random integer
	uniform_real_distribution<double> ran_u(0.0, 1.0); //Our uniform variable generator

	double tstar; //Control parameter
	double deltat, deltat_crit; //Change in control parameter by iteration
	double tmax, tmin, tcrit_up, tcrit_down; //Max and min temperature, and interval where we apply Wolff

    tmax = 5.0;
    tmin = 0.1;
    tcrit_up = 2.4;
    tcrit_down = 2.2;

    deltat = 0.1;
    deltat_crit = 0.01;

	//ofstream output; //Output of the stream

        initialize(spins, L, gen, brandom); //Init randomly
	get_neighbors(neighs, L); //Get neighbour table
	energy = get_energy(spins, neighs, L); //Compute initial energy

    	std::ostringstream spin_config_filename;
    	spin_config_filename << "data/initial_spin_config.txt";
	write(spins, L, spin_config_filename.str());


    std::ofstream output(filename.str());
    if (!output.is_open()) {
        std::cerr << "Error: could not open file " << filename.str() << std::endl;
        return 1;
    }

	//output.open(filename);
	for (tstar = tmax; tstar > tcrit_up; tstar -= deltat)
	{
	    do_step(spins, neighs, L, tstar, N, h, energy, gen, ran_u, ran_pos, m);
	    w_output(output, tstar, N, m);
	    cout << "\rT=" << tstar << "   " << flush;
	}
    for (tstar = tcrit_up - deltat_crit; tstar > tcrit_down; tstar -= deltat_crit)
	{
	    do_step_wolff(spins, neighs, L, tstar, N, energy, gen, ran_u, ran_pos, m);
	    w_output(output, tstar, N, m);
	    cout << "\rT=" << tstar << "   " << flush;
	}
    for (tstar = tcrit_down - deltat; tstar >= tmin; tstar -= deltat)
	{
	    do_step(spins, neighs, L, tstar, N, h, energy, gen, ran_u, ran_pos, m);
	    w_output(output, tstar, N, m);
	    cout << "\rT=" << tstar << "   " << flush;
	}
    cout << endl;
	output.close();

	return 0;

}

