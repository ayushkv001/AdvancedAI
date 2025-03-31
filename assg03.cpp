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
vector<vector<pair<int, string>>> dfbbMenu,dfbbK,aStarMenu,aStarK;
int totalSchedules = 0, minDays = 1000, maxDays = 0, minCost = 10000, maxCost = 0, t3 = 10000,t4=10000, minposcost = 10000;
vector<MenuItem> menuItems;
vector<MenuCount> inputMenu;
int nodeDFS=0,nodeAStarMenu=0,nodeDFBBMenu=0,nodeDFBBk=0,nodeAStarK=0;
int k;
bool isLess = false, menuValid = false,menuA=false,menuDFBB=false,kA=false,kDFBB=false;
int menuCost;

int getFoodCost(const string &food, const vector<MenuItem> &menuItems) {
    auto it = find_if(menuItems.begin(), menuItems.end(), [&food](const MenuItem &m) { return m.food == food; });
    return it != menuItems.end() ? it->cost : 0;
}

bool checkTemp(map<string, int> temp) {
    for (auto a : inputMenu) {
        if (a.count < temp[a.food]) {
            return false;
        }
    }
    return true;
}

void printSchedule(const vector<vector<pair<int, string>>>& schedulePath, vector<MenuItem> menuItems) {
    int numDays = schedulePath.size();
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
    cout << " >\nTotal time: " << numDays << " days\nDaily cost: " << totalCost << endl;
}

//for menu
void printSchedule2(const vector<vector<pair<int, string>>>& schedulePath) {
    int numDays = schedulePath.size();
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
    for (const auto &item : inputMenu) {
        cout << item.food << " - " << item.count << " ";
    }
    cout << " >\nTotal time: " << numDays << " days"<<"\nDaily Cost: "<<menuCost<< endl;
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

    if (minDays > numDays) {
        minDays = numDays;
        //mindaypath = schedulePath;
    }

    if (maxDays < numDays)
        maxDays = numDays;

    if (minCost > totalCost) {
        minCost = totalCost;
        //mincostpath = schedulePath;
    }

    if (maxCost < totalCost)
        maxCost = totalCost;

    if (menuValid) {
        if (checkTemp(foodCount)) {
            if (t3 > numDays) {
                t3 = numDays;
                minmenupath = schedulePath;
            }
        }
    }

    if (numDays <= k) {
        isLess = true;
        if (minposcost > totalCost) {
            minposcost = totalCost;
            minposcostpath = schedulePath;
        }
    }
}

//for DFBB menu
void scheduleConditionCheck2(const vector<vector<pair<int, string>>> &schedulePath) {
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
        
    dfbbMenu = schedulePath;
}

//for DFBB k
void scheduleConditionCheck3(const vector<vector<pair<int, string>>> &schedulePath) {
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
        
    dfbbK = schedulePath;
}

bool hasAchievedFinalOutcome(const vector<int> &availableAssignments, const vector<int> &goalStates) {
    return all_of(goalStates.begin(), goalStates.end(), [&availableAssignments](int goal) {
        return find(availableAssignments.begin(), availableAssignments.end(), goal) != availableAssignments.end();
    });
}


//DFS
void exploreScheduleByDFS(vector<int> &taskStatus, vector<int> &availableAssignments, vector<vector<pair<int, string>>> &schedulePath,
                          vector<Dependency> &dependencies, int groupMember, vector<int> &goalStates) {


    nodeDFS++;
    if (hasAchievedFinalOutcome(availableAssignments, goalStates)) {
        scheduleConditionCheck(schedulePath);
        return;
    }

    vector<pair<int, pair<int, string>>> validAssignments;
    for (int depId = 1; depId <= dependencies.size(); depId++) {
        if (taskStatus[depId] == 0 && find(availableAssignments.begin(), availableAssignments.end(), dependencies[depId - 1].input1) != availableAssignments.end()
            && find(availableAssignments.begin(), availableAssignments.end(), dependencies[depId - 1].input2) != availableAssignments.end()) {
            validAssignments.push_back({depId, {dependencies[depId - 1].output, dependencies[depId - 1].food}});
        }
    }

    if (validAssignments.empty()) return;

    int n = validAssignments.size();
    vector<bool> taskSelection(n, false);
    fill(taskSelection.begin(), taskSelection.begin() + groupMember, true);
    do {
        vector<int> nextTaskStatus = taskStatus, nextAvailableItems = availableAssignments;
        vector<vector<pair<int, string>>> nextSchedulePath = schedulePath;
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
        exploreScheduleByDFS(nextTaskStatus, nextAvailableItems, nextSchedulePath, dependencies, groupMember, goalStates);

    } while (prev_permutation(taskSelection.begin(), taskSelection.end()));
}

