#include <vector>

using namespace std;



class Solution {
public:
    bool canJump(vector<int>& nums) {
        
        if(nums.size() == 0 || nums.size() == 1) return 1;
        if(nums.size() > 1 && nums[0] == 0) return 0;

        int maxjump = nums[0];

        for (int i = 1 ; i < nums.size() ; i++) {
            maxjump--;
            maxjump > nums[i] ? : maxjump = nums[i];
            if (maxjump == 0 && i != nums.size() - 1) return 0;
        }

        return 1;
    }
};
