#include <iostream>
#include <queue>
#include <stack>
using namespace std;

int n=0;
class node
{
private:
    /* data */
public:
    int* arr;
    int idx;
    node* parent;
    int g;
    int f;
    node* up;
    node* down;
    node* left;
    node* right;
    node(node* parent, int idx){
        this->arr=new int[n*n];
        this->parent=parent;
        this->idx=idx;
        up=down=left=right=NULL;
    }
    node(int arr[], int n, int idx, node* parent, int g, int f){
        this->arr=new int[n*n];
        for(int i=0;i<n*n;i++){
            this->arr[i]=arr[i];
        }
        this->idx=idx;
        this->parent=parent;
        this->g=g;
        this->f=f;
        up=down=left=right=NULL;
    }
    void traverse(stack<node*>);
    void prepareArray(){//to move blank space
        for(int i=0;i<n*n;i++){
            arr[i]=parent->arr[i];
        }
        int t=arr[parent->idx];
        arr[parent->idx]=arr[idx];
        arr[idx]=t;
    }
    bool isSolvable(){
        int sum=0;
        for(int i=0;i<n*n-1;i++){
            for(int j=i+1;j<n*n;j++){
                if(arr[i]>arr[j] && arr[j]!=0){
                    sum+=1;
                }
            }
        }
        if(!(sum%2)){
            return true;
        }
        return false;
    }
    int chooseHeuristic(){
        // return heuristic1();
        return heuristic2();
    }
    int heuristic1(){
        int ans=0;
        for(int i=0;i<n*n;i++){
            if(arr[i]!=0 && arr[i]!=i+1){
                ans++;
            }
        }
        return ans;
    }
    int heuristic2(){
        int x1,x2,y1,y2,ans=0;
        for(int i=0;i<n*n;i++){
            if(arr[i]!=0){
                x1=i/n;
            y1=i%n;
            if(arr[i]%n){
                x2=arr[i]/n;
                y2=arr[i]%n-1;
            }else{
                x2=arr[i]/n-1;
                y2=n-1;
            }
                ans+=abs(x1-x2)+abs(y1-y2);
            } 
        }
        return ans;
    }
    void print(){
        for(int i=0;i<n*n;i++){
            cout<<arr[i]<<' ';
            if((i+1)%n==0){
                cout<<'\n';
            }
        }
        cout<<'\n';
    }
    ~node(){
        delete[] arr;
    }
};
class compare{
    public:
bool operator()(node* a, node* b) {
    return a->f > b->f;
}
};
void printPuzzle(node* top){
    stack<node*> q;//closed list
    while (top->parent!=NULL)
    {
        q.push(top);
        top=top->parent;
    }
    q.push(top);
    cout<<"Minimum number of moves = "<<q.size()-1<<'\n';
    while (!q.empty())
    {
        q.top()->print();
        q.pop();
    }
    
}
void node::traverse(stack<node*> q){
        priority_queue<node*,vector<node*>,compare> pq;//open list
        node* top = NULL;
        int explored=1,expanded=0;
        int it=0;
        pq.push(this);
        while (!pq.empty())///O(V)
        {
            top = pq.top();
            q.push(top);
            pq.pop();///O(logV)
            expanded++;
            node *u,*d,*l,*r;
            if(top->idx-n>=0 && top->idx-n<n*n){//up
                if(top->parent==NULL||top->parent->idx!=top->idx-n){
                    u=new node(top,top->idx-n);
                u->prepareArray();
                int t=u->chooseHeuristic();
                if(!t){
                    q.push(u);
                    top=u;
                    ///print first
                    break;
                }
                    
                u->f=top->g+1+t;
                u->g=top->g+1;
                top->up=u;
                pq.push(u);
                explored++;
                q.push(u);
                }
            }
            if(top->idx-1>=0 && top->idx-1<n*n && top->idx/n==(top->idx-1)/n){//left
                if(top->parent==NULL||top->parent->idx!=top->idx-1){
                    l=new node(top,top->idx-1);
                l->prepareArray();
                int t=l->chooseHeuristic();
                if(!t){
                    q.push(l);
                    top=l;
                    ///print first
                    break;
                }
                l->f=top->g+1+t;
                l->g=top->g+1;
                top->left=l;
                pq.push(l);
                explored++;
                q.push(l);
                }
            }
            if(top->idx+n>=0 && top->idx+n<n*n){//down
                if(top->parent==NULL||top->parent->idx!=top->idx+n){
                    d=new node(top,top->idx+n);
                d->prepareArray();
                int t=d->chooseHeuristic();
                if(!t){
                    q.push(d);
                    top=d;
                    ///print first
                    break;
                }
                d->f=top->g+1+t;
                d->g=top->g+1;
                top->down=d;
                pq.push(d);
                explored++;
                q.push(d);
                }
            }
            if(top->idx+1>=0 && top->idx+1<n*n && top->idx/n==(top->idx+1)/n){//right
                if(top->parent==NULL||top->parent->idx!=top->idx+1){
                    r=new node(top,top->idx+1);
                r->prepareArray();
                int t=r->chooseHeuristic();
                if(!t){
                    q.push(r);
                    top=r;
                    ///print first
                    break;
                }
                r->f=top->g+1+t;
                r->g=top->g+1;
                top->right=r;
                pq.push(r);
                explored++;
                q.push(r);
                }
            }
    }
    cout<<"Number of explored nodes= "<<explored<<'\n';
    cout<<"Number of expanded nodes= "<<expanded<<'\n';
    printPuzzle(top);
}
void freeList(stack<node*> s){
    node* top=NULL;
    while (!s.empty())
    {
        top=s.top();
        s.pop();
        delete top;
    }
    
}
int main(){
    cin>>n;
    int arr[n*n];
    int idx0=-1;
    for(int i=0;i<n*n;i++){
        cin>>arr[i];
        if(!arr[i]){
            idx0=i;
        }
    }
    stack<node*> s;
    node* root=new node(arr,n,idx0,NULL,0,0);
    bool flag=root->isSolvable();
    if(flag){
        root->traverse(s);
    }else{
        cout<<"Unsolvable puzzle"<<'\n';
    }
    freeList(s);
}