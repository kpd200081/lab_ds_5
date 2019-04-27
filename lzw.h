#ifndef LZW_H
#define LZW_H
#include <map>
#include <sstream>
#include <string>
#include <queue>
#include <fstream>
#include <cmath>
#include <md5.h>
#include <hashtable.h>
using namespace std;

#define TABLE_SIZE 128

size_t str_len(string str){
    return str.length();
}

class LZW{
public:
    enum Hashes{STD,MD5,Length};
    string in_filename;                     //name of input file
    string out_filename;                    //name of output file
    string lzwPrefix;
    std::function<size_t(std::string)> hash;



    LZW(string& in_file,string& out_file, string prefix="LZ-11", Hashes _hash=STD):in_filename(in_file), out_filename(out_file), lzwPrefix(prefix){
        switch (_hash) {
        case Hashes::STD:
            hash=std::hash<std::string>();
            break;
        case Hashes::MD5:
            hash=md5_f;
            break;
        case Hashes::Length:
            hash=str_len;
        }
    }

    std::ifstream::pos_type filesize(const char* filename)
    {
        std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        return in.tellg();
    }

    void compress(){
        ofstream out(out_filename);
        string in;
        ifstream inf(in_filename, ifstream::binary);
        char c=0;
        while (inf.good()) {
            inf.read(&c,1);
            in+=c;
        }
        inf.close();
        out<<lzwPrefix<<endl;
        out.close();
        short n=8;
        string coded;
        HashTable table(TABLE_SIZE,hash);
        for(int i=-128;i<128;i++){                          //ASCII-8
            table.add(string(1,char(i)));
        }
        string x(1,in[0]);
        for(size_t i=1;i<in.length();i++){
            string symbol(1,in[i]);
            if(table.get_code(x+symbol)!=0){
                x=x+symbol;
            }else{
                size_t code=table.get_code(x);
                for(int j=n-1;j>=0;j--){
                    coded+=((code>>j)&1)+48;
                }
                code=table.add(x+symbol);
                for(short j=n;j<=16*8;j++){
                    if(code+1<pow(2,j)&&code>=pow(2,j-1)){
                        n=j;
                        break;
                    }
                }
                x=symbol;
            }
        }
        size_t code=table.get_code(x);
        for(int j=n-1;j>=0;j--){
            coded+=((code>>j)&1)+48;
        }
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
        cout<<"Compress ratio(LZW): "<<(double)((double)filesize(in_filename.c_str())/(double)filesize(out_filename.c_str()))<<"\t"<<in_filename<<"/"<<out_filename<<endl;
    }

    size_t read(unsigned short n, string& s){
        size_t res=0;
        for(short i=n-1;i>=0;i--){
            res|=(s[0]-48)<<i;
            s.erase(0,1);
        }
        return res;
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
        if(fprefix!=lzwPrefix){
            cerr<<"Incorrect prefix\n";
            exit(-1);
        }
        unsigned short n=8;
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
        HashTable table(TABLE_SIZE,hash);
        for(int i=-128;i<128;i++){                          //ASCII-8
            table.add(string(1,char(i)));
        }
        size_t old=read(n,coded);
        string symbol=table.get_by_code(old);
        uncoded+=symbol;
        n=9;
        while(coded.length()>=n){
            size_t _new=read(n,coded);
            string x;
            if(table.get_by_code(_new)==""){
                x=table.get_by_code(old);
                x=x+symbol;
            }else{
                x=table.get_by_code(_new);
            }
            uncoded+=x;
            symbol=x[0];
            size_t code=table.add(table.get_by_code(old)+symbol);
            for(unsigned short j=n;j<=16*8;j++){
                if(code+1<pow(2,j)&&code+1>=pow(2,j-1)){
                    n=j;
                    break;
                }
            }
            old=_new;
        }
        ofstream out(out_filename,ofstream::binary);
        for(char c:uncoded){
            out.write(&c,1);
        }
        out.close();
    }
};

#endif // LZW_H
