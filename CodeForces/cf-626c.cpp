#include <iostream> 
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <bitset>
#include <numeric>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <cassert>

using namespace std;
using ll = long long;
using ld = long double;
using ii = pair<int,int>;
using vi = vector<int>;
using vb = vector<bool>;
using vvi = vector<vi>;
using vii = vector<ii>;
using vvii = vector<vii>;

const int INF = 2000000000;
const ll LLINF = 9000000000000000000;

int main() {
	
	ll N, M;
	cin >> N >> M;
	
	ll last2 = 2LL * N;
	ll last3 = 3LL * (M + N / 3LL);
	ll ans = LLINF;
	for (ll m = 0; m <= min(N / 3LL, M / 2LL); ++m) {
		ans = min(ans, max(last2, last3));
		last2 += 2LL;
		last3 -= 3LL;
		if (last3 >= last2 && last2 % 6 == 0)
			last2 += 2LL;
	}
	
	cout << ans << endl;
	
	return 0;
}
