#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

const int maxsize = 16;

const bool displaymintermscount = false;

const bool displayminterms = true;

unsigned int* mulres = (unsigned int*)malloc((1 << maxsize) * sizeof(unsigned int));

void optimize(unsigned int newmask, vector<int> &cureval, vector<int> &isactive, bool &hit, const unsigned int &u, const unsigned int shiftconstantmain){
	const int size = ((1 << maxsize) >> __builtin_popcount(isactive[u]));
	const int shiftconst = (cureval[u] & isactive[u]);
	for(int h = 0; h < size; ++h){
		int assembledindex = shiftconst, temph = h;
		for(int j = 0; j < maxsize; ++j){
			if(isactive[u] & (1 << j))
				temph <<= 1;
			else if(temph & (1 << j))
				assembledindex |= (1 << j);
		}
		for(int j = 0; j < maxsize; ++j){
			if(newmask & (1 << j)){
				if((mulres[(assembledindex ^ (1 << j))] & shiftconstantmain) == 0){
					newmask ^= (1 << j);
				}
			}	
		}
		if(newmask == 0)
			break;
	}
	int t = __builtin_popcount(newmask);
	if(t > 1){
		hit = true;
		isactive[u] ^= (1 << __builtin_ctz(newmask));
		optimize(newmask ^ (1 << __builtin_ctz(newmask)), cureval, isactive, hit, u, shiftconstantmain);
	}
	else if(t & 1){
		hit = true;
		isactive[u] ^= newmask;
	}
}

int main()
{
	char num[32] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7'};
    char lnum[32] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '!', '@', '#', '$', '%', '^', '&'};
    for(int i = 0; i < (1 << maxsize); ++i)
        mulres[i] = 0;
	vector<int> cureval, isactive;
	for (;;)
	{
		int temp = 0, isactivetemp = 0;
		char buf;
		for (;;){
			cin >> buf;
			if(buf == '=' or buf == '+'){
                cureval.push_back(temp);
                isactive.push_back(isactivetemp);
                for(int u = 0; u < (1 << maxsize); ++u)
                    if(((u ^ temp) & isactivetemp) == 0)
                        mulres[u] |= 1;
				break;
			}
			if(buf != 32){
                //cout << int(buf) << endl;
				if(((64 + maxsize) - buf) < maxsize and buf < 97 and ((isactivetemp >> ((64 + maxsize) - buf)) & 1) == 0){
                    isactivetemp |= (1 << ((64 + maxsize) - buf));
                    temp |= (1 << ((64 + maxsize) - buf));
				}
				else if(((96 + maxsize) - buf) < maxsize and ((isactivetemp >> ((96 + maxsize) - buf)) & 1) == 0){
					isactivetemp |= (1 << ((96 + maxsize) - buf));
				}
			}
		}
		if(buf == '=')
			break;
	}
    for (;;)
    {
        bool hit = false;
        for (unsigned int u = 0; u < cureval.size(); ++u)
        {
            for (unsigned int j = u + 1; j < cureval.size(); ++j)
            {
                if(isactive[u] == isactive[j]){
                    const unsigned int check = ((cureval[u] ^ cureval[j]) & isactive[u]);
                    if(__builtin_popcount(check) == 1){
                        hit = true;
                        isactive.erase(isactive.begin() + j);
                        cureval.erase(cureval.begin() + j);
                        --j;
                        isactive[u] &= ~check;
                        break;
                    }
                    if(check == 0){
                        hit = true;
                        isactive.erase(isactive.begin() + j);
                        cureval.erase(cureval.begin() + j);
                        --j;
                    }
                }
                else
                {
                    const unsigned int maskand = (isactive[j] & isactive[u]);
                    const unsigned int check = ((cureval[u] ^ cureval[j]) & maskand);
                    if(check == 0){
                        const unsigned int tempfir = (isactive[u] ^ maskand);
                        const unsigned int tempsec = (isactive[j] ^ maskand);
                        if(tempfir and tempsec == 0){
                            hit = true;
                            isactive.erase(isactive.begin() + u);
                            cureval.erase(cureval.begin() + u);
                            --u;
                            break;
                        }
                        else if(tempfir == 0 and tempsec){
                            hit = true;
                            isactive.erase(isactive.begin() + j);
                            cureval.erase(cureval.begin() + j);
                            --j;
                        }
                    }
                    else if(__builtin_popcount(check) == 1){
                        const unsigned int tempfir = (isactive[u] ^ maskand);
                        const unsigned int tempsec = (isactive[j] ^ maskand);
                        if(tempfir and tempsec == 0){
                            hit = true;
                            isactive[u] &= ~check;
                        }
                        else if(tempfir == 0 and tempsec){
                            hit = true;
                            isactive[j] &= ~check;
                        }
                    }
                }
            }
        }
        if(hit == false){
            for(unsigned int u = 0; u < cureval.size(); ++u)
                optimize(isactive[u], cureval, isactive, hit, u, 1);
            if(hit == false)
                break;
        }
    }
    unsigned int* tempevalarr = (unsigned int*)malloc((1 << (maxsize)) * sizeof(unsigned int));
    memset(tempevalarr, 0, (1 << (maxsize)) * sizeof(unsigned int));
    for(unsigned int h = 0; h < cureval.size(); ++h){
        const unsigned int mask = isactive[h];
        const int size = ((1 << (maxsize)) >> __builtin_popcount(mask));
        const unsigned int shiftconst = (cureval[h] & mask);
        for(int h = 0; h < size; ++h){
            int assembledindex = shiftconst, temph = h;
            for(int j = 0; j < maxsize; ++j){
                if(mask & (1 << j))
                    temph <<= 1;
                else if(temph & (1 << j))
                    assembledindex |= (1 << j);
            }
            ++tempevalarr[assembledindex];
        }
    }
    for(unsigned int j = 0; j < cureval.size(); ++j){
        bool isredundant = true;
        const unsigned int mask = isactive[j];
        const int size = ((1 << (maxsize)) >> __builtin_popcount(mask));
        const unsigned int shiftconst = (cureval[j] & mask);
        for(int h = 0; h < size; ++h){
            int assembledindex = shiftconst, temph = h;
            for(int j = 0; j < maxsize; ++j){
                if(mask & (1 << j))
                    temph <<= 1;
                else if(temph & (1 << j))
                    assembledindex |= (1 << j);
            }
            if(tempevalarr[assembledindex] == 1){
                isredundant = false;
                break;
            }
        }
        if(isredundant){
            for(int h = 0; h < size; ++h){
                int assembledindex = shiftconst, temph = h;
                for(int j = 0; j < maxsize; ++j){
                    if(mask & (1 << j))
                        temph <<= 1;
                    else if(temph & (1 << j))
                        assembledindex |= (1 << j);
                }
                --tempevalarr[assembledindex];
            }
            isactive.erase(isactive.begin() + j);
            cureval.erase(cureval.begin() + j);
            --j;
        }
    }
    delete tempevalarr;
    if(displaymintermscount)
        cout << cureval.size();
    if(displayminterms){
        bool isfirst = false;
        for(unsigned int u = 0; u < cureval.size(); ++u)
        {
            if(isfirst)
                cout << " + ";
            else
                isfirst = true;
            for(unsigned int j = 0; j < (maxsize); j++)
                if(isactive[u] & (1 << (maxsize - 1 - j)))
                {
                    if(cureval[u] & (1 << (maxsize - 1 - j)))
                        cout << "\033[1;32m" << num[j] << "\033[1;0m";
                    else
                        cout << "\033[1;31m" << lnum[j] << "\033[1;0m";
                }
        }
    }
    cout << endl;
}
