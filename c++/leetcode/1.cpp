#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

class Solution{
    public:
    vector<int>  twoSum(vector<int>& nums, int target) {

        if (nums.size() < 2) return {};

        unordered_map<int , int> median;
        for (int i = 0; i < nums.size(); i++){
            //当前key有没有匹配值
            if (median[target - nums[i]]) {
            //当找到了匹配值返回，没找到把当然的 tar-key写入写入value
                return {median[target - nums[i]] - 1 , i};
            }
            median[nums[i]] = i + 1;
        }
        return {};
    }
};


int main(){
    vector<int> nums;
    int target;
    cin >> target;
    cout << "next:" << endl;
    int n;
    cin >> n;
    cout << "next:" << endl;
    nums.resize(n);
    for ( int i = 0 ; i < n; i++) {
        cin >> nums[i];
    }
    Solution test;
    test.twoSum(nums , target);



}



