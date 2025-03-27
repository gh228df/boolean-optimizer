#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <bitset>
#include <cstring>

using namespace std;
using namespace chrono;

#define CHECK_FUNCTION true

#define DISPLAY_FUNCTION false

#define DISPLAY_MINTERM_COUNT true

#define DISPLAY_MINTERMS false

#define BITS 8

const char num[32] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7'};
const char lnum[32] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '!', '@', '#', '$', '%', '^', '&'};

unsigned int *mulres = (unsigned int *)malloc((1 << (BITS * 2)) * sizeof(unsigned int));

void optimize(unsigned int newmask, unsigned int &function_term, unsigned int &function_mask, const unsigned int shiftconstantmain)
{
	if (newmask == 0)
		return;
	const int size = ((1 << (BITS * 2)) >> __builtin_popcount(function_mask));
	const int shiftconst = (function_term & function_mask);
	int current_number = shiftconst;
	for (int i = 0; i < size; ++i)
	{
		int assembledindex = current_number;
		for (int j = 0; j < BITS * 2; ++j)
		{
			if (newmask & (1 << j))
			{
				if ((mulres[(assembledindex ^ (1 << j))] & shiftconstantmain) == 0)
				{
					newmask ^= (1 << j);
					if (newmask == 0)
						return;
				}
			}
		}
		current_number = (((current_number | function_mask) + 1) & ~function_mask) | shiftconst;
	}
	function_mask ^= (1 << __builtin_ctz(newmask));
	optimize(newmask ^ (1 << __builtin_ctz(newmask)), function_term, function_mask, shiftconstantmain);
}

void mark_zero(const unsigned int mask, const unsigned int function_term, unsigned int *tempevalarr)
{
	const int size = ((1 << (BITS * 2)) >> __builtin_popcount(mask));
	const unsigned int shiftconst = (function_term & mask);
	int current_number = shiftconst;
	for (int j = 0; j < size; ++j)
	{
		tempevalarr[current_number] = 0;
		current_number = (((current_number | mask) + 1) & ~mask) | shiftconst;
	}
}

void print_minterm(const unsigned int minterm_term, const unsigned int minterm_mask)
{
	for (unsigned int j = 0; j < (BITS * 2); j++)
		if (minterm_mask & (1 << (BITS * 2 - 1 - j)))
		{
			if (minterm_term & (1 << (BITS * 2 - 1 - j)))
				cout << "\033[1;32m" << num[j] << "\033[1;0m";
			else
				cout << "\033[1;31m" << lnum[j] << "\033[1;0m";
		}
}

