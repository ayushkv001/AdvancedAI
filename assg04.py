import os
from z3 import *


flag = True

# Initialize variables for number of students and tasks
num_students = 1  
num_tasks = 0  
task_completion_times = []  

def read_input_file():
    global num_students, num_tasks
    filename = input("Enter the input file name (file should be present in the same directory as this code): ")

    if not os.path.isfile(filename):
        print("Error opening the file")
        return

    with open(filename, 'r') as file:
        lines = file.readlines()

    for line in lines:
        line = line.strip()
        if not line or line.startswith('%'):  # Skip empty or comment lines
            continue
        
        divided = line.split()
        command = divided[0]

        if command == "N":  # Number of tasks
            num_tasks = int(divided[1])
        
        elif command == "G":  # Number of students
            num_students = int(divided[1])
        
        elif command == "P":  # Task completion times
            task_completion_times.append(int(divided[2]))

# Read the input file
read_input_file()

# Print values for verification
print("Number of tasks :")
print(num_tasks)
print("Number of students :")
print(num_students)
print("List of Task Completion time :")
print(task_completion_times)
print("Sum of completion time :")
print(sum(task_completion_times))
print()

x= sum(task_completion_times)

# Initialize the Z3 solver
solver = Solver()

# Create Z3 variables
task_assignment = [[Bool(f"task_{i}_{j}") for j in range(num_students)] for i in range(num_tasks)]
student_load = [Int(f"load_{j}") for j in range(num_students)]
max_time = Int('max_time')

# Constraints to ensure each task is assigned to exactly one student
for i in range(num_tasks):
    solver.add(Sum([If(task_assignment[i][j], 1, 0) for j in range(num_students)]) == 1)

# Constraints to calculate each student's total load
for j in range(num_students):
    solver.add(student_load[j] == Sum([If(task_assignment[i][j], task_completion_times[i], 0) for i in range(num_tasks)]))

# Constraints for the maximum load across all students
for j in range(num_students):
    solver.add(max_time >= student_load[j])
for j in range(num_students):
    solver.add(student_load[j] <= max_time)

# Define lower and upper bounds for max_time
lower_bound = (x // num_students) + 1  
upper_bound = x  - lower_bound

# Search for a feasible solution by trying different values for max_time
for candidate_time in range(lower_bound, upper_bound + 1):
    solver.push()  # Save current solver state
    solver.add(max_time == candidate_time)

    if solver.check() == sat:
        model = solver.model()
        flag = False

        print()
        
        print(f"Solution found with max completion time : {candidate_time}")
        
        # Output task assignments
        for i in range(num_tasks):
            for j in range(num_students):
                if model[task_assignment[i][j]]:
                    print(f"Task {i+1} assigned to Student {j+1}")
        
        # Output student loads
        for j in range(num_students):
            print(f"Student {j+1} load: {model[student_load[j]]}")
        
        print(f"Maximum time: {model[max_time]}")
        solver.pop()  # Restore solver state
        break
    else:
        print("No feasible solution found for this candidate time")
    solver.pop()  # Restore solver state
if(flag):
    print("No solution found within the given bounds")