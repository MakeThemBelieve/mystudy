#include <vector> 

using namespace std;


class Solution {
public:
    int canCompleteCircuit(vector<int>& gas, vector<int>& cost) {
        
        if (gas.size() == 1 && (gas[0] == 0 || gas[0] < cost[0])) return -1;

        int total = 0;
        int max = 0;
        int begin = 0;

        for (int i = 0 ; i < gas.size() ; i++) {
            int count = gas[i] - cost[(i - 1 + gas.size()) % gas.size()];
            total += count;
            if (total + gas[begin % gas.size()] < 0 && max != 0) max = 0;
            if (max < count) {
                max = count;
                begin = (i - 1 + gas.size()) % gas.size();
            }
        }
        if(gas[begin] < cost[begin]) begin = (begin + 1) % gas.size();
        if(total >= 0) return begin;
        else return -1;
    }
};

int main(){
    vector<int> gas = {5,1,2,3,4};
    vector<int> cost = {4,4,1,5,1};
    Solution a;
    a.canCompleteCircuit(gas,cost);

}



