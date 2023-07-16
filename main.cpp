#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>

class Item;
class Container;


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
      clearExtra();
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
    float m_price;

  public:
    Item() : m_id {s_id_generator ++}
    {}
    
    friend std::ostream& operator<< (std::ostream& out, const Item& item)
    {
      out << item.m_name;
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
    }
    
    void print_details()
    {
      std::cout << "Name: " << m_name << "\nId: " << m_id <<
      "\nQuantity: " << m_quantity << "\nPrice: " << m_price << '\n';
    }

    std::string write_details()
    {
      std::string _temp_messg{};
      _temp_messg = std::to_string(m_id) + " "
      + std::to_string(m_quantity) + " "
      + std::to_string(m_price) + " " + m_name;
      return _temp_messg;
    }
   
    void update_quantity_add(int _temp_qty) {m_quantity += _temp_qty;}

    void update_quantity_subtract(int _temp_qty) {m_quantity -= _temp_qty;}

    void update_name()
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

class Container
{
  std::vector<Item> u_item_list{};
  public:
    Container() = default;

    void add_item(Item& _itm)
    {
      u_item_list.emplace_back(_itm);
    }

    void display_items()
    {
      for(auto& i : u_item_list)
      i.print_details();
    }

    std::vector<Item>& get_list() {return u_item_list;}
};


void save_data(Container& cont)
{
  std::ofstream file_out;
  file_out.open("stock.dat");
  if(file_out.is_open())
  {
    for(auto& i : cont.get_list())
    {
      file_out << i.write_details() << '\n';
    }
    file_out.close();
  }
  else
  {
    std::cerr << "ERROR: Could not open file \n";
  }
}

void load_data()
{
  std::string line;
  std::ifstream file_in;
  file_in.open("stock.dat");
  if(file_in.is_open())
  {
    while(std::getline(file_in,line))
    {
      std::cout << line << '\n';
    }
    file_in.close();
  }
  else
  {
    std::cerr << "ERROR: Could not open file \n";
  }
}

void add_sales(Container& cont)
{
  std::vector<Item>& item_list = cont.get_list();
  float _total{};
  cont.display_items();

  int _id = prompt_for_numeric<int>("Enter id of product: ");
  for(auto& i : item_list)
  {
    if(_id == i.get_id())
    {
      bool valid{false};
      while(!valid)
      {
        int _qty = prompt_for_numeric<int>("Enter quantity: ");
        if(_qty > i.get_qty())
        {
          std::cerr << "ERROR: Not enough in stock \n";
          continue;
        }
        else if(_qty <= i.get_qty())
        {
          _total += (_qty * i.get_price());
          i.update_quantity_subtract(_qty);
          valid = true;
        }
      }
    }
  }
  std::cout << "Total Bill: $" << _total << '\n';
}

int main()
{
  Container ct;

  Item it1{};
  it1.create_item();
  ct.add_item(it1);
  save_data(ct);

  Item it2{};
  it2.create_item();
  ct.add_item(it2);
  save_data(ct);

  add_sales(ct);
  save_data(ct);

  return 0;
}
