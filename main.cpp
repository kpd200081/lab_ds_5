#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <queue>
#include <fstream>
#include <cmath>
#include <lzw.h>
#include <huff.h>
using namespace std;

//Arguments-------------------------------------------------------------------------
bool work;                              //false mean compress, true - decompress
string in_filename;                     //name of input file
string out_filename;                    //name of output file
string huffPrefix="HF-11";
string lzwPrefix="LZ-11";               //prefix for algorithm idtefication
bool useHuff=false;
bool useLZW=false;
bool isHuffFirst=false;
string tempFileName="temp.txt";

template <class charT>
struct no_separator : public std::numpunct_byname<charT> {
    explicit no_separator(const char* name, size_t refs=0)
        : std::numpunct_byname<charT>(name,refs) {}
protected:
    virtual string do_grouping() const
    { return "\000"; } // groups of 0 (disable)
    virtual wchar_t do_decimal_point() const
    { return '.'; }
};

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int main(int argc, char** argv){
    //locale::global(locale(std::locale(""), new no_separator<wchar_t>("ru_RU.UTF-8")) );
    string sargv;
    for (size_t i=1;i<argc;i++) {
        sargv+=argv[i];
        sargv+=' ';
    }
    size_t start_pos, end_pos;
    if(sargv.find("--compress")!=string::npos)
        work=false;
    else{
        if(sargv.find("--decompress")!=string::npos)
            work=true;
        else {
            cerr<<"Incorrect arguments\n";
            return 0;
        }
    }
    if(sargv.find("--lzw")!=string::npos)
        useLZW=true;
    if(sargv.find("--huff")!=string::npos)
        useHuff=true;
    if((start_pos=sargv.find("--input_filename="))!=string::npos){
        if((end_pos=sargv.find(" ", start_pos))!=string::npos){
            in_filename=sargv.substr(start_pos+17, end_pos-start_pos-17);
        }else
            in_filename=sargv.substr(start_pos+17, sargv.length()-start_pos-17);
    }else {
        cerr<<"Incorrect arguments\n";
        return 0;
    }
    if((start_pos=sargv.find("--output_filename="))!=string::npos){
        if((end_pos=sargv.find(" ", start_pos))!=string::npos){
            out_filename=sargv.substr(start_pos+18, end_pos-start_pos-18);
        }else
            out_filename=sargv.substr(start_pos+18, sargv.length()-start_pos-18);
    }else {
        cerr<<"Incorrect arguments\n";
        return 0;
    }
    if(!(useLZW||useHuff)){
        cerr<<"Incorrect arguments\n";
        return 0;
    }
    //End of parsing--------------------------------------------------------------------

    if(useLZW&&useHuff){
        if(sargv.find("--lzw")>sargv.find("--huff")){
            isHuffFirst=true;
            if((start_pos=sargv.find("--prefix_phrase_1="))!=string::npos){
                if((end_pos=sargv.find(" ", start_pos))!=string::npos){
                    huffPrefix=sargv.substr(start_pos+16, end_pos-start_pos-16);
                }else {
                    huffPrefix=sargv.substr(start_pos+16, sargv.length()-start_pos-16);
                }
            }
            if((start_pos=sargv.find("--prefix_phrase_2="))!=string::npos){
                if((end_pos=sargv.find(" ", start_pos))!=string::npos){
                    lzwPrefix=sargv.substr(start_pos+16, end_pos-start_pos-16);
                }else {
                    lzwPrefix=sargv.substr(start_pos+16, sargv.length()-start_pos-16);
                }
            }
        }else{
            if((start_pos=sargv.find("--prefix_phrase_2="))!=string::npos){
                if((end_pos=sargv.find(" ", start_pos))!=string::npos){
                    huffPrefix=sargv.substr(start_pos+16, end_pos-start_pos-16);
                }else {
                    huffPrefix=sargv.substr(start_pos+16, sargv.length()-start_pos-16);
                }
            }
            if((start_pos=sargv.find("--prefix_phrase_1="))!=string::npos){
                if((end_pos=sargv.find(" ", start_pos))!=string::npos){
                    lzwPrefix=sargv.substr(start_pos+16, end_pos-start_pos-16);
                }else {
                    lzwPrefix=sargv.substr(start_pos+16, sargv.length()-start_pos-16);
                }
            }
        }
    }
    if(useHuff&&useLZW){
        string t_in=in_filename;
        string t_out=out_filename;
        out_filename=tempFileName;
        if(!work){
            if(isHuffFirst){
                Huffman huff(in_filename,out_filename,huffPrefix);
                huff.compress();
                out_filename=t_out;
                in_filename=tempFileName;
                LZW lzw(in_filename, out_filename, lzwPrefix, LZW::Hashes::STD);
                lzw.compress();
                cout<<"Final compress ratio: "<<(double)((double)filesize(t_in.c_str())/(double)filesize(out_filename.c_str()))<<"\t"<<t_in<<"/"<<out_filename<<endl;
            }else{
                LZW lzw(in_filename, out_filename, lzwPrefix, LZW::Hashes::STD);
                lzw.compress();
                out_filename=t_out;
                in_filename=tempFileName;
                Huffman huff(in_filename,out_filename,huffPrefix);
                huff.compress();
                cout<<"Final compress ratio: "<<(double)((double)filesize(t_in.c_str())/(double)filesize(out_filename.c_str()))<<"\t"<<t_in<<"/"<<out_filename<<endl;
            }
        }else{
            if(isHuffFirst){
                Huffman huff(in_filename,out_filename,huffPrefix);
                huff.decompress();
                out_filename=t_out;
                in_filename=tempFileName;
                LZW lzw(in_filename, out_filename, lzwPrefix, LZW::Hashes::STD);
                lzw.decompress();
            }else{
                LZW lzw(in_filename, out_filename, lzwPrefix, LZW::Hashes::STD);
                lzw.decompress();
                out_filename=t_out;
                in_filename=tempFileName;
                Huffman huff(in_filename,out_filename,huffPrefix);
                huff.decompress();
            }
        }
        return 0;
    }
    if(useHuff){
        Huffman huff(in_filename,out_filename,huffPrefix);
        if(!work)
            huff.compress();
        else
            huff.decompress();
    }
    if(useLZW){
        LZW lzw(in_filename, out_filename, lzwPrefix, LZW::Hashes::STD);
        if(!work)
            lzw.compress();
        else
            lzw.decompress();
    }
    return 0;
}
