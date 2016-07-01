namespace xml
{
  
  class handler:public HandlerBase
  {
    
  };
  
  class internal_handler:public handler
  {
    
  };
  
  class external_handler:public handler
  {
  private:
  
    lua_state State;
    
  };
  
}
