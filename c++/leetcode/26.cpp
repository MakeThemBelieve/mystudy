#include <vector>

using namespace std;


class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        
        if(nums.size() == 0 || nums.size() == 1) return nums.size();

        int num = 0,nextNum = 1;
        int total = 1;
        vector<int> newNums;

        newNums.push_back(nums[0]);
        while(nextNum < nums.size() ) {
            if(nums[nextNum] != nums[num]) {
                num = nextNum;
                newNums.push_back(nums[nextNum++]);
                total++;
            } else {
                nextNum++;
            }
        }

        nums = newNums;
        return total;
    }
};