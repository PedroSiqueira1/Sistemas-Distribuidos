import sys

def check_file_lines(filename, n, r):
    with open(filename, 'r') as file:
        lines = file.readlines()
    
    total_lines = len(lines)
    expected_lines = n * r
    
    if total_lines == expected_lines:
        print(f"The file '{filename}' has {expected_lines} lines.")
    else:
        print(f"The file '{filename}' does not have {expected_lines} lines. Actual lines: {total_lines}")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python check_file_lines.py <filename> <n> <r>")
        sys.exit(1)
    
    filename = sys.argv[1]
    r = int(sys.argv[2])
    n = int(sys.argv[3])
    
    check_file_lines(filename, n, r)