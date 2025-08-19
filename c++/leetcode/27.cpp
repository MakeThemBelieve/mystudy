#include <vector>

using namespace std;

class Solution {
public:
    int removeElement(vector<int>& nums, int val) {
        
        if (nums.size() == 0) return 0;

        int total = 0;
        vector<int> newNums;

        for(int i = 0; i < nums.size(); i++) {
            if(nums[i] != val) {
                newNums.push_back(nums[i]);
                total++;
            }
        }

        nums = newNums;
        return total;
    }
};