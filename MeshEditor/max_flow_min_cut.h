#pragma once

#include <iostream>
#include <queue>
#include <vector>

#define NIL -1
enum Color{white, gray};

template <typename T>
class CoreSolver {
public:
	CoreSolver()
	{
		vertex_num = 0;
	}
	CoreSolver(int vertex_num, int s, int t)
	{
		this->vertex_num = vertex_num;
		this->s = s;
		this->t = t;
		precessor.resize (vertex_num);
		state.resize (vertex_num);
		capacity.resize (vertex_num);
		flow.resize (vertex_num);
		residual.resize (vertex_num);

		for (int i=0; i<vertex_num; i++) {
			capacity[i].resize (vertex_num);
			flow[i].resize (vertex_num);
			residual[i].resize (vertex_num);
		}

		for (int i=0; i<vertex_num; i++)
			for (int j=0; j<vertex_num; j++)
				capacity[i][j] = 0;
	}
	void insert_edge(int u, int v, T weight)
	{
		if (u<0 || u>=vertex_num || v<0 || v>=vertex_num)
			return;
		capacity[u][v] = weight;
	}
	T ford_fulkerson()
	{
		T max_flow = (T)0.0;
		initial();
		while (find_argumenting_path()) {
			T minIncrease = get_min_increase();
			max_flow += minIncrease;
			update(minIncrease);
		}
		return max_flow;
	}
	std::vector<std::pair<int, int> > mincut (){
		std::vector<std::pair<int, int> > result;
		std::vector<bool> visited;
		visited.resize (vertex_num, false);

		std::queue<int> bfs;
		bfs.push (s);
		visited[s] = true;

		do 
		{
			int cur_node = bfs.front ();
			bfs.pop ();
			for (int i = 0; i != vertex_num; ++i){
				if (i == cur_node) continue;
				if (residual[cur_node][i] && !visited[i]){
					bfs.push (i); visited[i] = true;
				}
			}

		} while (!bfs.empty ());

		for (int i = 0; i != vertex_num - 1; ++i){
			for (int j = i + 1; j != vertex_num; ++j){
				if (capacity[i][j] && (visited[i] ^ visited[j])){
					result.push_back (std::make_pair (i, j));
				}
			}
		}

		return result;
	}
private:
	void update(T min)
	{
		int cur = t;
		int pre = precessor[t];
		do {
			flow[pre][cur] += min;
			flow[cur][pre] = -flow[pre][cur];
			residual[pre][cur] = capacity[pre][cur] - flow[pre][cur];
			residual[cur][pre] = capacity[cur][pre] - flow[cur][pre];
			cur = pre;
		} while((pre = precessor[pre]) != NIL);
	}

	bool find_argumenting_path()
	{
		for (int i = 0; i < vertex_num; i++)
			state[i] = white;
		std::queue<int> que;
		que.push(s);
		state[s] = gray;
		while (!que.empty()) {
			int head = que.front();
			que.pop();
			for (int i=0; i < vertex_num; i++) {
				if (residual[head][i] > (T)0.0 && state[i] == white) {
					precessor[i] = head;
					if (i == t)
						return true;
					que.push(i);
					state[i] = gray;
				}
			}
		}
		return false;
	}
	T get_min_increase()
	{
		int pre = precessor[t];
		T min = residual[pre][t];
		while (pre != s) {
			int cur = pre;
			pre = precessor[cur];
			if (min > residual[pre][cur])
				min = residual[pre][cur];
		}
		return min;
	}
	void initial(){
		for (int i=0; i<vertex_num; i++) {
			for (int j=0; j<vertex_num; j++) {
				flow[i][j] = 0;
				residual[i][j] = capacity[i][j] - flow[i][j];
			}
		}
		precessor[s] = NIL;
	}

	int vertex_num;
	int s; // Ô´
	int t; // ¶Ë
	std::vector<int> precessor;
	std::vector<Color> state;
	std::vector<std::vector<T> > capacity;
	std::vector<std::vector<T> > flow;
	std::vector<std::vector<T> > residual;
};