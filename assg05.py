import os
import time
import psutil
from z3 import Solver, Bool, sat, Or
from pysat.solvers import Glucose3
from pysat.formula import CNF
from pysat.solvers import Lingeling



numPorts = 0
numVehicles = 0
data = []

vars1 = 0
clauses1 = 0
vars2 = 0
clauses2 = 0

option1CNF = "option1.cnf"
option2CNF = "option2.cnf"


arb1 = 0
arb2 = 0
vIDList = []


def compute(v1,v2):
    return v1/v2


def check(v1,v2,v3,v4):
    return v1 <= v2 < v3 + v4

def writeCNF(filename,varC,clauses,n):
    length = len(clauses)
    with open(filename, 'w') as f:
        f.write(f"c DIMACS CNF encoding ==> Option {n}\n")
        f.write(f"c {numPorts} ports\n")
        f.write(f"c {numVehicles} vehicles\n")
        f.write(f"p cnf {varC} {length}\n")
        
        for clause in clauses:
            f.write(" ".join(map(str, clause)) + " 0\n")

def genOption1():
    global arb1
    # Initialize data structures
    variable_map = {}
    constraints = []
    var_index = 0

    nP = numPorts + 1
    nV = numVehicles + 1
    
    # Define variables for each vehicle's charging start time at a port
    for vehicle in range(1, nV):
        arrival, departure, charge_duration = data[vehicle - 1][1:4]

        x = departure - charge_duration + 1
        
        for port in range(1, nP):
            for start_time in range(arrival, x):
                arb1+=1
                var_index += 1
                variable_map[(vehicle, port, start_time)] = var_index
    
    # Constraint 1: Each vehicle must begin charging exactly once
    for vehicle in range(1, nV):
        arrival, departure, charge_duration = data[vehicle - 1][1:4]
        vehicle_choices = []

        x = departure - charge_duration + 1
        
        for port in range(1, nP):
            for start_time in range(arrival, x):
                arb1+=1
                vehicle_choices.append(variable_map[(vehicle, port, start_time)])
        
        # Ensure at least one charging start time is selected
        constraints.append(vehicle_choices)
        
        # Prevent multiple start times (mutual exclusion)
        for i, var1 in enumerate(vehicle_choices):
            for var2 in vehicle_choices[i + 1:]:
                arb1+=1
                constraints.append([-var1, -var2])
    
    # Constraint 2: No simultaneous charging at the same port
    for port in range(1, nP):
        for vehicle1 in range(1, nV):
            a1, d1, c1 = data[vehicle1 - 1][1:4]

            temp1 = d1 - c1 + 1
            
            for t1 in range(a1,temp1 ):
                for vehicle2 in range(1, nV):
                    if vehicle1 != vehicle2:
                        a2, d2, c2 = data[vehicle2 - 1][1:4]
                        
                        temp2 = d2 - c2 + 1
                        for t2 in range(a2, temp2):
                            arb1+=1
                            # Check for overlapping charging periods
                            if (check(t1,t2,t1,c1)) or (check(t2,t1,t2,c2)):
                                if (vehicle1, port, t1) in variable_map and (vehicle2, port, t2) in variable_map:
                                    constraints.append([-variable_map[(vehicle1, port, t1)], -variable_map[(vehicle2, port, t2)]])
    
    # Write constraints to CNF file
    writeCNF(option1CNF, var_index, constraints, 1)

    length = len(constraints)
    
    return var_index, length

