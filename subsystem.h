/* $Id: subsystem.h 440 2016-02-18 22:04:30Z roma $ */
#ifndef	__SUBSYSTEM_H__
#define __SUBSYSTEM_H__

#include "../gui/menu.h"
#include <unordered_map>

/**	\brief	����������. */
class subsystem
{
public:

	/**	\brief ������������� ����������. */
	typedef db::addr_t	id;

	/**	\brief	���� ���������.
	*/
	class menu:public gui::menu
	{
	public:

		/**	\brief	������� ���� ��������� �� ���� MFC.
			���� MFC ������ ���� ������� �������� CreateSubsysMenu.
			\param[in]	_m	���� MFC.
			\exception	std::exception	������ ��� �������� ����.
		*/
		menu(CMenu& _m) throw(...) :gui::menu(_m){}

		/**	\brief	������� ���� ��������� �� �������������� ��������.
			\param[in]	_id	������������� ��������.
			\exception	std::exception	������ ��� �������� ����.
		*/
		menu(db::addr_t _id) throw(...);

		/**	\brief	������������� ����. */
		typedef db::addr_t	id;

		/**	\brief ����������� ������ ����. */
		std::auto_ptr<wxAcceleratorTable> Accelerator;

		/**	\brief	����������� ������������� ����. */
		static const int MinId;

		/**	\brief	������������ ������������� ����. */
		static const int MaxId;
	};

	/**	\brief	������� ���������� ��� ������ ����.
		\param[in]	_id	������������� ������ ����.
		\param[in]	_w	����, �� �������� �������� ����������.
		\exception	ffl::utilities::ad_hoc_error	������������ ������������� ����������.
	*/
	subsystem(menu::id _id, wxWindow* _w = 0);

	~subsystem() throw();

	/**	\brief	��������� ����������.
	*/
	void execute() throw();

	/**	\brief	���� ����������. */
	class stack
	{
	public:

		/**	\brief	������� ����� ����������. */
		void initialize() throw();

		/** \brief �������� �������� � ���� ����������.
			\param[in]	_a	��������.
		*/
		void push(db::addr_t _a) throw();

		/**	\brief	��� ������ ����������. */
		class pool
		{
		private:

			/**	\brief	������ ������. */
			std::vector<stack> Stack;

		public:

			/**	\brief	������� ��� ������ ���������� �������.
				\param[in]	_n	������ ����.
			*/
			pool(size_t _n) throw():Stack(_n)
			{
				for(int i = 0; i < _n; i++)
					Stack[i].Index = i;
			}

			/**	\brief	�������� ���� �� ����.
				\param[in]	_i	����� �����.
			*/
			stack& operator[](int _i) throw()
			{
				return Stack[_i];
			}

		};

	private:
	
		/** \brief	������ ����� � ����. */
		int Index;

	};

	/**	\brief	����� ������� ����������.
		\param[in]	_n	��� �������.
		\param[in]	_a	������ ����������.
		\param[out]	_v	������������ ��������.
		\param[out]	_r	������������ ��������.
		\exception	std::exception	������ ��� ���������� ����������.
	*/
	static bool call(const char* _n, const char* _a, double& _v, short& _r);

private:

	/**	\brief	������������� ����������. */
	id Id;

	static subsystem* Subsystem;

	subsystem* Up;

	wxWindow* Window;	//!< ����, �� �������� �������� ����������.

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

		static const size_t MaxSize = 81;	//!< ������������ ������ �������.

		char* Ptr;	//!< ��������� �� ������ ������ ����������.

		wxString Argument;	//!< �������� ���������.

	};

	bool evaluate(const wxString& _f, argument& _a, double& _v, short& _r);


	typedef short (subsystem::*function)(argument&, double&);

	WX_DECLARE_STRING_HASH_MAP(function, function_map);

	static function_map Function;

	/**	\brief	����� ��������� � ���� ���������.

		DEBUG'['<��������>{<��������>}']'

		\param[in]	_a	������ ����������.
		\param[in]	_v	������������ ��������.
		\return	������ ������������ ��������.
		\exception	std::exception	������ ��� ���������� �������.
	*/
	short evaluate_DEBUG(argument& _a, double& _v);

	/**	\brief	��������� ������� ����.

		WINDOW'['<�������>']'

		<�������>	::=	RELOAD
					::=	RELOAD <����� ������� ��� ��������� �������>

		\param[in]	_a	������ ����������.
		\param[in]	_v	������������ ��������.
		\return	������ ������������ ��������.
		\exception	std::exception	������ ��� ���������� �������.
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

	/** \brief ��������� ������� ����������. */
	class object
	{
	public:

		/** \brief ��������� ��������� �������. */
		typedef short (object::*message)(argument& _a, double& _v);

		/** \brief ������� ��������� ������� (��������� �������). */
		WX_DECLARE_STRING_HASH_MAP(message, iface);

		/** \brief �������� ��������� ������� (����� ������) � �����������.
			\param[in]   _m  ��� ������.
			\param[in]   _a  ������ ����������.
			\param[out]  _v  ������������ ��������.
			\return ������������ ��������.
			\exception failure ����������� ����� �������.
		*/
		short send(const wxString& _m, argument& _a, double& _v);

	protected:
    
		/** \brief �������� ������� ������� ������� ��� ������������ ������.
		����������� ����� ���������� ���������� ����������� ���� ������� ����������. ��� �����
		�� ������ ������� �������, ������� ����� ������������� ������� ������� ��� ������ ����������
		������.
		\return ������ �� ������� �������.
		*/
		virtual iface& get_iface() = 0;
    };

};



#endif