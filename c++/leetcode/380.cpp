#include <vector>

using namespace std;


class Solution {
public:
    vector<int> productExceptSelf(vector<int>& nums) {

        vector<int> result = {1};
        int multiply = result[0];

        for (int i = 1 ; i < nums.size() ; i++) {
            multiply *= nums[i - 1];
            result.push_back(multiply);
        }
        multiply = 1;
        for (int i = nums.size() - 2 ; i >= 0 ; i--) {
            multiply *= nums[i + 1];
            result[i] = result[i] * multiply;
        }

        return result;
    }
};