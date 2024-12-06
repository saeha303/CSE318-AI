#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <random>
using namespace std;
// Shuffle the dataset randomly
static random_device rd;
static mt19937 g(rd());
class Node
{
public:
    // pair<string, int> name_val;
    string name;
    int val;
    vector<Node> children;
    Node()
    {
        // this->name_val = make_pair("", -1);
        this->name = "";
        this->val = -1;
        // cout << "in def node" << '\n';
    }
    Node(pair<string, int> name_val)
    {
        // this->name_val = name_val;
        this->name = name_val.first;
        this->val = name_val.second;

        for (int i = 0; i < 4; i++)
        {
            children.push_back(Node());
        }
        children.resize(4);
        // cout << "in node" << '\n';
    }
};
class DecisionTree
{
public:
    vector<map<string, int>> attributes;
    map<string, int> output_values;
    map<string, int> attribute_map;
    Node root;
    DecisionTree()
    {
    }
    // Function to find a key based on a value
    // template <typename K, typename V>
    string find_key_by_value(std::map<string, int> &m, int &value)
    {
        for (const auto &pair : m)
        {
            if (pair.second == value)
            {
                return pair.first;
            }
        }
        // Return a default value if the value is not found
        return " ";
    }
    Node decision_tree_learning(vector<vector<int>> examples, vector<vector<int>> parent_examples, int iter)
    {
        int idx = -1;
        vector<int> examples_output(output_values.size(), 0);
        vector<int> parent_examples_output(output_values.size(), 0);
        count_outputs(examples, examples_output);
        count_outputs(parent_examples, parent_examples_output);
        if (examples.size() == 0)
        {
            // cout << "case 1" << '\n';
            return count_plurality(parent_examples_output);
        }
        else if (count_classification(examples_output,idx) != -1)
        {
            // cout << "case 2" << '\n';
            string key = find_key_by_value(output_values, idx);
            return Node(make_pair(key, idx));
        }
        else if (attribute_map.size() == 0)
        {
            // cout << "case 3" << '\n';
            return count_plurality(examples_output);
        }
        else
        {
            // cout << "case 4" << '\n';
            int A = -1;
            double info_gain = INT_MIN;
            for (int i = 0; i < attributes.size(); i++)
            {
                if (find_key_by_value(attribute_map, i) != " ")
                {
                    double temp_ig = InformationGain(i, examples_output, examples);
                    if (temp_ig > info_gain)
                    {
                        info_gain = temp_ig;
                        A = i;
                    }
                }
            }
            string key = find_key_by_value(attribute_map, A);
            Node tree(make_pair(key, A));
            string str;
            for (int i = 0; i < attributes[A].size(); i++)
            {
                vector<vector<int>> exs;
                helper_exs(A, i, examples, exs);
                // Iterate through the map to find and erase by value
                for (auto it = attribute_map.begin(); it != attribute_map.end(); ++it)
                {
                    if (it->second == A)
                    {
                        str=it->first;
                        attribute_map.erase(it);
                        break; // Stop after erasing the first occurrence
                    }
                }
                // attributes.erase(attributes.begin() + A);
                Node subtree = decision_tree_learning(exs, examples, iter + 1);
                key = find_key_by_value(attribute_map, i);
                tree.children[i] = subtree;
                // print(tree);
            }
            if (!iter)
            {
                root = tree;
            }
            attribute_map[str]=A;
            return tree;
        }
        return Node();
    }
    void print(Node tree)
    {
        for(int i=0;i<tree.children.size();i++){
            cout<<tree.children[i].name<<","<<tree.children[i].val<<'\n';
            cout<<"per"<<'\n';
            for(int j=0;j<tree.children.size();j++){
                if(tree.children[i].val!=-1)
                cout<<tree.children[i].children[j].name<<","<<tree.children[i].children[j].val<<'\n';
            }
        }
        cout<<"after print"<<'\n';
    }
    int test(vector<int> testvct)
    {
        Node *node = &root;
        Node *parent;
        while (true)
        {
            if (node->val != -1)
            {
                parent = node;
                node = &node->children[testvct[node->val]];
            }

            else{
                break;
            }
                
        }
        if (parent->val == testvct[attributes.size()])
        {
            return 1;
        }
        return 0;
    }
    void helper_exs(int attr_idx, int attr_val, vector<vector<int>> &examples, vector<vector<int>> &exs)
    {
        int i = 0;
        int nAttr = attributes.size();
        for (int j = 0; j < examples.size(); j++)
        {
            if (attr_val == examples[j][attr_idx])
            {
                exs.push_back(examples[j]);
                // exs[i]=example[j];
                i++;
            }
        }
    }
    void helper(int attr_idx, int attr_val, int &sum, vector<int> &temp, vector<vector<int>> examples)
    {
        int nAttr = attributes.size();
        for (int j = 0; j < examples.size(); j++)
        {
            if (attr_val == examples[j][attr_idx])
            {
                sum++;
                temp[examples[j][nAttr]]++;
            }
        }
    }
    double InformationGain(int attridx, vector<int> examples_output, vector<vector<int>> examples)
    {
        vector<int> temp(output_values.size(),0);
        int sum = 0;
        double remainder = 0;
        double h_v = H_V(examples.size(), examples_output);
        for (int i = 0; i < attributes[attridx].size(); i++)
        {
            helper(attridx, i, sum, temp, examples);
            remainder += ((double)sum / (double)examples.size()) * H_V(sum, temp);
            sum=0;
            for(int j=0;j<output_values.size();j++){
                temp[j]=0;
            }
        }
        return h_v - remainder;
    }

