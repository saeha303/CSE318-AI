#include <iostream>
#include <Windows.h>
#include <unistd.h>
#include <stack>
int HEIGHT = 10;
int isBonus = 0;
int w1 = 1, w2 = 1, w3 = 1, w4 = 1;
int h_for_p1 = 4, h_for_p2 = 2;
using namespace std;
class Mancala
{
public:
    int aibin[6] = {4, 4, 4, 4, 4, 4};
    int mybin[6] = {4, 4, 4, 4, 4, 4};
    int mystorage = 0;
    int aistorage = 0;
    int aibinsum = 24;
    int mybinsum = 24;
    Mancala *children[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
    Mancala *p = NULL;

    int ai_additional_mv = 0;
    int my_additional_mv = 0;
    int alpha = INT32_MIN;
    int beta = INT32_MAX;
    bool minmax = true; 
    bool bonus = false;
    bool terminal = false;
    int height = 0;
    Mancala()
    {
    }
    Mancala(bool minmax)
    {
        this->minmax = minmax;
    }
    Mancala copy()
    {
        Mancala t;
        for (int i = 0; i < 6; i++)
        {
            t.aibin[i] = aibin[i];
            t.mybin[i] = mybin[i];
            t.children[i] = NULL;
        }
        t.mystorage = mystorage;
        t.aistorage = aistorage;
        t.mybinsum = mybinsum;
        t.aibinsum = aibinsum;
        t.p = NULL;
        t.ai_additional_mv = 0;
        t.my_additional_mv = 0;
        t.alpha = INT32_MIN;
        t.beta = INT32_MAX;
        t.minmax = minmax;
        t.bonus = bonus;
        t.terminal = false;
        t.height = 0;
        return t;
    }
    bool prepare(int validBin)
    {
        for (int i = 0; i < 6; i++)
        {
            mybin[i] = p->mybin[i];
            aibin[i] = p->aibin[i];
        }
        mystorage = p->mystorage;
        aistorage = p->aistorage;
        mybinsum = p->mybinsum;
        aibinsum = p->aibinsum;
        for (int i = 0; i < 6; i++)
        {
            children[i] = NULL;
        }
        alpha = p->alpha;
        beta = p->beta;
        height = p->height;
        height++;
        if (!p->bonus)
        {
            minmax = !(p->minmax);
        }
        else
        {
            minmax = p->minmax;
        }
        // parent copied
        int *arr_own, *arr_op;
        if (minmax)
        {
            arr_own = aibin;
            arr_op = mybin;
        }
        else
        {
            arr_op = aibin;
            arr_own = mybin;
        }
        int n = arr_own[validBin];
        int t = n;
        while (t > 0)
        {

            for (int i = validBin + 1; i <= validBin + n && i < 6; i++)
            {
                t--;
                arr_own[i]++;
            }
            arr_own[validBin] -= (n - t);
            if (!t && arr_own[validBin + n] == 1 && arr_op[5 - validBin - n] > 0) // if last ball is in an empty pit on my side, collect balls
            {
                if (minmax)
                {
                    aistorage += (1 + arr_op[5 - validBin - n]);
                    aibinsum--;
                    mybinsum -= arr_op[5 - validBin - n];
                }
                else
                {
                    mystorage += (1 + arr_op[5 - validBin - n]);
                    mybinsum--;
                    aibinsum -= arr_op[5 - validBin - n];
                }
                arr_op[5 - validBin - n] = 0;
                arr_own[validBin + n] = 0;
            }
            if (validBin + n >= 6)
            {
                t--;
                if (minmax)
                {
                    aistorage++;
                    aibinsum--;
                }
                else
                {
                    mystorage++;
                    mybinsum--;
                }
                arr_own[validBin]--;
                if (!t)
                {
                    // if last ball is in storage, bonus round
                    bonus = true;
                    checkAllZero();
                    if (minmax)
                        ai_additional_mv++;
                    else
                        my_additional_mv++;
                    return true;
                }
            }
            if (validBin + n > 6)
            {
                for (int i = 0; t && i < 6; i++)
                {
                    t--;
                    arr_op[i]++;
                    arr_own[validBin]--;
                    if (minmax)
                    {
                        aibinsum--;
                        mybinsum++;
                    }
                    else
                    {
                        mybinsum--;
                        aibinsum++;
                    }
                }
                if (t > 0)
                {
                    int i;
                    for (i = 0; t && i <= validBin; i++)
                    {
                        t--;
                        arr_own[i]++;
                        arr_own[validBin]--;
                    }
                    if (!t && arr_own[i - 1] == 1 && arr_op[5 - i + 1] > 0) // if last ball is in an empty pit, collect ball
                    {
                        if (minmax)
                        {
                            aistorage += (1 + arr_op[5 - i + 1]);
                            aibinsum--;
                            mybinsum -= arr_op[5 - i + 1];
                        }
                        else
                        {
                            mystorage += (1 + arr_op[5 - i + 1]);
                            mybinsum--;
                            aibinsum -= arr_op[5 - i + 1];
                        }
                        arr_op[5 - i + 1] = 0;
                        arr_own[i - 1] = 0;
                    }
                }
            }
            n = t;
        }
        checkAllZero();
        return false;
    }
    void checkAllZero()
    {
        bool myallZero = true, aiallZero = true;
        for (int i = 0; i < 6; i++)
        {
            if (mybin[i])
            {
                myallZero = false;
                break;
            }
        }
        for (int i = 0; i < 6; i++)
        {
            if (aibin[i])
            {
                aiallZero = false;
                break;
            }
        }
        if (myallZero || aiallZero)
        {
            int sum = 0;

            for (int i = 0; i < 6; i++)
            {
                sum += (!aiallZero * aibin[i] + !myallZero * mybin[i]);
                if (aibin[i])
                    aibin[i] = 0;
                if (mybin[i])
                    mybin[i] = 0;
            }
            if (!aiallZero)
            {
                aistorage += sum;
                aibinsum -= sum;
            }
            else if (!myallZero)
            {
                mystorage += sum;
                mybinsum -= sum;
            }
        }
    }
    bool isTerminal()
    {
        if (height > HEIGHT - 1)
        {
            return true;
        }
        return false;
    }
    int nextState(int i) 
    {
        if (!bonus)
        {
            if (minmax * mybin[i] || !minmax * aibin[i])
            {
                return i;
            }
        }
        else
        {
            if (!minmax * mybin[i] || minmax * aibin[i])
            {
                return i;
            }
        }
        return -1;
    }
    int chooseHeuristic(int i)
    {
        switch (i)
        {
        case 1:
            return h1();
        case 2:
            return h2();
        case 3:
            return h3();
        case 4:
            return h4();
        }
    }
    int DFS(stack<Mancala *> s, int n)
    {
        int validBin;
        for (int i = 0; i < 6; i++)
        {
            int validBin = nextState(i);
            if (isTerminal())
            {
                terminal = true;
                if (minmax)
                {
                    return alpha = chooseHeuristic(n);
                }

                return beta = chooseHeuristic(n);
            }
            if (!terminal && validBin >= 0 && validBin < 6)
            {

                children[validBin] = new Mancala();
                s.push(children[validBin]);
                children[validBin]->p = this;
                bool rb = children[validBin]->prepare(validBin);
                int rv = children[validBin]->DFS(s, n);
                if (!terminal)
                {
                    if (minmax)
                    {
                        if (rv > alpha)
                            alpha = rv;
                    }
                    else
                    {
                        if (rv < beta)
                            beta = rv;
                    }
                    if (alpha >= beta)
                    {
                        break;
                    }
                }
            }
        }

        return minmax ? alpha : beta;
    }
    int h1()
    {
        if (minmax)
        {
            return aistorage - mystorage;
        }
        return mystorage - aistorage;
    }
    int h2()
    {
        /*W1 * (stones_in_my_storage – stones_in_opponents_storage) + W2 * (stones_on_my_side –
        stones_on_opponents_side)*/
        if (minmax)
        {
            return w1 * (aistorage - mystorage) + w2 * (aibinsum - mybinsum);
        }
        return w1 * (mystorage - aistorage) + w2 * (mybinsum - aibinsum);
    }
    int h3()
    {
        /*W1 * (stones_in_my_storage – stones_in_opponents_storage) + W2 * (stones_on_my_side –
        stones_on_opponents_side) + W3 * (additional_move_earned)*/
        if (minmax)
        {
            return w1 * (aistorage - mystorage) + w2 * (aibinsum - mybinsum) + w3 * ai_additional_mv;
        }
        return w1 * (mystorage - aistorage) + w2 * (mybinsum - aibinsum) + w3 * my_additional_mv;
    }
    int h4()
    {
        /*W1 * (stones_in_my_storage – stones_in_opponents_storage) + W2 * (stones_on_my_side –
stones_on_opponents_side) + W3 * (additional_move_earned) + W4 * (stones_captured)*/
        if (minmax)
        {
            return w1 * (aistorage - mystorage) + w2 * (aibinsum - mybinsum) + w3 * ai_additional_mv + w4 * (aibinsum - p->aibinsum);
        }
        return w1 * (mystorage - aistorage) + w2 * (mybinsum - aibinsum) + w3 * my_additional_mv + w4 * (mybinsum - p->mybinsum);
    }
    Mancala traversal()
    {
        if (minmax)
        {
        for (int i = 0; i < 6; i++)
        {
            if (children[i] != NULL && ((children[i]->minmax != minmax && children[i]->beta == alpha && children[i]->alpha < children[i]->beta) || (children[i]->minmax == minmax && children[i]->alpha == alpha) || (children[i]->minmax != minmax && children[i]->alpha == alpha))) //(children[i] != NULL && ((children[i]->minmax != minmax && children[i]->beta == alpha && children[i]->alpha < children[i]->beta) || (children[i]->minmax == minmax && children[i]->alpha == alpha) || (children[i]->minmax != minmax && children[i]->alpha == alpha)))
            {
                children[i]->print(1);
                Mancala t = children[i]->copy();
                return t;
            }
        }
        }
        else
        {
            for (int i = 0; i < 6; i++)
            {
                if (children[i] != NULL && ((children[i]->minmax != minmax && children[i]->alpha == beta && children[i]->alpha < children[i]->beta) || (children[i]->minmax == minmax && children[i]->beta == beta) || (children[i]->minmax != minmax && children[i]->beta == beta)))
                {
                    children[i]->print(1);
                    Mancala t = children[i]->copy();
                    return t;
                }
            }
        }
    }
    void print(int n)
    {
        if (minmax && n)
            cout << "Player 2's turn" << '\n';
        else if (!minmax && n)
            cout << "Player 1's turn" << '\n';
        cout << "      Player 1's side" << '\n';
        cout << "   " << mybin[5] << "  " << mybin[4] << "  " << mybin[3] << "  " << mybin[2] << "  " << mybin[1] << "  " << mybin[0] << "   " << '\n';
        cout << mystorage << "                     " << aistorage << '\n';
        cout << "   " << aibin[0] << "  " << aibin[1] << "  " << aibin[2] << "  " << aibin[3] << "  " << aibin[4] << "  " << aibin[5] << "   " << '\n';
        cout << "       Player 2's side" << '\n';
    }
    void examine()
    {
        if (minmax)
        {
            cout << alpha << '\n';
            cout << beta << '\n';
            cout << "child" << '\n';
            for (int i = 0; i < 6; i++)
            {
                if (children[i] != NULL)
                {
                    cout << children[i]->alpha << '\n';
                    cout << children[i]->beta << '\n';
                }
            }
        }
        else
        {
            cout << alpha << '\n';
            cout << beta << '\n';
            cout << "child" << '\n';
            for (int i = 0; i < 6; i++)
            {
                if (children[i] != NULL)
                {
                    cout << children[i]->alpha << '\n';
                    cout << children[i]->beta << '\n';
                }
            }
        }
    }
};
void freeList(stack<Mancala *> s)
{
    Mancala *top = NULL;
    while (!s.empty())
    {
        top = s.top();
        s.pop();
        delete top;
    }
}
int main()
{
    stack<Mancala *> s;
    Mancala m; // true player 1 human side, false player 2 ai side
    m.print(0);
    m.DFS(s, h_for_p1);
    // m.examine();
    Mancala t = m.traversal();
    while (true)
    {
        if ((!t.minmax && t.bonus) || (t.minmax && !t.bonus))
        {
            t.DFS(s, h_for_p1);
        }
        else if ((t.minmax && t.bonus) || (!t.minmax && !t.bonus))
        {
            t.DFS(s, h_for_p2);
        }
        Mancala tt = t.traversal();
        if (tt.aistorage + tt.mystorage == 48)
            break;
        freeList(s);
        t = tt;
    }
}
