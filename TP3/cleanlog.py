def aggregate_lines(input_file, output_file):
    aggregated_lines = []

    # Read input file and aggregate lines while excluding consecutive duplicates
    with open(input_file, 'r') as file:
        previous_line = None
        for line in file:
            line = line.strip()
            if line != previous_line:
                aggregated_lines.append(line)
            previous_line = line

    # Write aggregated lines to output file
    with open(output_file, 'w') as file:
        for line in aggregated_lines:
            file.write(line + '\n')

# Usage example
input_file = 'log.txt'  # Replace with your input file name
output_file = 'log.txt'  # Replace with your desired output file name

aggregate_lines(input_file, output_file)