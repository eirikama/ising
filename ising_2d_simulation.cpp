// ising_2d_simulation.cpp
#include "ising_2d_simulation.h"



void write(bool* spins, int L, string filename)
{
    int SIZE = L*L;	

    ofstream output;
    output.open(filename);
    for (int i=0; i < SIZE; i++)
    {
        if (i % SIZE == 0 and i != 0) output << endl;
        output << spins[i] << " ";
    }
    return;
}

//Initialices the grid in which we are going to do the Ising, using random values
void initialize(bool* spins, int L, mt19937& gen, uniform_int_distribution<int>& brandom)
{
	int SIZE = L*L;
	//Init spins with a random distribution
	for (int i=0; i < SIZE; i++)
	{
        spins[i] = brandom(gen); //Generate numbers
	}

	return;
}

//Fills the neigbour table
void get_neighbors(int** neighs, int L)
{
	int SIZE = L*L;

	int u,d,r,l;

	for (int i=0; i < L; i++)
	{
		for (int j=0; j < L; j++)
		{
		    //Get the (x,y) with periodic boundaries
			u = j+1 == L ? 0 : j+1;
			d = j-1 == -1 ? L-1 : j-1;
			r = i+1 == L ? 0 : i+1;
			l = i-1 == -1 ? L-1 : i-1;

            //(x,y) to index notation and store in table
			neighs[i+j*L][UP] = i+u*L;
			neighs[i+j*L][DOWN] = i+d*L;
			neighs[i+j*L][RIGHT] = r+j*L;
			neighs[i+j*L][LEFT] = l+j*L;
		}
	}

	return;
}

//Do all the things needed for a certain temperature
void do_step(bool* spins,  int** neighs, int L, double tstar, int N, double h[5], double& energy, mt19937& gen, uniform_real_distribution<double>& ran_u, uniform_int_distribution<int>& ran_pos, double m[DATA])
{
    int SIZE = L*L;
    
    double sum;//To compute the sum of spins
    double energysum;
    double chi, heat;
    double old_sum, old_chi, old_heat, old_energy;

    for (int i=0; i < DATA; i++) m[i] = 0.0; //Init the values

	//Compute the factors of exp(-dH/kT)
	for (int i=-4; i <= 4; i += 2)
	{
		h[(i+4)/2] =  min(1.0, exp(- 2.0 * i / tstar));
	}

	//Thermalize the state
	for (int j=0; j < 1100; j++)
	{
		flip_spin(spins, neighs, L, h, energy, gen, ran_u, ran_pos);
	}

	///----- TODO: optimize the number of steps for thermalization/measures
	old_sum = 0.0;
	old_chi = 0.0;
	old_heat = 0.0;
	old_energy = 0.0;
	for (int i=0; i < N; i++)
	{
		//Make changes and then average
		for (int j=0; j < 1100; j++)
		{
			flip_spin(spins, neighs, L, h,  energy, gen, ran_u, ran_pos);
		}

	//Compute quantities at time j
	sum = abs(magnetization(spins, L));
	chi = sum * sum;
	heat = energy * energy;

	//Add all the quantities
		m[MAG] += sum; // Magnetization
		m[MAG2] += chi; // For the susceptibility
		m[MAG4] += chi * chi; // For the Binder cumulant and also variance of susceptibility
		m[ENE] += energy; // Energy
		m[ENE2] += heat; // For specific heat
		m[ENE4] += heat * heat; // For the variance of specific heat
	
		//This are used for errors,
		m[MAGERR] += old_sum * sum;  // in magnetization
		m[SUSERR] += old_chi * chi; // in susceptibility
		m[ENERR] += old_energy * energy; // in energy
		m[CHERR] += old_heat * heat; // in specific heat

		//Get the value for the next iteration
		old_sum = sum;
		old_energy = energy;
		old_chi = chi;
		old_heat = heat;
	}

    //Finish the average
    for (int i=0; i < DATA; i++)  m[i] /= (1.0 * N);

	return;
}


