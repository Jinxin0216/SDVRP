/*
*   Copyright (C) 2014 All rights reserved.
*   
*   filename: vrp.cpp
*   author: doublehh
*   e-mail: sserdoublehh@foxmail.com
*   create time: 2014-09-01
*   last modified: 2014-09-01 10:02:58
*/
#include <bits/stdc++.h>
using namespace std;

#define FOR(i, n) for (int i = 0; i < (n); i++)
#define foreach(it, S) for (__typeof(S.begin()) it = S.begin(); it != S.end(); it++)

const int MAXC = 1000;
const int MAXR = 1000;
const int MAXT = 200;
const double INF = 1e10;
const double MIN_CHANGE = 1e-5;
const int MAX_BAD_TIMES = 500;
int MAX_TABU = -1;

enum {
	RELOCATION,
	EXCHANCE,
	SPLIT
};

struct ServerInfo
{
	int rid, cid, q;
	ServerInfo(int rid, int cid):
		rid(rid), cid(cid), q(0) {}
};

struct RouteNode
{
	list<ServerInfo> L;
	int r, mr;
};

struct CustomerNode
{
	map<int, int> M;
	int r, ar;
};

struct Solution
{
	CustomerNode customer[MAXC];
	RouteNode route[MAXR];
	double ttc;
	int dfs_clock;
	bool fesiable;
	bool vis[MAXC];

	void dfsr(int u, int fa);
	void dfsc(int u, int fa);
	void init();
	bool isFeasible();
	double cal();
	void print();
	void judge();
};

int cap;
int nc;
int nr;
int nt;
int TL1[MAXC][MAXR];
list<ServerInfo>::iterator iter;
int demand[MAXC];
double c[MAXC][MAXC];
Solution Scur, Sbest;
pair<int, int> TL[10];
int strategy;

inline double random1to0();
inline bool IsAccept(Solution &sol, double now, double best);

bool ChepestInsert(Solution &sol, int cid, int rid, double &change, bool work=false, bool tabu=false, bool force=false);
bool Erase(Solution &sol, int cid, int rid, double &change, bool work=false, bool tabu=false, bool force=false);

bool Relocation(Solution &sol, int &cid1, int &rid1, int &rid2, double &best, bool work=false, bool tabu=false);
bool Exchange(Solution &sol, int &cid1, int &rid1, int &cid2, int &rid2, double &best, bool work=false, bool tabu=false);
bool Split(Solution &sol, int &cid1, vector<int> &vec, double &best, bool work=false, bool tabu=false);
Solution Neightbor(Solution &sol);

bool Judge();
void InitSolution();
bool Init();
void Solve(int init_strategy);
void InitTabuTable();
void UpdateTabuTable();
void Record();

inline bool IsAccept(Solution &sol, double now, double best)
{
	return now < best
		&& fabs(now) > MIN_CHANGE
		&& sol.isFeasible();
}
inline double random1to0()
{ return double(rand()) / RAND_MAX; }

bool ChepestInsert(Solution &sol, int cid, int rid, double &change, bool work, bool tabu, bool force)
{
	if (!force && TL1[cid][rid])
		return false;

	if (sol.customer[cid].M.count(rid))
		return false;

	list<ServerInfo>::iterator best;

	int t1, t2;

	change = INF;
	double tmp;
	int pre = nc, cur;
	foreach (it, sol.route[rid].L)
	{
		cur = it->cid;
		tmp = c[pre][cid] + c[cid][cur] - c[pre][cur];

		if (tmp < change)
		{
			t1 = pre, t2 = cur;
			change = tmp;
			best = it;
		}
		pre = cur;
	}
	cur = nc;
	tmp = c[pre][cid] + c[cid][cur] - c[pre][cur];
	if (tmp < change)
	{
		t1 = pre, t2 = cur;
		change = tmp;
		best = sol.route[rid].L.end();
	}

	if (change != INF)
	{
		if (work)
		{
			sol.route[rid].L.insert(best, ServerInfo(rid, cid));
			sol.customer[cid].M[rid] = -1;
		}
		iter = best;
		return true;
	}
	return false;
}