//for pruning
bool checkPruning(map<string, int> temp) {
    for (auto a : inputMenu) {
        if (a.count < temp[a.food]) {
            return false;
        }
    }
    return true;
}
bool pruning(const vector<vector<pair<int, string>>> &schedulePath){
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

    return checkPruning(foodCount);
}

//A* for menu
int calculateHeuristic(const vector<int>& taskStatus) {

    int g=0;
    for(auto x : taskStatus){
        if(x==0)
        g++;
    }

    return g-1;
}

void exploreScheduleByAStar(vector<int> &taskStatus, vector<int> &availableAssignments, vector<vector<pair<int, string>>> &schedulePath, 
                            vector<Dependency> &dependencies, int groupMember, vector<int> &goalStates) {
    
    priority_queue<pair<int, tuple<vector<int>, vector<int>, vector<vector<pair<int, string>>>, vector<int>>>, 
                   vector<pair<int, tuple<vector<int>, vector<int>, vector<vector<pair<int, string>>>, vector<int>>>>, 
                   greater<pair<int, tuple<vector<int>, vector<int>, vector<vector<pair<int, string>>>, vector<int>>>>> openList;
    
    set<pair<vector<int>, vector<int>>> visitedStates;
    int firstHeu = calculateHeuristic(availableAssignments)/groupMember;
    
    openList.push({firstHeu, {taskStatus, availableAssignments, schedulePath, goalStates}});
    nodeAStarMenu++;

    while (!openList.empty()) {
        auto front = openList.top();
        openList.pop();

        int waste = front.first;
        vector<int> currentTaskStatus = get<0>(front.second);
        vector<int> currentAvailableItems = get<1>(front.second);
        vector<vector<pair<int, string>>> currentSchedulePath = get<2>(front.second);
        vector<int> currentFinalOutcome = get<3>(front.second);

        if (hasAchievedFinalOutcome(currentAvailableItems, currentFinalOutcome)) {
            aStarMenu=currentSchedulePath;
            nodeAStarMenu++;
            menuA=true;
            return;
        }
        nodeAStarMenu++;
        if(!pruning(currentSchedulePath))
        continue;

        vector<pair<int, pair<int, string>>> validAssignments;
        for (int depId = 1; depId <= dependencies.size(); depId++) {
            if (currentTaskStatus[depId] == 0 && find(currentAvailableItems.begin(), currentAvailableItems.end(), dependencies[depId - 1].input1) != currentAvailableItems.end()
                && find(currentAvailableItems.begin(), currentAvailableItems.end(), dependencies[depId - 1].input2) != currentAvailableItems.end()) {
                validAssignments.push_back({depId, {dependencies[depId - 1].output, dependencies[depId - 1].food}});
            }
        }

        if (validAssignments.empty()) continue;

        int n = validAssignments.size();
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
            if (visitedStates.insert(stateKey).second) {
                int currentCost = nextSchedulePath.size();
                int heuristicValue = calculateHeuristic(nextTaskStatus)/groupMember;
                openList.push({currentCost + heuristicValue, {nextTaskStatus, nextAvailableItems, nextSchedulePath, currentFinalOutcome}});
            }
        } while (prev_permutation(taskSelection.begin(), taskSelection.end()));
    }
}

//DFBB for menu
int mindays=10000;
void exploreScheduleByDFBB(vector<int> &taskStatus, vector<int> &availableAssignments, vector<vector<pair<int, string>>> &schedulePath,
                          vector<Dependency> &dependencies, int groupMember, vector<int> &goalStates) {

    nodeDFBBMenu++;
    int lb = schedulePath.size();
    if (hasAchievedFinalOutcome(availableAssignments, goalStates)) {
        if(lb<mindays){
            scheduleConditionCheck2(schedulePath);
            mindays=lb;
        }
        menuDFBB=true;
        return;
    }

    if(!pruning(schedulePath))
    return;

    if(lb==mindays)
        return;

    vector<pair<int, pair<int, string>>> validAssignments;
    for (int depId = 1; depId <= dependencies.size(); depId++) {
        if (taskStatus[depId] == 0 && find(availableAssignments.begin(), availableAssignments.end(), dependencies[depId - 1].input1) != availableAssignments.end()
            && find(availableAssignments.begin(), availableAssignments.end(), dependencies[depId - 1].input2) != availableAssignments.end()) {
            validAssignments.push_back({depId, {dependencies[depId - 1].output, dependencies[depId - 1].food}});
        }
    }

    if (validAssignments.empty()) return;

    int n = validAssignments.size();
    vector<bool> taskSelection(n, false);
    fill(taskSelection.begin(), taskSelection.begin() + groupMember, true);
    do {
        vector<int> nextTaskStatus = taskStatus, nextAvailableItems = availableAssignments;
        vector<vector<pair<int, string>>> nextSchedulePath = schedulePath;
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
        exploreScheduleByDFBB(nextTaskStatus, nextAvailableItems, nextSchedulePath, dependencies, groupMember, goalStates);

    } while (prev_permutation(taskSelection.begin(), taskSelection.end()));
}

