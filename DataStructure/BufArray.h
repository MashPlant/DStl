#pragma once
namespace ds
{
	template <typename T,int Num,int Size>
	class BufArray
	{
		static T buffer[Num][Size];
		static unsigned usage[Num / 32];
		T *arr = nullptr;
		int len = 0;
	public:
		BufArray()
		{
			for (int i = 0; i < sizeof(usage) / 4; ++i)
				if (usage[i] != 0xffffffff)
				{
					for (int bit = 0; bit < 31; ++bit)
						if (~usage[i] & 1 << bit)
						{
							arr = buffer[i * 32] + bit;
							usage[i] |= 1 << bit;
							return;
						}
				}
		}
		T &operator[](int pos) { return arr[pos]; }
		T &back() { return arr[len - 1]; }
		void push_back(const T &val) { arr[len++] = val; }
		int size() const { return len; }
		bool empty() const { return len == 0; }
		~BufArray()
		{
			for (int i = 0; i < len; ++i)
				(arr + i)->~T();
			int where = arr - buffer;
			usage[where / 32] ^= 1 << (where % 32);
		}
	};
	template <typename T, int Num, int Size>
	T BufArray<T, Num, Size>::buffer[Num][Size];
	template <typename T, int Num, int Size>
	unsigned BufArray<T, Num, Size>::usage[Num / 32];
}