bool Erase(Solution &sol, int cid, int rid, double &change, bool work, bool tabu, bool force)
{
	int pre = nc, cur, nex;
	foreach (it, sol.route[rid].L)
	{
		cur = it->cid;
		if (cid == it->cid)
		{
			it++;
			if (it == sol.route[rid].L.end())
				nex = nc;
			else 
				nex = it->cid;
			it--;

			change = c[pre][nex] - c[pre][cur] - c[cur][nex];

			if (!sol.customer[cid].M.erase(rid))
				return false;

			iter = it;

			if (work)
			{
				sol.route[rid].L.erase(it);
				if (tabu)
					TL[0] = make_pair(cid, rid);
			}

			return true;
		}
		pre = cur;
	}
	return false;
}

bool Relocation(Solution &sol, int &cid1, int &rid1, int &rid2, double &best, bool work, bool tabu)
{
	double tmp, tp;

	if (work)
	{
		assert(Erase(sol, cid1, rid1, tmp, true, tabu));
		assert(ChepestInsert(sol, cid1, rid2, tmp, true, tabu));
		return true;
	}

	best = INF;
	cid1 = rid1 = rid2 = -1;

	FOR (c1, nc) foreach (it1, sol.customer[c1].M)
	{
		int r1 = it1->first;
		Solution S1 = sol;

		double change = 0;
		if (!Erase(S1, c1, r1, tmp, true))
			continue;
		tp = tmp;

		FOR (r2, nr)
		{
			if (r1 == r2) continue;
			if (!ChepestInsert(S1, c1, r2, tmp, true))
				continue;
			if (IsAccept(S1, tp+tmp, best))
			{
				best = tp + tmp;
				cid1 = c1, rid1 = r1, rid2 =r2;
			}
			assert(Erase(S1, c1, r2, tmp, true, false, true));
		}

	}

	return rid1 != -1;
}

bool Exchange(Solution &sol, int &cid1, int &rid1, int &cid2, int &rid2, double &best, bool work, bool tabu)
{
	double tmp, tp;

	if (work)
	{
		assert(Erase(sol, cid1, rid1, tmp, true, tabu));
		assert(Erase(sol, cid2, rid2, tmp, true, tabu));
		assert(ChepestInsert(sol, cid1, rid2, tmp, true));
		assert(ChepestInsert(sol, cid2, rid1, tmp, true));
		return true;
	}
	cid1 = rid1 = cid2 = rid2 = -1;

	best = INF;

	FOR (c1, nc) foreach (it1, sol.customer[c1].M)
	{
		for (int c2 = c1+1; c2 < nc; c2++) foreach (it2, sol.customer[c2].M)
		{
			int r1 = it1->first, r2 = it2->first;

			if (r1 == r2) continue;

			Solution S2 = sol;

			tp = 0;
			if (!Erase(S2, c1, r1, tmp, true))
				continue;
			tp += tmp;
			if (!Erase(S2, c2, r2, tmp, true))
				continue;
			tp += tmp;

			if (!ChepestInsert(S2, c1, r2, tmp, true))
				continue;
			tp += tmp;
			if (!ChepestInsert(S2, c2, r1, tmp, true))
				continue;
			tp += tmp;

			if (IsAccept(S2, tp, best))
			{
				best = tp;
				cid1 = c1, rid1 = r1;
				cid2 = c2, rid2 = r2;
			}
		}
	}

	return cid1 != -1;
}

bool Split(Solution &sol, int &cid, vector<int> &vec, double &best, bool work, bool tabu)
{
	static pair<double, int> p[MAXR];
	static int m, stamp = 0;
	double tmp, tp, change;

	if (work)
	{
		map<int, int> tmpMap = sol.customer[cid].M;

		best = 0;
		foreach (it, tmpMap)
		{
			assert(Erase(sol, cid, it->first, tmp, true, true, true));
			best += tmp;
		}

		FOR (i, vec.size())
		{
			assert(ChepestInsert(sol, cid, vec[i], tmp, true, true, false));
			best += tmp;
		}

		assert(sol.isFeasible());

		return true;
	}

	best = INF;
	vec.clear();
	cid = -1;

	FOR (c1, nc)
	{
		Solution S3 = sol;

		change = 0;
		foreach (it, sol.customer[c1].M)
		{
			assert(Erase(S3, c1, it->first, tmp, true, false, true));
			change += tmp;
		}

		m = 0;
		FOR (i, nr)
		{
			if (ChepestInsert(S3, c1, i, tmp))
			{
				FOR (j, nc)
					S3.vis[j] = false;
				S3.dfsr(i, -1);
				p[m++] = make_pair(tmp / S3.route[i].mr, i);
			}
		}
		sort(p, p+m);

		m = min(m, 10);

		FOR (mask, 1<<m)
		{
			Solution tS3 = S3;
			bool ok = true;
			tp = 0;
			FOR (i, m) if (mask & (1<<i))
			{
				if (!ChepestInsert(tS3, c1, p[i].second, tmp, true))
				{
					ok = false;
					break;
				}
				tp += tmp;
			}

			if (ok && IsAccept(tS3, tp + change, best))
			{
				vec.clear();
				FOR (i, m) if (mask & (1<<i))
					vec.push_back(p[i].second);
				best = tp + change;
				cid = c1;
			}
		}
	}

	return !vec.empty();
}

