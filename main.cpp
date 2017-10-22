//
//  main.cpp
//  INFDecision_Tree
//
//  Created by Mujie Zhang on 9/10/17.
//  Copyright © 2017 Mujie.Zhang. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <map>
#include <string>

using namespace std;

vector<vector<string>> attBoard;
vector<string>attIndex;
map<string, vector<string>> attValue;
#define Len 7 ///the attribute length
class Node{
public:
    string value;///the value the node has arrived for the last attribute
    string attribute; /// the new attribute for classification
    vector<Node*> children;
};

void ReadFile();
Node* FindnextLevel(Node* ,vector<string>,vector<vector<string>>);
void printIDTree(Node*,int );
void PredictResult(Node*, vector<string>);
void ConstructTestData(vector<string>&);

int main(int argc, const char * argv[]) {
    ReadFile();
    vector<string>left_attribute(attIndex.begin(),attIndex.end()-1); ///total 6 attributes without enjoy
    ////the key is at every node recompute left_attribute and left_data: which left_attribute and left_data is the left attribute and data we should continue classify under the path from root to this node;
    vector<vector<string>>left_data(attBoard);
    Node* root= NULL;
    root=FindnextLevel(root,left_attribute,left_data);
    printIDTree(root,0);
    vector<string> testData(6);
    ConstructTestData(testData);
    PredictResult(root,testData);
    return 0;
}

void ConstructTestData(vector<string>&testData)
{
    testData[0]="Moderate";
    testData[1]="Cheap";
    testData[2]="Loud";
    testData[3]="City-Center";
    testData[4]="No";
    testData[5]="No";
}

void ReadFile()
{
    ///prepare job, fill of attBoard, attValue, attIndex
    ////first attIndex
    attIndex.push_back("Occupied");
    attIndex.push_back("Price");
    attIndex.push_back("Music");
    attIndex.push_back("Location");
    attIndex.push_back("VIP");
    attIndex.push_back("Favorite Beer");
    attIndex.push_back("Enjoy");
    ////read file to fill attBoard and attValue
    ifstream in("dt-data.txt");
    if(!in.is_open())
    { cout<< "can't open the data file"; exit(1);}
    string buffet;
    int LineCount=0;
    while(!in.eof())
    {
        getline(in,buffet);
        if(LineCount<2)
        {
            LineCount++;
            continue;
        }
        string::size_type pre=buffet.find_first_of(' ');
        string::size_type cur;
        vector<string> oneComb;
        for(int i=0;i<Len-1;i++)
        {
            cur=buffet.find_first_of(' ',pre+1);
            string temp=buffet.substr(pre+1,cur-pre-2);
            oneComb.push_back(temp);
            pre=cur;
        }
        cur=buffet.find_first_of(';');
        string temp=buffet.substr(pre+1,cur-pre-1);
        oneComb.push_back(temp);
        attBoard.push_back(oneComb);
        LineCount++;
    }
    attBoard.pop_back();///delete the empty line at the end;
    /////attValue
    
    for(int i=0;i<attBoard[0].size();i++)
    { ////for each attribute a vector<string> is create for diff values
        vector<string> oneComb;
        for(int j=0;j<attBoard.size();j++)
        {
            int k=0;
            for(;k<oneComb.size();k++)
            {
                if(oneComb[k].compare(attBoard[j][i])==0)
                    break;
            }
            if(k==oneComb.size())
            {
                oneComb.push_back(attBoard[j][i]);
                ///cout<<attBoard[j][i];
            }
        }
        attValue[attIndex[i]]=oneComb;
    }
}

void printIDTree(Node* root,int depth)
{
    if(root==NULL)return;
    if(!root->value.empty())
    {
        for(int i=0;i<depth;i++)
        {
            cout<<'\t';
        }
        cout<<root->value<<endl;
    }
    for(int i=0;i<depth+1;i++)
    {
        cout<<'\t';
    }
    cout<<root->attribute<<endl;
    for(int i=0;i<root->children.size();i++)
    {
        printIDTree(root->children[i],depth+1);
    }
}


string MostRemainValue(vector<vector<string>> left_data)
{
    int yesCount=0;
    for(int i=0;i<left_data.size();i++)
    {
        if(left_data[i][Len-1].compare("Yes"))
            yesCount++;
    }
    if(yesCount>left_data.size()-yesCount)
        return "Yes";
    else if(yesCount==left_data.size()-yesCount)
        return "Tie";
    return "No";
}

