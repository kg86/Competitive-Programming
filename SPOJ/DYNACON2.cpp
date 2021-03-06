#include <bits/stdc++.h>

using namespace std;
using ll = long long;
using ld = long double;
using ii = pair<ll, ll>;
using vi = vector<ll>;
using vb = vector<bool>;
using vvi = vector<vi>;
using vii = vector<ii>;
using vvii = vector<vii>;

constexpr int INF = 2000000000;
constexpr ll LLINF = 9000000000000000000;


template <class T>
struct seq {
	T val; int size_, priority;
	seq<T> *l = nullptr, *r = nullptr, *p = nullptr;
	seq(T _v) : val(_v), size_(1) { priority = rand(); }

	static int size(seq<T> *c) { return c == nullptr ? 0 : c->size_; }
	seq<T> *update() {
		size_ = 1;
		if(l != nullptr) l->p = this, size_ += l->size_;
		if(r != nullptr) r->p = this, size_ += r->size_;
		return this;
	}
	int index() {
		int ind = size(this->l);
		seq<T> *c = this;
		while(c->p != nullptr) {
			if(c->p->l != c) ind += 1 + size(c->p->l);
			c = c->p;
		}
		return ind;
	}
	seq<T> *root() { return this->p == nullptr ? this : p->root(); }
	seq<T> *min() { return this->l == nullptr ? this : l->min(); }
	seq<T> *max() { return this->r == nullptr ? this : r->max(); }
	seq<T> *next() { return this->r == nullptr ? this->p : this->r->min(); }
	seq<T> *prev() { return this->l == nullptr ? this->p : this->l->max(); }
};

// Note: Assumes both nodes are the roots of their sequences.
template <class T>
seq<T> *merge(seq<T> *A, seq<T> *B) {
	if(A == nullptr) return B;
	if(B == nullptr) return A;
	if(A->priority > B->priority) {
		A->r = merge(A->r, B);
		return A->update();
	} else {
		B->l = merge(A, B->l);
		return B->update();
	}
}

// Note: Assumes all nodes are the roots of their sequences.
template <class T, typename... Seqs>
seq<T> *merge(seq<T> *l, Seqs... seqs) {
	return merge(l, merge(seqs...));
}

// Split into [0, index) and [index, ..)
template <class T>
pair<seq<T> *, seq<T> *> split(seq<T> *A, int index) {
	if(A == nullptr) return {nullptr, nullptr};
	A->p = nullptr;
	if(index <= seq<T>::size(A->l)) {
		auto pr = split(A->l, index);
		A->l = pr.second;
		return {pr.first, A->update()};
	} else {
		auto pr = split(A->r, index - (1 + seq<T>::size(A->l)));
		A->r = pr.first;
		return {A->update(), pr.second};
	}
}

// return [0, x), [x, ..)
template <class T>
pair<seq<T> *, seq<T> *> split(seq<T> *A) {
	if (A == nullptr) return {nullptr, nullptr};
	seq<T> *B = A, *lr = A; A = A->l;
	if (A == nullptr) {
		while (lr->p != nullptr && lr->p->l == B) lr = B = lr->p;
		if (lr->p != nullptr) {
			lr = A = lr->p; lr->r = B->p = nullptr;
		}
	} else	A->p = lr->l = nullptr;
	while (lr->update()->p != nullptr) {
			if (lr->p->l == lr) {
				if (lr == A) swap(A->p, B->p), B->p->l = B;
				lr = B = B->p;
			} else {
				if (lr == B) swap(A->p, B->p), A->p->r = A;
				lr = A = A->p;
			}
	}			
	return {A, B};
}

struct EulerTourTree {
	struct edge { int u, v; };
	vector<seq<edge>> vertices;
	vector<unordered_map<int, seq<edge>>> edges;
	EulerTourTree(int n) {
		vertices.reserve(n); edges.reserve(n);
		for (int i = 0; i < n; ++i) add_vertex();
	}
		
	// Create a new vertex.
	int add_vertex() {
		int id = (int)vertices.size();
		vertices.push_back(edge{id, id});
		edges.emplace_back();
		return id;
	}
	// Find root of the subtree containg this vertex.
	int root(int u) { return vertices[u].root()->min()->val.u; }
	bool connected(int u, int v) { return vertices[u].root()==vertices[v].root(); }
	int size(int u) { return (vertices[u].root()->size_ + 2) / 3; }
	// Make v the parent of u. Assumes u has no parent!
	void attach(int u, int v) {
		seq<edge> *i1, *i2;
		tie(i1, i2) = split(&vertices[v]);
		::merge(i1,
				&(edges[v].emplace(u, seq<edge>{edge{v, u}}).first)->second,
				vertices[u].root(),
				&(edges[u].emplace(v, seq<edge>{edge{u, v}}).first)->second,
				i2);
	}
	// Reroot the tree containing u at u.
	void reroot(int u) {
		seq<edge> *i1, *i2;
		tie(i1, i2) = split(&vertices[u]);
		merge(i2, i1);
	}
	// Links u and v.
	void link(int u, int v) { reroot(u); attach(u, v); }
	// Cut {u, v}. Assumes it exists!!
	void cut(int u, int v) {
		auto uv = edges[u].find(v), vu = edges[v].find(u);
		if (uv->second.index() > vu->second.index()) swap(u, v), swap(uv, vu);
		seq<edge> *i1, *i2;
		tie(i1, i2) = split(&uv->second); split(i2, 1);
		merge(i1, split(split(&vu->second).second, 1).second);
		edges[u].erase(uv); edges[v].erase(vu);
	}
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(NULL);
	
	int n, q;
	cin >> n >> q;
	EulerTourTree ett(n);
	vvii qu(n, vii());
	while (q--) {
		string s; int a, b;
		cin >> s >> a >> b; --a; --b;
		int ida = ett.vertices[a].root()->val.u;
		int idb = ett.vertices[b].root()->val.u;
		if (s[0] == 'a') {
			if (ida != idb) {
				ett.link(a, b);
				if (ida != ett.vertices[a].root()->val.u)
					swap(ida, idb);
				qu[ida].insert(qu[ida].end(), qu[idb].begin(), qu[idb].end());
				qu[idb].clear();
			} else {
				qu[ida].push_back({a, b});
			}
		}
		if (s[0] == 'r') {
			ett.cut(a, b);
			int nida = ett.vertices[a].root()->val.u;
			int nidb = ett.vertices[b].root()->val.u;
			vii tmp; vb ta(qu[ida].size(), false); swap(tmp, qu[ida]);
			for (size_t i = 0; i < tmp.size(); ++i) {
				auto &pr = tmp[i];
				int idf = ett.vertices[pr.first ].root()->val.u;
				int ids = ett.vertices[pr.second].root()->val.u;
				if (idf != nida) swap(idf, ids);
				if (idf == nida && ids == nidb) {
					ett.link(pr.first, pr.second);
					tmp.erase(find(tmp.begin(), tmp.end(),pr));
					swap(tmp, qu[ett.vertices[a].root()->val.u]);
					break;
				}
				if (idf == nidb) ta[i] = true;
			}
			for (size_t i = 0; i < tmp.size(); ++i)
				qu[ta[i]?nidb:nida].push_back(tmp[i]);
		}
		if (s[0] == 'c') cout << (ida == idb ? "YES" : "NO") << '\n';
	}
	
	return 0;
}