def genOption2():
    global arb2
    # Variables:
    # x_ij -> Vehicle i is assigned to port j
    # y_it -> Vehicle i starts charging at time t
    variable_map = {}
    constraints = []
    var_index = 0

    nP = numPorts + 1
    nV = numVehicles + 1

    # Define x_ij variables (vehicle-port assignment)
    for vehicle in range(1, nV):
        for port in range(1, nP):
            var_index += 1
            variable_map[('x', vehicle, port)] = var_index

    # Define y_it variables (vehicle charging start times)
    for vehicle in range(1, nV):
        arrival, departure, charge_duration = data[vehicle - 1][1:4]

        x = departure - charge_duration + 1

        for start_time in range(arrival, x):
            arb2+=1
            var_index += 1
            variable_map[('y', vehicle, start_time)] = var_index

    # Constraint 1: Each vehicle is assigned exactly one port
    for vehicle in range(1, nV):
        port_choices = [variable_map[('x', vehicle, port)] for port in range(1, numPorts + 1)]
        
        # Ensure at least one port is chosen
        constraints.append(port_choices)
        
        # Ensure at most one port is assigned (mutual exclusivity)
        for p1 in range(1, nP):
            for p2 in range(p1 + 1, nP):
                arb2+=1
                constraints.append([-variable_map[('x', vehicle, p1)], -variable_map[('x', vehicle, p2)]])

    # Constraint 2: Each vehicle must have exactly one charging start time
    for vehicle in range(1, nV):
        arrival, departure, charge_duration = data[vehicle - 1][1:4]
        start_time_vars = [variable_map[('y', vehicle, t)] for t in range(arrival, departure - charge_duration + 1)]
        
        # At least one valid charging start time
        constraints.append(start_time_vars)

        x = departure - charge_duration + 1
        
        # Ensure at most one start time is chosen (mutual exclusivity)
        for t1 in range(arrival, x):
            for t2 in range(t1 + 1, x):
                arb2+=1
                constraints.append([-variable_map[('y', vehicle, t1)], -variable_map[('y', vehicle, t2)]])

    # Constraint 3: Prevent simultaneous charging conflicts at the same port
    for port in range(1, nP):
        for v1 in range(1, nV):
            a1, d1, c1 = data[v1 - 1][1:4]

            temp1 = d1 - c1 + 1

            for t1 in range(a1,temp1 ):
                for v2 in range(v1 + 1, nV):
                    a2, d2, c2 = data[v2 - 1][1:4]

                    temp2 = d2 - c2 + 1

                    for t2 in range(a2,temp2 ):
                        arb2+=1
                        # Check if charging intervals overlap
                        if (check(t1,t2,t1,c1)) or (check(t2,t1,t2,c2)):
                            # Ensure vehicles don't charge simultaneously at the same port
                            constraints.append([-variable_map[('x', v1, port)], -variable_map[('x', v2, port)],
                                               -variable_map[('y', v1, t1)], -variable_map[('y', v2, t2)]])

    # Write constraints to CNF file
    writeCNF(option2CNF, var_index, constraints, 2)

    length = len(constraints)

    return var_index, length


def takeInput(filename):
    global numPorts,numVehicles

    if not os.path.isfile(filename):
        print("File not exist hence default input")
        numPorts=5
        numVehicles=10
        temp=[(1, 4, 10, 3), (2, 7, 20, 6), (3, 8, 27, 10), (4, 5, 15, 4), (5, 10, 25, 8), (6, 12, 30, 5), (7, 3, 12, 6), (8, 9, 18, 7), (9, 6, 22, 9), (10, 11, 28, 4)]
        for i in temp:
            data.append(i)
        return
    

    with open(filename, 'r') as f:
        lines = f.readlines()

    for line in lines:
        parts = line.split()
        
        if not line:
            continue
        if line.startswith('%'):
            continue
        if line.startswith('NumPorts'):
            a = parts[1]
            numPorts = int(a)
        elif line.startswith('NumVehicles'):
            b=parts[1]
            numVehicles = int(b)
        elif line.startswith('V'):
            c=parts[0]
            vID = int(parts[1])
            vIDList.append(vID)
            aT = int(parts[2])
            dT = int(parts[3])
            cT = int(parts[4])
            to_app=(vID, aT, dT, cT)
            data.append(to_app)
    
#
#Z3 Solver
#
def withZ3(dimacs_file):
    # Track execution time and memory usage
    start_time = time.time()
    process = psutil.Process(os.getpid())
    start_memory = process.memory_info().rss / (1024 * 1024)  # Convert bytes to MB

    # Initialize Z3 solver
    solver = Solver()
    variable_map = {}

    # Read and parse the DIMACS file
    with open(dimacs_file, 'r') as file:
        for line in file:
            line = line.strip()

            # Ignore comments
            if line.startswith('c'):
                continue

            # Parse the problem header
            if line.startswith('p cnf'):
                _, _, var_count, _ = line.split()
                var_count = int(var_count)

                vC = var_count + 1

                # Create Boolean variables for Z3
                for i in range(1, vC):
                    variable_map[i] = Bool(f"x_{i}")
                continue

            # Process clauses
            literals = list(map(int, line.split()[:-1]))  # Ignore trailing zero
            clause = [(variable_map[abs(lit)] if lit > 0 else ~variable_map[abs(lit)]) for lit in literals]

            # Add clause to solver
            solver.add(Or(clause))

    # Solve SAT problem
    result = solver.check()

    # Measure execution time and memory usage
    end_time = time.time()
    end_memory = process.memory_info().rss / (1024 * 1024)  # Convert bytes to MB

    return {
        'is_satisfiable': result == sat,
        'time_taken': end_time - start_time,
        'memory_usage': end_memory - start_memory
    }



#
#Glucose
#

def withGlucose(dimacs_file):
    start_time = time.time()
    process = psutil.Process(os.getpid())
    start_memory = process.memory_info().rss / 1024 / 1024  # Memory in MB

    # Create a Glucose solver
    solver = Glucose3()

    # Parse the DIMACS file
    formula = CNF(from_file=dimacs_file)
    for clause in formula.clauses:
        solver.add_clause(clause)

    # Check satisfiability
    result = solver.solve()

    end_time = time.time()
    end_memory = process.memory_info().rss / 1024 / 1024

    time_taken = end_time - start_time
    memory_usage = end_memory - start_memory


    return {
        'is_satisfiable': result,
        'time_taken': time_taken,
        'memory_usage': memory_usage
    }

