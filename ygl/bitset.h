#ifndef _YGL_BITSET_
#define _YGL_BITSET_

typedef unsigned char uchar;

#define BITSET_UNIT (sizeof(uchar)<<3)
#define BITS_COUNT_TO_UNIT_COUNT(b) (((b)+(BITSET_UNIT-1))/BITSET_UNIT)

template <size_t BITS_COUNT>
class bitset
{
public:
	// Assert(size_>0)
	bitset():size_(BITS_COUNT),cnt_(BITS_COUNT_TO_UNIT_COUNT(BITS_COUNT))
	{

	}

	bool operator[](int pos)
	{
		if(pos<0||pos>=size_) return false;
		int base=pos/BITSET_UNIT;
		int rem=pos%BITSET_UNIT;
		uchar bit=bits_[base];
		return bit&(1<<rem);
	}

	// wrap
	int lowest0(int begin=0,bool set=true)
	{
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
// 	void reset()
// 	{
// 		memset(bits_,0,sizeof(uchar)*cnt_);
// 		uchar rem=size_%BITSET_UNIT;
// 		if(rem)
// 			bits_[cnt_-1]|=~((uchar)((1<<rem)-1));
// 	}
	void set(int pos)
	{
		if(pos<0||pos>=size_) return;
		int base=pos/BITSET_UNIT;
		int rem=pos%BITSET_UNIT;
		bits_[base]|=((uchar)(1<<rem));
	}
	void clear(int pos)
	{
		if(pos<0||pos>=size_) return;
		int base=pos/BITSET_UNIT;
		int rem=pos%BITSET_UNIT;
		bits_[base]&=(uchar)~((uchar)(1<<rem));
	}

	int size_; // how many bits
private:
	int cnt_; // how many uchars
	uchar bits_[BITS_COUNT_TO_UNIT_COUNT(BITS_COUNT)];
};
#endif