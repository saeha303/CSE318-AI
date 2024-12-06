#include <bits/stdc++.h>
#include <time.h>
using namespace std;
int local_it = 0;
int local_cut_wt = 0;
static random_device rd;                      // Random device engine, usually based on /dev/random on UNIX-like systems
static default_random_engine generator(rd()); // Initialize Mersennes' twister using rd to generate the seed
// Generate random doubles in the interval [initial, last)
double random_real(double initial, double last)
{
    uniform_real_distribution<double> distribution(initial, last);
    return distribution(generator); // Use rng as a generator
}
// Generate random int in the interval [initial, last]
int random_int(int initial, int last)
{
    uniform_int_distribution<int> distribution(initial, last);
    return distribution(generator); // Use rng as a generator
}
class Graph
{
    int V;
    int **matrix;
    vector<int> X, Y; // partitions are saved so that argument pass will be easier
    vector<int> S, S_;
    int cut_wt;      // for X, Y
    int real_cut_wt; // for S, S_
    int *sigma_X;
    int *sigma_Y;

public:
    Graph(int V)
    {
        this->V = V;
        matrix = new int *[V];
        for (int i = 0; i < V; i++)
        {
            matrix[i] = new int[V];
        }
        for (int i = 0; i < V; i++)
        {
            for (int j = 0; j < V; j++)
            {
                matrix[i][j] = 0;
            }
        }
        sigma_X = new int[V];
        sigma_Y = new int[V];
    }
    ~Graph()
    {
        for (int i = 0; i < V; i++)
        {
            delete[] matrix[i];
        }
        delete[] matrix;
        delete[] sigma_X;
        delete[] sigma_Y;
    }
    void addEdge(int x, int y, int w)
    {
        matrix[x][y] = matrix[y][x] = w;
    }
    int get_real_cut_wt()
    {
        return real_cut_wt;
    }
    void print(vector<int> &T)
    {
        for (int i = 0; i < T.size(); i++)
        {
            cout << T[i];
            if (i < T.size() - 1)
                cout << ",";
        }
    }
    void get_rcl_e(vector<pair<int,int>> &rcl, double meu)
    {
        for (int i = 0; i < V; i++)
        {
            for (int j = 0; j < i; j++)
            {
                if (matrix[i][j] >= meu)
                {
                    rcl.push_back(make_pair(i, j));
                }
            }
        }
    }
    void get_V_(vector<int> &V_, vector<int> &X, vector<int> &Y)
    {
        vector<int> temp;
        int x = X.size(), y = Y.size();
        for (int i = 0; i < x; i++)
        {
            temp.push_back(X[i]);
        }
        for (int i = 0; i < y; i++)
        {
            temp.push_back(Y[i]);
        }
        sort(temp.begin(), temp.end());
        int j = 0;
        for (int i = 0; i < V; i++)
        {
            if (j < temp.size() && temp[j] == i)
            {
                j++;
            }
            else
            {
                V_.push_back(i);
            }
        }
    }
    int get_sigma(int v, vector<int> &T)
    {
        int sum = 0, j = 0;
        for (int i = 0; i < T.size(); i++)
        {
            sum += matrix[v][T[i]];
        }
        return sum;
    }
    int get_min(int *t, int size)
    {
        int min = t[0];
        for (int i = 0; i < size; i++)
        {
            if (t[i] < min)
            {
                min = t[i];
            }
        }
        return min;
    }
    int get_argmax(vector<int> &t, int size)
    {
        int max = t[0];
        int argmax = 0;
        for (int i = 0; i < t.size(); i++)
        {
            if (t[i] > max)
            {
                max = t[i];
                argmax = i;
            }
        }
        return argmax;
    }
    void get_rcl_v(vector<int> &V_, int *sigma_x, int *sigma_y, double meu, vector<int> &rcl_v)
    {
        for (int v : V_)
        {
            int max_ = max(sigma_x[v], sigma_y[v]);
            if (max_ >= meu)
            {
                rcl_v.push_back(v);
            }
        }
    }
    int get_cut_wt()
    {
        int x = X.size(), y = Y.size();
        cut_wt = 0;
        for (int i = 0; i < x; i++)
        {
            for (int j = 0; j < y; j++)
            {
                cut_wt += matrix[X[i]][Y[j]];
            }
        }
        return cut_wt;
    }

