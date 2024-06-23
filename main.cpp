#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <bitset>
#include <cstring>

using namespace std;
using namespace chrono;

const bool checkfunction = true;

const bool displayfunction = false;

const bool displaymintermscount = true;

const bool displayminterms = false;

const unsigned int bits = 8;

unsigned int* mulres = (unsigned int*)malloc((1 << (bits * 2)) * sizeof(unsigned int));

void optimize(unsigned int newmask, unsigned int &cureval, unsigned int &isactive, bool &hit, const unsigned int shiftconstantmain){
	const int size = ((1 << (bits * 2)) >> __builtin_popcount(isactive));
	const int shiftconst = (cureval & isactive);
	for(int h = 0; h < size; ++h){
		int assembledindex = shiftconst, temph = h;
		for(int j = 0; j < bits * 2; ++j){
			if(isactive & (1 << j))
				temph <<= 1;
			else if(temph & (1 << j))
				assembledindex |= (1 << j);
		}
		for(int j = 0; j < bits * 2; ++j){
			if(newmask & (1 << j)){
				if((mulres[(assembledindex ^ (1 << j))] & shiftconstantmain) == 0){
					newmask ^= (1 << j);
				}
			}	
		}
		if(newmask == 0)
			return;
	}
	int t = __builtin_popcount(newmask);
	if(t > 1){
		hit = true;
		isactive ^= (1 << __builtin_ctz(newmask));
		optimize(newmask ^ (1 << __builtin_ctz(newmask)), cureval, isactive, hit, shiftconstantmain);
	}
	else if(t & 1){
		hit = true;
		isactive ^= newmask;
	}
}

void deleteElement(unsigned int*& array, unsigned int &size, unsigned int &indexToDelete) {
    copy(array + indexToDelete + 1, array + size, array + indexToDelete);
}

