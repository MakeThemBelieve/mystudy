#include <vector>

using namespace std;


class Solution {
public:
    int maxProfit(vector<int>& prices) {

        if (prices.size() == 1 || prices.size() == 0) return 0;

        int price,nextPrice;
        int i = 1;
        int profit = 0;

        do {
        price = prices[i - 1];
        nextPrice = prices[i];
        
        if (nextPrice - price > 0) profit += nextPrice - price;
        i++;
        } while (i < prices.size());
        
        return profit;
    }
};