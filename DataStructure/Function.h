#pragma once
#include <cstdlib>
#include <utility>
#include <type_traits>

//std::function���ҿ���������
//�����ģ��stl��ʵ��һ��,û�д���[��������̫��ʱ�ڶ��������ڴ�]
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
		Func f_; //�����Ŀɵ��ö���
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
		Base *f_; //���û���ָ��ʵ�ֶ�̬
		std::aligned_storage<3 * sizeof(void *)>::type buf_;
		//aligned_storage�ǿ���ջ�ϵģ���֤������ڴ�
	public:
		template <typename Func> 
		Function(Func &&f, typename std::enable_if
			< //ע�������remove_reference,��Ϊis_same����Ҫ��ȷƥ��(����universial reference�Ĺ���)
			!std::is_same<typename std::remove_reference<Func>::type, Function>::value
			>::type* = nullptr) //SFINAE,ʹ�ÿ��������޷�ƥ���������
		{
			typedef FuncImpl<typename std::remove_reference<Func>::type, Ret, Args...> ImplType;
			//��֮ǰһֱ�����ĵط����ڣ���α����������δ֪�Ķ���
			//��ʵֻ��Ҫ�ڹ����ʱ�����������Ϣ��Ȼ����һ�����ж�̬�Ե�ָ����������������
			f_ = reinterpret_cast<Base *>(&buf_); 
			new(f_) ImplType(std::forward<Func>(f));	//ֱ�ӹ��죬ָ��ǿת
		}
		Function(const Function &rhs) 
		{
			//��ʱ��Ҫ���õ���̬���캯������������ʵ�ǲ����ڵ�
			//�û����е�clone������
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
