#include <vector>

using namespace std;

class Solution {
public:
    void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
        
        if (n == 0 || nums2.size() == 0) return ;
        if (m == 0) {
            nums1 = nums2;
            return ;
        }
        int n1 = 0,n2 = 0;
        vector<int> mergeNums;

        while (n1 < m && n2 < n) {
            if (nums1[n1] <= nums2[n2]) {
                mergeNums.push_back(nums1[n1++]);
            } else {
                mergeNums.push_back(nums2[n2++]);
            }
        }
        while (n1 < m) mergeNums.push_back(nums1[n1++]);
        while (n2 < n) mergeNums.push_back(nums2[n2++]);

        nums1 = mergeNums;
        return;
    }
};