#include<iostream>
#include<fstream>
#include<sstream>
#include<bits/stdc++.h>

using namespace std;

//struct for different inputs
struct dependencyList{
    int id;
    int input1;
    int input2;
    int outcome;
    string food;
};

struct menuItem{
    string food;
    int cost;
};


struct menuCount{
    string food;
    int cnt;
};

struct data{
    int totaldays;
    int totalcost;
};

//storing all the schedules with menus
vector<pair<vector<vector<pair<int, string>>>,vector<menuCount>>> storageOfSchedule;
vector<pair<vector<vector<pair<int, string>>>,pair<int,int>>> storageOfOtherData;

int totalschedule=0;

int minDays=1000,maxDays=0;
int minCost = 100000,maxCost=0;
int auxvar=0;

//returning cost of the food items
int returnCost(string a,vector<menuItem>& foodCost){
    int n = foodCost.size();
    for(int i=0;i<n;i++){
        if(a==foodCost[i].food)
        return foodCost[i].cost;
    }
    return 0;
}

//storing the schedules and printing it
void storeandprintpath(const vector<vector<pair<int, string>>>& path,vector<menuItem>& foodCost ) {
    totalschedule++;

    int n=path.size();
    map<string,int> temp;
    vector<menuCount> count;
    //cout<<"Schedule no. : "<<totalschedule<<endl;

    for(int i=0;i<n;i++){
        map<string,int> temp2;
        int k=path[i].size();
        for(int j =0;j<k;j++){
                temp2[path[i][j].second]++;
        }  
        for(auto i: temp2){
            string s = i.first;
            int num = i.second;
            temp[s]= max(temp[s], num);
        }
    }

    for(int i = 0;i<n;i++) {
        //cout<<"Day "<<i+ 1<<" : "<<endl;
        for (auto j : path[i]) {
            //cout<<"Assignment: "<<j.first<< "\t";
        }
        //cout<<endl;
    }
    int sum=0;
    //cout<<"Daily Menu : ==>"<<endl;
    //cout<<"< ";
    for (auto i:temp) {
        //cout << i.first << " - "<<i.second<<" "; 
        sum = sum + (returnCost(i.first,foodCost))*i.second;

    }
    
    //cout<<" > "<<endl;
    sum = sum*n;
    //cout<<"Total time:\t"<<n<<" days"<<endl;
    //cout<<"Total cost:\t"<<sum<<endl;

    if(minDays>n)
    minDays=n;

    if(maxDays<n)
    maxDays=n;

    if(minCost>sum)
    minCost=sum;

    if(maxCost<sum)
    maxCost=sum;

    vector<vector<pair<int, string>>> copyPath = path;

    for(auto i : temp){
        count.push_back({i.first,i.second});
    }

    storageOfSchedule.push_back({copyPath,count});
    storageOfOtherData.push_back({copyPath,{n,sum}});
    //cout << endl;
}

//checking if all final outcome have been done
bool checkOutcome(vector<int>& availableItems,vector<int>& finalOutcome){
    int q=finalOutcome.size();
    int p=0;
    for(auto goal : finalOutcome) {
        for(auto item : availableItems) {
            if (item == goal) {
                p++;
                break;
            }
        }
    }
    if(p==q){
        return true;
    }
    else
        return false;

}

//to check if vector is empty
bool isempty(vector<pair<int,pair<int,string>>>& ind_assg){
    int n = ind_assg.size();
    if(n==0)
        return true;
    else
        return false;
}

