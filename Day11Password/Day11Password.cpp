#include <iostream>

using namespace std;
    
string IncPassword(string p, size_t i)
{
    p[i]++;
    if (p[i] > 'z')
    {
        p[i] = 'a';
        p = IncPassword(p, i-1);
    }
    
    return p;
}

bool Rule1(const string& p)
{
    for (size_t i = 0; i < p.size() - 2; ++i)
    {
        if (p[i]+1 == p[i+1] && p[i]+2 == p[i+2])
        {
            return true;
        }
    }
    return false;
}

bool Rule2(const string& p)
{
    char first = 0;
    
    for (size_t i = 0; i < p.size() - 1; ++i)
    {
        if (p[i] == p[i+1])
        {
            if (first == 0)
            {
                first = p[i];
            }
            else if (first != p[i])
            {
                return true;
            }
        }
    }
    
    return false;
}

bool Rule3(const string& p)
{
    const string illegal = "iol";
    return p.find_first_of(illegal) == string::npos;
}

bool IsValid(const string& p)
{
    return Rule1(p) && Rule2(p) && Rule3(p);
}

string FindNextPassword(string p)
{
    do
    {
        p = IncPassword(p, p.size()-1);
    } while(!IsValid(p));
    
    return p;
}

int main()
{
    const string kPassword = "vzbxkghb";
    
    string p1 = FindNextPassword(kPassword);
    string p2 = FindNextPassword(p1);
    
    cout << p1 << endl;
    cout << p2 << endl;
    return 0;
}