    double H_V(int vct_size, vector<int> &out)
    {
        double sum = 0;
        int size = output_values.size();
        for (int i = 0; i < size; i++)
        {
            if (out[i]){
                double probability = static_cast<double> (double(out[i])) / (double(vct_size));//
                sum += (-probability * log2(probability));
            }   
        }
        return sum;
    }
    void count_outputs(vector<vector<int>> &vct, vector<int> &out)
    {
        int size = output_values.size();
        int idx = attributes.size();
        for (int i = 0; i < vct.size(); i++)
        {
            if(vct[i][idx]!=-1)
                out[vct[i][idx]]++;
        }
    }
    int count_classification(vector<int> &examples_output, int &idx)
    {
        int size = output_values.size();
        int sum = 0;
        for (int i = 0; i < size; i++)
        {
            if (examples_output[i] == 0)
            {
                sum++;
            }
            else
            {
                idx = i;
            }
        }
        if (sum == size - 1)
        {
            return 0;
        }
        return -1;
    }
    Node count_plurality(vector<int> &out)
    {
        int size = output_values.size();
        int max = out[0];
        int max_idx = 0;
        for (int i = 0; i < size; i++)
        {
            if (out[i] > max)
            {
                max = out[i];
                max_idx = i;
            }
        }
        string key = find_key_by_value(output_values, max_idx);
        return Node(make_pair(key, max_idx));
    }
};

string trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" .\t");
    if (first == std::string::npos)
    {
        return "";
    }
    size_t last = str.find_last_not_of(" .\t");
    return str.substr(first, last - first + 1);
}
int main()
{
    DecisionTree dataset;
    vector<vector<int>> examples;
    vector<vector<int>> parent_examples;
    // Define the ratio for training and testing
    double trainingRatio = 0.8;                // 80% training
    double testingRatio = 1.0 - trainingRatio; // 20% testing
    // Read the data from the file
    ifstream inputFile("car evaluation dataset/car.c45-names");
    if (!inputFile.is_open())
    {
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    string line;
    while (getline(inputFile, line))
    {
        if (line == "--class values")
        {
            getline(inputFile, line);
            getline(inputFile, line);
            istringstream classValuesStream(line);
            string classValue;
            int i = 0;
            while (getline(classValuesStream, classValue, ','))
            {
                classValue = trim(classValue);
                dataset.output_values[classValue] = i;
                i++;
            }
        }
        else if (line == "--attributes")
        {
            string attributeName;
            int i = 0;
            getline(inputFile, line);
            while (getline(inputFile, attributeName, ':'))
            {
                dataset.attribute_map[attributeName] = i;
                getline(inputFile, line);
                istringstream attributeValuesStream(line);
                string attributeValue;
                int j = 0;
                map<string, int> innerMap;
                while (getline(attributeValuesStream, attributeValue, ','))
                {
                    attributeValue = trim(attributeValue);
                    // Create a sample inner vector and push the pair

                    innerMap[attributeValue] = j;

                    j++;
                }
                dataset.attributes.push_back(innerMap);
                i++;
            }
        }
    }
    inputFile.close();
    inputFile.open("car evaluation dataset/car.data");
    int i = 0;
    while (getline(inputFile, line))
    {
        istringstream dataStream(line);
        string data;
        // int i = 0;
        int j = 0;
        while (getline(dataStream, data, ','))
        {
            // dataset.examples.push_back(vector<int>());
            examples.push_back(vector<int>());
            data = trim(data);
            if (j < dataset.attributes.size())
            {
                auto pair = dataset.attributes[j].find(data);
                int k = pair->second;
                if (pair != dataset.attributes[j].end())
                    examples[i].push_back(pair->second);
            }
            else if (j == dataset.attributes.size())
            {
                auto pair = dataset.output_values.find(data);
                examples[i].push_back(pair->second);
            }

            j++;
        }
        parent_examples.push_back(vector<int>(examples[i].size(), -1));
        i++;
    }
    examples.resize(i);
    parent_examples.resize(i);
    int count = 0;
    double accuracy[20];
    while (count < 20)
    {
    shuffle(examples.begin(), examples.end(), g);
    // Calculate the sizes for training and testing sets
    int dataSize = examples.size();
    int trainingSize = static_cast<int>(dataSize * trainingRatio);
    int testingSize = dataSize - trainingSize;
    // Divide the dataset into training and testing sets
    vector<vector<int>> trainingSet(examples.begin(), examples.begin() + trainingSize);
    vector<vector<int>> testingSet(examples.begin() + trainingSize, examples.end());
    dataset.decision_tree_learning(trainingSet, parent_examples, 0);
    int sum = 0;
    for (int i = 0; i < testingSize; i++)
    {
        int result = dataset.test(testingSet[i]);
        if (result)
        {
            sum++;
        }
    }
    accuracy[count] = (double)sum / (double)testingSize;
    count++;
    }
    double s = 0;
    for (int i = 0; i < 20; i++)
    {
        s += accuracy[i];
    }
    double mean = s / 20;
    cout << "Average accuracy: " << mean << '\n';
    double variance = 0.0;

    // Calculate the sum of squared differences from the mean
    for (int i = 0; i < 20; i++)
    {
        variance += (accuracy[i] - mean) * (accuracy[i] - mean);
    }

    // Calculate the variance and then the standard deviation
    variance /= 20;
    double standardDeviation = sqrt(variance);
    cout << "Standard deviation of accuracy: " << variance << '\n';
    return 0;
}