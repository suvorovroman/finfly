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
    
    void startDocument()
    {
      luaL_checkstack(L, 4);
      /* chunk */
      if(lua_pcall(L, 0, 1, 0))
        throw SAXException(lua_tostring(L, -1));
      /* handler */
      lua_pushcfunction(L, cf_startDocument); 
      lua_pushvalua(L, -2);
      if(lua_pcall(L, 1, 3, 0))
        throw SAXException(lua_tostring(L, -1));
      /* element.finish element.start document.finish handler */
    }
    
  };
  
}
