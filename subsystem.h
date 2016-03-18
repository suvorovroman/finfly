class subsystem
{
  
private:  

  /** \brief Интерфейс объекта подсистемы. */
  class object
  {
  public:
  
    /** \brief Сигнатура метода объекта. */
    typedef short (*method)(argument& _a, double& _v);
    
    /** \brief Таблица методов объекта (интерфейс объекта). */
    typedef std::unsorted_map<wxString, method> iface;
    
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
