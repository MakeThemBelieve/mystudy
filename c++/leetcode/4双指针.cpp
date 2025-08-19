#include <iostream>
#include <vector>

using namespace std;


class Solution {
public:
    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {

        int totalSize = nums1.size() + nums2.size();
        if (totalSize == 0) return 0.0; // 处理空数组情况
        if (totalSize == 1 && nums1.size() == 0) return (double )nums2[0];
        if (totalSize == 1 && nums2.size() == 0) return (double )nums1[0];
        int num1 = 0,num2 = 0;
        double nowNum = 0.0;
        double nextNum = 0.0;
        // 两根指针比较大小，选小的
        for(int i = 0 ; i < (totalSize + 1)/2; i++){

            //双指针要考虑到边界情况，不然会产生越界
            if (num1 >= nums1.size()) {
                nowNum = nums2[num2++];
            } else if (num2 >= nums2.size()) {
                nowNum = nums1[num1++];
            } else if (nums1[num1] <= nums2[num2]) {
                nowNum = nums1[num1++];
            } else {
                nowNum = nums2[num2++];
            }

        }

        if (num1 >= nums1.size()) {
            nextNum = nums2[num2];
        } else if (num2 >= nums2.size()) {
            nextNum = nums1[num1];
        } else if (nums1[num1] <= nums2[num2]) {
            nextNum = nums1[num1];
        } else {
            nextNum = nums2[num2];
        }

        // 到中位，在判断一次奇偶返回结果
        if(totalSize%2){
            //奇数
            return nowNum;
        }else{
            return (nowNum + nextNum)/2.0;
        }

    }
};