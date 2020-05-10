#ifndef __SINGLETON_H__
#define __SINGLETON_H__

/// <summary>
/// Template Singleton Class
/// </summary>
template<typename  T>
class Singleton
{
public:
	
	/// <summary>
	/// Get an instance of the singleton. Creates one if
	/// one doesn't exist
	/// </summary>
	/// <returns></returns>
	static T* GetInstance()
	{
		static MemGuard memg; // clean up on program end
		if(m_pInstance == nullptr)
		{
			m_pInstance = new T();
		}

		return m_pInstance;
	}

	Singleton(const Singleton&) = delete;
	Singleton& operator= (const Singleton) = delete;

protected:
	Singleton() = default;
	virtual ~Singleton() = default;

private:
	static T* m_pInstance;


	//Class for deletion on program end
	class MemGuard {
      public: 
        ~MemGuard() {
			delete m_pInstance;
			m_pInstance = nullptr;
        }
    };

	
};

template<typename T>
T* Singleton<T>::m_pInstance = nullptr;

#endif //!__SINGLETON_H__