//finding all the schedules
void findallschedules(vector<int>& assgdone, vector<int>& availableItems, vector<vector<pair<int, string>>>& assgdoneprev,vector<dependencyList>& assignments,vector<menuItem>& foodCost,int groupSize,vector<int>& finalOutcome) {
    
    if(checkOutcome(availableItems,finalOutcome)){
        storeandprintpath(assgdoneprev,foodCost);
        return;
    }
    
    int n = assignments.size();

    vector<pair<int,pair<int,string>>> ind_assg;

    for(int assgid =1;assgid<=n;assgid++) {
        if (assgdone[assgid]== 0) {  
            bool c = true;
            int requiredItem1 = assignments[assgid-1].input1;
            int requiredItem2 = assignments[assgid-1].input2; 
            if(find(availableItems.begin(),availableItems.end(),requiredItem1) ==availableItems.end()) {
                c = false;
            }
            if(find(availableItems.begin(),availableItems.end(),requiredItem2)==availableItems.end()) {
                c = false;
            }
            if(c){
                ind_assg.push_back({assgid, {assignments[assgid-1].outcome, assignments[assgid-1].food}});
            }
        }
    }

    if(isempty(ind_assg)){
        return;
    }

    int k = ind_assg.size();
    int m = groupSize;
    vector<bool> selectionss(k, false);
    fill(selectionss.begin(), selectionss.begin() +m,true);

    do{
        vector<int> completeassgcpy = assgdone;
        vector<int> availcpy = availableItems;
        vector<vector<pair<int,string>>> traversedassgcpy = assgdoneprev;
        vector<pair<int,string>> currentassg;
        vector<pair<int,pair<int,string>>> choosenassg;

        for(int i = 0; i <k;i++) {
            if(selectionss[i]==true) {
                choosenassg.push_back(ind_assg[i]);
            }
        }

        for(auto y : choosenassg) {
            int out = assignments[y.first-1].outcome;
            currentassg.push_back({y.first, assignments[y.first-1].food});
            availcpy.push_back(out);
            completeassgcpy[y.first] = 1;
            auxvar++;
        }
        traversedassgcpy.push_back(currentassg);
        findallschedules(completeassgcpy,availcpy,traversedassgcpy,assignments,foodCost,groupSize,finalOutcome);  
    } while(prev_permutation(selectionss.begin(),selectionss.end())); 
}

int main(){
    ifstream f("../testcases/input01.dag"); 
    if(!f.is_open()) {
        cout<<"error in opening the file"<<endl;
        return 1;
    }

    vector<dependencyList> assignment;
    vector<menuItem> foodCost;
    int groupSize;
    vector<int> books,finalOutcome;
    string l;
    
    while(getline(f, l)) {
        if (l.empty()||l[0]=='%') {
            continue;
        }
        stringstream s(l);
        string st;
        s>>st;
        if(st=="C") {
            string name;
            int val;
            s>>name>>val;
            foodCost.push_back({name, val});
        }
        else if (st=="G") {
            s>>groupSize;
        }
        else if (st=="I") {
            int temp1;
            while (s>>temp1) {
                books.push_back(temp1);
            }
        } 
        else if (st=="O") {
            int temp2;
            while(s>>temp2){
                finalOutcome.push_back(temp2);
            }
        }
        else if(st=="A"){
            dependencyList assg;
            s>>assg.id>>assg.input1>>assg.input2>>assg.outcome>>assg.food;
            assignment.push_back(assg);
        }
    };
    f.close(); 

    sort(assignment.begin(),assignment.end(),[](const dependencyList& a, const dependencyList& b) {
        return a.id<b.id; 
    });

    int n=assignment.size();

    //for(int i=1;i<groupSize;i++){
        vector<int> assgCompleted(n+1,0);
        vector<vector<pair<int,string>>> path;
        findallschedules(assgCompleted,books,path,assignment,foodCost,groupSize,finalOutcome);
    //}

    cout<<"Number of reasonable schedules discovered :\t"<<totalschedule<<endl;
    cout<<"Minimum days to complete all assignment :\t"<<minDays<<endl;
    cout<<"Maximum days to complete all assignment :\t"<<maxDays<<endl;
    cout<<"Minimum total cost to complete all assignment :\t"<<minCost<<endl;
    cout<<"Maximum total cost to complete all assignment :\t"<<maxCost<<endl;
}