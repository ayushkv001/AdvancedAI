TO RUN THE CODE :
1. open the code directory(where the codefile is saved) in terminal (codefile is saved as "filename.cpp")
2. run command "g++ filename.cpp" in terminal


INPUT IN CODE :
The code takes input in file format (*.dag)
Three testcases are already provided in testcases directory
In main() function of code testcase file is input as "../testcases/input01.dag" in code line 'ifstream f("../testcases/input01.dag");'
By default file "input01.dag" is set
To run testcase file "input02.dag" and "input03.dag" ==>
Change the file_path string to run input02.dag and input03.dag
    1. Change file_path string to
        1. "../testcases/input02.dag" to run on input02.dag
        2. "../testcases/input03.dag" to run on input03.dag
    2. run "g++ filename.cpp" in terminal
To run other testcases create a file name "input.dag" in code directory(same as .cpp file) and chage file_path string in main function to "input.dag"
Prepare the input file (input.dag) in the following format:
        Comments: % comment that will be ignored in input
	Food costs: C <food-item> <value>
        Group size: G <group-size>
        Initial resources: I <resource1> <resource2> ... -1
        Target resources: O <resource1> <resource2> ... -1
        Assignments: A <id> <input1> <input2> <output> <food-item> 


OUTPUT
The code print all reasonable schedules and their corresponding daily menu in terminal
As well as total number of schedules discovered 
Minimum and maximum total cost to complete all assignment discovered
Minimum and maximum numbers of days required to complete all assignement