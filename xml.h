namespace xml
{
  
  class interpreter:public HandlerBase, public XMLFormatTarget
  {
  public:
  
    interpreter(const wxString& _f)
    {
      L = luaL_newstate();
      if(L)
      {
        lua_atpanic(L, cf_panic);
        if(luaL_loadfile(L, _f))
        {
          wxLogError("%s", lua_tostring(L, -1));
          throw failure();
        }
      }
      else
        throw std::bad_alloc("failed to allocate new Lua state for xml handler");
    }
    
    ~interpreter()
    {
      lua_close(L);
    }
    
  private:
  
    lua_State* L;
    
    static int cf_panic(lua_State* _l)
    {
      throw SAXException(lua_tostring(_l, -1));
      return 0;
    }
    
  };
  
}
