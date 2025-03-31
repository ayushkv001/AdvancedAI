import random

GROUP_SIZE_START = 5
GROUP_SIZE_END = 20
PROBLEM_COUNT_START = 10
PROBLEM_COUNT_END = 100
TIME_REQUIRED_START = 5
TIME_REQUIRED_END = 100

FILE_NUMBER=100

def genTestCase():
    group_size = random.randint(GROUP_SIZE_START, GROUP_SIZE_END)


    num_problems = random.randint(PROBLEM_COUNT_START, PROBLEM_COUNT_END)
    
    test_case = []
    test_case.append(f"%groupsize \nG {group_size}")
    test_case.append(f"%Totalnumberofproblems \nN {num_problems}")
    
    test_case.append("%problem-id time-required\n")
    for i in range(1,num_problems+1):
        problem_id = i
        time_required = random.randint(TIME_REQUIRED_START, TIME_REQUIRED_END)  
        test_case.append(f"P {problem_id} {time_required}")
    
    return "\n".join(test_case)

def genAllTestCases():
    for i in range(1,FILE_NUMBER + 1):
        with open(f"test_case{i}.dag", "w") as file:
            test_case = genTestCase()
            file.write(test_case)  

genAllTestCases()

print("100 test cases generated and saved as 'test_case[00].dag' where [00] is the number of file grom 1 to 100")