int returnPerDayCost(const vector<vector<pair<int, string>>>& schedulePath) {
    int numDays = schedulePath.size();
    vector<MenuCount> count;
    map<string, int> foodCount;
    int totalCost = 0;

    for (const auto &day : schedulePath) {
        map<string, int> dayFoodCount;
        for (const auto &task : day) dayFoodCount[task.second]++;
        for (const auto &item : dayFoodCount) foodCount[item.first] = max(foodCount[item.first], item.second);
    }

    for (const auto &item : foodCount) {
        totalCost += getFoodCost(item.first, menuItems) * item.second;
    }

    return totalCost;
}


//AStar for min k days

void exploreScheduleByAStar2(vector<int> &taskStatus, vector<int> &availableAssignments, vector<vector<pair<int, string>>> &schedulePath, 
                            vector<Dependency> &dependencies, int groupMember, vector<int> &goalStates) {
    
    priority_queue<pair<int, tuple<vector<int>, vector<int>, vector<vector<pair<int, string>>>, vector<int>>>, 
                   vector<pair<int, tuple<vector<int>, vector<int>, vector<vector<pair<int, string>>>, vector<int>>>>, 
                   greater<pair<int, tuple<vector<int>, vector<int>, vector<vector<pair<int, string>>>, vector<int>>>>> openList;
    
    set<pair<vector<int>, vector<int>>> visitedStates;
    
    openList.push({0, {taskStatus, availableAssignments, schedulePath, goalStates}});
    nodeAStarK++;

    while (!openList.empty()) {
        auto front = openList.top();
        openList.pop();

        int waste = front.first;
        vector<int> currentTaskStatus = get<0>(front.second);
        vector<int> currentAvailableItems = get<1>(front.second);
        vector<vector<pair<int, string>>> currentSchedulePath = get<2>(front.second);
        vector<int> currentFinalOutcome = get<3>(front.second);

        if (hasAchievedFinalOutcome(currentAvailableItems, currentFinalOutcome)) {
            aStarK=currentSchedulePath;
            nodeAStarK++;
            kA=true;
            return;
        }
        nodeAStarK++;

        if(currentSchedulePath.size()>=k)
        continue;

        vector<pair<int, pair<int, string>>> validAssignments;
        for (int depId = 1; depId <= dependencies.size(); depId++) {
            if (currentTaskStatus[depId] == 0 && find(currentAvailableItems.begin(), currentAvailableItems.end(), dependencies[depId - 1].input1) != currentAvailableItems.end()
                && find(currentAvailableItems.begin(), currentAvailableItems.end(), dependencies[depId - 1].input2) != currentAvailableItems.end()) {
                validAssignments.push_back({depId, {dependencies[depId - 1].output, dependencies[depId - 1].food}});
            }
        }

        if (validAssignments.empty()) continue;

        int n = validAssignments.size();
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
            if (visitedStates.insert(stateKey).second) {
                int currentCost = returnPerDayCost(schedulePath);
                int heuristicValue = returnPerDayCost(nextSchedulePath)-returnPerDayCost(schedulePath);
                openList.push({currentCost + heuristicValue, {nextTaskStatus, nextAvailableItems, nextSchedulePath, currentFinalOutcome}});
            }
        } while (prev_permutation(taskSelection.begin(), taskSelection.end()));
    }
}

//DFBB for min k days
int mincost=10000;
void exploreScheduleByDFBB2(vector<int> &taskStatus, vector<int> &availableAssignments, vector<vector<pair<int, string>>> &schedulePath,
                          vector<Dependency> &dependencies, int groupMember, vector<int> &goalStates) {

    nodeDFBBk++;
    int currdays = schedulePath.size();
    if(currdays>k)
    return;
    int lb = returnPerDayCost(schedulePath);
    if (hasAchievedFinalOutcome(availableAssignments, goalStates)) {
        if(lb<mincost){
            scheduleConditionCheck3(schedulePath);
            mincost=lb;
        }
        kDFBB=true;
        return;
    }


    if(lb>=mincost)
        return;

    vector<pair<int, pair<int, string>>> validAssignments;
    for (int depId = 1; depId <= dependencies.size(); depId++) {
        if (taskStatus[depId] == 0 && find(availableAssignments.begin(), availableAssignments.end(), dependencies[depId - 1].input1) != availableAssignments.end()
            && find(availableAssignments.begin(), availableAssignments.end(), dependencies[depId - 1].input2) != availableAssignments.end()) {
            validAssignments.push_back({depId, {dependencies[depId - 1].output, dependencies[depId - 1].food}});
        }
    }

    if (validAssignments.empty()) return;

    int n = validAssignments.size();
    vector<bool> taskSelection(n, false);
    fill(taskSelection.begin(), taskSelection.begin() + groupMember, true);
    do {
        vector<int> nextTaskStatus = taskStatus, nextAvailableItems = availableAssignments;
        vector<vector<pair<int, string>>> nextSchedulePath = schedulePath;
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
        exploreScheduleByDFBB2(nextTaskStatus, nextAvailableItems, nextSchedulePath, dependencies, groupMember, goalStates);

    } while (prev_permutation(taskSelection.begin(), taskSelection.end()));
}