//Do all the things needed for a certain temperature
void do_step_wolff(bool* spins,  int** neighs, int L, double tstar, int N, double& energy, mt19937& gen, uniform_real_distribution<double>& ran_u, uniform_int_distribution<int>& ran_pos, double m[DATA])
{
    int SIZE = L*L;

    double sum; //To compute the sum of spins
    double chi, heat; //To compute magnetic susceptibility
    double old_sum, old_chi, old_heat, old_energy;


    double pa = 1.0 - exp(- 2.0 / tstar); //TODO change

    for (int i=0; i < DATA; i++) m[i] = 0.0; //Init the values

    //Make changes and then average
    for (int j=0; j < 15; j++) add_to_cluster(spins, neighs, L, ran_pos(gen), energy, pa, gen, ran_u);

	///----- TODO: optimize the number of steps for thermalization/measures
	old_sum = 0.0;
	old_chi = 0.0;
	old_heat = 0.0;
	old_energy = 0.0;


	for (int i=0; i < N; i++){

    	    for (int j=0; j < 12; j++){
		add_to_cluster(spins, neighs, L, ran_pos(gen), energy, pa, gen, ran_u);
	    }

	sum = abs(magnetization(spins, L));
	chi = sum * sum;
	heat = energy * energy;

	//Add all the quantities
	m[MAG] += sum; //Magnetization
	m[MAG2] += chi; //For the susceptibility
	m[MAG4] += chi * chi; //For the Binder cumulant and also variance of susceptibility
	m[ENE] += energy; //Energy
	m[ENE2] += heat; //For specific heat
	m[ENE4] += heat * heat; //For the variance of specific heat
	
	//This are used for errors,
	m[MAGERR] += old_sum * sum; //in magnetization
	m[ENERR] += old_energy * energy; //in energy
	m[SUSERR] += old_chi * chi; //in susceptibility
	m[CHERR] += old_heat * heat; //in specific heat

	//Get the value for the next iteration
	old_sum = sum;
	old_energy = energy;
	old_chi = chi;
	old_heat = heat;
    }

    for (int i=0; i < DATA; i++)  m[i] /= (1.0 * N);

	return;
}

//Flip a spin via Metropolis
void flip_spin(bool* spins, int** neighs, int L, double h[5], double& energy, mt19937& gen, uniform_real_distribution<double>& ran_u, uniform_int_distribution<int>& ran_pos)
{

	int SIZE = L*L;
    int index = ran_pos(gen); 
    int sum_neigh = spins[neighs[index][UP]] + spins[neighs[index][DOWN]] + spins[neighs[index][RIGHT]] + spins[neighs[index][LEFT]];
    int change = spins[index] ? 2.0 * (sum_neigh) - 4.0 : 4.0 - 2.0 * (sum_neigh);

    //Apply Metropolis skim
    if (ran_u(gen) < h[(change+4)/2])
    {
        spins[index] = !spins[index];
        energy += (2.0*change)/(1.0*SIZE);
    }

    return;
}

//Compute the magnetization
double magnetization(bool* spins, int L)
{
	int SIZE = L*L;

    double sum = 0.0;
    //Sum all the values of the spins
    for (int i=0; i < SIZE; i++)
    {
        sum += spins[i];
    }
    //And then return them
    return (2.0 * sum - SIZE) / SIZE;
}

//Computes the energy of the system
double get_energy(bool* spins, int** neighs, int L)
{

    int SIZE = L*L;
    int sum_neigh;
    int energy = 0; //Sum

    //For every spin,
    for (int i=0; i < SIZE; i++)
    {
        //Get sum of the neighbours
        sum_neigh = spins[neighs[i][UP]] + spins[neighs[i][DOWN]] + spins[neighs[i][RIGHT]] + spins[neighs[i][LEFT]];
        //And compute the energy change
        energy += spins[i] ? 2.0 * (sum_neigh) - 4.0 : 4.0 - 2.0 * (sum_neigh);
    }

    return 2.0 * energy / (1.0 * SIZE); //Return the energy
}