Solution Neightbor(Solution &sol)
{
	Solution ret;
	double tmp, best = INF;
	int ty = -1;
	int rid1, cid1, rid2, cid2;
	int r1, c1, r2, c2;
	int bestState;

	if (Relocation(sol, c1, r1, r2, tmp))
	{
		if (tmp < best)
		{
			best = tmp;
			ty = RELOCATION;
			cid1 = c1;
			rid1 = r1;
			rid2 = r2;
		}
	}

	if (Exchange(sol, c1, r1, c2, r2, tmp))
	{
		if (tmp < best)
		{
			best = tmp;
			ty = EXCHANCE;
			cid1 = c1, rid1 = r1;
			cid2 = c2, rid2 = r2;
		}
	}

	vector<int> vec;
	if (best > 0 && Split(sol, c1, vec, tmp))
	{
		if (tmp < best)
		{
			best = tmp;
			ty = SPLIT;
			cid1 = c1;
		}
	}

	switch (ty)
	{
		case RELOCATION:
			ret = sol;
			assert(Relocation(ret, cid1, rid1, rid2, tmp, true, true));
			assert(ret.isFeasible());
			return ret;
		case EXCHANCE:
			ret = sol;
			assert(Exchange(ret, cid1, rid1, cid2, rid2, tmp, true, true));
			assert(ret.isFeasible());
			return ret;
		case SPLIT:
			ret = sol;
			assert(Split(ret, cid1, vec, tmp, true, true));
			assert(ret.isFeasible());
			return ret;
	}

	assert(0);
}

void Solution::init()
{
	FOR (i, MAXC)
		customer[i].M.clear();
	FOR (i, MAXR)
		route[i].L.clear();
}

void Solution::dfsr(int u, int fa)
{
	route[u].r = cap;

	foreach (it, route[u].L)
	{
		int v = it->cid;
		if (v == fa)
		{
			it->q = -1;
			continue;
		}
		dfsc(v, u);
		route[u].r -= customer[v].r;
		it->q = customer[v].r;
	}

	if (route[u].r < 0)
		fesiable = false;
	if (fa < 0)
		route[u].mr = route[u].r;
}

void Solution::dfsc(int u, int fa)
{
	customer[u].r = demand[u];

	if (vis[u])
	{
		fesiable = false;
		return;
	}

	vis[u] = true;

	foreach (it, customer[u].M)
	{
		int v = it->first;
		if (v == fa)
			continue;
		dfsr(v, u);
		int tmp = min(customer[u].r, route[v].r);
		customer[u].r -= tmp;
		it->second = tmp;
	}

	if (fa < 0 && customer[u].r > 0)
		fesiable = false;
}

bool Solution::isFeasible()
{
	nt = 0;
	fesiable = true;
	FOR (i, nc)
		vis[i] = false;

	FOR (i, nc) if (!vis[i])
	{
		nt++;
		dfsc(i, -1);
	}

	return fesiable;
}

double Solution::cal()
{
	ttc = 0;
	FOR (i, nr)
	{
		int pre = nc, cur;
		foreach (it, route[i].L)
		{
			cur = it->cid;
			ttc += c[pre][cur];
			pre = cur;
		}
		cur = nc;
		ttc += c[pre][cur];
	}
	return ttc;
}

void Solution::print()
{
	FOR (i, nr)
	{
		int cur = nc;
		printf("%d", cur);
		foreach (it, route[i].L)
		{
			cur = it->cid;
			printf("->%d", cur);
		}
		cur = nc;
		printf("->%d\n", cur);
	}
}

