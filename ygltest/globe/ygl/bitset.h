#ifndef _YANJUN_BITSET_H_
#define _YANJUN_BITSET_H_

#include "string.h"

#define CALC_UNITS(x) (((x/sizeof(unsigned long))>>3)+1)

template <size_t TotBitCnt>
class BitSet
{
public:
	~BitSet()
	{
	}

	BitSet()
	{
		totalBytes=CALC_UNITS(TotBitCnt);
		memset(bits,0,totalBytes*sizeof(unsigned long));
	}
	void set(int pos)
	{
		int lp=pos/32,rp=pos%32;
		bits[lp]|=1<<rp;		
	}	
	void clear(int pos)
	{
		int lp=pos/32,rp=pos%32;
		bits[lp]&=~(1<<rp);	
	}
	bool get(int pos)
	{
		int lp=pos/32,rp=pos%32;

		return bits[lp]&(1<<rp);
	}
	int getLowest0()
	{
		unsigned long res;
		for(int i=0;i<totalBytes;i++)
		{
			res=(bits[i]+1)&~bits[i];
			if(res)
			{
				i<<=3;
				while(res) res>>=1,++i;
				return i;
			}
		}
		return -1;
	}
	//return failed cnt
	int getLowest0(int n, unsigned int *dst)
	{
		unsigned long res,save;
		for(int i=0;i<totalBytes;i++)
		{
			res=(bits[i]+1)&~bits[i];
			if(res)
			{
				save=res;

				while(res)
				{					
					*dst=i<<3;bits[i]|=res;
					while(res) res>>=1,++*dst;
					dst++;if(!--n) break;					
					res=(bits[i]+1)&~bits[i];
				}
				bits[i]=save;
				if(!n) return 0;
			}
		}
		return n;
	}

	unsigned long bits[CALC_UNITS(TotBitCnt)];
	int totalBytes;
};

#endif