void checkGivenMenu() {
    int m = menuItems.size();
    int n = 0;
    for (auto x : menuItems) {
        for (auto y : inputMenu) {
            if (x.food == y.food) {
                n++;
                break;
            }
        }
    }
    if (n == m)
        menuValid = true;
}

int menuCostCal(){
    int sum=0;
    for(auto a : inputMenu){
        for(auto b : menuItems){
            if(a.food == b.food){
                sum = sum+ b.cost*a.count;
            }
        }
    }
    return sum;
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

    cout << "Enter User Input Menu" << endl;
    for (int j = 0; j < menuItems.size(); j++) {
        string food;
        int count;
        cin >> food >> count;
        inputMenu.push_back({food, count});
    }

    menuCost = menuCostCal();

    cout << "\nEnter 'k' days" << endl;
    cin >> k;
    cout<<"\n";

    checkGivenMenu();

    //for (int i = 1; i <= groupMember; i++) {
    vector<int> taskStatus(dependencies.size() + 1, 0);
    vector<vector<pair<int, string>>> schedulePath;
    exploreScheduleByDFS(taskStatus, initialItems, schedulePath, dependencies, groupMember, goalStates);

    vector<int> taskStatus2(dependencies.size() + 1, 0);
    vector<vector<pair<int, string>>> schedulePath2;
    exploreScheduleByAStar(taskStatus2, initialItems, schedulePath2, dependencies, groupMember, goalStates);

    vector<int> taskStatus3(dependencies.size() + 1, 0);
    vector<vector<pair<int, string>>> schedulePath3;
    exploreScheduleByDFBB(taskStatus3, initialItems, schedulePath3, dependencies, groupMember, goalStates);

    vector<int> taskStatus4(dependencies.size() + 1, 0);
    vector<vector<pair<int, string>>> schedulePath4;
    exploreScheduleByAStar2(taskStatus4, initialItems, schedulePath4, dependencies, groupMember, goalStates);

    vector<int> taskStatus5(dependencies.size() + 1, 0);
    vector<vector<pair<int, string>>> schedulePath5;
    exploreScheduleByDFBB2(taskStatus5, initialItems, schedulePath5, dependencies, groupMember, goalStates);
    //}

    cout << "DFS\nFor Given Menu" << endl;

    if (t3 == 10000) {
        cout << "infeasible" << endl;
    } else
        printSchedule2(minmenupath);

    cout <<endl;
    cout << endl;

    cout << "For k days" << endl;
    if (isLess) {
        printSchedule(minposcostpath, menuItems);
    } else
        cout << "cannot be done under " << k << " days\n";
    cout << "\n***************************" << endl;
    cout << endl;
    cout<<"AStar for GIVEN MENU"<<endl;
    if(menuA)
    printSchedule2(aStarMenu);
    else
    cout<<"infeasible"<<endl;
    cout<<endl;
    cout<<"DFBB for GIVEN MENU"<<endl;
    if(menuDFBB)
    printSchedule2(dfbbMenu);
    else
    cout<<"infeasible"<<endl;
    cout<<endl;
    cout << "\n***************************" << endl;
    cout<<"AStar k days"<<endl;
    if(kA)
    printSchedule(aStarK,menuItems);
    else
    cout<<"Not possible within k days"<<endl;
    cout<<endl;
    cout<<"DFBB k days"<<endl;
    if(kDFBB)
    printSchedule(dfbbK,menuItems);
    else
    cout<<"Not possible within k days"<<endl;
    cout<<endl;
    cout<<"Nodes explored ==>"<<endl;
    cout<<"DFS : "<<nodeDFS<<"\nAStar Menu : "<<nodeAStarMenu<<"\nAStar K days : "<<nodeAStarK<<"\nDFBB Menu : "<<nodeDFBBMenu<<"\nDFBB K days : "<<nodeDFBBk<<endl;
    return 0;
}
