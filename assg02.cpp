/*
README:
for taking input
Enter the file name (*.dag file) after running the program (should be presnt in the same directory as this code)
User input Menu and 'k' days should be entered from console
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

struct Dependency {
    int id, input1, input2, output;
    string food;
};

struct MenuItem {
    string food;
    int cost;
};

struct MenuCount {
    string food;
    int count;
};

struct ScheduleData {
    int totalDays, totalCost;
};


vector<vector<pair<int, string>>> mindaypath;
vector<vector<pair<int, string>>> mincostpath;
vector<vector<pair<int, string>>> minposcostpath;
vector<vector<pair<int, string>>> minmenupath;
int totalSchedules = 0, minDays = 1000, maxDays = 0, minCost = 10000, maxCost = 0,t3=10000,minposcost=10000;
vector<MenuItem> menuItems;
vector<MenuCount> inputMenu;
int k;
bool isLess =false,menuValid=false;

int getFoodCost(const string &food, const vector<MenuItem> &menuItems) {
    auto it = find_if(menuItems.begin(), menuItems.end(), [&food](const MenuItem &m) { return m.food == food; });
    return it != menuItems.end() ? it->cost : 0;
}
bool checkTemp(map<string,int> temp){
    for(auto a: inputMenu){
        if(a.count<temp[a.food]){
            return false;
        }
    }
    return true;
}

void scheduleConditionCheck(const vector<vector<pair<int, string>>> &schedulePath) {
    totalSchedules++;
    map<string, int> foodCount;
    vector<MenuCount> menuCountList;
    int totalCost = 0, numDays = schedulePath.size();
    
    for (const auto &day : schedulePath) {
        map<string, int> dayFoodCount;
        for (const auto &task : day) dayFoodCount[task.second]++; 
        for (const auto &item : dayFoodCount) foodCount[item.first] = max(foodCount[item.first], item.second); 
    }
     
    for (const auto &item : foodCount) {
        totalCost += getFoodCost(item.first, menuItems) * item.second;
    }

    if(minDays>numDays){
        minDays=numDays;
        mindaypath=schedulePath;
    }

    if(maxDays<numDays)
    maxDays=numDays;

    if(minCost>totalCost){
        minCost=totalCost;
        mincostpath=schedulePath;
    }

    if(maxCost<totalCost)
    maxCost=totalCost;

    if(menuValid){
        if(checkTemp(foodCount)){
            if(t3>numDays){
                t3=numDays;
                minmenupath=schedulePath;
            }
        }
    }

    if(numDays<=k){
        isLess=true;
        if(minposcost>totalCost){
            minposcost=totalCost;
            minposcostpath=schedulePath;
        }
    }

}

bool hasAchievedFinalOutcome(const vector<int> &availableAssignments, const vector<int> &goalStates) {
    return all_of(goalStates.begin(), goalStates.end(), [&availableAssignments](int goal) {
        return find(availableAssignments.begin(), availableAssignments.end(), goal) != availableAssignments.end();
    });
}

void exploreScheduleByBFS(vector<int> &taskStatus, vector<int> &availableAssignments, vector<vector<pair<int, string>>> &schedulePath, 
                     vector<Dependency> &dependencies, int groupMember, vector<int> &goalStates) {

    queue<tuple<vector<int>, vector<int>, vector<vector<pair<int, string>>>, vector<int>>> bfsQueue;
    set<pair<vector<int>, vector<int>>> visitedStates;
    bfsQueue.push({taskStatus, availableAssignments, schedulePath, goalStates});

    while (!bfsQueue.empty()) {
        auto front = bfsQueue.front();
        vector<int> currentTaskStatus = get<0>(front);
        vector<int> currentAvailableItems = get<1>(front);
        vector<vector<pair<int, string>>> currentSchedulePath = get<2>(front);
        vector<int> currentFinalOutcome = get<3>(front);

        bfsQueue.pop();

        if (hasAchievedFinalOutcome(currentAvailableItems, currentFinalOutcome)) {
            scheduleConditionCheck(currentSchedulePath);
            continue;
        }

        vector<pair<int, pair<int, string>>> validAssignments;
        for (int depId = 1; depId <= dependencies.size(); depId++) {
            if (currentTaskStatus[depId] == 0 && find(currentAvailableItems.begin(), currentAvailableItems.end(), dependencies[depId - 1].input1) != currentAvailableItems.end()
                && find(currentAvailableItems.begin(), currentAvailableItems.end(), dependencies[depId - 1].input2) != currentAvailableItems.end()) {
                validAssignments.push_back({depId, {dependencies[depId - 1].output, dependencies[depId - 1].food}});
            }
        }

        if (validAssignments.empty()) continue;

        int n=validAssignments.size();

        vector<bool> taskSelection(n, false);
        fill(taskSelection.begin(), taskSelection.begin() + groupMember, true);
        do {
            vector<int> nextTaskStatus = currentTaskStatus, nextAvailableItems = currentAvailableItems;
            vector<vector<pair<int, string>>> nextSchedulePath = currentSchedulePath;
            vector<pair<int, string>> currentAssignments;

            for (int i = 0; i < n; i++) {
                if (taskSelection[i]) {
                    int depId = validAssignments[i].first;
                    currentAssignments.push_back({depId, validAssignments[i].second.second});
                    nextAvailableItems.push_back(dependencies[depId - 1].output);
                    nextTaskStatus[depId] = 1;
                }
            }

            nextSchedulePath.push_back(currentAssignments);
            pair<vector<int>, vector<int>> stateKey = {nextTaskStatus, nextAvailableItems};
            if (visitedStates.insert(stateKey).second) bfsQueue.push({nextTaskStatus, nextAvailableItems, nextSchedulePath, currentFinalOutcome});
        } while (prev_permutation(taskSelection.begin(), taskSelection.end()));
    }
}

void printSchedule(const vector<vector<pair<int, string>>>& schedulePath,vector<MenuItem> menuItems){
    int numDays=schedulePath.size();
    vector<MenuCount> count;
    map<string, int> foodCount;
    int totalCost = 0;
    

    for (const auto &day : schedulePath) {
        map<string, int> dayFoodCount;
        for (const auto &task : day) dayFoodCount[task.second]++;
        for (const auto &item : dayFoodCount) foodCount[item.first] = max(foodCount[item.first], item.second);
    }

    for (int i = 0; i < numDays; i++) {
        cout << "Day " << i + 1 << " :\n";
        for (const auto &task : schedulePath[i]) cout << "Assignment: " << task.first << "\t";
        cout << endl;
    }

    cout << "Menu : ==>\n< ";
    for (const auto &item : foodCount) {
        cout << item.first << " - " << item.second << " ";
        totalCost += getFoodCost(item.first, menuItems) * item.second;
    }
    cout << " >\nTotal time: " << numDays << " days\nDaily cost: " << totalCost  << endl;
}

void checkGivenMenu(){
    int m=menuItems.size();
    int n=0;
    for(auto x:menuItems){
        for(auto y: inputMenu ){
            if(x.food==y.food)
            {
                n++;
                break;
            }
        }
    }
    if(n==m)
    menuValid=true;
}


int main() {
    cout<<"Enter the input file name(file should be present in same directory as this code)"<<endl;
    string filename;
    cin>>filename; 
    ifstream inputFile(filename);
    if (!inputFile) { 
        cout << "Error opening the file\n"; 
        return 1; 
    }

    vector<Dependency> dependencies;
    int groupMember;
    vector<int> initialItems, goalStates;
    string line;
    vector<string> lines;

    while (getline(inputFile, line)) lines.push_back(line);
    inputFile.close();

    for (const string &line : lines) {
        if (line.empty() || line[0] == '%') continue;
        stringstream stringRead(line);
        string command;
        stringRead >> command;

        if (command == "C") { 
            string foodName; 
            int cost; 
            stringRead >> foodName >> cost; 
            menuItems.push_back({foodName, cost});
        }
        if (command == "G") stringRead >> groupMember;
        if (command == "I") { 
            int item; 
            while (stringRead >> item) initialItems.push_back(item); 
        }
        if (command == "O") { 
            int item; 
            while (stringRead >> item) goalStates.push_back(item); 
        }
        if (command == "A") { 
            Dependency dep; 
            stringRead >> dep.id >> dep.input1 >> dep.input2 >> dep.output >> dep.food; 
            dependencies.push_back(dep); 
        }
    }


    cout<<"Enter User Input Menu as <Food> <Quantity>"<<endl;
    for(int j=0;j<menuItems.size();j++){
        string food;
        int count;
        cin>>food>>count;
        inputMenu.push_back({food,count});
    }

    cout<<"\nEnter 'k' days"<<endl;
    cin>>k;

    checkGivenMenu();
    
    
    vector<int> taskStatus(dependencies.size() + 1, 0);
    vector<vector<pair<int, string>>> schedulePath;
    exploreScheduleByBFS(taskStatus, initialItems, schedulePath, dependencies, groupMember, goalStates);

    //cout<<"Total schedules found:\t"<<totalSchedules<<endl;
    //cout<<endl;

    cout<<"Question 1 : Minimum Days"<<endl;
    printSchedule(mindaypath,menuItems);
    cout<<"\n***************************"<<endl;
    cout<<endl;
    cout<<"Question 2 : Minimum Per days cost"<<endl;
    printSchedule(mincostpath,menuItems);
    cout<<"\n***************************"<<endl;
    cout<<endl;

    cout<<"Question 3 : Given Menu"<<endl;

    if(t3==10000){
        cout<<"infeasible"<<endl;
    }
    else
    printSchedule(minmenupath,menuItems);

    cout<<"\n***************************"<<endl;
    cout<<endl;

    cout<<"Question 4 : 'k' days"<<endl;
    if(isLess){
        printSchedule(minposcostpath,menuItems);
    }
    else
    cout<<"cannot be done under "<<k<<" days\n";
    cout<<"\n***************************"<<endl;
    cout<<endl;
    return 0;
}