bool Judge(bool ok[])
{
	for (int i = 0; i < nc; i++)
		if (!ok[i] && demand[i] <= cap)
			return true;

	return false;
}

bool Init()
{
	srand(time(NULL));
	static int x[MAXC], y[MAXC];
	if (scanf("%d%d", &nc, &cap) == EOF)
		return false;
	FOR (i, nc)
	{
		if (scanf("%d", &demand[i]) == EOF)
			return false;
	}
	if (scanf("%d%d", &x[nc], &y[nc]) == EOF)
		return false;
	FOR (i, nc)
	{
		if (scanf("%d%d", &x[i], &y[i]) == EOF)
			return false;
	}
	FOR (i, nc+1) FOR (j, nc+1)
	{
		if (i == j && i != nc)
			c[i][j] = INF;
		else
			c[i][j] = sqrt((x[i]-x[j])*(x[i]-x[j]) + (y[i]-y[j])*(y[i]-y[j]));
	}
	return true;
}

void InitSolution()
{
	InitTabuTable();
	static bool ok[MAXC];
	int tot = nc;
	FOR (i, nc)
		ok[i] = false;

	Scur.init();

	for (nr = 0; Judge(ok); nr++)
	{
		int tc = cap;
		double tmp;
		for (;;)
		{
			double best = INF;
			int cid = -1;
			FOR (i, nc)
			{
				if (ok[i]) continue;
				if (tc >= demand[i] && ChepestInsert(Scur, i, nr, tmp))
				{
					if (tmp < best)
					{
						best = tmp;
						cid = i;
					}
				}
			}
			if (best == INF)
				break;
			ChepestInsert(Scur, cid, nr, tmp, true);
			tc -= demand[cid];
			ok[cid] = true;
		}
	}
	FOR (i, nc)
	{
		if (ok[i]) continue;
		FOR (j, (demand[i]+cap-1)/cap)
		{
			double tmp;
			ChepestInsert(Scur, i, nr, tmp);
			ok[i] = true;
			nr++;
		}
		ok[i] = true;
	}
	Sbest = Scur;

	MAX_TABU = nc * .4;
	printf("MAX_TABU: %d\n", MAX_TABU);
}

void Solve(int init_strategy)
{
	strategy = init_strategy;
	printf("strategy: %d\n", strategy);

	InitTabuTable();

	time_t start_time = clock();
	int badtimes = 0;
	for (int times = 0; ; times++)
	{
		time_t now_time = clock();
		double dt = (double(now_time) - start_time) / CLOCKS_PER_SEC;

		if (dt < 0)
			start_time = now_time;

		Solution neightbor = Neightbor(Scur);
		Scur = neightbor;

		if (Scur.cal() < Sbest.cal())
		{
			badtimes = 0;
			Sbest = Scur;
			printf("%d: dt: %.2lf, best cost: %lf\n", times, dt, Sbest.cal());
		}
		else
			badtimes++;

		UpdateTabuTable();

		if (badtimes >= MAX_BAD_TIMES)
			break;
	}
}
void InitTabuTable()
{
	FOR (i, MAXC) FOR (j, MAXR)
		TL1[i][j] = 0;
}
void UpdateTabuTable()
{
	FOR (i, nc) FOR (j, nr)
		TL1[i][j] = max(0, TL1[i][j]-1);

	TL1[TL[0].first][TL[0].second] = MAX_TABU;
}
void Record(const char fname[])
{
	ofstream out((string(fname)+".result").c_str());
	out << fname << endl;
	out << "# of customer: " << nc << endl;
	out << "# of route: " << nr << endl;
	for (int i = 0; i < nr; i++)
	{
		out << nc << "(begin)";
		foreach (it, Sbest.route[i].L)
		{
			out << "->" << it->cid << "(";
			if (it->q != -1)
				out << it->q;
			else
				out << Sbest.customer[it->cid].M[i];
			out << ")";
		}
		out << "->" << nc << "(end)" << endl;
	}
	out << "total: cost: " << Sbest.ttc << endl;
}

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
	{
		puts(argv[i]);
		FILE* fp = freopen(argv[i], "r", stdin);
		assert(Init());
		InitSolution();

		Solve(1);

		puts(argv[i]);
		assert(Sbest.isFeasible());
		printf("total cost: %lf\n", Sbest.cal());

		Record(argv[i]);
	}
}