int main()
{
	//124816
	char num[32] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7'};
    char lnum[32] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '!', '@', '#', '$', '%', '^', '&'};
	for (unsigned int i = 0; i < (1 << (bits * 2)); ++i){
		if(displayfunction){
			for(int u = bits * 2 - 1; u > -1; --u)
				if((i >> u) & 1)
					cout << "\033[1;32m1 \033[1;0m";
				else
					cout << "\033[1;31m0 \033[1;0m";
			cout << " | ";
		}
		//Your function: (x-bit multiplicator as an example)
		mulres[i] = (i >> bits) * (i & ((1 << bits) - 1));
		if(displayfunction){
			for(int u = bits * 2 - 1; u > -1; --u)
				if((mulres[i] >> u) & 1)
					cout << "\033[1;32m1 \033[1;0m";
				else
					cout << "\033[1;31m0 \033[1;0m";
			cout << endl;
		}
	}
    auto start = high_resolution_clock::now();
	for (unsigned int i = 0; i < (bits * 2); ++i)
	{
		unsigned int toallocate = 0;
		cout << "O" << i + 1 << " = ";
		for (unsigned int u = 0; u < (1 << (bits * 2)); ++u)
			if (mulres[u] & (1 << i))
				++toallocate;
		unsigned int* cureval = (unsigned int*)malloc(toallocate * sizeof(unsigned int));
		unsigned int* isactive = (unsigned int*)malloc(toallocate * sizeof(unsigned int));
		toallocate = 0;
		for (unsigned int u = 0; u < (1 << (bits * 2)); ++u)
			if (mulres[u] & (1 << i))
			{
				cureval[toallocate] = u;
				isactive[toallocate] = (1 << (bits * 2)) - 1;
				++toallocate;
			}
		for (;;)
		{
			bool hit = false;
			for (unsigned int u = 0; u < toallocate; ++u)
			{
				for (unsigned int j = u + 1; j < toallocate; ++j)
				{
					if(isactive[u] == isactive[j]){
						const unsigned int check = ((cureval[u] ^ cureval[j]) & isactive[u]);
						if(__builtin_popcount(check) == 1){
							hit = true;
							deleteElement(isactive, toallocate, j);
							deleteElement(cureval, toallocate, j);
							--toallocate;
							--j;
							isactive[u] &= ~check;
							break;
						}
						if(check == 0){
							hit = true;
							deleteElement(isactive, toallocate, j);
							deleteElement(cureval, toallocate, j);
							--toallocate;
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
								deleteElement(isactive, toallocate, u);
								deleteElement(cureval, toallocate, u);
								--toallocate;
								--u;
								break;
							}
							else if(tempfir == 0 and tempsec){
								hit = true;
								deleteElement(isactive, toallocate, j);
								deleteElement(cureval, toallocate, j);
								--toallocate;
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
				for(unsigned int u = 0; u < toallocate; ++u)
					optimize(isactive[u], cureval[u], isactive[u], hit, (1 << i));
				if(hit == false)
					break;
				for (unsigned int u = 0; u < toallocate; ++u)
				{
					for (unsigned int j = u + 1; j < toallocate; ++j)
					{
						if(isactive[u] == isactive[j] and ((cureval[u] ^ cureval[j]) & isactive[u]) == 0){
							deleteElement(isactive, toallocate, j);
							deleteElement(cureval, toallocate, j);
							--toallocate;
							--j;
						}
					}
				}
				break;
			}
		}
		unsigned int* tempevalarr = (unsigned int*)malloc((1 << (bits * 2)) * sizeof(unsigned int));
		memset(tempevalarr, 0, (1 << (bits * 2)) * sizeof(unsigned int));
		for(unsigned int h = 0; h < toallocate; ++h){
			const unsigned int mask = isactive[h];
			const int size = ((1 << (bits * 2)) >> __builtin_popcount(mask));
			const unsigned int shiftconst = (cureval[h] & mask);
			for(int h = 0; h < size; ++h){
				int assembledindex = shiftconst, temph = h;
				for(int j = 0; j < bits * 2; ++j){
					if(mask & (1 << j))
						temph <<= 1;
					else if(temph & (1 << j))
						assembledindex |= (1 << j);
				}
				++tempevalarr[assembledindex];
			}
		}
		for(unsigned int j = 0; j < toallocate; ++j){
			bool isredundant = true;
			const unsigned int mask = isactive[j];
			const int size = ((1 << (bits * 2)) >> __builtin_popcount(mask));
			const unsigned int shiftconst = (cureval[j] & mask);
			for(int h = 0; h < size; ++h){
				int assembledindex = shiftconst, temph = h;
				for(int j = 0; j < bits * 2; ++j){
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
					for(int j = 0; j < bits * 2; ++j){
						if(mask & (1 << j))
							temph <<= 1;
						else if(temph & (1 << j))
							assembledindex |= (1 << j);
					}
					--tempevalarr[assembledindex];
				}
				deleteElement(isactive, toallocate, j);
				deleteElement(cureval, toallocate, j);
				--toallocate;
				--j;
			}
		}
		delete tempevalarr;
		if(displaymintermscount)
			cout << toallocate;
		if(displayminterms){
			bool isfirst = false;
			for(unsigned int u = 0; u < toallocate; ++u)
			{
				if(isfirst)
					cout << " + ";
				else
					isfirst = true;
				for(unsigned int j = 0; j < (bits * 2); j++)
					if(isactive[u] & (1 << (bits * 2 - 1 - j)))
					{
						if(cureval[u] & (1 << (bits * 2 - 1 - j)))
							cout << "\033[1;32m" << num[j] << "\033[1;0m";
						else
							cout << "\033[1;31m" << lnum[j] << "\033[1;0m";
					}
			}
		}
		if(checkfunction){
			unsigned int* temparr = (unsigned int*)malloc((1 << (bits * 2)) * sizeof(unsigned int));
			for(int u = 0; u < (1 << (bits * 2)); ++u)
				temparr[u] = 0;
			for(unsigned int j = 0; j < toallocate; ++j)
				for(unsigned int u = 0; u < (1 << (bits * 2)); ++u)
					if(((u ^ cureval[j]) & isactive[j]) == 0)
						temparr[u] |= (1 << i);
			for(int u = 0; u < (1 << (bits * 2)); ++u)
				mulres[u] ^= temparr[u];
			for(unsigned int u = 0; u < (1 << (bits * 2)); ++u)
				if(mulres[u] & (1 << i)){
					cout << "Error" << endl;
				}
			delete temparr;
		}
		cout << endl;
		delete cureval;
		delete isactive;
	}
	auto stop = high_resolution_clock::now();
	auto duration1 = duration_cast<milliseconds>(stop - start);
	cout << "Duration in milliseconds: " << duration1.count() << endl;
}
