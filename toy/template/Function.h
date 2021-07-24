#pragma once
#include <cstdlib>
#include <utility>
#include <type_traits>

//std::function在我看来很神奇
//这里就模仿stl来实现一个,没有处理[函数对象太大时在堆上申请内存]
namespace ds
{
	template <typename Ret, typename ...Args>
	class FuncBase
	{
	public:
		virtual Ret operator()(Args&&...) = 0;
		virtual void clone(FuncBase *) const = 0;
		virtual ~FuncBase() {}
	};

	template <typename Func, typename Ret, typename ...Args>
	class FuncImpl : public FuncBase<Ret, Args...>
	{
		typedef FuncBase<Ret, Args...> Base;
		typedef FuncImpl<Func, Ret, Args...> Type;
		Func f_; //真正的可调用对象
	public:
		FuncImpl(const Func &f) : f_(f) {}
		Ret operator()(Args&&... args) override
		{
			return f_(std::forward<Args>(args)...);
		}
		void clone(Base *dest) const override
		{
			new(dest) Type(f_);
		}
	};
	template <typename ...>
	class Function;

	template <typename Ret, typename ...Args>
	class Function<Ret(Args...)>
	{
		typedef FuncBase<Ret, Args...> Base;
		Base *f_; //利用基类指针实现多态
		std::aligned_storage<3 * sizeof(void *)>::type buf_;
		//aligned_storage是开在栈上的，保证对齐的内存
	public:
		template <typename Func> 
		Function(Func &&f, typename std::enable_if
			< //注意下面的remove_reference,因为is_same必须要求精确匹配(考虑universial reference的规则)
			!std::is_same<typename std::remove_reference<Func>::type, Function>::value
			>::type* = nullptr) //SFINAE,使得拷贝构造无法匹配这个函数
		{
			typedef FuncImpl<typename std::remove_reference<Func>::type, Ret, Args...> ImplType;
			//我之前一直不理解的地方在于，如何保存这个类型未知的对象
			//其实只需要在构造的时候给出类型信息，然后用一个具有动态性的指针来保存它就行了
			f_ = reinterpret_cast<Base *>(&buf_); 
			new(f_) ImplType(std::forward<Func>(f));	//直接构造，指针强转
		}
		Function(const Function &rhs) 
		{
			//这时需要利用到多态构造函数，但是这其实是不存在的
			//用基类中的clone来代替
			f_ = reinterpret_cast<Base *>(&buf_);
			rhs.f_->clone(f_);
		}
		Ret operator()(Args &&...args)
		{
			return f_->operator()(std::forward<Args>(args)...);
		}
		~Function()
		{
			if (f_)
				f_->~Base();
		}
	};
}
