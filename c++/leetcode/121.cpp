#include <vector> 

using namespace std;


class Solution {
public:
    int maxProfit(vector<int>& prices) {

        if (prices.size() == 0 || prices.size() == 1) return 0;

       int profit = 0, cost = prices[0];

       for (int i = 1; i < prices.size() ; i++) {
            cost = min(prices[i] , cost);
            profit = max(profit , prices[i] - cost);
       }
       
       return profit;
    }
};