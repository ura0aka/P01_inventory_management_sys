#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

class Item;
std::vector<Item> u_item_list{}; 

void clearExtra()
{
  // ignores all of the characters up until newline
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

template <typename T>
T prompt_for_numeric(std::string message)
{
  while(1)
  {
    T _entry;
    bool _isNumeric {false};
    std::cout << message;
    if(std::cin >> _entry)
    {
      _isNumeric = true;
      if(_entry < 0)
      {
        std::cerr << "Cannot be (less than) < 0.\n";
        continue;
      }
      return _entry;
    } else {
      _isNumeric = false;
      std::cerr << "ERROR: Invalid entry.\n";
      std::cin.clear();
      clearExtra();
      continue;
    }
  }
}

class Item
{
  private:
    static inline int s_id_generator {1};
    int m_id;
    std::string m_name;
    int m_quantity;
    double m_price;

  public:
    Item() : m_id {s_id_generator ++}
    {}
    

    friend std::ostream& operator<< (std::ostream& out, const Item& item)
    {
      out << item.m_name << item.m_id << item.m_quantity << item.m_price;
      return out;
    }

    void create_item()
    {
      bool _isNumeric {false};

      std::cout << "Enter name: ";
      std::getline(std::cin, m_name);
      if(std::cin.fail())
      {
        std::cin.clear();
        clearExtra();
      }
      m_price = prompt_for_numeric<double>("Enter price ($): ");
      m_quantity = prompt_for_numeric<int>("Enter stock: ");

      u_item_list.emplace_back(std::move(this));
    }

    void print_details()
    {
      std::cout << "================== \n";
      std::cout << "Name: " << m_name <<
      "\n ID: " << m_id << "\n Stock: " << m_quantity
      << "\n Price: $" << m_price << '\n';
      std::cout << "================== \n";
    }
    
    void change_name()
    {
      clearExtra();
      std::cout << "Current name: " << m_name
      << "\nChange name to: ";
      std::getline(std::cin, m_name);
      if(std::cin.fail())
      {
        std::cin.clear();
        clearExtra();
      }
    }

    int get_id() {return m_id;}
    std::string get_name() {return m_name;}
    int get_qty() {return m_quantity;}
    double get_price() {return m_price;}
};




int main()
{
  Item it1{};
  it1.create_item();
  it1.print_details();
  it1.change_name();
  it1.print_details();
  Item it2{};
  it2.create_item();
  it2.print_details();

  for(auto& i : u_item_list)
    std::cout << '[' << i << "]\n";
  return 0;
}