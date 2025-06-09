#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <string>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <cstring>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <ctime>
#include <bitset>
#include <complex>
#include <random>
#include <functional>

#ifdef DEBUG
#include <edulcni.hpp>
#endif

using namespace std;

typedef long long ll;
typedef unsigned long long ull;

const int E2 = 100, E3 = 1000, E4 = 10000, E5 = 100000, E6 = 1000000, E9 = 1000000000;
const ll E18 = 1ll * E9 * E9;
const int FFT_MOD = 998244353;
const int SIMPLE_MOD = E9 + 7;
mt19937 rng(time(0));

#define all(v) v.begin(), v.end()
#define forn(x, n) for (int x = 0; x < n; x++)
#define forab(x, a, b) for (int x = a; x < b; x++)
#define sz(v) int(v.size())
#define pii pair<int, int>
#define F first
#define S second
#define vi vector<int>
#define T(x, i) get<i>(x)

#ifdef DEBUG
	#define range(begin, end)                                       \
		do {                                                              \
			cerr << "[" << #begin << "; " << #end << "): ";       		  \
			for (auto it = begin; it != end; ++it)                        \
				cerr << *it << ' ';                                       \
			cerr << '\n';                                         		  \
		} while (0)
	#define var(var) cerr << #var << ": " << var << '\n'
	#define table(t, x0, x1, y0, y1)                                                              \
		do {																							\
			cerr << "{" << #t << "[" << x0 << " : " << x1 << "][" << y0 << " : " << y1 << "]\n"; 		\
			for (int i = x0; i < x1; i++) {																\
				for (int j = y0; j < y1; j++)															\
					cerr << t[i][j] << ' ';																\
				cerr << '\n';																			\
			}													 										\
		} while(0)
    #define EDULCNI_CALL(x) edulcni::x

#else
	#define cerr if (false) cerr
	#define var(var)
	#define range(begin, end)
	#define table(t, x0, x1, y0, y1)
    #define EDULCNI_CALL(x)
#endif


const int MAXN = 10;
const int INF = E9;
const int MOD = SIMPLE_MOD;

int n, x;
int a[MAXN];



inline void init() {
    EDULCNI_CALL(array_widget("a", a, a));
    EDULCNI_CALL(label_widget("L", ""));
    EDULCNI_CALL(label_widget("R", ""));
    EDULCNI_CALL(label_widget("Len", ""));
    EDULCNI_CALL(label_widget("X", ""));
    EDULCNI_CALL(step());
}

inline void solve() {
	init();
    forn(i, n) cin >> a[i];
    EDULCNI_CALL(array_widget("a", a, a + n));
    EDULCNI_CALL(step());
    int l = -1;
    forn(i, n) if (a[i] == 1) {
        l = i;
        EDULCNI_CALL(array_highlight("a", l));
        break;
    }
    EDULCNI_CALL(step());
    int r = -1;
    forn(i, n) if (a[n-1-i] == 1) {
        r = n-i-1;
        EDULCNI_CALL(array_highlight("a", r));
        break;
    }
    EDULCNI_CALL(step());
    EDULCNI_CALL(label_widget("L", l));
    EDULCNI_CALL(label_widget("R", r));
    EDULCNI_CALL(label_widget("Len", r-l+1));
    EDULCNI_CALL(label_widget("X", x));
    EDULCNI_CALL(step());
    EDULCNI_CALL(array_clear_highlights("a"));
    EDULCNI_CALL(step());

    if (r - l + 1 <= x) {
        cout << "YES\n";
    } else {
        cout << "NO\n";
    }
}

signed main() {
	#ifdef DEBUG
		freopen("input.in", "r", stdin);
		freopen("output.out", "w", stdout);
	#else
	
	#endif
	ios_base::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);
    EDULCNI_CALL(initialize("2117-A"));
	int _t; cin >> _t;
	while (cin >> n >> x)
		solve();
	return 0;
}