#ifndef HUFF_H
#define HUFF_H
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <queue>
#include <fstream>
#include <cmath>
using namespace std;

class Huffman{
public:
    string in_filename;                     //name of input file
    string out_filename;                    //name of output file
    string huffPrefix;

    Huffman(string& in_file,string& out_file, string prefix="HF-11"):in_filename(in_file), out_filename(out_file), huffPrefix(prefix){}

    struct Node {
        string value;
        string code;
        unsigned amount;
        Node * left;
        Node * right;

        bool operator() (const Node& x, const Node& y) const {
            return x.amount > y.amount;
        }

        Node (const string& value = "", unsigned amount = 0, Node * left = nullptr, Node * right = nullptr) {
            this->value = value;
            this->code = "";
            this->amount = amount;
            this->left = left;
            this->right = right;
        }

        Node * join (Node x) {
            return new Node(x.value + value, x.amount + amount, new Node(x), this);
        }

        void traversal_code(string code, map<char,string>& codes) {
            this->code = code;
            if(value.length()==1)
                codes[value[0]]=code;
            if (left != nullptr || right != nullptr) {
                left->traversal_code(code + "1",codes);
                right->traversal_code(code + "0",codes);
            }
        }

        static Node * builder(priority_queue<Node, vector<Node>, Node> graph) {
            while (graph.size() > 1) {
                Node *n = new Node(graph.top());
                graph.pop();
                graph.push(*n->join(*new Node(graph.top())));
                graph.pop();
            }
            return new Node(graph.top());
        }
    };

    std::ifstream::pos_type filesize(const char* filename)
    {
        std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        return in.tellg();
    }


    void compress(){
        map<char, size_t> statistics;
        string in;
        ifstream inf(in_filename, ifstream::binary);
        char c=0;
        while (inf.good()) {
            inf.read(&c,1);
            in+=c;
        }
        inf.close();
        for(char it:in){
            if(statistics.find(it)!=statistics.end()){
                statistics[it]++;
            }else {
                statistics[it]=1;
            }
        }
        if(statistics.size()>0){
            priority_queue<Node, vector<Node>, Node> graph;
            for(auto it: statistics){
                graph.emplace(string(1,it.first), it.second);
            }
            Node *n = Node::builder(graph);
            map<char, string> codes;
            n->traversal_code("",codes);
            string coded;
            size_t symbols_count=0;
            for(char it:in){
                coded+=codes[it];
                symbols_count++;
            }
            ofstream out(out_filename);
            out<<huffPrefix<<" "<<symbols_count-1<<" "<<statistics.size()<<" ";
            for(auto it: statistics){
                out<<(int)(it.first)<<":"<<it.second<<" ";
            }
            out<<endl;
            out.close();
            out.open(out_filename, ofstream::app|ofstream::binary);
            while(coded.length()%8!=0)
                coded+="0";
            for(size_t i=0; i<coded.length();i+=8){
                char c=0;
                for(size_t j=0;j<8*4;j++)
                    c=c|(coded[i+j]-48)<<(7-j);
                out.write(&c,1);
            }
            out.close();
        }else{
            ofstream out(out_filename);
            out<<huffPrefix<<" 0 0 ";
            out.close();
        }
        cout<<"Compress ratio(Huff): "<<(double)((double)filesize(in_filename.c_str())/(double)filesize(out_filename.c_str()))<<"\t"<<in_filename<<"/"<<out_filename<<endl;

    }

    void decompress(){
        ifstream in(in_filename);
        string first_str;
        getline(in,first_str);
        in.close();
        size_t start_pos, end_pos;
        start_pos=0;
        end_pos=first_str.find(" ",start_pos);
        string fprefix=first_str.substr(start_pos, end_pos-start_pos);
        if(fprefix!=huffPrefix){
            cerr<<"Incorrect prefix\n";
            exit(-1);
        }
        start_pos=end_pos+1;
        end_pos=first_str.find(" ",start_pos);
        string ssym_c=first_str.substr(start_pos, end_pos-start_pos);
        size_t symbols_count=stoi(ssym_c);
        start_pos=end_pos+1;
        end_pos=first_str.find(" ",start_pos);
        string sst_s=first_str.substr(start_pos, end_pos-start_pos);
        size_t statistics_size= stoi(sst_s);
        map<char, size_t> statistics;
        for(size_t i=0;i<statistics_size;i++){
            start_pos=end_pos+1;
            end_pos=first_str.find(":",start_pos);
            string sc=first_str.substr(start_pos, end_pos-start_pos);
            char ic=stoi(sc);
            start_pos=end_pos+1;
            end_pos=first_str.find(" ",start_pos);
            string sa=first_str.substr(start_pos, end_pos-start_pos);
            size_t ia=stoi(sa);
            statistics[ic]=ia;
        }
        if(statistics.size()>0){
            priority_queue<Node, vector<Node>, Node> graph;
            for(auto it: statistics){
                graph.emplace(string(1,it.first), it.second);
            }
            Node *n = Node::builder(graph);
            map<char, string> codes;
            n->traversal_code("",codes);
            string coded;
            in.open(in_filename,wifstream::binary);
            char c=0;
            string temp;
            getline(in,temp);
            while(in.good()){
                in.read(&c,1);
                for(size_t i=8;i>=1;i--){
                    coded+=((c>>(i-1))&1)+48;
                }
            }
            coded.erase(coded.length()-8,8);
            string uncoded;
            for(size_t i=0;i<symbols_count;){
                string code;
                code+=coded[0];
                coded.erase(0,1);
                bool found=false;
                for(auto it:codes){
                    if(it.second==code){
                        uncoded+=it.first;
                        found=true;
                        i++;
                    }
                }
                while(!found){
                    code+=coded[0];
                    coded.erase(0,1);
                    for(auto it:codes){
                        if(it.second==code){
                            uncoded+=it.first;
                            found=true;
                            i++;
                        }
                    }
                }
            }
            ofstream out(out_filename,ofstream::binary);
            for(char c:uncoded){
                out.write(&c,1);
            }
            out.close();
        }else{
            ofstream out(out_filename);
            out<<endl;
            out.close();
        }
    }


};

#endif // HUFF_H
