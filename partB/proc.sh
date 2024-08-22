#!/bin/bash

# Function to generate and run processes
create_sets() {
    i=$1
    n=$2
    output_file="set_${i}.txt"
    
    echo "Set no. $i: Random number n = $n" >> "$output_file"
    echo "We are generating $n multiples of $i:" >> "$output_file"
    
    # Prepare the input for a.out
    {
        echo "$n"  # First we give the value of n
        for (( j=1; j<=n; j++ ))
        do
            echo "$((i * j))"  # Each subsequent line is a multiple of i
        done
    } | ./a.out >> "$output_file" & 
    # '&' should send the process in background making this a parallel execution
}

# Create 50 processes
for i in {1..50}
do
    # Generate a random number between 1 and 100 (both inclusive)
    n=$((RANDOM % 100 + 1))
    create_sets $i $n
done

# Wait for all processes to complete
wait

echo "All processes completed."
