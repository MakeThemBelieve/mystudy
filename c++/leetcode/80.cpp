#include <vector>

using namespace std;


class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        
        if(nums.size() == 0 || nums.size() == 1) return nums.size();

        int num = 0,nextNum = 1,now = 1;

        while(nextNum < nums.size() ) {
            if(nums[nextNum] != nums[num] || (nums[nextNum] == nums[num] && nextNum - num < 2)) {
                if(nums[nextNum] != nums[num]) num = nextNum;
                nums[now++] = nums[nextNum++];
            } else {
                nextNum++;
            }
        }
        return now;
    }
};