#
#Lingeling
#
def withLingeling(dimacs_file):
    start_time = time.time()
    process = psutil.Process(os.getpid())
    start_memory = process.memory_info().rss / 1024 / 1024  # Memory in MB

    # Create a Lingeling solver
    solver = Lingeling()

    # Parse the DIMACS file
    formula = CNF(from_file=dimacs_file)
    for clause in formula.clauses:
        solver.add_clause(clause)

    # Check satisfiability
    result = solver.solve()

    end_time = time.time()
    end_memory = process.memory_info().rss / 1024 / 1024

    time_taken = end_time - start_time
    memory_usage = end_memory - start_memory


    return {
        'is_satisfiable': result,
        'time_taken': time_taken,
        'memory_usage': memory_usage
    }


def calSize(dimacs_file):
    #sizes = {2: 0, 3: 0, 'more': 0}
    a=0
    b=0
    c=0
    
    with open(dimacs_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line.startswith('c') and not line.startswith('p'):
                literals = line.split()[:-1]  # Exclude the trailing 0
                size = len(literals)
                if size == 2:
                    a += 1
                elif size == 3:
                    b += 1
                elif size > 3:
                    c += 1
    
    d=[]
    d.append(a)
    d.append(b)
    d.append(c)
    return d


filename = input("Enter the file to read and perform\n")
takeInput(filename)

#print(data)
    
    
vars1, clauses1 = genOption1()
vars2, clauses2 = genOption2()


opt1 = calSize(option1CNF)
opt2 = calSize(option2CNF)



def output(res1,res2,st):
    print(f"=== Analysis {st} ===")
    print("\nScheme 1 (x_ijt):")
    print(f"Computation time: {res1['time_taken']:.4f} seconds")
    print(f"Memory usage: {res1['memory_usage']:.4f} MB")
    if res1['is_satisfiable']:
        print("Satisfiable")
    else:
        print("Unsatisfiable")
    
    print("\nScheme 2 (x_ij, y_it):")
    print(f"Computation time: {res2['time_taken']:.4f} seconds")
    print(f"Memory usage: {res2['memory_usage']:.4f} MB")
    if res2['is_satisfiable']:
        print("Satisfiable")
    else:
        print("Unsatisfiable")

    cT = compute(res1['time_taken'],res2['time_taken'])
    mU = compute(res1['memory_usage'],res2['memory_usage'])

    print(f"\nComparison ratio of Option 1 to Option 2 {st} :")
    print(f"Option 1 ratio Option 2 w.r.t. computation time: {cT:.4f}x")
    print(f"Option 1 ratio Option 2 w.r.t. memory usage: {mU:.4f}x")
    


result1Z3 = withZ3(option1CNF)
result2Z3 = withZ3(option2CNF)

result1Glu = withGlucose(option1CNF)
result2Glu = withGlucose(option2CNF)

result1L = withLingeling(option1CNF)
result2L = withLingeling(option2CNF)



print("\n")
print("For Option 1 Scheme (x_ijt) ")
print(f"Variables: {vars1}")
print(f"Clauses: {clauses1}")
size1 = os.path.getsize(option1CNF) / 1024
print(f"Option1 CNF size: {size1:.2f} kB")
print(f"Clauses with 2 literals: {opt1[0]}")
print(f"Clauses with 3 literals: {opt1[1]}")
print(f"Clauses with 3+ literals: {opt1[2]}")
print("\n")
print("For Option 2 Scheme (x_ij, y_it)")
print(f"Variables: {vars2}")
print(f"Clauses: {clauses2}")
size2 = os.path.getsize(option2CNF) /1024
print(f"Option2 CNF size: {size2:.2f} kB")
print(f"Clauses with 2 literals: {opt2[0]}")
print(f"Clauses with 3 literals: {opt2[1]}")
print(f"Clauses with 3+ literals: {opt2[2]}")
print("\n")
varC = compute(vars1,vars2)
clausesC = compute(clauses1,clauses2)
sizeC = compute(size1,size2)
print(f"Option 1 ratio Option 2 w.r.t. variables: {varC:.4f}x")
print(f"Option 1 ratio Option 2 w.r.t. clauses: {clausesC:.4f}x")
print(f"Option 1 ratio Option 2 w.r.t. file size: {sizeC:.4f}x")


print("\n\n")
output(result1Z3,result2Z3,"For Z3")
print("\n\n\n")
output(result1Glu,result2Glu,"For Glucose3")
print("\n\n\n")
output(result1L,result2L,"For Lingeling")