    int find_node(vector<int> &T, int v)
    {
        for (int i = 0; i < T.size(); i++)
        {
            if (T[i] == v)
                return v;
        }
        return -1;
    }
    void remove_node(vector<int> &T, int v)
    {
        for (int i = 0; i < T.size(); i++)
        {
            if (T[i] == v)
            {
                T.erase(T.begin() + i, T.begin() + i + 1);
                break;
            }
        }
    }
    void add_node(vector<int> &T, int v)
    {
        T.push_back(v);
    }
    int randomized_maxcut()
    {
        for (int i = 0; i < V; i++)
        {
            int random = random_int(0, 1);
            if (!random)
            {
                Y.push_back(i);
            }
            else
            {
                X.push_back(i);
            }
        }
        int temp = get_cut_wt();
        X.clear();
        Y.clear();
        return temp;
    }
    int greedy_maxcut(int s_max, int d_max)
    {
        X.push_back(s_max);
        Y.push_back(d_max);
        int x = 1, y = 1;
        vector<int> V_;
        for (int i = 0; i < V; i++)
        {
            V_.push_back(i);
        }
        V_.erase(find(V_.begin(), V_.end(), s_max));
        V_.erase(find(V_.begin(), V_.end(), d_max));
        while (x + y != V)
        {
            vector<int> g(V, INT_MIN);
            for (int v : V_)
            {
                sigma_X[v] = get_sigma(v, Y);
                sigma_Y[v] = get_sigma(v, X);
                g[v] = max(sigma_X[v], sigma_Y[v]);
            }
            int vertex = get_argmax(g, V);
            if (sigma_X[vertex] > sigma_Y[vertex])
            {
                X.push_back(vertex);
                x++;
            }
            else
            {
                Y.push_back(vertex);
                y++;
            }
            V_.erase(find(V_.begin(), V_.end(), vertex));
            memset(sigma_X, 0, V);
            memset(sigma_Y, 0, V);
        }
        int temp = get_cut_wt();
        X.clear();
        Y.clear();
        return temp;
    }
    void semi_greedy_maxcut(int w_min, int w_max)
    {
        double alpha = random_real(0, 1); // print alpha        
        double meu = w_min + alpha * (w_max - w_min);
        vector<pair<int, int>> rcl;
        get_rcl_e(rcl, meu);
        int random = random_int(0, rcl.size() - 1);
        pair<int, int> rand_edge = rcl[random];
        X.push_back(rand_edge.first);
        Y.push_back(rand_edge.second);
        int x = 1, y = 1;
        vector<int> V_;
        for (int i = 0; i < V; i++)
        {
            V_.push_back(i);
        }
        V_.erase(find(V_.begin(), V_.end(), rand_edge.first));
        V_.erase(find(V_.begin(), V_.end(), rand_edge.second));
        while (x + y != V)
        {
            int sigmax_min = INT_MAX, sigmax_max = INT_MIN, sigmay_min = INT_MAX, sigmay_max = INT_MIN;
            for (int v : V_)
            {
                sigma_X[v] = get_sigma(v, Y);
                if (sigma_X[v] < sigmax_min)
                {
                    sigmax_min = sigma_X[v];
                }
                if (sigma_X[v] > sigmax_max)
                {
                    sigmax_max = sigma_X[v];
                }
                sigma_Y[v] = get_sigma(v, X);
                if (sigma_Y[v] < sigmay_min)
                {
                    sigmay_min = sigma_Y[v];
                }
                if (sigma_Y[v] > sigmay_max)
                {
                    sigmay_max = sigma_Y[v];
                }
            }
            w_min = min(sigmax_min, sigmay_min);
            w_max = max(sigmax_max, sigmay_max);
            meu = w_min + alpha * (w_max - w_min);
            vector<int> rcl_v;
            get_rcl_v(V_, sigma_X, sigma_Y, meu, rcl_v);
            random = random_int(0, rcl_v.size() - 1);
            int rand_vertex = rcl_v[random];
            if (sigma_X[rand_vertex] > sigma_Y[rand_vertex])
            {
                X.push_back(rand_vertex);
                x++;
            }
            else
            {
                Y.push_back(rand_vertex);
                y++;
            }
            V_.erase(find(V_.begin(), V_.end(), rand_vertex));
            memset(sigma_X, 0, V);
            memset(sigma_Y, 0, V);
        }
        get_cut_wt();
    }
    void local_search_maxcut()
    {
        bool change = true;
        int it = 0;
        while (change) // print the iter
        {
            it++;
            change = false;
            for (int i = 0; i < V; i++)
            {
                int isInX = find_node(X, i);
                int isInY = find_node(Y, i);
                int sigmax = get_sigma(i, Y);
                int sigmay = get_sigma(i, X);
                
                
                // int sigmax = sigma_X[i];
                // int sigmay = sigma_Y[i];
                if (isInX == i && sigmay - sigmax > 0)
                {
                    remove_node(X, i);
                    add_node(Y, i);
                    change = true;
                }
                else if (isInY == i && sigmax - sigmay > 0)
                {
                    remove_node(Y, i);
                    add_node(X, i);
                    change = true;
                }
            }
        }
        get_cut_wt();
        local_it += it;
        local_cut_wt += cut_wt;
        // cout << "No. of iterations in local-1: " << it << '\n';
        // cout << "Best value in local-1: " << cut_wt << '\n';
    }
    int grasp_maxcut(int w_min, int w_max)
    {
        int best_obj = INT_MIN;
        int max_iter = 50;
        int it = 0;
        int semi_greedy = 0;

        while (it < max_iter) // koy iter
        {
            semi_greedy_maxcut(w_min,w_max);
            semi_greedy += cut_wt;
            local_search_maxcut();
            if (cut_wt > best_obj)
            {
                S = X;
                S_ = Y;
                real_cut_wt = cut_wt;
                best_obj = cut_wt;
            }
            X.clear();
            Y.clear();
            it++;
        }
        cout << "Semi Greedy-1 Max-cut: " << (int)semi_greedy / max_iter << '\n';
        cout << "No. of iterations in local-1: " << (int)local_it / max_iter << '\n';
        cout << "Best value in local-1: " << (int)local_cut_wt / max_iter << '\n';
        cout << "No. of iterations in GRASP-1: " << max_iter << '\n';
        cout << "Best value in GRASP-1: " << best_obj << '\n';
        return best_obj;
    }
};

