#include "bitset.h"


bool bitset::operator[](int pos)
{
	if(pos<0||pos>=size_) return true;
	int base=pos/BITSET_UNIT;
	int rem=pos%BITSET_UNIT;
	uchar bit=bits_[base];
	return bit&(1<<rem);
}
void bitset::reset()
{
	memset(bits_,0,sizeof(uchar)*cnt_);
	uchar rem=size_%BITSET_UNIT;
	if(rem)
		bits_[cnt_-1]|=~((uchar)((1<<rem)-1));
}

// wrap
int bitset::lowest0(int begin,bool set)
{
	++begin;
	if(begin<0||begin>=size_) begin=0;
	int base=begin/BITSET_UNIT;
	int rem=begin%BITSET_UNIT;
	int skip=((1<<rem)-1);

	int i=0,j;
	if((bits_[base]|skip)==255)
	{
		i=1;		
		while(i<cnt_&&bits_[(i+base)%cnt_]==255) ++i;
		if(i==cnt_)
		{
			return -1;
		}
		uchar bit=bits_[(i+base)%cnt_];
		j=0;
		while(bit&1) ++j,(bit>>=1);
	}
	else
	{
		uchar bit=bits_[(i+base)%cnt_];
		j=rem;bit>>=j;
		while(bit&1) ++j,(bit>>=1);
	}
	if(set) bits_[(i+base)%cnt_]|=(1<<j);
	return ((i+base)%cnt_)*BITSET_UNIT+j;
}

void bitset::clear(int pos)
{
	if(pos<0||pos>=size_) return;
	int base=pos/BITSET_UNIT;
	int rem=pos%BITSET_UNIT;
	bits_[base]&=(uchar)~((uchar)(1<<rem));
}