Node*  FindnextLevel(Node* root, vector<string>left_att, vector<vector<string>>left_data)
{
    if(root==NULL)
        root=new Node();
    
    ///no left attribute
    int n=left_att.size();
    if(n==0)
    {
        root->attribute=MostRemainValue(left_data);
        return root;
    };
    
    ///for parent entropy, no left state
    int m=left_data.size();
    if(m==0)
    {
        root->attribute="Tie";
        return root;
    }
    ///for parent entropy = 0;
    int parentyes=0;
    for(int i=0;i<m;i++)
    {
        if(left_data[i][Len-1].compare("Yes")==0)
            parentyes++;
    }
    ///parent entropy =0, no more classification.
    if(parentyes==m)
    {
        root->attribute="Yes";
        return root;
    }
    else if(parentyes==0)
    {
        root->attribute="No";
        return root;
    }

    ///find the next level attribute; use Information: Gam(left_att[i])=entropy_before-entropy_after(left_att[i]). so it means to find the minimum  entropy_after(left_att[i])
    double min_entropy=INT_MAX;
    string nextnode;
    int nextnodeIndex=-1;
    for(int i=0;i<n;i++)
    {   ////for each attribute, compute its entropy
        vector<string>multiValue(attValue[left_att[i]]);
        int num=multiValue.size();
        double att_entropy=0.0; ///entropy of this attribute;
        int att_index=-1;
        for(int l=0;l<Len;l++)
        {
            if(attIndex[l].compare(left_att[i])==0)
            {
                att_index=l;
                break;
            }
        }
        for(int j=0;j<num;j++)
        {   ///for each value in each attribute
            int count=0,enjoyCount=0,unenjoyCount=0;
            for(int k=0;k<m;k++)
            {   ////compute one value's possibility
                if(left_data[k][att_index].compare(multiValue[j])==0)
                {
                    count++;
                    if(left_data[k][Len-1].compare("Yes"))
                        enjoyCount++;
                    else
                        unenjoyCount++;
                }
            }
            if(enjoyCount!=count && unenjoyCount!=count)
            { //p1 ,p2 !=0
                double p1=1.0*enjoyCount/count;
                double p2=1.0*unenjoyCount/count;
                att_entropy+= 1.0*count/m*(p1*log(1/p1)+p2*log(1/p2));
            }
            else
            {
                ///p1 || p2==0
                att_entropy+=0;
            }
        }
        if(att_entropy<min_entropy)
        {
            min_entropy=att_entropy;
            nextnode=left_att[i];
            nextnodeIndex=att_index;
        }
    }
     /////find the nextNode, build it;
    root->attribute=nextnode;
    vector<string > rootvalue=attValue[nextnode];
    for(int i=0;i<rootvalue.size();i++)
    {
        Node* valueNode= new Node();
        valueNode->value=rootvalue[i];
        root->children.push_back(valueNode);
        ///// recursive compute the nextLevel classification
        ///// update the left_attribute, left_state
        ///update left_attribute
        vector<string >next_att;
        for(int j=0;j<n;j++)
        {
            if(left_att[j].compare(nextnode))
                next_att.push_back(left_att[j]);
        }
        ///update left_state
        vector<vector<string> >next_state;
        for(int j=0;j<m;j++)
        {
            if(left_data[j][nextnodeIndex].compare(rootvalue[i])==0)
                next_state.push_back(left_data[j]);
        }
        ////if there is no next_state anymore for this value, return Yes or No;
        if(next_state.size()==0)
            valueNode->attribute=MostRemainValue(next_state);
        else ///recursive compute the nextLevel
            FindnextLevel(valueNode,next_att,next_state);
    }
    return root;
}

void PredictResult(Node* root, vector<string>test)
{
    if(root==NULL)return;
    int att=-1;
    for(int i=0;i<Len-1;i++)
    {
        if(attIndex[i].compare(root->attribute)==0)
        {
            att=i;
            break;
        }
    }
    if(att==-1)
    {////at leaf node, no more classification
        cout<<"the prediction result is "<<root->attribute<<endl;
        return;
    }
    for(int i=0;i<root->children.size();i++)
    {
        if(root->children[i]->value.compare(test[att])==0)
        {
            PredictResult(root->children[i],test);
            break;
        }
    }
}