int main()
{
    int V, E;
    int s1, s2, w,w_min=INT_MAX,w_max=INT_MIN,s_max,d_max,s_min,d_min;
    ifstream file("set1/g54.rud");
    file >> V >> E;
    cout << "|V|:" << V << '\n';
    cout << "|E|:" << E << '\n';
    Graph g(V);
    while (E--)
    {
        file >> s1 >> s2 >> w;
        s1--;
        s2--;
        if(w>=w_max){
            w_max=w;
            s_max=s1;
            d_max=s2;
        }
        if(w<=w_min){
            w_min=w;
            s_min=s1;
            d_min=s2;
        }
            
        g.addEdge(s1, s2, w);
    }
    // g.sort_edges();
    // cout<<s_min<<","<<d_min<<","<<w_min<<","<<s_max<<","<<d_max<<","<<w_max<<","<<endl;
    int max_iter = 10;
    int it = 0;
    int rand = 0;
    while (it < max_iter)
    {
        rand += g.randomized_maxcut();
        it++;
    }
    cout << "Simple Randomized or Ranomized-1 Max-cut: " << (int)rand / max_iter << '\n';
    int greedy = g.greedy_maxcut(s_max,d_max);
    cout << "Simple Greedy or Greedy-1 Max-cut: " << greedy << '\n';
    int semi_greedy = g.grasp_maxcut(w_min,w_max);
    // cout<<"Known best solution or upper bound: "<<max(greedy,max(semi_greedy,rand))<<'\n';
    file.close();
}