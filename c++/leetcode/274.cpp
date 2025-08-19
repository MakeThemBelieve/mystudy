#include <vector>
#include <algorithm>

using namespace std;


class Solution {
public:
    int hIndex(vector<int>& citations) {
        
        if (citations.size() == 0) return 0;

        sort(citations.begin(), citations.end());
        int maxh = 0;

        for (int i = citations.size() - 1; i >= 0; --i) {
            int count = citations.size() - i;
            if (citations[i] >= count) {
                maxh = count; // 更新h指数
            } else {
                break; // 遇到不满足条件的论文，后续不可能更大
            }
        }
        return maxh;
    }
};



