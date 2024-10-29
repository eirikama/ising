#!/bin/bash

# Define the range or list of values for L
L_values=(8 16 32 64 128 256 516)  # Add your desired values of L here

# Loop through each value of L
for L in "${L_values[@]}"; do
    echo "Running simulation for system size L = $L"
    ./ising_simulation "$L" 100000  # Replace 'your_program_name' with the actual executable name
done

echo "All simulations completed."