int main()
{
	for (unsigned int i = 0; i < (1 << (BITS * 2)); ++i)
	{
		if (DISPLAY_FUNCTION)
		{
			for (int u = BITS * 2 - 1; u >= 0; --u)
				if ((i >> u) & 1)
					cout << "\033[1;32m1 \033[1;0m";
				else
					cout << "\033[1;31m0 \033[1;0m";
			cout << " | ";
		}
		// Your function: (x-bit multiplication as an example)
		mulres[i] = (i >> BITS) * (i & ((1 << BITS) - 1));
		if (DISPLAY_FUNCTION)
		{
			for (int u = BITS * 2 - 1; u >= 0; --u)
				if ((mulres[i] >> u) & 1)
					cout << "\033[1;32m1 \033[1;0m";
				else
					cout << "\033[1;31m0 \033[1;0m";
			cout << endl;
		}
	}
	auto start = high_resolution_clock::now();
	for (unsigned int i = 0; i < (BITS * 2); ++i)
	{
		unsigned int total_elements = 0;
		cout << "O" << i + 1 << " = ";
		for (unsigned int u = 0; u < (1 << (BITS * 2)); ++u)
			if (mulres[u] & (1 << i))
				++total_elements;
		unsigned int *function_term = (unsigned int *)malloc(total_elements * sizeof(unsigned int));
		unsigned int *function_mask = (unsigned int *)malloc(total_elements * sizeof(unsigned int));
		total_elements = 0;
		for (unsigned int u = 0; u < (1 << (BITS * 2)); ++u)
			if (mulres[u] & (1 << i))
			{
				function_term[total_elements] = u;
				function_mask[total_elements] = (1 << (BITS * 2)) - 1;
				++total_elements;
			}
		bool hit = true;
		while (hit)
		{
			hit = false;
			for (unsigned int u = 0; u < total_elements; ++u)
			{
				for (unsigned int j = u + 1; j < total_elements; ++j)
				{
					if (function_mask[u] == function_mask[j])
					{
						const unsigned int check = ((function_term[u] ^ function_term[j]) & function_mask[u]);
						if (__builtin_popcount(check) == 1)
						{
							hit = true;
							memmove(function_mask + j, function_mask + j + 1, (total_elements - j - 1) * sizeof(unsigned int));
							memmove(function_term + j, function_term + j + 1, (total_elements - j - 1) * sizeof(unsigned int));
							--total_elements;
							--j;
							function_mask[u] ^= check;
						}
						if (check == 0)
						{
							hit = true;
							memmove(function_mask + j, function_mask + j + 1, (total_elements - j - 1) * sizeof(unsigned int));
							memmove(function_term + j, function_term + j + 1, (total_elements - j - 1) * sizeof(unsigned int));
							--total_elements;
							--j;
						}
					}
					else
					{
						const unsigned int maskand = (function_mask[j] & function_mask[u]);
						const unsigned int check = ((function_term[u] ^ function_term[j]) & maskand);
						if (check == 0)
						{
							const unsigned int tempfir = (function_mask[u] ^ maskand);
							const unsigned int tempsec = (function_mask[j] ^ maskand);
							if (tempfir and tempsec == 0)
							{
								hit = true;
								memmove(function_mask + u, function_mask + u + 1, (total_elements - u - 1) * sizeof(unsigned int));
								memmove(function_term + u, function_term + u + 1, (total_elements - u - 1) * sizeof(unsigned int));
								--total_elements;
								--u;
								break;
							}
							else if (tempfir == 0 and tempsec)
							{
								hit = true;
								memmove(function_mask + j, function_mask + j + 1, (total_elements - j - 1) * sizeof(unsigned int));
								memmove(function_term + j, function_term + j + 1, (total_elements - j - 1) * sizeof(unsigned int));
								--total_elements;
								--j;
							}
						}
						else if (__builtin_popcount(check) == 1)
						{
							const unsigned int tempfir = (function_mask[u] ^ maskand);
							const unsigned int tempsec = (function_mask[j] ^ maskand);
							if (tempfir and tempsec == 0)
							{
								hit = true;
								function_mask[u] ^= check;
							}
							else if (tempfir == 0 and tempsec)
							{
								hit = true;
								function_mask[j] ^= check;
							}
						}
					}
				}
			}
		}
		for (unsigned int u = 0; u < total_elements; ++u)
			optimize(function_mask[u], function_term[u], function_mask[u], (1 << i));
		for (unsigned int u = 0; u < total_elements; ++u)
		{
			for (unsigned int j = u + 1; j < total_elements; ++j)
			{
				if (function_mask[u] == function_mask[j] and ((function_term[u] ^ function_term[j]) & function_mask[u]) == 0)
				{
					memmove(function_mask + j, function_mask + j + 1, (total_elements - j - 1) * sizeof(unsigned int));
					memmove(function_term + j, function_term + j + 1, (total_elements - j - 1) * sizeof(unsigned int));
					--total_elements;
					--j;
				}
			}
		}
		unsigned int *tempevalarr = (unsigned int *)calloc((1 << (BITS * 2)), sizeof(unsigned int));
		bool *is_unique = (bool *)malloc(total_elements * sizeof(bool));
		for (unsigned int h = 0; h < total_elements; ++h)
		{
			const unsigned int mask = function_mask[h];
			const int size = ((1 << (BITS * 2)) >> __builtin_popcount(mask));
			const unsigned int shiftconst = (function_term[h] & mask);
			int current_number = shiftconst;
			for (int j = 0; j < size; ++j)
			{
				++tempevalarr[current_number];
				current_number = (((current_number | mask) + 1) & ~mask) | shiftconst;
			}
		}
		for (unsigned int j = 0; j < total_elements; ++j)
		{
			is_unique[j] = false;
			const unsigned int mask = function_mask[j];
			const int size = ((1 << (BITS * 2)) >> __builtin_popcount(mask));
			const unsigned int shiftconst = (function_term[j] & mask);
			int current_number = shiftconst;
			for (int m = 0; m < size; ++m)
			{
				if (tempevalarr[current_number] == 1)
				{
					is_unique[j] = true;
					mark_zero(mask, function_term[j], tempevalarr);
					break;
				}
				current_number = (((current_number | mask) + 1) & ~mask) | shiftconst;
			}
		}
		while (true)
		{
			int best_combs = 0;
			int best_combs_index = -1;
			for (unsigned int j = 0; j < total_elements; ++j)
			{
				if (is_unique[j])
					continue;
				const unsigned int mask = function_mask[j];
				const int size = ((1 << (BITS * 2)) >> __builtin_popcount(mask));
				const unsigned int shiftconst = (function_term[j] & mask);
				int current_number = shiftconst;
				int cur_combs = 0;
				for (int m = 0; m < size; ++m)
				{
					if (tempevalarr[current_number] == 2)
					{
						++cur_combs;
					}
					current_number = (((current_number | mask) + 1) & ~mask) | shiftconst;
				}
				if (cur_combs < best_combs or best_combs_index == -1)
				{
					best_combs = cur_combs;
					best_combs_index = j;
					if (best_combs == 0)
						break;
				}
			}
			if (best_combs_index == -1)
				break;
			const unsigned int mask = function_mask[best_combs_index];
			const int size = ((1 << (BITS * 2)) >> __builtin_popcount(mask));
			const unsigned int shiftconst = (function_term[best_combs_index] & mask);
			int current_number = shiftconst;
			for (int m = 0; m < size; ++m)
			{
				--tempevalarr[current_number];
				if (tempevalarr[current_number] == 1)
				{
					for (int l = 0; l < total_elements; ++l)
					{
						if (is_unique[l] || l == best_combs_index)
							continue;
						if ((current_number & function_mask[l]) == (function_term[l] & function_mask[l]))
						{
							is_unique[l] = true;
							mark_zero(function_mask[l], function_term[l], tempevalarr);
							break;
						}
					}
				}
				current_number = (((current_number | mask) + 1) & ~mask) | shiftconst;
			}
			memmove(function_mask + best_combs_index, function_mask + best_combs_index + 1, (total_elements - best_combs_index - 1) * sizeof(unsigned int));
			memmove(function_term + best_combs_index, function_term + best_combs_index + 1, (total_elements - best_combs_index - 1) * sizeof(unsigned int));
			memmove(is_unique + best_combs_index, is_unique + best_combs_index + 1, (total_elements - best_combs_index - 1) * sizeof(bool));
			--total_elements;
		}
		free(tempevalarr);
		free(is_unique);
		if (DISPLAY_MINTERM_COUNT)
			cout << total_elements;
		if (DISPLAY_MINTERMS)
		{
			bool isfirst = false;
			for (unsigned int u = 0; u < total_elements; ++u)
			{
				if (isfirst)
					cout << " + ";
				else
					isfirst = true;
				for (unsigned int j = 0; j < (BITS * 2); j++)
					if (function_mask[u] & (1 << (BITS * 2 - 1 - j)))
					{
						if (function_term[u] & (1 << (BITS * 2 - 1 - j)))
							cout << "\033[1;32m" << num[j] << "\033[1;0m";
						else
							cout << "\033[1;31m" << lnum[j] << "\033[1;0m";
					}
			}
		}
		if (CHECK_FUNCTION)
		{
			bool *current_cover = (bool *)calloc(1 << (BITS * 2), sizeof(bool));
			for (unsigned int j = 0; j < total_elements; ++j)
			{
				const unsigned int mask = function_mask[j];
				const int size = ((1 << (BITS * 2)) >> __builtin_popcount(mask));
				const unsigned int shiftconst = (function_term[j] & mask);
				int current_number = shiftconst;
				for (int h = 0; h < size; ++h)
				{
					current_cover[current_number] = true;
					current_number = (((current_number | mask) + 1) & ~mask) | shiftconst;
				}
			}
			for (int h = 0; h < (1 << (BITS * 2)); ++h)
			{
				bool is_cur_true = (mulres[h] & (1 << i)) != 0;
				if (is_cur_true != current_cover[h])
					cout << "Error" << endl;
			}
			free(current_cover);
		}
		cout << endl;
		free(function_term);
		free(function_mask);
	}
	auto stop = high_resolution_clock::now();
	auto duration1 = duration_cast<milliseconds>(stop - start);
	cout << "Duration in milliseconds: " << duration1.count() << endl;
}