//Executes Wolff algorithm in a recursive way
void add_to_cluster(bool* spins, int** neighs, int L, int pos, double& energy, double p, mt19937& gen, uniform_real_distribution<double>& ran_u)
{
    cout << "top of atc" << pos << endl;
    int SIZE = L*L;
    int n; //Neighbour position


    //Compute sum of neighbours and change in energy
    cout << "spins indexing in atc" << pos << endl;
    int sum_neigh = spins[neighs[pos][UP]] + spins[neighs[pos][DOWN]] + spins[neighs[pos][RIGHT]] + spins[neighs[pos][LEFT]];
    cout << "after spins indexing in atc" << endl;
    int delta_energy = spins[pos] ? 2.0 * (sum_neigh) - 4.0 : 4.0 - 2.0 * (sum_neigh);
    cout << "after delta energy" << endl;

    //Then modify the energy
    energy += (2.0 * delta_energy) / (1.0 * SIZE);


    cout << "spin flip" << endl;
    spins[pos] = !spins[pos]; //Flip the spin

    cout << "before spin flip" << endl;

    //cout << "just before atc-loop" << endl;
    //For every neighbour,
    for (int i=0; i < 4; i++)
    {
    cout << "inside atc-loop" << endl;
        n = neighs[pos][i]; //Get its position
        if (spins[n] != spins[pos]) //Check if it the same (remember we flipped)
        {
	cout << "in if-test" << endl;
            //Add it to the cluster with certain probability.
            if (ran_u(gen) < p)
            {
		    cout << "before recursive atc-call" << endl;
                add_to_cluster(spins, neighs, L, n, energy, p, gen, ran_u);
		    cout << "after recursive atc-call" << endl;
            }
        }
    }

    return;
}

void w_output(ofstream& file, double tstar, int N, double m[DATA])
{
    file << tstar << " " << 1.0/tstar << " "; //Write T and B

    //We here take in account residual errors, which, for low T, makes the quantities chi, ch, etc.
    //to diverge. This must be substracted. That's why we use an abs for correlation time and also
    //a check to avoid zero value of variances.

    //Then write the quantities and the corresponding errors to a file. The four things are equal,
    //but each one referred to a different quantity.

    double chi = m[MAG2] - m[MAG] * m[MAG]; //Magnetic susceptibility (up to T factor)
    double rhom = chi != 0 ? (m[MAGERR] - m[MAG] * m[MAG]) / chi : 0.0; //Rho magnetization, computed if chi != 0
    double taugm = rhom != 1.0 ? rhom / (1.0 - rhom) : 0.0; //Taug magnetization, computed if rhom != 0
    file << m[MAG] << " " << sqrt(chi * abs(2.0 * taugm + 1) / (1.0*N)) << " "; //Write everything

    double fourth = m[MAG4] - m[MAG2] * m[MAG2]; //Susceptibility variance
    double rhos = fourth != 0.0 ? (m[SUSERR] - m[MAG2] * m[MAG2]) / fourth : 0.0; //Rho susceptibility
    double taugs = rhos != 1.0 ? rhos /(1.0 - rhos) : 0.0; //Taug susceptibility
    double error_sq = sqrt(fourth * abs(2.0 * taugs + 1) / (1.0*N));
    file << " " << chi << " " << error_sq << " ";

    double heat = m[ENE2] - m[ENE] * m[ENE]; //Specific heat (up to T^2 factor)
    double rhoe = heat != 0.0 ? (m[ENERR] - m[ENE]*m[ENE]) / heat : 0.0;
    double tauge = rhoe != 1.0 ? rhoe / (1.0 - rhoe) : 0.0;
    file << " " << m[ENE] << " " << sqrt(heat * abs(2.0 * tauge + 1) / (1.0*N)) << " ";

    double fourth_ene = m[ENE4] - m[ENE2] * m[ENE2];
    double rhoc = fourth_ene != 0.0 ? (m[CHERR] - m[ENE2] * m[ENE2]) / fourth_ene : 0.0;
    double taugc = rhoc != 1.0 ? rhoc / (1.0 - rhoc) : 0.0;
    file << " " << heat << " " << sqrt(fourth_ene * abs(2.0 * taugc + 1) / (1.0*N)) << " ";

    //Binder cumulant
    double binder = 1.0 - m[MAG4]/(3.0 * m[MAG2] * m[MAG2]); //Computes 4th cumulant minus one, b-1.
    file << binder << " " << 2.0 * (1.0 - binder) * (error_sq / m[MAG2]) << endl;

    return;
}

