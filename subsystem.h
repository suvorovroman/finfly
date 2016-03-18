/* $Id: subsystem.h 440 2016-02-18 22:04:30Z roma $ */
#ifndef	__SUBSYSTEM_H__
#define __SUBSYSTEM_H__

#include "../gui/menu.h"
#include <unordered_map>

/**	\brief	Подсистема. */
class subsystem
{
public:

	/**	\brief Идентификатор подсистемы. */
	typedef db::addr_t	id;

	/**	\brief	Меню подсистем.
	*/
	class menu:public gui::menu
	{
	public:

		/**	\brief	Создать меню подсистем из меню MFC.
			Меню MFC должно быть создано функцией CreateSubsysMenu.
			\param[in]	_m	Меню MFC.
			\exception	std::exception	Ошибка при создании меню.
		*/
		menu(CMenu& _m) throw(...) :gui::menu(_m){}

		/**	\brief	Создать меню подсистем по идентификатору иерархии.
			\param[in]	_id	Идентификатор иерархии.
			\exception	std::exception	Ошибка при создании меню.
		*/
		menu(db::addr_t _id) throw(...);

		/**	\brief	Идентификатор меню. */
		typedef db::addr_t	id;

		/**	\brief Акселератор клавиш меню. */
		std::auto_ptr<wxAcceleratorTable> Accelerator;

		/**	\brief	Минимальный идентификатор меню. */
		static const int MinId;

		/**	\brief	Максимальный идентификатор меню. */
		static const int MaxId;
	};

	/**	\brief	Создать подсистему для пункта меню.
		\param[in]	_id	Идентификатор пункта меню.
		\param[in]	_w	Окно, из которого запущена подсистема.
		\exception	ffl::utilities::ad_hoc_error	Недопустимый идентификатор подсистемы.
	*/
	subsystem(menu::id _id, wxWindow* _w = 0);

	~subsystem() throw();

	/**	\brief	Выполнить подсистему.
	*/
	void execute() throw();

	/**	\brief	Стек подсистемы. */
	class stack
	{
	public:

		/**	\brief	Очистка стека подсистемы. */
		void initialize() throw();

		/** \brief Добавить значение в стек подсистемы.
			\param[in]	_a	Значение.
		*/
		void push(db::addr_t _a) throw();

		/**	\brief	Пул стеков подсистемы. */
		class pool
		{
		private:

			/**	\brief	Вектор стеков. */
			std::vector<stack> Stack;

		public:

			/**	\brief	Создать пул стеков указанного размера.
				\param[in]	_n	Размер пула.
			*/
			pool(size_t _n) throw():Stack(_n)
			{
				for(int i = 0; i < _n; i++)
					Stack[i].Index = i;
			}

			/**	\brief	Получить стек из пула.
				\param[in]	_i	Номер стека.
			*/
			stack& operator[](int _i) throw()
			{
				return Stack[_i];
			}

		};

	private:
	
		/** \brief	Индекс стека в пуле. */
		int Index;

	};

	/**	\brief	Вызов функции подсистемы.
		\param[in]	_n	Имя функции.
		\param[in]	_a	Список аргументов.
		\param[out]	_v	Возвращаемое значение.
		\param[out]	_r	Возвращаемое значение.
		\exception	std::exception	Ошибка при выполнении подсистемы.
	*/
	static bool call(const char* _n, const char* _a, double& _v, short& _r);

private:

	/**	\brief	Идентификатор подсистемы. */
	id Id;

	static subsystem* Subsystem;

	subsystem* Up;

	wxWindow* Window;	//!< Окно, из которого запущена подсистема.

	class argument
	{
	public:

		argument(const char* _a) throw():Ptr((char*)_a){};

		enum kind
		{
			STRING,
			WORD
		};

		bool next(kind _k) throw();

		wxString get() throw()
		{
			return Argument;
		}

		void get(db::addr_t& _v) throw()
		{
			_v = get_db8_fromarg((char*)(const char*)Argument.c_str());
		}

		void get(long& _v) throw() 
		{
			_v = get_long_fromarg((char*)(const char*)Argument);
		}

		void get(void*& _v) throw()
		{
			_v = (void*)get_long_fromarg((char*)(const char*)Argument);
		}

		void get(int& _v) throw()
		{
			_v = get_int_fromarg((char*)(const char*)Argument);
		}

	private:

		static const size_t MaxSize = 81;	//!< Максимальный размер лексемы.

		char* Ptr;	//!< Указатель на начало списка аргументов.

		wxString Argument;	//!< Значение аргумента.

	};

	bool evaluate(const wxString& _f, argument& _a, double& _v, short& _r);


	typedef short (subsystem::*function)(argument&, double&);

	WX_DECLARE_STRING_HASH_MAP(function, function_map);

	static function_map Function;

	/**	\brief	Вывод сообщения в окно отладчика.

		DEBUG'['<значение>{<значение>}']'

		\param[in]	_a	Список аргументов.
		\param[in]	_v	Возвращаемое значение.
		\return	Второе возвращаемое значение.
		\exception	std::exception	Ошибка при выполнении функции.
	*/
	short evaluate_DEBUG(argument& _a, double& _v);

	/**	\brief	Выполнить команду окна.

		WINDOW'['<команда>']'

		<команда>	::=	RELOAD
					::=	RELOAD <ключь объекта для повторной загруки>

		\param[in]	_a	Список аргументов.
		\param[in]	_v	Возвращаемое значение.
		\return	Второе возвращаемое значение.
		\exception	std::exception	Ошибка при выполнении функции.
	*/
	short evaluate_WINDOW(argument& _a, double& _v);

	short evaluate_NEW(argument& _a, double& _v);

	short evaluate_SEND(argument& _a, double& _v);

	short evaluate_WINDOW(wxWindow* _w, const wxString& _c, argument& _a, double& _v);

	class failure:public std::runtime_error
	{
	public:

		failure():std::runtime_error("Subsystem error logged"){}

	};

	/** \brief Интерфейс объекта подсистемы. */
	class object
	{
	public:

		/** \brief Сигнатура сообщения объекта. */
		typedef short (object::*message)(argument& _a, double& _v);

		/** \brief Таблица сообщений объекта (интерфейс объекта). */
		WX_DECLARE_STRING_HASH_MAP(message, iface);

		/** \brief Отправка сообщения объекту (вызов метода) с параметрами.
			\param[in]   _m  Имя метода.
			\param[in]   _a  Список аргументов.
			\param[out]  _v  Возвращаемое значение.
			\return Возвращаемое значение.
			\exception failure Неизвестный матод объекта.
		*/
		short send(const wxString& _m, argument& _a, double& _v);

	protected:
    
		/** \brief Получить таблицу методов объекта для производного класса.
		Производный класс определяет реализацию конкретного типа объекта подсистемы. Для этого
		он должен таблицу методов, которая будет запрашиваться базовым классом для вызова отдельного
		метода.
		\return Ссылка на таблицу методов.
		*/
		virtual iface& get_iface() = 0;
    };

};



#endif