/* $Id: subsystem.cpp 440 2016-02-18 22:04:30Z roma $ */
#include "BHSYS.H"
#include "subsystem.h"
#include "altKeys.h"
#include <wx/log.h>
#include "wx/msw/private/keyboard.h"
#include <ffl\utilities\utilities.h>
#include "../main/application.h"
#include <typeinfo>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int subsystem::menu::MinId = 4000;
const int subsystem::menu::MaxId = 32000;

subsystem::menu::menu(db::addr_t _id) throw(...)
{
	extern int CreateSubsysMenu(db::addr_t _adr_own, CMenu *_cm, int *_cout_alt, struct AltKeys *_ak, int _max_ak, int *_count_key, int _new );
	int na, nk;
	struct AltKeys a[120];
	CMenu r;
	r.CreatePopupMenu();
	CreateSubsysMenu(_id, &r, &na, a, 120, &nk, 1);
	append(r);

	if(nk > 0)
	{
		wxAcceleratorEntry* e = new wxAcceleratorEntry[nk];
		for(int i = 0; i < nk; i++)
		{
			int f = wxACCEL_NORMAL;
			if(a[i].KeyFlag&0x01)
				f |= wxACCEL_CTRL;
			if(a[i].KeyFlag&0x02)
				f |= wxACCEL_SHIFT;
			if(a[i].KeyFlag&0x04)
				f |= wxACCEL_ALT;
	
			FILE_NO g;
			db::addr_t s;
			d_decode_dba(a[i].AdrExt, (int16_t *)&g, &s);

			int c = MinId + s;

			if(c > MaxId)
			{
				std::stringstream s;
				s << "Menu ID " << c << " must be in range [" << MinId << "," << MaxId << "]";
				throw std::logic_error(s.str().c_str());
			}

			e[i].Set(f, wxMSWKeyboard::VKToWX(a[i].KeyID), c);
		}

		try
		{
			Accelerator.reset(new wxAcceleratorTable(nk, e));
		}
		catch(...)
		{
			delete[] e;
			throw;
		}

		delete[] e;
	}

}

subsystem::subsystem(menu::id _id, wxWindow* _w) throw(...):Up(Subsystem), Window(_w)
{
	Subsystem = this;

	if(Function.empty())
	{
		Function["DEBUG"]	= &subsystem::evaluate_DEBUG;
		Function["WINDOW"]	= &subsystem::evaluate_WINDOW;
		Function["NEW"]		= &subsystem::evaluate_NEW;
		Function["SEND"]	= &subsystem::evaluate_SEND;
	}

	if(d_recfrst(SUBSYS, task, CURR_DB) == S_OKAY)
	{
		db::addr_t a;
		if(d_crget(&a, task, CURR_DB) == S_OKAY)
		{
			FILE_NO f;
			db::addr_t s;
			d_decode_db8(a, (int16_t *)&f, &s);
			d_encode_db8(f, (db::addr_t)_id, &Id);
			if(d_crset(&Id, task, CURR_DB) == S_OKAY)
				return;
		}
	}

	throw ffl::utilities::ad_hoc_error(wxT("Ошибка при получении идентификатора подсистемы."));
}

subsystem::~subsystem()
{
	Subsystem = Up;
}

void subsystem::execute() throw()
{
	ExecuteSystem(Id, 1);
}

void subsystem::stack::initialize() throw()
{
	InitMarkArea(Index);
}

void subsystem::stack::push(db::addr_t _a) throw()
{
	AddMarkRecord(_a, Index);
}

bool subsystem::call(const char* _n, const char* _a, double& _v, short& _r)
{
	return Subsystem ? Subsystem->evaluate(wxString(_n).Upper(), argument(_a), _v, _r):false;
}

subsystem* subsystem::Subsystem = 0;

subsystem::function_map subsystem::Function;

bool subsystem::evaluate(const wxString& _f, argument& _a, double& _v, short& _r)
{
	function_map::const_iterator it = Function.find(_f);
	if(it != Function.end())
	{
		_r = (this->*(it->second))(_a, _v);
		return true;
	}
	return false;
}

short subsystem::evaluate_DEBUG(argument& _a, double& _v)
{
#ifdef _DEBUG
	wxString s;
	while(_a.next(argument::STRING))
		s += _a.get();
	wxLogDebug(wxT("%s"), s);
#endif
	return 0;
}

short subsystem::evaluate_WINDOW(argument& _a, double& _v)
{
	_v = (double)(unsigned int)Window;

	short r = 0;
	if(Window)
	{
		if(_a.next(argument::WORD))
			r = evaluate_WINDOW(Window, _a.get().Upper(), _a, _v);
	}

	return r;
}

short subsystem::evaluate_WINDOW(wxWindow* _w, const wxString& _c, argument& _a, double& _v)
{
	if(_c == "RELOAD")
	{
		wxCommandEvent e(command::RELOAD);
		if(_a.next(argument::WORD))
		{
			long v;

			_a.get(v);
			e.SetExtraLong(v);
		}
		wxPostEvent(_w, e);
	}
	else
	if(_c == "LOCATE")
	{
		if(_a.next(argument::WORD))
		{
			wxCommandEvent e(command::LOCATE);
			long v;

			_a.get(v);
			e.SetExtraLong(v);
			wxPostEvent(_w, e);	
		}
		else
		{
			wxLogError(wxT("Не указан адрес записи для команды WINDOW[LOCATE]"));
			throw failure();
		}
	}
	else
	if(_c == "TABLE")
	{
	}

	return 0;
}

short subsystem::evaluate_NEW(argument& _a, double& _v)
{
	wxLogDebug("subsystem::evaluate_NEW");
	return 0;
}

short subsystem::evaluate_SEND(argument& _a, double& _v)
{
	return 0;
}

bool subsystem::argument::next(kind _k) throw()
{
	char s[MaxSize];
	bool r = (_k == STRING ? get_string(Ptr, &Ptr, s):get_word(Ptr, &Ptr, s)) != 0;
	Argument = s;
	return r;
}

short subsystem::object::send(const wxString& _m, argument& _a, double& _v)
{
	iface& i = get_iface();
	iface::iterator it = i.find(_m);
	if(it != i.end())
		return (this->*(it->second))(_a, _v);
	else
	{
		wxLogError(wxT("Неизвестный метод \"%s\" объекта"), _m);
		throw failure();
	}
}	




