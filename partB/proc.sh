#!/bin/bash

# Function to generate and run processes
run_process() {
    i=$1
    n=$2
    output_file="process_${i}_output.txt"
    
    # Start the output file with basic info
    echo "Process $i: Random number n = $n" > "$output_file"
    echo "Generated multiples of $i up to $n:" >> "$output_file"
    
    # Prepare the input for a.out
    {
        echo "$n"  # First, pass the value of n
        for (( j=1; j<=n; j++ ))
        do
            echo "$((i * j))"  # Each subsequent line is a multiple of i
        done
    } | ./a.out >> "$output_file" &  # Pipe the input to a.out and append the output to the file
}

# Create 10 processes
for i in {1..50}
do
    # Generate a random number between 1 and 100
    n=$((RANDOM % 100 + 1))
    run_process $i $n
done

# Wait for all processes to complete
wait

echo "All processes completed."
