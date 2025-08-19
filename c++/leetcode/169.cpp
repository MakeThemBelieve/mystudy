#include <vector>
#include <unordered_map>

using namespace std;

class Solution {
public:
    int majorityElement(vector<int>& nums) {
        
    if(nums.size() == 1) return nums[0];

    int max = 0;
    int maxnum = nums[0];

    unordered_map<int,int> time;

    for (int i = 0; i < nums.size(); i++) {
        time[nums[i]]++;
        if(time[nums[i]] > max){
            maxnum = nums[i];
            max = time[nums[i]];
        }
    }

    return maxnum;
    }
};