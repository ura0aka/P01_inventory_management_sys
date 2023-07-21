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
    static inline int s_id_generator {1}; // keep this in mind
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
      _temp_messg = std::to_string(m_id) + "\n"
      + std::to_string(m_quantity) + "\n"
      + std::to_string(m_price) + "\n" + m_name;
      return _temp_messg;
    }
   
    void set_id(int _id) {m_id = _id;}
    void set_name(std::string _str) {m_name = _str;}
    void set_price(int _price) {m_price = _price;}
    void set_qty(int _quantity) {m_quantity = _quantity;}

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

std::string go_to_line(std::ifstream& f, unsigned int num)
{
  std::string str;
  f.seekg(std::ios::beg);
  for(int i{0}; i<num; ++i)
  {
    f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  std::getline(f,str);
  return str;
}

int count_lines_in_file()
{
  std::ifstream f;
  int count_line{0};
  int prod_count{0};
  std::string line;
  f.open("stock.dat");
  if(f.is_open())
  {
    while(std::getline(f,line))
    {
      ++ count_line;
    }
    prod_count = count_line/4;
    std::cout << prod_count << '\n';
  }
  return prod_count;
}


// loads data from save file (stock.dat) by creating item and loading previously entered data
void load_data(Container& cont)
{
  int prod_count{0};
  int count{0};
  bool finished_count{false};
  int count_line{0};
  std::string line;
  std::ifstream file_in;

  file_in.open("stock.dat");
  prod_count = count_lines_in_file();

  if(file_in.is_open())
  {
    while(!finished_count)
    {
      std::cout << "File successfully opened ... \n";
      Item tmp_it{}; // create temporary item
      tmp_it.set_id(std::stoi(go_to_line(file_in,0+count_line)));
      tmp_it.set_qty(std::stoi(go_to_line(file_in,1+count_line)));
      tmp_it.set_price(std::stof(go_to_line(file_in,2+count_line)));
      tmp_it.set_name(go_to_line(file_in,3+count_line));
    
      cont.add_item(tmp_it);
      cont.display_items();

      ++count;
      count_line += 4;
      if(prod_count == count)
      {
        finished_count = true;
      }
    }
  }
  else
  {
    std::cerr << "ERROR: Could not open file \n";
  }
  file_in.close();
}

void add_sales(Container& cont)
{
  std::vector<Item>& item_list = cont.get_list();
  float _total{};
  cont.display_items();
  
  char _query{'y'};
  while(_query == 'y' || _query == 'Y')
  {
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
    std::cout << "Add more items to order? (y/n): ";
    std::cin >> _query;
  }
  std::cout << "Total Bill: $" << _total << '\n';
}



void run_program(Container& cont)
{
  char _query{'y'}; 
  while(_query == 'y' || _query == 'Y')
  {
    int _opt{};
    std::cout << "##### MENU #####"
    << "\n1. Create item" << "\n2. Add sales" << "\n3. Display items" << "\n 4. Exit program \n";
    _opt = prompt_for_numeric<int>("Enter option: ");
    switch(_opt)
    {
      case 1:
      {
        Item it{};
        it.create_item();
        cont.add_item(it);
        save_data(cont);
        break;
      }
      case 2:
      {
        add_sales(cont);
        save_data(cont);
        break;
      }
      case 3:
      {
        load_data(cont);
        //cont.display_items();
        break;
      }
      case 4:
      {
        exit(0);
      }
      default: exit(0); break;
    }
    std::cout << "Continue? (y/n): ";
    std::cin >> _query;
  }
}

int main()
{
  Container ct;

  /*
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
  */
  run_program(ct);

  return 0;
}
